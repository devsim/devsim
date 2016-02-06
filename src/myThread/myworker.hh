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
