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

#include "dsTimer.hh"
#include "tcl.h"
#include <sstream>

dsTimer::dsTimer(const std::string &msg, OutputStream::OutputType outtype) : msg_(msg), output_type_(outtype), tic_(new Tcl_Time)
{
  //// Timer is initialized to now in its constructor
  Tcl_GetTime(reinterpret_cast<Tcl_Time *>(tic_));

  std::ostringstream os;
  os << "\nBEGIN " << msg_ << "\n";
  OutputStream::WriteOut(output_type_, os.str());
}

dsTimer::~dsTimer()
{
  Tcl_Time toc;
  Tcl_GetTime(&toc);

  const Tcl_Time &tic = *(reinterpret_cast<Tcl_Time *>(tic_));

  double timediff = toc.sec - tic.sec + 1e-6 * (toc.usec - tic.usec);

  std::ostringstream os;
  os << "\nEND " << msg_ << " (" << timediff << ")\n";
  OutputStream::WriteOut(output_type_, os.str());
  delete reinterpret_cast<Tcl_Time *>(tic_);
}

