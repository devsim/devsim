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

#ifndef DS_BASE64_HH
#define DS_BASE64_HH
#include <string>
#include <vector>
namespace dsUtility
{
std::string encodeBase64(const char * /*input*/, size_t /*length*/);
template <typename T> std::string convertVectorToBase64(const std::vector<T> &);
template <typename T> std::string convertVectorToZlibBase64(const std::vector<T> &);

bool decodeBase64(const std::string &/*input*/, std::string &/*output*/);
}
#endif

