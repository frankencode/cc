/*
 * Copyright (C) 2007-2016 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

#include <signal.h>
#include <cc/Singleton>

namespace cc {

class Thread;

/** \class SignalSet SignalSet.h cc/SignalSet
  * \brief Signal set
  * \see Thread
  */
class SignalSet: public Object
{
public:
    inline static Ref<SignalSet> createEmpty() {
        return new SignalSet(Empty);
    }
    inline static Ref<SignalSet> createFull() {
        return new SignalSet(Full);
    }

    void insert(int signal);
    void remove(int signal);

    bool contains(int signal) const;

    inline sigset_t *rawSet() { return &rawSet_; }

private:
    friend class Thread;
    friend class Singleton<SignalSet>;

    inline static Ref<SignalSet> create() {
        return new SignalSet;
    }

    enum { Empty, Full, Query };

    SignalSet(int preset = Query);
    sigset_t rawSet_;
};

} // namespace cc
