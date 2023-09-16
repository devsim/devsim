/***
DEVSIM
Copyright 2023 DEVSIM LLC

SPDX-License-Identifier: Apache-2.0
***/

#include "ControlGIL.hh"
#include "Python.h"

void * MasterGILControl::thread_state_ = nullptr;

MasterGILControl::MasterGILControl()
{
    allow_threading();
}

MasterGILControl::~MasterGILControl()
{
    end_allow_threading();
}

void MasterGILControl::allow_threading()
{
    if (!thread_state_)
    {
        thread_state_ = reinterpret_cast<void *>(PyEval_SaveThread());
        owned = true;
    }
    else
    {
        owned = false;
    }
}

void MasterGILControl::end_allow_threading()
{
    if (owned && thread_state_)
    {
        PyEval_RestoreThread(reinterpret_cast<PyThreadState *>(thread_state_));
        thread_state_ = nullptr;
    }
}

EnsurePythonGIL::EnsurePythonGIL()
{
    scoped_state_ = PyGILState_Ensure();
}

EnsurePythonGIL::~EnsurePythonGIL()
{
    PyGILState_Release(static_cast<PyGILState_STATE>(scoped_state_));
}




