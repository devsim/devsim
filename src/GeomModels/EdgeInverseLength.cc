/***
DEVSIM
Copyright 2013 DEVSIM LLC

SPDX-License-Identifier: Apache-2.0
***/

#include "EdgeInverseLength.hh"
#include "EdgeModel.hh"
#include "Vector.hh"
#include "Region.hh"
#include "Edge.hh"
#include "dsAssert.hh"

template <typename DoubleType>
EdgeInverseLength<DoubleType>::EdgeInverseLength(RegionPtr rp) :
EdgeModel("EdgeInverseLength", rp, EdgeModel::DisplayType::SCALAR)
{
    RegisterCallback("EdgeLength");
}


template <typename DoubleType>
void EdgeInverseLength<DoubleType>::calcEdgeScalarValues() const
{
    ConstEdgeModelPtr elen = GetRegion().GetEdgeModel("EdgeLength");
    dsAssert(elen.get(), "UNEXPECTED");

    const EdgeScalarList<DoubleType> &evals = elen->GetScalarValues<DoubleType>();

    std::vector<DoubleType> ev(evals.size());


    for (size_t i = 0; i < ev.size(); ++i)
    {
        ev[i] = 1.0 / evals[i];
    }
    SetValues(ev);
}

template <typename DoubleType>
void EdgeInverseLength<DoubleType>::Serialize(std::ostream &of) const
{
  SerializeBuiltIn(of);
}

template class EdgeInverseLength<double>;
#ifdef DEVSIM_EXTENDED_PRECISION
#include "Float128.hh"
template class EdgeInverseLength<float128>;
#endif

