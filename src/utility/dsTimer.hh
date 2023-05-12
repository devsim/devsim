/***
DEVSIM
Copyright 2013 DEVSIM LLC

SPDX-License-Identifier: Apache-2.0
***/

#ifndef DSTIMER_HH
#define DSTIMER_HH
#include "OutputStream.hh"
#include <string>
#include <chrono>

class dsTimer {
  public:
    dsTimer(const std::string &/*token*/, OutputStream::OutputType outtype = OutputStream::OutputType::VERBOSE2);
    ~dsTimer();
  private:
    dsTimer(const dsTimer &);
    dsTimer &operator=(const dsTimer &);

    const                    std::string msg_;
    OutputStream::OutputType output_type_;
    std::chrono::time_point<std::chrono::system_clock> tic_;
};
#endif

