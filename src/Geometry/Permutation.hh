/***
DEVSIM
Copyright 2013 DEVSIM LLC

SPDX-License-Identifier: Apache-2.0
***/

#ifndef PERMUTATION_HH
#define PERMUTATION_HH
#include <cstdlib>
#include <map>
#if 0
class Contact;
typedef const Contact *ConstContactPtr;
class Interface;
typedef const Interface *ConstInterfacePtr;
#endif

class PermutationEntry;
// the first entry is the original row
typedef std::map<size_t /*row*/, PermutationEntry> PermutationMap;

class PermutationEntry
{
    public:
        PermutationEntry(size_t /*newrow*/, bool /*keepcopy*/);

        size_t GetRow() const
        {
            return newrow;
        }

        // special case for handling new interface type
        bool KeepCopy() const
        {
            return keepcopy;
        }

//      // the defaults are needed
        PermutationEntry();
        PermutationEntry(const PermutationEntry &);
        PermutationEntry &operator=(const PermutationEntry &);

    private:
        size_t newrow;
        bool   keepcopy;
#if 0
        ConstContactPtr contact;
        ConstInterfacePtr interface;
#endif
};
#endif

