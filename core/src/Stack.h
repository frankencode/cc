/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

#include <cc/containers>

namespace cc {

/** \class Stack Stack.h cc/Stack
  * \ingroup container
  * \brief %Stack data container
  */
template<class T>
class Stack: public Object
{
public:
    /// Item type
    typedef T Item;

    /** Create a new stack
      * \param capacity max number of items storable in the stack
      * \return new object instance
      */
    inline static Ref<Stack> create(int capacity) {
        return new Stack(capacity);
    }

    /// Maximum number of items storable in the stack
    inline int capacity() const { return size_; }

    /// Current number of items stored in the stack
    inline int count() const { return fill_; }

    /** Check if index is within range
      * \param i item index
      * \return true if i is a valid index
      */
    inline bool has(int i) const { return (0 <= i) && (i < fill_); }

    /** Access item at index i (readonly)
      * \param i item index
      * \return low-level reference
      */
    inline const T &at(int i) const { return bottom(i); }


    /** Access item at index i
      * \param i item index
      * \return low-level reference
      */
    inline T &at(int i) { return bottom(i); }

    /** Put a new item onto the stack
      * \param item item value
      */
    inline void push(const T &item) {
        CC_ASSERT(fill_ < size_);
        buffer_[fill_++] = item;
    }

    /** Remove an item from the top of the stack
      * \param item return item value
      */
    inline void pop(T *item) {
        CC_ASSERT(fill_ > 0);
        *item = buffer_[--fill_];
    }

    /** Remove an item from the top of the stack
      * \return item value
      */
    inline T pop() {
        T item;
        pop(&item);
        return item;
    }

    /** Remove n items from the top of the stack
      * \return low-level reference
      */
    inline Stack &popMore(int n) {
        CC_ASSERT(fill_ >= n);
        fill_ -= n;
        return *this;
    }

    /// Reset the stack to an empty state
    inline void deplete() { fill_ = 0; }

    /** Access the top of the stack (readonly)
      * \param i item index
      * \return low-level reference
      */
    inline const T &top(int i = 0) const {
        CC_ASSERT(i < fill_);
        return buffer_[fill_-i-1];
    }

    /** Access the top of the stack
      * \param i item index
      * \return low-level reference
      */
    inline T &top(int i = 0) {
        CC_ASSERT(i < fill_);
        return buffer_[fill_-i-1];
    }

    /** Access the bottom of the stack (readonly)
      * \param i item index
      * \return low-level reference
      */
    inline const T &bottom(int i = 0) const {
        CC_ASSERT(i < fill_);
        return buffer_[i];
    }

    /** Access the bottom of the stack
      * \param i item index
      * \return low-level reference
      */
    inline T &bottom(int i = 0) {
        CC_ASSERT(i < fill_);
        return buffer_[i];
    }

private:
    Stack(int size):
        fill_(0),
        size_(size),
        buffer_(new T[size])
    {}

    ~Stack()
    {
        delete[] buffer_;
    }

    int fill_;
    int size_;
    T *buffer_;
};

} // namespace cc
