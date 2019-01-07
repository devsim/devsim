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

#include "threadBaseClass.hh"
#include "mymutex.hh"
#include "mycondition.hh"
#include <cstdio>
#if _WIN32
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
#if _WIN32
//unsigned (__stdcall threadFactory)(ClientData x)
DWORD WINAPI threadFactory(LPVOID x)
#else
void *(threadFactory)(void *x)
#endif
{
    threadBaseClass *y = (reinterpret_cast<threadBaseClass *>(x));
    y->run();

#if _WIN32
	return 0;
#else
    pthread_exit(0);
#endif
}

#if 0

#ifdef _WIN32
unsigned (__stdcall testfoo)(ClientData x)
#else
void (testfoo)(ClientData x)
#endif
{
#ifdef _WIN32
	return 0;
#endif
}
#endif

threadBaseClass::~threadBaseClass()
{
}

size_t threadBaseClass::getThreadId()
{
#if _WIN32
    ThreadStuff *p = reinterpret_cast<ThreadStuff *>(id);
	return p->id;
#else
	return size_t(id);
#endif
}

void threadBaseClass::start()
{
#if _WIN32
	ThreadStuff *p = new ThreadStuff;
	id = p;
	p->hhandle = CreateThread(
            nullptr,                   // default security attributes
            0,                      // use default stack size
            threadFactory,       // thread function name
            this,          // argument to thread function
            0,                      // use default creation flags
			&(p->id));   // returns the thread identifier
#else
  int ret=pthread_create(reinterpret_cast<pthread_t *>(&id), nullptr, threadFactory, this);
  if (ret != 0)
  {
      perror("threadBaseClass::start()");
  }
#endif
}

void threadBaseClass::join()
{
#if _WIN32
  ThreadStuff *p = reinterpret_cast<ThreadStuff *>(id);
  WaitForSingleObject(p->hhandle, INFINITE);
#else
  int ret = pthread_join(reinterpret_cast<pthread_t >(id), nullptr);
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

