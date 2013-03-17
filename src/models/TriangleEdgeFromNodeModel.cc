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

#include "TriangleEdgeFromNodeModel.hh"
#include "TriangleEdgeSubModel.hh"
#include "Region.hh"
#include "Node.hh"
#include "Edge.hh"
#include "Triangle.hh"
#include "NodeModel.hh"
#include "dsAssert.hh"

// TODO:"TEST THIS"



TriangleEdgeFromNodeModel::TriangleEdgeFromNodeModel(const std::string &edgemodel0, const std::string &edgemodel1, const std::string &edgemodel2, const std::string &nodemodel, RegionPtr rp)
    : TriangleEdgeModel(edgemodel0, rp, TriangleEdgeModel::NODISPLAY), nodeModelName(nodemodel), edgeModel1Name(edgemodel1), edgeModel2Name(edgemodel2)
{
  RegisterCallback(nodemodel);
  new TriangleEdgeSubModel(edgeModel1Name, rp, this->GetSelfPtr(), TriangleEdgeModel::NODISPLAY);
  new TriangleEdgeSubModel(edgeModel2Name, rp, this->GetSelfPtr(), TriangleEdgeModel::NODISPLAY);
}

//// Need to figure out the deleter situation from sub models
//// Perhaps a Delete SubModels method??????

void TriangleEdgeFromNodeModel::calcTriangleEdgeScalarValues() const
{
  const Region &reg = GetRegion();

#if 0
  const Device &dev = *reg.GetDevice();
  const size_t dimension = dev.GetDimension();
  dsAssert(dimension == 2, "UNEXPECTED");
#endif

  const ConstNodeModelPtr nmp = reg.GetNodeModel(nodeModelName);
  dsAssert(nmp, "UNEXPECTED");


  const ConstTriangleEdgeModelPtr temp1 = reg.GetTriangleEdgeModel(edgeModel1Name);
  dsAssert(temp1, "UNEXPECTED");

  const ConstTriangleEdgeModelPtr temp2 = reg.GetTriangleEdgeModel(edgeModel2Name);
  dsAssert(temp2, "UNEXPECTED");

  const Region::TriangleToConstEdgeList_t &ttelist = reg.GetTriangleToEdgeList();
  const ConstTriangleList &triangleList = reg.GetTriangleList();

  dsAssert(triangleList.size() == ttelist.size(), "UNEXPECTED");

  std::vector<double> ev0(3*triangleList.size());
  std::vector<double> ev1(3*triangleList.size());
  std::vector<double> ev2(3*triangleList.size());

  const NodeScalarList &nsl = nmp->GetScalarValues();

  for (size_t i = 0; i < triangleList.size(); ++i)
  {
    const ConstEdgeList &el = ttelist[i];
    const Triangle &triangle = *triangleList[i];
    const ConstNodeList &tnl = triangle.GetNodeList();

    for (size_t j = 0; j < el.size(); ++j)
    {
      const size_t eindex = 3 * i + j;

      const ConstNodeList &nl = el[j]->GetNodeList();

      const size_t ni0 = nl[0]->GetIndex();
      const size_t ni1 = nl[1]->GetIndex();

      //// we are guaranteed that the node is across from the edge
      const size_t ni2 = tnl[j]->GetIndex();

      ev0[eindex] = nsl[ni0];
      ev1[eindex] = nsl[ni1];
      ev2[eindex] = nsl[ni2];
    }
  }

  SetValues(ev0);
  //// TODO: take care of const problem once and for all
  std::tr1::const_pointer_cast<TriangleEdgeModel, const TriangleEdgeModel>(temp1)->SetValues(ev1);
  std::tr1::const_pointer_cast<TriangleEdgeModel, const TriangleEdgeModel>(temp2)->SetValues(ev2);
}

void TriangleEdgeFromNodeModel::Serialize(std::ostream &of) const
{
  of << "COMMAND element_from_node_model -device \"" << GetDeviceName() << "\" -region \"" << GetRegionName() << "\" -node_model \"" << nodeModelName << "\"";
}

