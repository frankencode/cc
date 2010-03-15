/*
 * Channel.hpp -- generic M to N thread communication and synchronisation media
 *
 * Copyright (c) 2007-2010, Frank Mertens
 *
 * See ../LICENSE for the license.
 */
#ifndef PONA_CHANNEL_HPP
#define PONA_CHANNEL_HPP

#include "atoms"
#include "Queue.hpp"
#include "Mutex.hpp"
#include "Condition.hpp"

namespace pona
{

template<class T, template<class T> class Queue = pona::Queue>
class Channel: public Instance, public NonCopyable
{
public:
	Channel(int size = 0)
		: queue_(size + (size == 0)),
		  ackMode_(size == 0)
	{}
	
	Channel& push(const T& item)
	{
		queueMutex_.acquire();
		while (queue_.fill() == queue_.size())
			notFull_.wait(&queueMutex_);
		queue_ << item;
		if (ackMode_) {
			ackMutex_.acquire();
			ack_.signal();
			notEmpty_.signal();
			queueMutex_.release();
			ack_.wait(&ackMutex_);
			ackMutex_.release();
		}
		else {
			notEmpty_.signal();
			queueMutex_.release();
		}
		return *this;
	}
	
	Channel& pop(T& item)
	{
		queueMutex_.acquire();
		while (queue_.fill() == 0)
			notEmpty_.wait(&queueMutex_);
		if (ackMode_) {
			ackMutex_.acquire();
			ack_.signal();
			ackMutex_.release();
		}
		queue_ >> item;
		notFull_.signal();
		queueMutex_.release();
		return *this;
	}
	
	inline T pop() {
		T item;
		pop(item);
		return item;
	}
	
	inline Channel& operator<<(const T& item) { return push(item); }
	inline Channel& operator>>(T& item) { return pop(item); }
	
	template<template<class> class CB>
	inline Channel& operator<<(CB<T>& cb) {
		while (!cb.empty()) {
			T item;
			cb >> item;
			*this << item;
		}
		return *this;
	}
	
private:
	Queue<T> queue_;
	Mutex queueMutex_;
	Condition notEmpty_, notFull_;
	bool ackMode_;
	Mutex ackMutex_;
	Condition ack_;
};

} // namespace pona

#endif // PONA_CHANNEL_HPP
