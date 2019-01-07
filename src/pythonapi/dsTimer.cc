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

#ifdef _WIN32
#include <time.h>
#include <Windows.h>
#else
#include <sys/time.h>
#endif
#include <sstream>

#ifdef _WIN32
dsTimer::dsTimer(const std::string &msg, OutputStream::OutputType outtype) : msg_(msg), output_type_(outtype), tic_(new FILETIME)
{
  //// Timer is initialized to now in its constructor
  GetSystemTimeAsFileTime(reinterpret_cast<FILETIME *>(tic_));

  std::ostringstream os;
  os << "\nBEGIN " << msg_ << "\n";
  OutputStream::WriteOut(output_type_, os.str());
}

dsTimer::~dsTimer()
{
  FILETIME toc;
  GetSystemTimeAsFileTime(reinterpret_cast<FILETIME *>(&toc));

  const FILETIME &tic = *(reinterpret_cast<FILETIME *>(tic_));
  //// This is in 1000 nanosecond increments
  unsigned __int64 t0 = ((static_cast<__int64>(tic.dwHighDateTime) << 32) + (tic.dwLowDateTime));
  unsigned __int64 t1 = ((static_cast<__int64>(toc.dwHighDateTime) << 32) + (tic.dwLowDateTime));
  double timediff = 1.0e-7 * (t1- t0);

  std::ostringstream os;
  os << "\nEND " << msg_ << " (" << timediff << ")\n";
  OutputStream::WriteOut(output_type_, os.str());
  delete reinterpret_cast<FILETIME *>(tic_);
}
#else
dsTimer::dsTimer(const std::string &msg, OutputStream::OutputType outtype) : msg_(msg), output_type_(outtype), tic_(new timeval)
{
  //// Timer is initialized to now in its constructor
  gettimeofday(reinterpret_cast<timeval *>(tic_), nullptr);

  std::ostringstream os;
  os << "\nBEGIN " << msg_ << "\n";
  OutputStream::WriteOut(output_type_, os.str());
}

dsTimer::~dsTimer()
{
  timeval toc;
  gettimeofday(&toc, nullptr);

  const timeval &tic = *(reinterpret_cast<timeval *>(tic_));

  double timediff = toc.tv_sec - tic.tv_sec + 1e-6 * (toc.tv_usec - tic.tv_usec);

  std::ostringstream os;
  os << "\nEND " << msg_ << " (" << timediff << ")\n";
  OutputStream::WriteOut(output_type_, os.str());
  delete reinterpret_cast<timeval *>(tic_);
}
#endif
