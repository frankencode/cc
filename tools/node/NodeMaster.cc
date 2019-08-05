/*
 * Copyright (C) 2007-2019 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/Process>
#include <cc/User>
#include <cc/Group>
#include <cc/IoMonitor>
#include <cc/SignalMaster>
#include <cc/http/exceptions>
#include "ErrorLog.h"
#include "AccessLog.h"
#include "SystemLog.h"
#include "NodeConfig.h"
#include "ServiceRegistry.h"
#include "ConnectionManager.h"
#include "HttpServerSocket.h"
#include "SecurityCache.h"
#include "NodeMaster.h"

namespace ccnode {

using namespace cc::http;

int NodeMaster::run(int argc, char **argv)
{
    Thread::blockSignals(SignalSet::createFull());

    SystemLog::open(String{argv[0]}->fileName(), 0, LOG_DAEMON);

    Ref<NodeMaster> node = new NodeMaster{argc, argv};

    if (node->config()->daemon() && !Process::isDaemonized())
        Process::daemonize();

    auto signalMaster = SignalMaster::start([=](Signal signal, bool *fin){
        node->signals_->pushBack(signal);
        *fin = (+signal == SIGINT || +signal == SIGTERM);
    });

    node->start();
    node->wait();

    signalMaster->wait();

    return node->exitCode_;
}

NodeMaster::NodeMaster(int argc, char **argv):
    config_{NodeConfig::load(argc, argv)},
    signals_{Signals::create()},
    exitCode_{0}
{}

void NodeMaster::run()
{
    try {
        ErrorLog::instance()->open(config()->errorLogConfig());
    }
    catch (Exception &ex) {
        CCNODE_ERROR() << ex << nl;
        exitCode_ = 1;
        return;
    }

    while (true) {
        try {
            runNode();
        }
        catch (Signaled &ex) {
            if (+ex->signal() != SIGHUP) {
                if (+ex->signal() == SIGINT || +ex->signal() == SIGTERM)
                    exitCode_ = 0;
                else
                    exitCode_ = +ex->signal() + 128;
                break;
            }
        }
        #ifdef NDEBUG
        catch (Exception &ex) {
            CCNODE_ERROR() << ex << nl;
            exitCode_ = 1;
            break;
        }
        #endif
    }
}

void NodeMaster::runNode()
{
    CCNODE_NOTICE() << "Starting (pid = " << Process::getId() << ")" << nl;

    typedef List< Ref<StreamSocket> > ListeningSockets;
    Ref<ListeningSockets> listeningSockets = ListeningSockets::create();

    for (SocketAddress *address: config()->address()) {
        CCNODE_NOTICE() << "Start listening at " << address << nl;
        listeningSockets->append(StreamSocket::listen(address));
    }

    if (config()->user() != "") {
        String userName = config()->user();
        String groupName = config()->group();
        if (groupName == "") groupName = userName;
        Ref<User> user = User::lookup(userName);
        Ref<Group> group = Group::lookup(groupName);
        if (!user->isValid()) throw UsageError{"No such user: \"" + userName + "\""};
        if (!group->isValid()) throw UsageError{"No such group: \"" + groupName + "\""};
        CCNODE_NOTICE() << "Dropping process persona to user:group " << userName << ":" << groupName << " (uid:gid = " << user->id() << ":" << group->id() << ")" << nl;
        Process::setUserId(user->id());
        Process::setGroupId(group->id());
    }

    CCNODE_NOTICE() << "Starting security master, if needed" << nl;
    Ref<SecurityCache> securityCache = SecurityCache::start(config());

    Ref<ConnectionManager> connectionManager = ConnectionManager::create(config());
    Ref<PendingConnections> pendingConnections = PendingConnections::create();
    Ref<ClosedConnections> closedConnections = connectionManager->closedConnections();

    CCNODE_NOTICE() << "Creating worker pool (concurrency = " << config()->concurrency() << ")" << nl;

    typedef Array< Ref<ServiceWorker> > WorkerPool;
    Ref<WorkerPool> workerPool = WorkerPool::create(config()->concurrency());
    for (Ref<ServiceWorker> &worker: workerPool) {
        worker = ServiceWorker::create(config(), pendingConnections, closedConnections);
        worker->start();
    }

    CCNODE_NOTICE() << "Up and running (pid = " << Process::getId() << ")" << nl;

    Ref<IoMonitor> ioMonitor = IoMonitor::create(listeningSockets->count());
    for (StreamSocket *socket: listeningSockets)
        ioMonitor->addEvent(IoReady::Accept, socket);

    CCNODE_DEBUG() << "Accepting connections" << nl;

    while (true) {
        Ref<IoActivity> activity = ioMonitor->wait(1000);
        for (const IoEvent *event: activity) {
            try {
                StreamSocket *listeningSocket = Object::cast<StreamSocket *>(event->target());
                Ref<HttpServerSocket> clientSocket = HttpServerSocket::accept(listeningSocket, config(), securityCache);
                Ref<HttpServerConnection> clientConnection = HttpServerConnection::open(clientSocket);
                if (connectionManager->accept(clientConnection)) {
                    CCNODE_DEBUG() << "Accepted connection from " << clientConnection->address() << " with priority " << clientConnection->priority() << nl;
                    pendingConnections->pushBack(clientConnection, clientConnection->priority());
                }
                else {
                    CCNODE_DEBUG() << "Rejected connection from " << clientConnection->address() << nl;
                }
            }
            catch (Exception &ex) {
                CCNODE_ERROR() << ex << nl;
            }
        }

        connectionManager->cycle();

        while (signals_->count() > 0) {
            Signal signal = signals_->popFront();
            if (+signal == SIGINT || +signal == SIGTERM || +signal == SIGHUP) {
                CCNODE_NOTICE() << "Received " << signal << ", shutting down" << nl;
                workerPool = nullptr;
                CCNODE_NOTICE() << "Shutdown complete" << nl;
                throw Signaled{signal};
            }
        }
    }
}

} // namespace ccnode
