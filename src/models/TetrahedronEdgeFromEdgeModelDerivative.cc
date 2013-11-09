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

#include "TetrahedronEdgeFromEdgeModelDerivative.hh"
#include "TetrahedronEdgeSubModel.hh"
#include "Region.hh"
#include "TetrahedronElementField.hh"
#include "dsAssert.hh"
#include "Vector.hh"



// TODO:"TEST THIS"

TetrahedronEdgeFromEdgeModelDerivative::TetrahedronEdgeFromEdgeModelDerivative(
        const std::string &edgemodel,
        const std::string &derivative,
        RegionPtr rp
    )
    : TetrahedronEdgeModel(edgemodel + "_x:" + derivative + "@en0", rp, TetrahedronEdgeModel::NODISPLAY),
      edgeModelName(edgemodel),
      nodeModelName(derivative)
{
  const std::string tmx = edgeModelName + "_x";
  const std::string tmy = edgeModelName + "_y";
  const std::string tmz = edgeModelName + "_z";

  const std::string tmpname = edgeModelName + ":" + nodeModelName;
  edgeModelName0 = tmpname + "@n0";
  edgeModelName1 = tmpname + "@n1";

  x_ModelName1 = tmx + ":" + nodeModelName + "@en1";
  x_ModelName2 = tmx + ":" + nodeModelName + "@en2";
  x_ModelName3 = tmx + ":" + nodeModelName + "@en3";
  y_ModelName0 = tmy + ":" + nodeModelName + "@en0";
  y_ModelName1 = tmy + ":" + nodeModelName + "@en1";
  y_ModelName2 = tmy + ":" + nodeModelName + "@en2";
  y_ModelName3 = tmy + ":" + nodeModelName + "@en3";
  z_ModelName0 = tmz + ":" + nodeModelName + "@en0";
  z_ModelName1 = tmz + ":" + nodeModelName + "@en1";
  z_ModelName2 = tmz + ":" + nodeModelName + "@en2";
  z_ModelName3 = tmz + ":" + nodeModelName + "@en3";

  RegisterCallback(edgeModelName0);
  RegisterCallback(edgeModelName1);
  new TetrahedronEdgeSubModel(x_ModelName1, rp, this->GetSelfPtr(), TetrahedronEdgeModel::NODISPLAY);
  new TetrahedronEdgeSubModel(x_ModelName2, rp, this->GetSelfPtr(), TetrahedronEdgeModel::NODISPLAY);
  new TetrahedronEdgeSubModel(x_ModelName3, rp, this->GetSelfPtr(), TetrahedronEdgeModel::NODISPLAY);
  new TetrahedronEdgeSubModel(y_ModelName0, rp, this->GetSelfPtr(), TetrahedronEdgeModel::NODISPLAY);
  new TetrahedronEdgeSubModel(y_ModelName1, rp, this->GetSelfPtr(), TetrahedronEdgeModel::NODISPLAY);
  new TetrahedronEdgeSubModel(y_ModelName2, rp, this->GetSelfPtr(), TetrahedronEdgeModel::NODISPLAY);
  new TetrahedronEdgeSubModel(y_ModelName3, rp, this->GetSelfPtr(), TetrahedronEdgeModel::NODISPLAY);
  new TetrahedronEdgeSubModel(z_ModelName0, rp, this->GetSelfPtr(), TetrahedronEdgeModel::NODISPLAY);
  new TetrahedronEdgeSubModel(z_ModelName1, rp, this->GetSelfPtr(), TetrahedronEdgeModel::NODISPLAY);
  new TetrahedronEdgeSubModel(z_ModelName2, rp, this->GetSelfPtr(), TetrahedronEdgeModel::NODISPLAY);
  new TetrahedronEdgeSubModel(z_ModelName3, rp, this->GetSelfPtr(), TetrahedronEdgeModel::NODISPLAY);
}

//// Need to figure out the deleter situation from sub models
//// Perhaps a Delete SubModels method??????

void TetrahedronEdgeFromEdgeModelDerivative::calcTetrahedronEdgeScalarValues() const
{
  const Region &reg = GetRegion();

  ConstEdgeModelPtr emp[2];
  emp[0] = reg.GetEdgeModel(edgeModelName0);
  emp[1] = reg.GetEdgeModel(edgeModelName1);

  for (size_t i = 0; i < 2; ++i)
  {
    dsAssert(emp[i].get(), "UNEXPECTED");
  }

  ConstTetrahedronEdgeModelPtr tempx[6];
  ConstTetrahedronEdgeModelPtr tempy[6];
  ConstTetrahedronEdgeModelPtr tempz[6];

  tempx[0] = this->GetConstSelfPtr();
  tempx[1] = reg.GetTetrahedronEdgeModel(x_ModelName1);
  tempx[2] = reg.GetTetrahedronEdgeModel(x_ModelName2);
  tempx[3] = reg.GetTetrahedronEdgeModel(x_ModelName3);
  tempy[0] = reg.GetTetrahedronEdgeModel(y_ModelName0);
  tempy[1] = reg.GetTetrahedronEdgeModel(y_ModelName1);
  tempy[2] = reg.GetTetrahedronEdgeModel(y_ModelName2);
  tempy[3] = reg.GetTetrahedronEdgeModel(y_ModelName3);
  tempz[0] = reg.GetTetrahedronEdgeModel(z_ModelName0);
  tempz[1] = reg.GetTetrahedronEdgeModel(z_ModelName1);
  tempz[2] = reg.GetTetrahedronEdgeModel(z_ModelName2);
  tempz[3] = reg.GetTetrahedronEdgeModel(z_ModelName3);

  for (size_t i = 0; i < 4; ++i)
  {
    dsAssert(tempx[i].get(), "UNEXPECTED");
    dsAssert(tempy[i].get(), "UNEXPECTED");
    dsAssert(tempz[i].get(), "UNEXPECTED");
  }


  const ConstTetrahedronList &tl = GetRegion().GetTetrahedronList();


  std::vector<std::vector<double> > evx(4);
  std::vector<std::vector<double> > evy(4);
  std::vector<std::vector<double> > evz(4);
  for (size_t i = 0; i < 4; ++i)
  {
    evx[i].resize(6*tl.size());
    evy[i].resize(6*tl.size());
    evz[i].resize(6*tl.size());
  }

  const TetrahedronElementField &efield = reg.GetTetrahedronElementField();

  //// For each tetrahedron
  for (size_t i = 0; i < tl.size(); ++i)
  {
    const Tetrahedron &tetrahedron = *tl[i];
    const std::vector<std::vector<Vector > > &v = efield.GetTetrahedronElementField(tetrahedron, *emp[0], *emp[1]);
    for (size_t nindex = 0; nindex < 4; ++nindex)
    {
      for (size_t eindex = 0; eindex < 6; ++eindex)
      {
        const size_t oindex = 6*i + eindex;
        const Vector &vec = v[nindex][eindex];
        evx[nindex][oindex] = vec.Getx();
        evy[nindex][oindex] = vec.Gety();
        evz[nindex][oindex] = vec.Getz();
      }
    }
  }
  for (size_t i = 0; i < 4; ++i)
  {
    std::const_pointer_cast<TetrahedronEdgeModel, const TetrahedronEdgeModel>(tempx[i])->SetValues(evx[i]);
    std::const_pointer_cast<TetrahedronEdgeModel, const TetrahedronEdgeModel>(tempy[i])->SetValues(evy[i]);
    std::const_pointer_cast<TetrahedronEdgeModel, const TetrahedronEdgeModel>(tempz[i])->SetValues(evz[i]);
  }
}

void TetrahedronEdgeFromEdgeModelDerivative::Serialize(std::ostream &of) const
{
  of << "COMMAND element_from_edge_model -device \"" << GetDeviceName() << "\" -region \"" << GetRegionName() << "\" -edge_model \"" << edgeModelName << "\" -derivative \"" << nodeModelName << "\"";
}

