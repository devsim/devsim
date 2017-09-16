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

#include "NodeVolume.hh"
#include "Region.hh"
#include "Edge.hh"
#include "EdgeModel.hh"
#include "EdgeScalarData.hh"
#include "dsAssert.hh"
#include "TriangleEdgeModel.hh"
#include "TetrahedronEdgeModel.hh"

template <typename DoubleType>
NodeVolume<DoubleType>::NodeVolume(RegionPtr rp)
    : NodeModel("NodeVolume", rp, NodeModel::DisplayType::SCALAR)
{
    const size_t dimension = rp->GetDimension();

    if (dimension == 1)
    {
      RegisterCallback("EdgeLength");
      RegisterCallback("EdgeCouple");
    }
    else if ((dimension == 2) || (dimension == 3))
    {
      RegisterCallback("ElementNodeVolume");
    }
}

template <typename DoubleType>
void NodeVolume<DoubleType>::calcNodeScalarValues() const
{
  const Region &r = GetRegion();
  const size_t dimension = r.GetDimension();

  std::vector<DoubleType> nv(r.GetNumberNodes());

  if (dimension == 1)
  {
    ConstEdgeModelPtr ec = r.GetEdgeModel("EdgeCouple");
    dsAssert(ec.get(), "UNEXPECTED");
    ConstEdgeModelPtr elen = r.GetEdgeModel("EdgeLength");
    dsAssert(elen.get(), "UNEXPECTED");

    EdgeScalarData<DoubleType> evol = EdgeScalarData<DoubleType>(*ec);
    evol.times_equal_model(*elen);

    //// valid only for dimension == 1
    evol.times_equal_scalar(0.5);

    for (size_t i = 0; i < nv.size(); ++i)
    {
      DoubleType volume = 0.0;

      const ConstEdgeList &el = r.GetNodeToEdgeList()[i];

      ConstEdgeList::const_iterator it = el.begin();
      const ConstEdgeList::const_iterator itend = el.end();
      for ( ; it != itend; ++it)
      {
        volume += evol[(*it)->GetIndex()]; 
      }

      nv[i] = volume;
    }
  }
  else if (dimension == 2)
  {
    ConstTriangleEdgeModelPtr eec = GetRegion().GetTriangleEdgeModel("ElementNodeVolume");
    dsAssert(eec.get(), "ElementNodeVolume missing");

    eec->GetScalarValuesOnNodes(TriangleEdgeModel::InterpolationType::SUM, nv);
  }
  else if (dimension == 3)
  {
    ConstTetrahedronEdgeModelPtr eec = GetRegion().GetTetrahedronEdgeModel("ElementNodeVolume");
    dsAssert(eec.get(), "UNEXPECTED");
    eec->GetScalarValuesOnNodes(TetrahedronEdgeModel::InterpolationType::SUM, nv);
  }
  else
  {
    dsAssert(false, "UNEXPECTED");
  }

  SetValues(nv);
}

template <typename DoubleType>
void NodeVolume<DoubleType>::setInitialValues()
{
    DefaultInitializeValues();
}

template <typename DoubleType>
void NodeVolume<DoubleType>::Serialize(std::ostream &of) const
{
  SerializeBuiltIn(of);
}

template class NodeVolume<double>;

