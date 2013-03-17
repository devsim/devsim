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

#ifdef WIN32
#include <time.h>
#include <Windows.h>
#else
#include <sys/time.h>
#endif
#include <sstream>

#ifdef WIN32
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
  gettimeofday(reinterpret_cast<timeval *>(tic_), NULL);

  std::ostringstream os;
  os << "\nBEGIN " << msg_ << "\n";
  OutputStream::WriteOut(output_type_, os.str());
}

dsTimer::~dsTimer()
{
  timeval toc;
  gettimeofday(&toc, NULL);

  const timeval &tic = *(reinterpret_cast<timeval *>(tic_));

  double timediff = toc.tv_sec - tic.tv_sec + 1e-6 * (toc.tv_usec - tic.tv_usec);

  std::ostringstream os;
  os << "\nEND " << msg_ << " (" << timediff << ")\n";
  OutputStream::WriteOut(output_type_, os.str());
  delete reinterpret_cast<timeval *>(tic_);
}
#endif
