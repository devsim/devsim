/***
DEVSIM
Copyright 2013 Devsim LLC

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
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

