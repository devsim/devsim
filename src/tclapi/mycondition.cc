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

#include "mycondition.hh"
#include "mymutex.hh"
#include <cassert>
#include <cstdio>
#include <errno.h>
#include "tcl.h"
mycondition::mycondition() : cond(nullptr)
{
#if 0
    // Need to define appropriate attributes
    int ret = pthread_cond_init(&cond, nullptr);
    if (ret != 0)
    {
	perror("mycondition::mycondition()");
    }
#endif
}

mycondition::~mycondition()
{
  void **address = &cond;
  Tcl_ConditionFinalize(reinterpret_cast<Tcl_Condition *>(address));
#if 0
    int ret = pthread_cond_destroy(&cond);
//    assert (ret == 0);
    if (ret != 0)
    {
	perror("mycondition::~mycondition()");
    }
#endif
}

#if 0
void mycondition::signal()
{
//// Doesn't really match up
  Tcl_ConditionNotify(cond);
#if 0
    int ret = pthread_cond_signal(&cond);
//    assert (ret == 0);
    if (ret != 0)
    {
	perror("mycondition::signal()");
    }
#endif
}
#endif

void mycondition::broadcast()
{
  void **address = &cond;
  Tcl_ConditionNotify(reinterpret_cast<Tcl_Condition *>(address));
#if 0
    int ret = pthread_cond_broadcast(&cond);
//    assert (ret == 0);
    if (ret != 0)
    {
	perror("mycondition::broadcast()");
    }
#endif
}


void mycondition::wait(mymutex &mutex)
{
  void **cond_address = &cond;
  void **mutex_address = &(mutex.mutex);
  Tcl_ConditionWait(reinterpret_cast<Tcl_Condition *>(cond_address), reinterpret_cast<Tcl_Mutex *>(mutex_address), nullptr);
}

