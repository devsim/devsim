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

#include "TetrahedronEdgeFromEdgeModel.hh"
#include "TetrahedronEdgeSubModel.hh"
#include "Region.hh"
#include "TetrahedronElementField.hh"
#include "dsAssert.hh"
#include "Vector.hh"



// TODO:"TEST THIS"

TetrahedronEdgeFromEdgeModel::TetrahedronEdgeFromEdgeModel(const std::string &edgemodel, RegionPtr rp)
    : TetrahedronEdgeModel(edgemodel + "_x", rp, TetrahedronEdgeModel::SCALAR), edgeModelName(edgemodel), y_ModelName(edgeModelName+ "_y"), z_ModelName(edgeModelName + "_z")
{
  RegisterCallback(edgemodel);
  new TetrahedronEdgeSubModel(y_ModelName, rp, this->GetSelfPtr(), TetrahedronEdgeModel::SCALAR);
  new TetrahedronEdgeSubModel(z_ModelName, rp, this->GetSelfPtr(), TetrahedronEdgeModel::SCALAR);
}

//// Need to figure out the deleter situation from sub models
//// Perhaps a Delete SubModels method??????

void TetrahedronEdgeFromEdgeModel::calcTetrahedronEdgeScalarValues() const
{
  const Region &reg = GetRegion();

  const ConstEdgeModelPtr emp = reg.GetEdgeModel(edgeModelName);
  dsAssert(emp, "UNEXPECTED");

  const ConstTetrahedronEdgeModelPtr tempy = reg.GetTetrahedronEdgeModel(y_ModelName);
  dsAssert(tempy, "UNEXPECTED");

  const ConstTetrahedronEdgeModelPtr tempz = reg.GetTetrahedronEdgeModel(z_ModelName);
  dsAssert(tempz, "UNEXPECTED");

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
  //// TODO: take care of const problem once and for all
  std::tr1::const_pointer_cast<TetrahedronEdgeModel, const TetrahedronEdgeModel>(tempy)->SetValues(evy);
  std::tr1::const_pointer_cast<TetrahedronEdgeModel, const TetrahedronEdgeModel>(tempz)->SetValues(evz);
}

void TetrahedronEdgeFromEdgeModel::Serialize(std::ostream &of) const
{
  of << "COMMAND element_from_edge_model -device \"" << GetDeviceName() << "\" -region \"" << GetRegionName() << "\" -edge_model \"" << edgeModelName << "\"";
}

