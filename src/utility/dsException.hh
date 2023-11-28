/***
DEVSIM
Copyright 2013 DEVSIM LLC

SPDX-License-Identifier: Apache-2.0
***/

#ifndef DS_EXCEPTION_HH
#define DS_EXCEPTION_HH
#include <stdexcept>
#include <string>

class dsException : std::runtime_error {
  public:

    dsException(const char *);
    dsException(const std::string &);

    using std::runtime_error::what;

  private:
        static const std::string msg;
};
#endif

