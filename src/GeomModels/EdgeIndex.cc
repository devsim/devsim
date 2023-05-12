/***
DEVSIM
Copyright 2013 DEVSIM LLC

SPDX-License-Identifier: Apache-2.0
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

