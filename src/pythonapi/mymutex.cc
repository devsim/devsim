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


#include "mymutex.hh"

#include <cassert>
#include <cstdio>
#if _WIN32
#include <Windows.h>
#else
#include <errno.h>
#include <pthread.h>
#endif

mymutex::mymutex() : mutex(NULL)
{
#if _WIN32
    mutex = new CRITICAL_SECTION;
	InitializeCriticalSection(reinterpret_cast<CRITICAL_SECTION *>(mutex));
#else
	// create mutex
    // need to no the defaults for second argument
    mutex = new pthread_mutex_t;
    int ret = pthread_mutex_init(reinterpret_cast<pthread_mutex_t *>(mutex), NULL);
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

