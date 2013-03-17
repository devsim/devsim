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

#include "mycondition.hh"
#include "mymutex.hh"
#include <cassert>
#include <cstdio>
#include <errno.h>
#include "tcl.h"
mycondition::mycondition() : cond(NULL)
{
#if 0
    // Need to define appropriate attributes
    int ret = pthread_cond_init(&cond, NULL);
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
  Tcl_ConditionWait(reinterpret_cast<Tcl_Condition *>(cond_address), reinterpret_cast<Tcl_Mutex *>(mutex_address), NULL);
}

