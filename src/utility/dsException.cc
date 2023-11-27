/***
DEVSIM
Copyright 2013 DEVSIM LLC

SPDX-License-Identifier: Apache-2.0
***/

#include "dsException.hh"

const std::string dsException::msg = "DEVSIM FATAL: ";

dsException::dsException(const char *w) : std::runtime_error(msg + w)
{
}

dsException::dsException(const std::string &w) : std::runtime_error(msg + w)
{
}
