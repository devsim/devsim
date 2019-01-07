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


#include "mymutex.hh"

#include <cassert>
#include <cstdio>
#include <errno.h>
#include "tcl.h"

mymutex::mymutex() : mutex(0)
{
#if 0
  char *add = reinterpret_cast<char *>(&mutex);
  const size_t len = sizeof(Tcl_Mutex);
  for (size_t i = 0; i < len; ++i)
  {
    add[i] = 0;
  }
#endif
#if 0
    // create mutex
    // need to no the defaults for second argument
    int ret = pthread_mutex_init(&mutex, nullptr);
    if (ret != 0)
    {
	perror("mymutex::mymutex()");
    }
//    assert(ret == 0);
#endif
}

mymutex::~mymutex()
{
  void **address = &mutex;
  Tcl_MutexFinalize(reinterpret_cast<Tcl_Mutex *>(address));
#if 0
    // delete mutex
    int ret = pthread_mutex_destroy(&mutex);
    if (ret != 0)
    {
	perror("mymutex::~mymutex()");
    }
//    assert (ret == 0);
#endif
}

void mymutex::lock()
{
  void **address = &mutex;
  Tcl_MutexLock(reinterpret_cast<Tcl_Mutex *>(address));
#if 0
    // lock it
    int ret = pthread_mutex_lock(&mutex);
//    assert (ret == 0);
    if (ret != 0)
    {
	perror("mymutex::lock()");
    }
#endif
}

void mymutex::unlock()
{
  void **address = &mutex;
  Tcl_MutexUnlock(reinterpret_cast<Tcl_Mutex *>(address));
#if 0
    // lock it
    int ret = pthread_mutex_unlock(&mutex);
//    assert (ret == 0);
    if (ret != 0)
    {
	perror("mymutex:unlock()");
    }
#endif
}

#if 0
void mymutex::wait(mycondition &cond)
{
    int ret = pthread_cond_wait(&cond.cond, &mutex);
//    assert (ret == 0);
    if (ret != 0)
    {
	perror("mymutex:wait()");
    }
}
#endif

