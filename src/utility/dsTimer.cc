/***
DEVSIM
Copyright 2013 DEVSIM LLC

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

#include <sstream>

dsTimer::dsTimer(const std::string &msg, OutputStream::OutputType outtype) : msg_(msg), output_type_(outtype), tic_(std::chrono::system_clock::now())
{
  std::ostringstream os;
  os << "\nBEGIN " << msg_ << "\n";
  OutputStream::WriteOut(output_type_, os.str());
}

dsTimer::~dsTimer()
{
  auto toc = std::chrono::system_clock::now();

  typedef std::chrono::duration<double> seconds;

  auto timediff = std::chrono::duration_cast<seconds>(toc - tic_).count();

  try
  {
    std::ostringstream os;
    os << "\nEND " << msg_ << " (" << timediff << " sec)\n";
    OutputStream::WriteOut(output_type_, os.str());
  }
  catch(...)
  {
  }
}

