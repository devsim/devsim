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

