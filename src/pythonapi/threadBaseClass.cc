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

#include "threadBaseClass.hh"
#include "mymutex.hh"
#include "mycondition.hh"
#include <cstdio>
#if WIN32
#include <Windows.h>
struct ThreadStuff{
  HANDLE hhandle;
  DWORD  id;
};
#else
#include <errno.h>
#include <pthread.h>
#endif

/**
 * the typename is the type of the input data
 */
#if WIN32
//unsigned (__stdcall threadFactory)(ClientData x)
DWORD WINAPI threadFactory(LPVOID x)
#else
void *(threadFactory)(void *x)
#endif
{
    threadBaseClass *y = (reinterpret_cast<threadBaseClass *>(x));
    y->run();

#if WIN32
	return 0;
#else
    pthread_exit(0);
#endif
}

#if 0

#ifdef __WIN32__
unsigned (__stdcall testfoo)(ClientData x)
#else
void (testfoo)(ClientData x)
#endif
{
#ifdef __WIN32__
	return 0;
#endif
}
#endif

threadBaseClass::~threadBaseClass()
{
}

size_t threadBaseClass::getThreadId()
{
#if WIN32
    ThreadStuff *p = reinterpret_cast<ThreadStuff *>(id);
	return p->id;
#else
	return size_t(id);
#endif
}

void threadBaseClass::start()
{
//#warning "fix all these up so they check errors
#if WIN32
	ThreadStuff *p = new ThreadStuff;
	id = p;
	p->hhandle = CreateThread( 
            NULL,                   // default security attributes
            0,                      // use default stack size  
            threadFactory,       // thread function name
            this,          // argument to thread function 
            0,                      // use default creation flags 
			&(p->id));   // returns the thread identifier 
#else
  int ret=pthread_create(reinterpret_cast<pthread_t *>(&id), NULL, threadFactory, this);
  if (ret != 0)
  {
      perror("threadBaseClass::start()");
  }
#endif
}

void threadBaseClass::join()
{
#if WIN32
  ThreadStuff *p = reinterpret_cast<ThreadStuff *>(id);
  WaitForSingleObject(p->hhandle, INFINITE);
#else
  int ret = pthread_join(reinterpret_cast<pthread_t >(id), NULL);
  if (ret != 0)
  {
      perror("threadBaseClass::join()");
  }
#endif
}

void threadBaseClass::wait()
{
    cond.wait(mutex);
}

void threadBaseClass::lock()
{
    mutex.lock();
}

#if 0
void threadBaseClass::signal()
{
    cond.signal();
}
#endif

void threadBaseClass::unlock()
{
    mutex.unlock();
}

