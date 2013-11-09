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

#include "TriangleEdgeFromEdgeModelDerivative.hh"
#include "TriangleEdgeSubModel.hh"
#include "Region.hh"
#include "TriangleElementField.hh"
#include "dsAssert.hh"
#include "Vector.hh"



// TODO:"TEST THIS"

TriangleEdgeFromEdgeModelDerivative::TriangleEdgeFromEdgeModelDerivative(
        const std::string &edgemodel,
        const std::string &derivative,
        RegionPtr rp
    )
    : TriangleEdgeModel(edgemodel + "_x:" + derivative + "@en0", rp, TriangleEdgeModel::NODISPLAY),
      edgeModelName(edgemodel),
      nodeModelName(derivative)
{
  const std::string tmx = edgeModelName + "_x";
  const std::string tmy = edgeModelName + "_y";

  const std::string tmpname = edgeModelName + ":" + nodeModelName;
  edgeModelName0 = tmpname + "@n0";
  edgeModelName1 = tmpname + "@n1";

  x_ModelName1 = tmx + ":" + nodeModelName + "@en1";
  x_ModelName2 = tmx + ":" + nodeModelName + "@en2";
  y_ModelName0 = tmy + ":" + nodeModelName + "@en0";
  y_ModelName1 = tmy + ":" + nodeModelName + "@en1";
  y_ModelName2 = tmy + ":" + nodeModelName + "@en2";

  RegisterCallback(edgeModelName0);
  RegisterCallback(edgeModelName1);
  new TriangleEdgeSubModel(x_ModelName1, rp, this->GetSelfPtr(), TriangleEdgeModel::NODISPLAY);
  new TriangleEdgeSubModel(x_ModelName2, rp, this->GetSelfPtr(), TriangleEdgeModel::NODISPLAY);
  new TriangleEdgeSubModel(y_ModelName0, rp, this->GetSelfPtr(), TriangleEdgeModel::NODISPLAY);
  new TriangleEdgeSubModel(y_ModelName1, rp, this->GetSelfPtr(), TriangleEdgeModel::NODISPLAY);
  new TriangleEdgeSubModel(y_ModelName2, rp, this->GetSelfPtr(), TriangleEdgeModel::NODISPLAY);
}

//// Need to figure out the deleter situation from sub models
//// Perhaps a Delete SubModels method??????

void TriangleEdgeFromEdgeModelDerivative::calcTriangleEdgeScalarValues() const
{
  const Region &reg = GetRegion();

  ConstEdgeModelPtr emp[2];
  emp[0] = reg.GetEdgeModel(edgeModelName0);
  emp[1] = reg.GetEdgeModel(edgeModelName1);

  for (size_t i = 0; i < 2; ++i)
  {
    dsAssert(emp[i].get(), "UNEXPECTED");
  }

  ConstTriangleEdgeModelPtr tempx[3];
  ConstTriangleEdgeModelPtr tempy[3];

  tempx[0] = this->GetConstSelfPtr();
  tempx[1] = reg.GetTriangleEdgeModel(x_ModelName1);
  tempx[2] = reg.GetTriangleEdgeModel(x_ModelName2);
  tempy[0] = reg.GetTriangleEdgeModel(y_ModelName0);
  tempy[1] = reg.GetTriangleEdgeModel(y_ModelName1);
  tempy[2] = reg.GetTriangleEdgeModel(y_ModelName2);

  for (size_t i = 0; i < 3; ++i)
  {
    dsAssert(tempx[i].get(), "UNEXPECTED");
    dsAssert(tempy[i].get(), "UNEXPECTED");
  }


  const ConstTriangleList &tl = GetRegion().GetTriangleList();

  std::vector<std::vector<double> > evx(3);
  std::vector<std::vector<double> > evy(3);
  for (size_t i = 0; i < 3; ++i)
  {
    evx[i].resize(3*tl.size());
    evy[i].resize(3*tl.size());
  }

  const TriangleElementField &efield = reg.GetTriangleElementField();

  //// For each triangle
  for (size_t i = 0; i < tl.size(); ++i)
  {
    const Triangle &triangle = *tl[i];
    const std::vector<std::vector<Vector> > &v = efield.GetTriangleElementField(triangle, *emp[0], *emp[1]);
    for (size_t nindex = 0; nindex < 3; ++nindex)
    {
      for (size_t eindex = 0; eindex < 3; ++eindex)
      {
        const size_t oindex = 3*i + eindex;
        const Vector &vec = v[nindex][eindex];
        evx[nindex][oindex] = vec.Getx();
        evy[nindex][oindex] = vec.Gety();
      }
    }
  }
  for (size_t i = 0; i < 3; ++i)
  {
    std::const_pointer_cast<TriangleEdgeModel, const TriangleEdgeModel>(tempx[i])->SetValues(evx[i]);
    std::const_pointer_cast<TriangleEdgeModel, const TriangleEdgeModel>(tempy[i])->SetValues(evy[i]);
  }
}

void TriangleEdgeFromEdgeModelDerivative::Serialize(std::ostream &of) const
{
  of << "COMMAND element_from_edge_model -device \"" << GetDeviceName() << "\" -region \"" << GetRegionName() << "\" -edge_model \"" << edgeModelName << "\" -derivative \"" << nodeModelName << "\"";
}

