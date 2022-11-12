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

