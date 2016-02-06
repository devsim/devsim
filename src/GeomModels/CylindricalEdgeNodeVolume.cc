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

#include "CylindricalEdgeNodeVolume.hh"
#include "Region.hh"
#include "dsAssert.hh"
#include "TriangleEdgeModel.hh"
#include "Edge.hh"
#include "EdgeSubModel.hh"

CylindricalEdgeNodeVolume::CylindricalEdgeNodeVolume(RegionPtr rp)
    : EdgeModel("CylindricalEdgeNodeVolume@n0", rp, EdgeModel::SCALAR)
{
    const size_t dimension = rp->GetDimension();
    dsAssert(dimension == 2, "CylindricalEdgeNodeVolume 2d Only");

    if (dimension == 2)
    {
      RegisterCallback("ElementCylindricalNodeVolume@en0");
      RegisterCallback("ElementCylindricalNodeVolume@en1");
    }

    node1Volume_ = EdgeSubModel::CreateEdgeSubModel("CylindricalEdgeNodeVolume@n1", rp, EdgeModel::SCALAR, this->GetSelfPtr());
}

void CylindricalEdgeNodeVolume::calcEdgeScalarValues() const
{
  const Region &r = GetRegion();
  const size_t dimension = r.GetDimension();

  dsAssert(dimension == 2, "CylindricalEdgeNodeVolume 2d Only");

  std::vector<double> nv(r.GetNumberNodes());


  if (dimension == 2)
  {
    const Region &region = GetRegion();

    ConstTriangleEdgeModelPtr eec0 = region.GetTriangleEdgeModel("ElementCylindricalNodeVolume@en0");
    ConstTriangleEdgeModelPtr eec1 = region.GetTriangleEdgeModel("ElementCylindricalNodeVolume@en1");

    dsAssert(eec0.get(), "ElementCylindricalNodeVolume@en0 missing");
    dsAssert(eec1.get(), "ElementCylindricalNodeVolume@en1 missing");

    const EdgeScalarList &nv0 = eec0->GetValuesOnEdges();
    const EdgeScalarList &nv1 = eec1->GetValuesOnEdges();

    SetValues(nv0);
    node1Volume_.lock()->SetValues(nv1);

  }
  else
  {
    dsAssert(false, "UNEXPECTED");
  }

  SetValues(nv);
}

void CylindricalEdgeNodeVolume::Serialize(std::ostream &of) const
{
  of << "DATAPARENT \"ElementCylindricalNodeVolume@en0\"";
}

