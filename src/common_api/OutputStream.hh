/***
DEVSIM
Copyright 2013 DEVSIM LLC

SPDX-License-Identifier: Apache-2.0
***/

#ifndef DS_OUTPUTSTREAM_HH
#define DS_OUTPUTSTREAM_HH
#include <string>
class OutputStream {
    public:
        enum class OutputType {INFO, VERBOSE1, VERBOSE2, ERROR, FATAL};
        enum class Verbosity_t {V0 = 0, V1, V2, UNKNOWN};
        static void WriteOut(OutputType, const std::string &);
        static void SetInterpreter(void *);
        static void WriteOut(OutputType, Verbosity_t verbosity, const std::string &);
        static Verbosity_t GetVerbosity(const std::string &);

    private:
        OutputStream();
        OutputStream(const OutputStream &);
        OutputStream & operator=(const OutputStream &);
        std::string          verbosity;
};
#endif

