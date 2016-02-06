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

#ifndef MYMUTEX_HH
#define MYMUTEX_HH
class mymutex {
    public:
	mymutex();
	~mymutex();
	/// wait until mutex available, then acquire it
	void lock();
	/// release mutex for others
	void unlock();

	friend class mycondition;
    private:
	void *mutex;
};
#endif

