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

#ifndef DSTIMER_HH
#define DSTIMER_HH
#include "OutputStream.hh"
#include <string>
#include <memory>

class dsTimer {
  public:
    dsTimer(const std::string &/*token*/, OutputStream::OutputType outtype = OutputStream::VERBOSE2);
    ~dsTimer();
  private:
    dsTimer(const dsTimer &);
    dsTimer &operator=(const dsTimer &);

    const                    std::string msg_;
    OutputStream::OutputType output_type_;
    void *tic_;
};
#endif
