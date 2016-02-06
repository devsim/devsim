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

#ifndef MYPACKET_HH
#define MYPACKET_HH
#include <memory>
class mypacket;
typedef std::shared_ptr<mypacket> mypacket_ptr;

/**
 * This class is the basis for any work to be queued by
 * the myworker class.
 *
 * Any derived class must provide a run method to be executed.
 * See vectorpacket for an implementation where data is actually provided.
 */
class mypacket{
    public:
	virtual ~mypacket() = 0;
	virtual void run() = 0;
};

#endif
