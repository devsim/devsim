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

#ifndef MYWORKER_HH
#define MYWORKER_HH
#include "threadBaseClass.hh"
#include "mypacket.hh"
#include <list>
/**
 * Class which creates worker threads.  These threads share the same queue
 * of mypacket's to be processed.
 * The size_t &threadExit must be protected by the mutex.
 * When threadExit == 1, it is time to terminate the thread.
 * Calling function would:
 * 1. Lock the mutex
 * 2. Set the threadExit = 1
 * 3. Broadcast the condition variable
 */
class myworker : public threadBaseClass {
    public:
	myworker(mymutex &, mycondition &, std::list<mypacket_ptr> &, size_t &);
	~myworker();
	void run();
    private:
	std::list<mypacket_ptr>    &queue;
	size_t &threadExit;
};
#endif
