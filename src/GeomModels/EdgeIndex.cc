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

#include "EdgeIndex.hh"
#include "EdgeModel.hh"
#include "Vector.hh"
#include "Region.hh"
#include "Edge.hh"
#include "Node.hh"

template <typename DoubleType>
EdgeIndex<DoubleType>::EdgeIndex(RegionPtr rp) :
EdgeModel("edge_index", rp, EdgeModel::DisplayType::SCALAR)
{
}


template <typename DoubleType>
void EdgeIndex<DoubleType>::calcEdgeScalarValues() const
{
    const ConstEdgeList &el = GetRegion().GetEdgeList();
    std::vector<DoubleType> ev(el.size());
    for (size_t i = 0; i < ev.size(); ++i)
    {
        ev[i] = calcEdgeIndex(el[i]);
    }
    SetValues(ev);
}

template <typename DoubleType>
DoubleType EdgeIndex<DoubleType>::calcEdgeIndex(ConstEdgePtr ep) const
{
    const DoubleType val = ep->GetIndex();
    return val;
}

template <typename DoubleType>
void EdgeIndex<DoubleType>::Serialize(std::ostream &of) const
{
  SerializeBuiltIn(of);
}

template class EdgeIndex<double>;
#ifdef DEVSIM_EXTENDED_PRECISION
#include "Float128.hh"
template class EdgeIndex<float128>;
#endif

