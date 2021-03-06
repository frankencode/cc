/*
 * Copyright (C) 2007-2019 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/http/DirectoryDelegate>
#include <cc/http/DeliveryWorker>
#include <cc/http/DirectoryInstance>
#include <cc/http/MediaTypeDatabase>
#include <cc/http/utils>
#include <cc/http/exceptions>
#include <cc/File>
#include <cc/Dir>

namespace cc {
namespace http {

Ref<DirectoryDelegate> DirectoryDelegate::create(DeliveryWorker *worker, ScriptHandler *scriptHandler)
{
    return new DirectoryDelegate{worker, scriptHandler};
}

DirectoryDelegate::DirectoryDelegate(DeliveryWorker *worker, ScriptHandler *scriptHandler):
    DeliveryDelegate{worker},
    directoryInstance_{worker->deliveryInstance()},
    scriptHandler_{scriptHandler}
{}

void DirectoryDelegate::process(const HttpRequest *request)
{
    String path = directoryInstance_->path() + "/" + request->uri();
    path = path->canonicalPath();
    String prefix = path->head(directoryInstance_->path()->count());
    if (path->head(directoryInstance_->path()->count()) != directoryInstance_->path()) throw Forbidden{};

    if ((!directoryInstance_->showHidden()) && path->baseName()->startsWith('.')) throw NotFound{};

    FileStatus fileStatus{path};

    if (scriptHandler_) {
        if (
            (!fileStatus->isValid()) ||
            (+(fileStatus->mode() & FileMode::AnyExec) && (fileStatus->type() != FileType::Directory))
        ) {
            if (scriptHandler_->process(request, fileStatus, directoryInstance_->path()))
                return;
        }
    }

    if (!fileStatus->isValid()) throw NotFound{};

    {
        String h;
        if (request->lookup("If-Modified-Since", &h)) {
            Date cacheDate = scanDate(h);
            if (cacheDate) {
                if (fileStatus->lastModified() <= cacheDate->time()) {
                    response()->setStatus(304);
                    response()->beginTransmission();
                    response()->endTransmission();
                    return;
                }
            }
        }
    }

    response()->setHeader("Last-Modified", formatDate(Date{fileStatus->lastModified()}));

    if (fileStatus->type() == FileType::Directory) {
        String indexPath, indexName;
        const char *candidateNames[] = { "index.html", "index.htm" };
        for (int i = 0, n = sizeof(candidateNames) / sizeof(candidateNames[0]); i < n; ++i) {
            String candidateName = candidateNames[i];
            String candidatePath = path->extendPath(candidateName);
            if (File::exists(candidatePath)) {
                indexPath = candidatePath;
                indexName = candidateName;
                break;
            }
        }
        if (indexPath != "") {
            if (!request->uri()->endsWith('/')) {
                // NOTE: not all user agents (e.g. FireFox) support Content-Location correctly
                // therefore a redirection is the only clean way to make the user agent use a proper full path
                // in subsequent requests
                response()->setStatus(301);
                response()->setHeader("Location", request->uri() + "/");
                response()->beginTransmission();
                response()->write("Moved permanently.");
                response()->endTransmission();
                return;
            }

            deliverFile(indexPath);
        }
        else listDirectory(request, path);
    }
    else if (fileStatus->type() == FileType::Regular && fileStatus->size() < 0x10000) {
        deliverFile(path);
    }
    else {
        streamFile(path);
    }
}

void DirectoryDelegate::listDirectory(const HttpRequest *request, const String &path)
{
    Dir dir{path};

    response()->setHeader("Content-Type", "text/html");
    response()->chunk() <<
        "<!DOCTYPE html>\n"
        "<html>\n"
        "<head>\n"
        "<title>" << request->uri() << "</title>\n" <<
        "<style type=\"text/css\">\n"
        "a.file { float: left; width: 100pt; font-size: 10pt; padding: 10pt; overflow: hidden; text-overflow: ellipsis }\n"
        "</style>\n"
        "</head>\n"
        "<body>\n";

    String prefix = request->uri()->canonicalPath();
    if (prefix->count() > 0) {
        if (prefix->at(prefix->count() - 1) != '/')
            prefix += "/";
    }

    StringList entries;
    for (String name; dir->read(&name);) {
        if (name == "." || name == "..") continue;
        if ((!directoryInstance_->showHidden()) && name->startsWith('.')) continue;
        entries->append(name);
    }
    entries = entries->sort();

    for (const String &name: entries)
        response()->chunk() << "<a class=\"file\" href=\"" << prefix << name << "\">" << name << "</a>\n";

    response()->chunk() <<
        "</body>\n"
        "</html>\n";
}

void DirectoryDelegate::deliverFile(const String &path)
{
    String content = File{path}->map();
    String mediaType = deliveryInstance()->mediaTypeDatabase()->lookup(path, content);
    if (mediaType != "") response()->setHeader("Content-Type", mediaType);
    response()->beginTransmission(content->count());
    response()->write(content);
    response()->endTransmission();
}

void DirectoryDelegate::streamFile(const String &path)
{
    File file{path};
    String head;
    ssize_t size = -1;
    if (file->seekable()) {
        head = file->readSpan(64);
        size = file->seek(0, Seek::End);
        file->seek(0, Seek::Begin);
    }
    String mediaType = deliveryInstance()->mediaTypeDatabase()->lookup(path, head);
    if (mediaType != "") response()->setHeader("Content-Type", mediaType);
    response()->beginTransmission(size);
    String buf = String::allocate(0x10000);
    while (true) {
        int n = file->read(mutate(buf));
        if (n == 0) break;
        response()->write(buf->select(0, n));
    }
    response()->endTransmission();
}

}} // namespace cc::http
