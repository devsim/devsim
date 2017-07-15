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

#include "EdgeInverseLength.hh"
#include "EdgeModel.hh"
#include "Vector.hh"
#include "Region.hh"
#include "Edge.hh"
#include "dsAssert.hh"

template <typename DoubleType>
EdgeInverseLength<DoubleType>::EdgeInverseLength(RegionPtr rp) :
EdgeModel("EdgeInverseLength", rp, EdgeModel::SCALAR)
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

