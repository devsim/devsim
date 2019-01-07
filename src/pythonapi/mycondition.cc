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
#ifdef _WIN32
#include "Windows.h"
#else
#include <errno.h>
#include <pthread.h>
#endif

// TODO: "Ensure processes acquire python lock when necessary."
mycondition::mycondition() : cond(nullptr)
{
#if _WIN32
	cond = new CONDITION_VARIABLE;
	InitializeConditionVariable (reinterpret_cast<CONDITION_VARIABLE *>(cond));
#else
    cond = new pthread_cond_t;
    // Need to define appropriate attributes
    int ret = pthread_cond_init(reinterpret_cast<pthread_cond_t *>(cond), nullptr);
    if (ret != 0)
    {
	perror("mycondition::mycondition()");
    }
#endif
}

mycondition::~mycondition()
{
#if _WIN32
	///// the API doesn't specify
#else
    pthread_cond_t *p = reinterpret_cast<pthread_cond_t *>(cond);
    int ret = pthread_cond_destroy(p);
    delete p;
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
#if _WIN32
    WakeAllConditionVariable(reinterpret_cast<CONDITION_VARIABLE *>(cond));
#else
    int ret = pthread_cond_broadcast(reinterpret_cast<pthread_cond_t *>(cond));
//    assert (ret == 0);
    if (ret != 0)
    {
	perror("mycondition::broadcast()");
    }
#endif
}


void mycondition::wait(mymutex &mutex)
{
#if _WIN32
  SleepConditionVariableCS(reinterpret_cast<CONDITION_VARIABLE *>(cond), reinterpret_cast<CRITICAL_SECTION *>(mutex.mutex), INFINITE);
#else
  pthread_cond_wait(reinterpret_cast<pthread_cond_t *>(cond), reinterpret_cast<pthread_mutex_t *>(mutex.mutex));
#endif
}

