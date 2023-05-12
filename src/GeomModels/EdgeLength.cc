/***
DEVSIM
Copyright 2013 DEVSIM LLC

SPDX-License-Identifier: Apache-2.0
***/

#include "EdgeLength.hh"
#include "EdgeModel.hh"
#include "Vector.hh"
#include "Region.hh"
#include "Edge.hh"
#include "Node.hh"

template <typename DoubleType>
EdgeLength<DoubleType>::EdgeLength(RegionPtr rp) :
EdgeModel("EdgeLength", rp, EdgeModel::DisplayType::SCALAR)
{
}


template <typename DoubleType>
void EdgeLength<DoubleType>::calcEdgeScalarValues() const
{
    const ConstEdgeList &el = GetRegion().GetEdgeList();
    std::vector<DoubleType> ev(el.size());
    for (size_t i = 0; i < ev.size(); ++i)
    {
        ev[i] = calcEdgeLength(el[i]);
    }
    SetValues(ev);
}

template <typename DoubleType>
DoubleType EdgeLength<DoubleType>::calcEdgeLength(ConstEdgePtr ep) const
{
    const auto &h0 = ep->GetNodeList()[0]->Position();
    const auto &h1 = ep->GetNodeList()[1]->Position();

    Vector<DoubleType> vm(h0.Getx(), h0.Gety(), h0.Getz());
    vm -= Vector<DoubleType>(h1.Getx(), h1.Gety(), h1.Getz());
    const DoubleType val = vm.magnitude();
    return val;
}

template <typename DoubleType>
void EdgeLength<DoubleType>::Serialize(std::ostream &of) const
{
  SerializeBuiltIn(of);
}

template class EdgeLength<double>;
#ifdef DEVSIM_EXTENDED_PRECISION
#include "Float128.hh"
template class EdgeLength<float128>;
#endif

