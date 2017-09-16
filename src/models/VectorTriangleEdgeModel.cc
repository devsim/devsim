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

#include "VectorTriangleEdgeModel.hh"
#include "TriangleEdgeSubModel.hh"
#include "Region.hh"
#include "TriangleElementField.hh"
#include "dsAssert.hh"
#include "Vector.hh"


template <typename DoubleType>
VectorTriangleEdgeModel<DoubleType>::VectorTriangleEdgeModel(const std::string &edgemodel, RegionPtr rp)
    : TriangleEdgeModel(edgemodel + "_x", rp, TriangleEdgeModel::DisplayType::SCALAR), elementEdgeModelName(edgemodel), y_ModelName(elementEdgeModelName + "_y")
{
  RegisterCallback(elementEdgeModelName);
  new TriangleEdgeSubModel<DoubleType>(y_ModelName, rp, this->GetSelfPtr(), TriangleEdgeModel::DisplayType::SCALAR);
}

template <typename DoubleType>
void VectorTriangleEdgeModel<DoubleType>::calcTriangleEdgeScalarValues() const
{
  const Region &reg = GetRegion();

  const ConstTriangleEdgeModelPtr emp = reg.GetTriangleEdgeModel(elementEdgeModelName);
  dsAssert(emp.get(), "UNEXPECTED");

  const ConstTriangleEdgeModelPtr tempy = reg.GetTriangleEdgeModel(y_ModelName);
  dsAssert(tempy.get(), "UNEXPECTED");

  const ConstTriangleList &tl = GetRegion().GetTriangleList();

  std::vector<DoubleType> evx(3*tl.size());
  std::vector<DoubleType> evy(3*tl.size());

  const TriangleElementField<DoubleType> &efield = reg.GetTriangleElementField<DoubleType>();
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
void VectorTriangleEdgeModel<DoubleType>::Serialize(std::ostream &of) const
{
  of << "COMMAND vector_element_model -device \"" << GetDeviceName() << "\" -region \"" << GetRegionName() << "\" -element_model \"" << elementEdgeModelName << "\"";
}

template class VectorTriangleEdgeModel<double>;


