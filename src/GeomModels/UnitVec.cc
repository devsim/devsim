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

#include "UnitVec.hh"
#include "Region.hh"
#include "Node.hh"
#include "Edge.hh"
#include "Vector.hh"
#include "EdgeSubModel.hh"
#include "dsAssert.hh"

template <typename DoubleType>
UnitVec<DoubleType>::~UnitVec()
{
}

template <typename DoubleType>
UnitVec<DoubleType>::UnitVec(RegionPtr rp) :
EdgeModel("unitx", rp, EdgeModel::DisplayType::SCALAR)
{
    if (rp->GetDimension() > 1)
    {
        unity = EdgeSubModel<DoubleType>::CreateEdgeSubModel("unity", rp, EdgeModel::DisplayType::SCALAR, this->GetSelfPtr());
    }

    if (rp->GetDimension() > 2)
    {
        unitz = EdgeSubModel<DoubleType>::CreateEdgeSubModel("unitz", rp, EdgeModel::DisplayType::SCALAR, this->GetSelfPtr());
    }
}


template <typename DoubleType>
void UnitVec<DoubleType>::calcEdgeScalarValues() const
{
  const ConstEdgeList &el = GetRegion().GetEdgeList();
  std::vector<DoubleType> ux(el.size());
  std::vector<DoubleType> uy(el.size());
  std::vector<DoubleType> uz(el.size());

  for (size_t i = 0; i < el.size(); ++i)
  {
     const Vector<DoubleType> &v = calcUnitVec(el[i]);
     ux[i] = v.Getx();
     uy[i] = v.Gety();
     uz[i] = v.Getz();
  }

  SetValues(ux);
  if (!unity.expired())
  {
    unity.lock()->SetValues(uy);
  }
  if (!unitz.expired())
  {
    unitz.lock()->SetValues(uz);
  }
}

template <typename DoubleType>
Vector<DoubleType> UnitVec<DoubleType>::calcUnitVec(ConstEdgePtr ep) const
{
    const Vector<DoubleType> &p0 = ep->GetHead()->Position();
    const Vector<DoubleType> &p1 = ep->GetTail()->Position();

    Vector<DoubleType> vm = p1;
    vm -= p0;
    vm /= vm.magnitude();

    return vm;
}

template <typename DoubleType>
void UnitVec<DoubleType>::Serialize(std::ostream &of) const
{
  SerializeBuiltIn(of);
}

template class UnitVec<double>;

