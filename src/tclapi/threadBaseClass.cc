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
#include "tcl.h"
#include <cstdio>
#include <errno.h>

/**
 * the typename is the type of the input data
 */
#ifdef _WIN32
unsigned (__stdcall threadFactory)(ClientData x)
#else
void (threadFactory)(ClientData x)
#endif
{
    threadBaseClass *y = (static_cast<threadBaseClass *>(x));
    y->run();
    Tcl_ExitThread(0);
#ifdef _WIN32
	return 0;
#endif
}

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

threadBaseClass::~threadBaseClass()
{
}

size_t threadBaseClass::getThreadId()
{
    return size_t(id);
}

void threadBaseClass::start()
{
#if 0
  Tcl_ThreadId testid;
  Tcl_CreateThread(&testid, testfoo, (ClientData) nullptr, TCL_THREAD_STACK_DEFAULT, TCL_THREAD_NOFLAGS);
#else
  void **address = &id;
  Tcl_CreateThread(reinterpret_cast<Tcl_ThreadId *>(address), threadFactory, (ClientData) this, TCL_THREAD_STACK_DEFAULT, TCL_THREAD_JOINABLE);
#endif
  //    assert(id == nullptr);
#if 0
    int ret=pthread_create(&id, nullptr, threadFactory, this);
    if (ret != 0)
    {
	perror("threadBaseClass::start()");
    }
#endif
}

void threadBaseClass::join()
{
  int result;
  Tcl_JoinThread(reinterpret_cast<Tcl_ThreadId>(id), &result);
//    assert(id == nullptr);
#if 0
    int ret = pthread_join(id, nullptr);
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

