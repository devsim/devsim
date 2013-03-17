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
        OutputStream::WriteOut(OutputStream::FATAL, os.str().c_str());
    }
}


