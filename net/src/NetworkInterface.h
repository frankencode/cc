/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

#include <sys/types.h>
#include <sys/socket.h>
#include <net/if.h> // IFF_UP, etc.
#include <cc/net/SocketAddress>
#include <cc/net/SocketAddressEntry>

namespace cc {
namespace net {

class RouteInfo;
class NetworkInterface;

typedef List< Ref<NetworkInterface> > NetworkInterfaceList;

/** \class NetworkInterface NetworkInterface.h cc/net/NetworkInterface
  * \brief Query network interface configuration: addresses, flags, etc.
  */
class NetworkInterface: public Object
{
public:
    /** Network interface flags
      */
    enum Flags {
        Up           = IFF_UP,        ///< Network interface is in up state
        #ifdef IFF_RUNNING
        Running      = IFF_RUNNING,
        #else
        Running      = 0,
        #endif                         ///< Network interface is up and running
        #ifdef IFF_LOWER_UP
        LowerUp      = IFF_LOWER_UP,
        #else
        LowerUp      = 0,
        #endif                         ///< Network interface is in lower-up state
        #ifdef IFF_DORMANT
        Dormant      = IFF_DORMANT,
        #else
        Dormant      = 0,
        #endif                         ///< Network interface is waiting for an 802.1X event
        Broadcast    = IFF_BROADCAST,  ///< Valid broadcast is set
        Multicast    = IFF_MULTICAST,  ///< Multicasting is supported
        Loopback     = IFF_LOOPBACK,   ///< Network interface is a loopback interface
        PointToPoint = IFF_POINTOPOINT ///< Network interface is a point-to-point link
    };

    /** Create an invalid network interface
      * \return new object instance
      */
    static Ref<NetworkInterface> create();


    /** Query information about a certain network interface
      * \param name system name of the interface
      * \param family limit the addressList() to a certain protocol family (e.g. AF_INET)
      * \return new object instance
      */
    static Ref<NetworkInterface> query(String name, int family = AF_UNSPEC);

    /** Query information about all available network interfaces
      * \param family limit the addressList() to a certain protocol family (e.g. AF_INET)
      * \return new object instance
      */
    static Ref<NetworkInterfaceList> queryAll(int family = AF_UNSPEC);

    /// Returns true if this object provides information about a valid network interface
    bool isValid() const { return name_ != ""; }

    /// %System name of the network interface
    String name() const { return name_; }

    /// %System index of the network interface
    int index() const { return index_; }

    /// Network interface face type
    unsigned type() const { return type_; }

    /// Network interface flags (a combination of Flags)
    unsigned flags() const { return flags_; }

    /// Hardware address
    const ByteArray *hardwareAddress() const { return hardwareAddress_; }

    /// Message transfer unit (MTU)
    uint32_t mtu() const { return mtu_; }

    /// Snapshot of the networking addresses associated to the interface
    const SocketAddressList *addressList() const { return addressList_; }

private:
    NetworkInterface();

    #ifdef __linux
    friend class RouteInfo;
    static Ref<NetworkInterface> getLink(NetworkInterfaceList *list = 0, int index = -1);
    inline static Ref<NetworkInterface> getLink(int index) { return getLink(0, index); }
    static Ref<NetworkInterfaceList> queryAllIoctl(int family);
    #endif

    String name_;
    int index_;
    unsigned type_;
    unsigned flags_;
    Ref<const ByteArray> hardwareAddress_;
    uint32_t mtu_;
    Ref<SocketAddressList> addressList_;
};

}} // namespace cc::net
