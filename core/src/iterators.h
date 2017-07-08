/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

#include <iterator>

namespace cc {

template<class Container>
class ConstIterator:
    public std::iterator<
        /*iterator_category*/ std::bidirectional_iterator_tag,
        /*value_type*/        typename Container::Item,
        /*difference_type*/   int,
        /*pointer*/           void,
        /*reference*/         typename Container::Item
    >
{
public:
    ConstIterator(const Container *c, int i):
        c_(c),
        i_(i)
    {}

    inline typename Container::Item operator*() const { return c_->at(i_); }
    inline ConstIterator &operator++() { ++i_; return *this; }
    inline ConstIterator &operator--() { --i_; return *this; }
    inline ConstIterator operator+(int delta) { return ConstIterator(c_, i_ + delta); }
    inline ConstIterator operator-(int delta) { return ConstIterator(c_, i_ - delta); }
    inline int operator-(const ConstIterator &b) const { return i_ - b.i_; }
    inline int operator<(const ConstIterator &b) const { return i_ < b.i_; }
    inline bool operator==(const ConstIterator &b) const { return i_ == b.i_; }
    inline bool operator!=(const ConstIterator &b) const { return i_ != b.i_; }

private:
    const Container *c_;
    int i_;
};

template<class Container>
class Iterator:
    public std::iterator<
        /*iterator_category*/ std::bidirectional_iterator_tag,
        /*value_type*/        typename Container::Item,
        /*difference_type*/   int,
        /*pointer*/           typename Container::Item *,
        /*reference*/         typename Container::Item &
    >
{
public:
    Iterator(Container *c, int i):
        c_(c),
        i_(i)
    {}

    inline typename Container::Item &operator*() const { return c_->at(i_); }
    inline typename Container::Item *operator->() const { return &c_->at(i_); }
    inline Iterator &operator++() { ++i_; return *this; }
    inline Iterator &operator--() { --i_; return *this; }
    inline Iterator operator+(int delta) { return Iterator(c_, i_ + delta); }
    inline Iterator operator-(int delta) { return Iterator(c_, i_ - delta); }
    inline int operator-(const Iterator &b) const { return i_ - b.i_; }
    inline int operator<(const Iterator &b) const { return i_ < b.i_; }
    inline bool operator==(const Iterator &b) const { return i_ == b.i_; }
    inline bool operator!=(const Iterator &b) const { return i_ != b.i_; }

    operator ConstIterator<Container>() const { return ConstIterator<Container>(c_, i_); }

private:
    Container *c_;
    int i_;
};

template<class Container>
inline typename Container::const_iterator begin(const Container *c) { return c->begin(); }

template<class Container>
inline typename Container::const_iterator end(const Container *c) { return c->end(); }

template<class Container>
inline typename Container::iterator begin(Container *c) { return c->begin(); }

template<class Container>
inline typename Container::iterator end(Container *c) { return c->end(); }

template<class Container>
inline typename Container::const_iterator begin(const Ref<const Container> &c) { return c->begin(); }

template<class Container>
inline typename Container::const_iterator end(const Ref<const Container> &c) { return c->end(); }

template<class Container>
inline typename Container::iterator begin(Ref<Container> c) { return c->begin(); }

template<class Container>
inline typename Container::iterator end(Ref<Container> c) { return c->end(); }

} // namespace cc
