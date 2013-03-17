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
#include "tcl.h"
#include <cstdio>
#include <errno.h>

/**
 * the typename is the type of the input data
 */
#ifdef __WIN32__
unsigned (__stdcall threadFactory)(ClientData x)
#else
void (threadFactory)(ClientData x)
#endif
{
    threadBaseClass *y = (static_cast<threadBaseClass *>(x));
    y->run();
    Tcl_ExitThread(0);
#ifdef __WIN32__
	return 0;
#endif
}

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

threadBaseClass::~threadBaseClass()
{
}

size_t threadBaseClass::getThreadId()
{
    return size_t(id);
}

void threadBaseClass::start()
{
//#warning "fix all these up so they check errors

#if 0
  Tcl_ThreadId testid;
  Tcl_CreateThread(&testid, testfoo, (ClientData) NULL, TCL_THREAD_STACK_DEFAULT, TCL_THREAD_NOFLAGS);
#else
  void **address = &id;
  Tcl_CreateThread(reinterpret_cast<Tcl_ThreadId *>(address), threadFactory, (ClientData) this, TCL_THREAD_STACK_DEFAULT, TCL_THREAD_JOINABLE);
#endif
  //    assert(id == NULL);
#if 0
    int ret=pthread_create(&id, NULL, threadFactory, this);
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
//    assert(id == NULL);
#if 0
    int ret = pthread_join(id, NULL);
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

