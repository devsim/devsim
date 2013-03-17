/***
DEVSIM
Copyright 2013 Devsim LLC

This file is part of DEVSIM.

DEVSIM is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation, version 3.

DEVSIM is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with DEVSIM.  If not, see <http://www.gnu.org/licenses/>.
***/

#ifndef THREADBASECLASS_HH
#define THREADBASECLASS_HH
#include <cstddef>
class mymutex;
class mycondition;

/**
 * Base class for my thread.  It has a lot of functionality detailed
 * inline below.
 */
class threadBaseClass {
    public:
	/**
	 * Every thread has a mutex and condition variable associated with
	 * it.
	 */
	threadBaseClass(mymutex &m, mycondition &c) :
	    mutex(m),
	    cond(c),
            id(0)
	{
	}

	/// return the threads pointer converted to a size_t as an
	/// identifier
	size_t getThreadId();

	/// pure virtual destructor
	virtual ~threadBaseClass();

	/// Create the thread and call its run method
	void start();
	/// Gets the thread started
	virtual void run() = 0;

	/// Wait for the thread to exit.  Make sure the thread knows it is
	/// done.
	void join();
    protected:
	/// wait on condition variable
	void wait();
	/// lock the mutex
	void lock();
	/// unlock the mutex
	void unlock();
	/// signal on the condition variable
#if 0
	void signal();
	/// signal all waiting threads on the condition variable
	void broadcast();
#endif

    private:
	mymutex &mutex;
	mycondition  &cond;
	void *id;
};

/// Internal implementation detail.  Start up the thread and calls the run
/// method.
//void *threadFactory(void *);
#endif

