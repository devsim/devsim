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

#ifndef DS_OUTPUTSTREAM_HH
#define DS_OUTPUTSTREAM_HH
#include <string>
class OutputStream {
    public:
        enum OutputType {INFO, VERBOSE1, VERBOSE2, ERROR, FATAL};
        enum Verbosity_t {V0 = 0, V1, V2, UNKNOWN};
        static void WriteOut(OutputType, const std::string &);
        static void SetInterpreter(void *);
        static void WriteOut(OutputType, Verbosity_t verbosity, const std::string &);
        static Verbosity_t GetVerbosity(const std::string &);

    private:


        OutputStream();
        OutputStream(const OutputStream &);
        OutputStream & operator=(const OutputStream &);

        static OutputStream *instance;
        std::string          verbosity;
        static void         *interpreter;
};
#endif

