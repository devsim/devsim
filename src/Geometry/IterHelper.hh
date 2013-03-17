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

#ifndef ITERHELPER_HH
#define ITERHELPER_HH
#include "MathEnum.hh"

class PermutationEntry;
typedef std::map<size_t, PermutationEntry> PermutationMap;
namespace dsMath {
template <typename T> class RowColVal;
typedef std::vector<RowColVal<double> > RealRowColValueVec;
typedef std::pair<int, double> RHSEntry;
typedef std::vector<RHSEntry> RHSEntryVec;
}

namespace IterHelper {

template <typename V, typename T>
void ForEachMapValue(const V &v, T t)
{
//    const EquationPtrMap_t &ep = GetEquationPtrList();
    typename V::const_iterator eit = v.begin();
    const typename V::const_iterator eend = v.end();
    for ( ; eit != eend; ++eit)
    {
        t(*((*eit).second));
    }
}

    template <typename T>
    struct assdc
    {
        assdc(dsMath::RealRowColValueVec &m, dsMath::RHSEntryVec &v, dsMathEnum::WhatToLoad w, dsMathEnum::TimeMode t) : mat(m), rhs(v), wtl(w), tm(t)
        {
        }

        void operator() (T &e)
        {
            e.Assemble(mat, rhs, wtl, tm);
        }

        dsMath::RealRowColValueVec &mat;
        dsMath::RHSEntryVec    &rhs;
        dsMathEnum::WhatToLoad      wtl;
        dsMathEnum::TimeMode        tm;
    };

    template <typename T>
    struct assdcp
    {
        assdcp(dsMath::RealRowColValueVec &m, dsMath::RHSEntryVec &v, PermutationMap &p, dsMathEnum::WhatToLoad w, dsMathEnum::TimeMode t) : mat(m), rhs(v), perm(p), wtl(w), tm(t)
        {
        }

        void operator() (T &e)
        {
            e.Assemble(mat, rhs, perm, wtl, tm);
        }

        dsMath::RealRowColValueVec &mat;
        dsMath::RHSEntryVec &rhs;
        PermutationMap &perm;
        dsMathEnum::WhatToLoad      wtl;
        dsMathEnum::TimeMode        tm;
    };
}
#endif
