/***
DEVSIM
Copyright 2013 Devsim LLC

This file is part of DEVSIM.

DEVSIM is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation, version 3.

DEVSIM is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with DEVSIM.  If not, see <http://www.gnu.org/licenses/>.
***/

#include "VectorTetrahedronEdgeModel.hh"
#include "TetrahedronEdgeSubModel.hh"
#include "Region.hh"
#include "TetrahedronElementField.hh"
#include "dsAssert.hh"
#include "Vector.hh"

VectorTetrahedronEdgeModel::VectorTetrahedronEdgeModel(const std::string &edgemodel, RegionPtr rp)
    : TetrahedronEdgeModel(edgemodel + "_x", rp, TetrahedronEdgeModel::SCALAR), elementEdgeModelName(edgemodel), y_ModelName(elementEdgeModelName+ "_y"), z_ModelName(elementEdgeModelName + "_z")
{
  RegisterCallback(edgemodel);
  new TetrahedronEdgeSubModel(y_ModelName, rp, this->GetSelfPtr(), TetrahedronEdgeModel::SCALAR);
  new TetrahedronEdgeSubModel(z_ModelName, rp, this->GetSelfPtr(), TetrahedronEdgeModel::SCALAR);
}

void VectorTetrahedronEdgeModel::calcTetrahedronEdgeScalarValues() const
{
  const Region &reg = GetRegion();

  const ConstTetrahedronEdgeModelPtr emp = reg.GetTetrahedronEdgeModel(elementEdgeModelName);
  dsAssert(emp.get(), "UNEXPECTED");

  const ConstTetrahedronEdgeModelPtr tempy = reg.GetTetrahedronEdgeModel(y_ModelName);
  dsAssert(tempy.get(), "UNEXPECTED");

  const ConstTetrahedronEdgeModelPtr tempz = reg.GetTetrahedronEdgeModel(z_ModelName);
  dsAssert(tempz.get(), "UNEXPECTED");

  const ConstTetrahedronList &tl = GetRegion().GetTetrahedronList();

  std::vector<double> evx(6*tl.size());
  std::vector<double> evy(6*tl.size());
  std::vector<double> evz(6*tl.size());

  const TetrahedronElementField &efield = reg.GetTetrahedronElementField();
  for (size_t i = 0; i < tl.size(); ++i)
  {
    const Tetrahedron &tetrahedron = *tl[i];
    const std::vector<Vector> &v = efield.GetTetrahedronElementField(tetrahedron, *emp);
    for (size_t j = 0; j < 6; ++j)
    {
      const Vector &vec = v[j];
      evx[6*i + j] = vec.Getx();
      evy[6*i + j] = vec.Gety();
      evz[6*i + j] = vec.Getz();
    }
  }

  SetValues(evx);
  std::const_pointer_cast<TetrahedronEdgeModel, const TetrahedronEdgeModel>(tempy)->SetValues(evy);
  std::const_pointer_cast<TetrahedronEdgeModel, const TetrahedronEdgeModel>(tempz)->SetValues(evz);
}

void VectorTetrahedronEdgeModel::Serialize(std::ostream &of) const
{
  of << "COMMAND vector_element_model -device \"" << GetDeviceName() << "\" -region \"" << GetRegionName() << "\" -element_model \"" << elementEdgeModelName << "\"";
}

