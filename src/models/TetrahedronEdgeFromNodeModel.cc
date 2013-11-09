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

#include "TetrahedronEdgeFromNodeModel.hh"
#include "TetrahedronEdgeSubModel.hh"
#include "Region.hh"
#include "NodeModel.hh"
#include "Node.hh"
#include "Edge.hh"
#include "EdgeData.hh"
#include "dsAssert.hh"


// TODO:"TEST THIS"

TetrahedronEdgeFromNodeModel::TetrahedronEdgeFromNodeModel(const std::string &en0, const std::string &en1, const std::string &en2, const std::string &en3, const std::string &nodemodel, RegionPtr rp)
    : TetrahedronEdgeModel(en0, rp, TetrahedronEdgeModel::SCALAR), nodeModelName(nodemodel), edgeModel1Name(en1), edgeModel2Name(en2), edgeModel3Name(en3)
{
  RegisterCallback(nodemodel);
  new TetrahedronEdgeSubModel(en1, rp, this->GetSelfPtr(), TetrahedronEdgeModel::SCALAR);
  new TetrahedronEdgeSubModel(en2, rp, this->GetSelfPtr(), TetrahedronEdgeModel::SCALAR);
  new TetrahedronEdgeSubModel(en3, rp, this->GetSelfPtr(), TetrahedronEdgeModel::SCALAR);
}


void TetrahedronEdgeFromNodeModel::calcTetrahedronEdgeScalarValues() const
{
  const Region &reg = GetRegion();

  const ConstNodeModelPtr nmp = reg.GetNodeModel(nodeModelName);
  dsAssert(nmp.get(), "UNEXPECTED");

  const ConstTetrahedronEdgeModelPtr temp1 = reg.GetTetrahedronEdgeModel(edgeModel1Name);
  dsAssert(temp1.get(), "UNEXPECTED");

  const ConstTetrahedronEdgeModelPtr temp2 = reg.GetTetrahedronEdgeModel(edgeModel2Name);
  dsAssert(temp2.get(), "UNEXPECTED");

  const ConstTetrahedronEdgeModelPtr temp3 = reg.GetTetrahedronEdgeModel(edgeModel3Name);
  dsAssert(temp3.get(), "UNEXPECTED");

  const Region::TetrahedronToConstEdgeDataList_t &ttelist = reg.GetTetrahedronToEdgeDataList();
  const ConstTetrahedronList &tetrahedronList = reg.GetTetrahedronList();
  dsAssert(tetrahedronList.size() == ttelist.size(), "UNEXPECTED");

  const NodeScalarList &nsl = nmp->GetScalarValues();

  std::vector<double> ev0(6*tetrahedronList.size());
  std::vector<double> ev1(6*tetrahedronList.size());
  std::vector<double> ev2(6*tetrahedronList.size());
  std::vector<double> ev3(6*tetrahedronList.size());

  for (size_t i = 0; i < tetrahedronList.size(); ++i)
  {
    const ConstEdgeDataList &edgeDataList = ttelist[i];

    for (size_t j = 0; j < edgeDataList.size(); ++j)
    {
      const EdgeData &edgeData = *edgeDataList[j];
      const Edge &edge = *(edgeData.edge);

      const ConstNodeList &nl = edge.GetNodeList();

      const size_t ni0 = nl[0]->GetIndex();
      const size_t ni1 = nl[1]->GetIndex();

      //// we are guaranteed that the node is across from the edge
      const size_t ni2 = edgeData.nodeopp[0]->GetIndex();
      const size_t ni3 = edgeData.nodeopp[1]->GetIndex();

      const size_t eindex = 6 * i + j;

      ev0[eindex] = nsl[ni0];
      ev1[eindex] = nsl[ni1];
      ev2[eindex] = nsl[ni2];
      ev3[eindex] = nsl[ni3];
    }
  }

  SetValues(ev0);
  std::const_pointer_cast<TetrahedronEdgeModel, const TetrahedronEdgeModel>(temp1)->SetValues(ev1);
  std::const_pointer_cast<TetrahedronEdgeModel, const TetrahedronEdgeModel>(temp2)->SetValues(ev2);
  std::const_pointer_cast<TetrahedronEdgeModel, const TetrahedronEdgeModel>(temp3)->SetValues(ev3);
}

void TetrahedronEdgeFromNodeModel::Serialize(std::ostream &of) const
{
  of << "COMMAND element_from_node_model -device \"" << GetDeviceName() << "\" -region \"" << GetRegionName() << "\" -node_model \"" << nodeModelName << "\"";
}

