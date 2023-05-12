/***
DEVSIM
Copyright 2013 DEVSIM LLC

SPDX-License-Identifier: Apache-2.0
***/

#include "TetrahedronEdgeFromNodeModel.hh"
#include "TetrahedronEdgeSubModel.hh"
#include "Region.hh"
#include "NodeModel.hh"
#include "Node.hh"
#include "Edge.hh"
#include "EdgeData.hh"
#include "dsAssert.hh"

template <typename DoubleType>
TetrahedronEdgeFromNodeModel<DoubleType>::TetrahedronEdgeFromNodeModel(const std::string &en0, const std::string &en1, const std::string &en2, const std::string &en3, const std::string &nodemodel, RegionPtr rp)
    : TetrahedronEdgeModel(en0, rp, TetrahedronEdgeModel::DisplayType::SCALAR), nodeModelName(nodemodel), edgeModel1Name(en1), edgeModel2Name(en2), edgeModel3Name(en3)
{
  RegisterCallback(nodemodel);
  new TetrahedronEdgeSubModel<DoubleType>(en1, rp, TetrahedronEdgeModel::DisplayType::SCALAR, this->GetSelfPtr());
  new TetrahedronEdgeSubModel<DoubleType>(en2, rp, TetrahedronEdgeModel::DisplayType::SCALAR, this->GetSelfPtr());
  new TetrahedronEdgeSubModel<DoubleType>(en3, rp, TetrahedronEdgeModel::DisplayType::SCALAR, this->GetSelfPtr());
}


template <typename DoubleType>
void TetrahedronEdgeFromNodeModel<DoubleType>::calcTetrahedronEdgeScalarValues() const
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

  const NodeScalarList<DoubleType> &nsl = nmp->GetScalarValues<DoubleType>();

  std::vector<DoubleType> ev0(6*tetrahedronList.size());
  std::vector<DoubleType> ev1(6*tetrahedronList.size());
  std::vector<DoubleType> ev2(6*tetrahedronList.size());
  std::vector<DoubleType> ev3(6*tetrahedronList.size());

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

template <typename DoubleType>
void TetrahedronEdgeFromNodeModel<DoubleType>::Serialize(std::ostream &of) const
{
  of << "COMMAND element_from_node_model -device \"" << GetDeviceName() << "\" -region \"" << GetRegionName() << "\" -node_model \"" << nodeModelName << "\"";
}

template class TetrahedronEdgeFromNodeModel<double>;
#ifdef DEVSIM_EXTENDED_PRECISION
#include "Float128.hh"
template class TetrahedronEdgeFromNodeModel<float128>;
#endif

TetrahedronEdgeModelPtr CreateTetrahedronEdgeFromNodeModel(const std::string &en0, const std::string &en1, const std::string &en2, const std::string &en3, const std::string &nodemodel, RegionPtr rp)
{
  const bool use_extended = rp->UseExtendedPrecisionModels();
  return create_tetrahedron_edge_model<TetrahedronEdgeFromNodeModel<double>, TetrahedronEdgeFromNodeModel<extended_type>>(use_extended, en0, en1, en2, en3, nodemodel, rp);
}

