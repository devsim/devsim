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

#include "TriangleEdgeFromEdgeModel.hh"
#include "TriangleEdgeSubModel.hh"
#include "Region.hh"
#include "TriangleElementField.hh"
#include "dsAssert.hh"
#include "Vector.hh"


template <typename DoubleType>
TriangleEdgeFromEdgeModel<DoubleType>::TriangleEdgeFromEdgeModel(const std::string &edgemodel, RegionPtr rp)
    : TriangleEdgeModel(edgemodel + "_x", rp, TriangleEdgeModel::SCALAR), edgeModelName(edgemodel), y_ModelName(edgeModelName + "_y")
{
  RegisterCallback(edgemodel);
  new TriangleEdgeSubModel<DoubleType>(y_ModelName, rp, this->GetSelfPtr(), TriangleEdgeModel::SCALAR);
}

//// Need to figure out the deleter situation from sub models
//// Perhaps a Delete SubModels method??????

template <typename DoubleType>
void TriangleEdgeFromEdgeModel<DoubleType>::calcTriangleEdgeScalarValues() const
{
  const Region &reg = GetRegion();

  const ConstEdgeModelPtr emp = reg.GetEdgeModel(edgeModelName);
  dsAssert(emp.get(), "UNEXPECTED");

  const ConstTriangleEdgeModelPtr tempy = reg.GetTriangleEdgeModel(y_ModelName);
  dsAssert(tempy.get(), "UNEXPECTED");

  const ConstTriangleList &tl = GetRegion().GetTriangleList();

  std::vector<DoubleType> evx(3*tl.size());
  std::vector<DoubleType> evy(3*tl.size());

  const TriangleElementField &efield = reg.GetTriangleElementField();
  for (size_t i = 0; i < tl.size(); ++i)
  {
    const Triangle &triangle = *tl[i];
    const std::vector<Vector<DoubleType> > &v = efield.GetTriangleElementField(triangle, *emp);
    for (size_t j = 0; j < 3; ++j)
    {
      evx[3*i + j] = v[j].Getx();
      evy[3*i + j] = v[j].Gety();
    }
  }

  SetValues(evx);
  std::const_pointer_cast<TriangleEdgeModel, const TriangleEdgeModel>(tempy)->SetValues(evy);
}

template <typename DoubleType>
void TriangleEdgeFromEdgeModel<DoubleType>::Serialize(std::ostream &of) const
{
  of << "COMMAND element_from_edge_model -device \"" << GetDeviceName() << "\" -region \"" << GetRegionName() << "\" -edge_model \"" << edgeModelName << "\"";
}

template class TriangleEdgeFromEdgeModel<double>;


