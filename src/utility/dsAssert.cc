/***
DEVSIM
Copyright 2013 DEVSIM LLC

SPDX-License-Identifier: Apache-2.0
***/

#include "dsAssert.hh"
#include "OutputStream.hh"
#include <string>
#include <sstream>

void dsAssert_(bool x, const std::string &msg)
{
    if (!x)
    {
        std::ostringstream os;
        os << "There was a fatal exception in the program.  Shutting down.  Please inspect program output.\n";
        os << msg << "\n";
        OutputStream::WriteOut(OutputStream::OutputType::FATAL, os.str().c_str());
    }
}


