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

