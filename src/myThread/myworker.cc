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
