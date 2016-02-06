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

#ifndef MYCONDITION_HH
#define MYCONDITION_HH
class mymutex;
/**
 * Creates a condition variable.  This condition variable is passed by
 * reference to all classes who require it.  Therefore, it must not go out of
 * scope until the last thread requiring it is finished.
 */
class mycondition {
    public:
	mycondition();
	~mycondition();
#if 0
	/// signal only one of the threads blocking on this variable
	void signal();
#endif
	/// signal on all of the threads blocking on this variable
	void broadcast();

	void wait(mymutex &);
    private:

	/// friendship is needed since we can tell a mutex to lock on its
	/// associated condition variable
	void *cond;
};

#endif
