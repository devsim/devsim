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
#if _WIN32
#include <Windows.h>
#else
#include <errno.h>
#include <pthread.h>
#endif

mymutex::mymutex() : mutex(nullptr)
{
#if _WIN32
    mutex = new CRITICAL_SECTION;
	InitializeCriticalSection(reinterpret_cast<CRITICAL_SECTION *>(mutex));
#else
	// create mutex
    // need to no the defaults for second argument
    mutex = new pthread_mutex_t;
    int ret = pthread_mutex_init(reinterpret_cast<pthread_mutex_t *>(mutex), nullptr);
    if (ret != 0)
    {
	perror("mymutex::mymutex()");
    }
#endif
}

mymutex::~mymutex()
{
#if _WIN32
	DeleteCriticalSection(reinterpret_cast<CRITICAL_SECTION *>(mutex));
	delete reinterpret_cast<CRITICAL_SECTION *>(mutex);
#else
    // delete mutex
    pthread_mutex_t *p = reinterpret_cast<pthread_mutex_t *>(mutex);
    int ret = pthread_mutex_destroy(p);
    delete p;
    if (ret != 0)
    {
	perror("mymutex::~mymutex()");
    }
#endif
}

void mymutex::lock()
{
#if _WIN32
    EnterCriticalSection(reinterpret_cast<CRITICAL_SECTION *>(mutex));
#else
    // lock it
    int ret = pthread_mutex_lock(reinterpret_cast<pthread_mutex_t *>(mutex));
//    assert (ret == 0);
    if (ret != 0)
    {
	perror("mymutex::lock()");
    }
#endif
}

void mymutex::unlock()
{
#if _WIN32
  LeaveCriticalSection(reinterpret_cast<CRITICAL_SECTION *>(mutex));
#else
  // lock it
  int ret = pthread_mutex_unlock(reinterpret_cast<pthread_mutex_t *>(mutex));
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
    int ret = pthread_cond_wait(&cond.cond, reinterpret_cast<pthread_mutex_t *>(&mutex));
//    assert (ret == 0);
    if (ret != 0)
    {
	perror("mymutex:wait()");
    }
}
#endif

