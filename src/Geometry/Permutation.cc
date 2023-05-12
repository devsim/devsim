/***
DEVSIM
Copyright 2013 DEVSIM LLC

SPDX-License-Identifier: Apache-2.0
***/

#include "Permutation.hh"
PermutationEntry::PermutationEntry(size_t nr, bool kc)
    : newrow(nr), keepcopy(kc)
{}

PermutationEntry::PermutationEntry()
    : newrow(size_t(-1)), keepcopy(false) //, contact(nullptr), interface(nullptr)
{
}

PermutationEntry::PermutationEntry(const PermutationEntry &f)
    : newrow(f.newrow), keepcopy(f.keepcopy)
{
}

PermutationEntry &PermutationEntry::operator=(const PermutationEntry &p)
{
    if (&p == this)
    {
        return *this;
    }

    newrow = p.newrow;
    keepcopy = p.keepcopy;
    return *this;
}

