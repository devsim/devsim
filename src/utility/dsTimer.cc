/***
DEVSIM
Copyright 2013 DEVSIM LLC

SPDX-License-Identifier: Apache-2.0
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

