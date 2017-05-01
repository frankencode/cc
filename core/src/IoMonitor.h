/*
 * Copyright (C) 2007-2016 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

#include <cc/Array>
#include <cc/Map>
#include <cc/IoTarget>
#include <cc/SystemIo>

namespace cc {

/** \class IoEvent IoMonitor.h cc/IoMonitor
  * \brief I/O event
  */
class IoEvent: public Object
{
public:
    inline IoReady type() const { return type_; } ///< type of I/O event
    inline IoReady ready() const { return ready_; } ///< tells if ready for reading or writing when the event occured
    inline IoTarget *target() const { return target_; } ///< I/O target

private:
    friend class IoMonitor;

    inline static Ref<IoEvent> create(int index, IoReady type, IoTarget *target) {
        return new IoEvent(index, type, target);
    }

    IoEvent(int index, IoReady type, IoTarget *target):
        index_(index),
        type_(type),
        ready_(IoReadyUndefined),
        target_(target)
    {}

    int index_;
    IoReady type_;
    IoReady ready_;
    IoTarget *target_;
};

/// I/O activity vector
typedef Array<const IoEvent *> IoActivity;

/** \class IoMonitor IoMonitor.h cc/IoMonitor
  * \ingroup concurrency
  * \brief Register and wait for I/O events
  */
class IoMonitor: public Object
{
public:
    /** Create a new I/O monitor
      * \param maxCount maximum number of I/O events
      * \return new object instance
      */
    static Ref<IoMonitor> create(int maxCount = 0);

    /** Add a new I/O event to the watch list
      * \param type I/O event type
      * \param target I/O event target
      * \return pointer to newly added I/O event
      */
    const IoEvent *addEvent(IoReady type, IoTarget *target);

    /** Remove an I/O event from the watch list
      * \param event pointer to I/O event
      */
    void removeEvent(const IoEvent *event);

    /** Wait for I/O events
      * \param timeout_ms timeout in milliseconds (or infinite if < 0)
      * \return list of I/O events that occured before timed out
      */
    Ref<IoActivity> wait(int timeout_ms = -1);

    /** Wait for a specific I/O event
      * \param event I/O event to wait for
      * \param timeout_ms timeout in milliseconds (or infinite if < 0)
      * \return true if the given I/O event occured before timeout, false if timed out or a different I/O event occured
      */
    bool waitFor(const IoEvent *event, int timeout_ms = -1);

private:
    typedef struct pollfd PollFd;
    typedef Array<PollFd> Fds;
    typedef Map<int, Ref<IoEvent> > Events;

    IoMonitor(int maxCount);

    Ref<Fds> fds_;
    Ref<Events> events_;
};

} // namespace cc
