/***
DEVSIM
Copyright 2013 DEVSIM LLC

SPDX-License-Identifier: Apache-2.0
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
