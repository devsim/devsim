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

#ifndef DSASSERT_HH
#define DSASSERT_HH
//#ifndef NDEBUG
//void dsAssert_(bool, const char *);
#include <string>
void dsAssert_(bool, const std::string &);
//#endif
#if 0
inline void dsAssert(bool cond, const char *msg)
{
//#ifndef NDEBUG
    if (!cond)
    {
        dsAssert_(cond, msg);
    }
//#endif
}
#endif

// macro to include file and line: http://www.decompile.com/cpp/faq/file_and_line_error_string.htm
#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)
#define AT "ASSERT " __FILE__ ":" TOSTRING(__LINE__) " "

#define dsAssert(cond, msg) \
  do \
  { \
    if (!(cond)) \
    { \
      dsAssert_(cond, std::string(AT) + msg); \
    } \
  } while(0)


#if 0
void dsExit(int);
#endif

#endif
