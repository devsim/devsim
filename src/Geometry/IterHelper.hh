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
