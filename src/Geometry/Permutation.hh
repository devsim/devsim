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

#ifndef PERMUTATION_HH
#define PERMUTATION_HH
#include <cstdlib>
#include <map>
class Contact;
typedef const Contact *ConstContactPtr;
class Interface;
typedef const Interface *ConstInterfacePtr;

class PermutationEntry;
// the first entry is the original row
typedef std::map<size_t /*row*/, PermutationEntry> PermutationMap;

class PermutationEntry
{
    public:
        PermutationEntry(size_t /*newrow*/);

        size_t GetRow()
        {
            return newrow;
        }

//      // the defaults are needed
        PermutationEntry();
        PermutationEntry(const PermutationEntry &);
        PermutationEntry &operator=(const PermutationEntry &);

    private:
        size_t newrow;
        ConstContactPtr contact;
        ConstInterfacePtr interface;
};
#endif
