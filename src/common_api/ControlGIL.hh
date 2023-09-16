/***
DEVSIM
Copyright 2023 DEVSIM LLC

SPDX-License-Identifier: Apache-2.0
***/

#ifndef CONTROL_GIL_HH
#define CONTROL_GIL_HH

// calls to this class may not be nested
class MasterGILControl
{
    public:
        MasterGILControl();
        ~MasterGILControl();
        void allow_threading();
        void end_allow_threading();

    private:
        bool owned = false;
        static void *thread_state_;
};

class EnsurePythonGIL
{
    public:
        EnsurePythonGIL();
        ~EnsurePythonGIL();
    private:
        int scoped_state_;
};

#endif
