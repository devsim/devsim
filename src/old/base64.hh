/***
DEVSIM
Copyright 2013 DEVSIM LLC

SPDX-License-Identifier: Apache-2.0
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

