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

#include "TriangleEdgeFromEdgeModel.hh"
#include "TriangleEdgeSubModel.hh"
#include "Region.hh"
#include "TriangleElementField.hh"
#include "dsAssert.hh"
#include "Vector.hh"


TriangleEdgeFromEdgeModel::TriangleEdgeFromEdgeModel(const std::string &edgemodel, RegionPtr rp)
    : TriangleEdgeModel(edgemodel + "_x", rp, TriangleEdgeModel::SCALAR), edgeModelName(edgemodel), y_ModelName(edgeModelName + "_y")
{
  RegisterCallback(edgemodel);
  new TriangleEdgeSubModel(y_ModelName, rp, this->GetSelfPtr(), TriangleEdgeModel::SCALAR);
}

//// Need to figure out the deleter situation from sub models
//// Perhaps a Delete SubModels method??????

void TriangleEdgeFromEdgeModel::calcTriangleEdgeScalarValues() const
{
  const Region &reg = GetRegion();

  const ConstEdgeModelPtr emp = reg.GetEdgeModel(edgeModelName);
  dsAssert(emp.get(), "UNEXPECTED");

  const ConstTriangleEdgeModelPtr tempy = reg.GetTriangleEdgeModel(y_ModelName);
  dsAssert(tempy.get(), "UNEXPECTED");

  const ConstTriangleList &tl = GetRegion().GetTriangleList();

  std::vector<double> evx(3*tl.size());
  std::vector<double> evy(3*tl.size());

  const TriangleElementField &efield = reg.GetTriangleElementField();
  for (size_t i = 0; i < tl.size(); ++i)
  {
    const Triangle &triangle = *tl[i];
    const std::vector<Vector> &v = efield.GetTriangleElementField(triangle, *emp);
    for (size_t j = 0; j < 3; ++j)
    {
      evx[3*i + j] = v[j].Getx();
      evy[3*i + j] = v[j].Gety();
    }
  }

  SetValues(evx);
  std::const_pointer_cast<TriangleEdgeModel, const TriangleEdgeModel>(tempy)->SetValues(evy);
}

void TriangleEdgeFromEdgeModel::Serialize(std::ostream &of) const
{
  of << "COMMAND element_from_edge_model -device \"" << GetDeviceName() << "\" -region \"" << GetRegionName() << "\" -edge_model \"" << edgeModelName << "\"";
}

