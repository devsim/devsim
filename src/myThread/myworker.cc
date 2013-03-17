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

#include "myworker.hh"
#include "mymutex.hh"
#include "mycondition.hh"

/// need to avoid slicing problem by using pointer to packet
myworker::myworker(mymutex &m, mycondition &c, std::list<mypacket_ptr> &q, size_t &threadexit)
    : threadBaseClass(m, c), queue(q), threadExit(threadexit)
{}

myworker::~myworker()
{}

void myworker::run()
{
    // This is the reference to my packet
    mypacket_ptr x;
    // keep looping forever
    while (true)
    {
	this->lock();
	while (queue.empty())
	{
	    if (threadExit == 1)
	    {
		this->unlock();
		return;
	    }
	    this->wait();
	}
	x = queue.front();
	queue.erase(queue.begin());
	this->unlock();
	// This will run
	// Somehow it must signal it is done to the owner
	x->run();
//	std::cout << getThreadId() << std::endl;
    }
}
