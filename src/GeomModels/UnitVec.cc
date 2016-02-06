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

UnitVec::~UnitVec()
{
}

UnitVec::UnitVec(RegionPtr rp) :
EdgeModel("unitx", rp, EdgeModel::SCALAR)
{
    if (rp->GetDimension() > 1)
    {
        unity = EdgeSubModel::CreateEdgeSubModel("unity", rp, EdgeModel::SCALAR, this->GetSelfPtr());
    }

    if (rp->GetDimension() > 2)
    {
        unitz = EdgeSubModel::CreateEdgeSubModel("unitz", rp, EdgeModel::SCALAR, this->GetSelfPtr());
    }
}


void UnitVec::calcEdgeScalarValues() const
{
  const ConstEdgeList &el = GetRegion().GetEdgeList();
  std::vector<double> ux(el.size());
  std::vector<double> uy(el.size());
  std::vector<double> uz(el.size());

  for (size_t i = 0; i < el.size(); ++i)
  {
     const Vector &v = calcUnitVec(el[i]);
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

Vector UnitVec::calcUnitVec(ConstEdgePtr ep) const
{
    const Vector &p0 = ep->GetHead()->Position();
    const Vector &p1 = ep->GetTail()->Position();

    Vector vm = p1;
    vm -= p0;
    vm /= vm.magnitude();

    return vm;
}

void UnitVec::Serialize(std::ostream &of) const
{
  SerializeBuiltIn(of);
}

