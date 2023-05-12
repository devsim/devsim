/***
DEVSIM
Copyright 2013 DEVSIM LLC

SPDX-License-Identifier: Apache-2.0
***/

#include "TriangleEdgeFromNodeModel.hh"
#include "TriangleEdgeSubModel.hh"
#include "Region.hh"
#include "Node.hh"
#include "Edge.hh"
#include "Triangle.hh"
#include "NodeModel.hh"
#include "dsAssert.hh"

template <typename DoubleType>
TriangleEdgeFromNodeModel<DoubleType>::TriangleEdgeFromNodeModel(const std::string &edgemodel0, const std::string &edgemodel1, const std::string &edgemodel2, const std::string &nodemodel, RegionPtr rp)
    : TriangleEdgeModel(edgemodel0, rp, TriangleEdgeModel::DisplayType::SCALAR), nodeModelName(nodemodel), edgeModel1Name(edgemodel1), edgeModel2Name(edgemodel2)
{
  RegisterCallback(nodemodel);
  new TriangleEdgeSubModel<DoubleType>(edgeModel1Name, rp, TriangleEdgeModel::DisplayType::SCALAR, this->GetSelfPtr());
  new TriangleEdgeSubModel<DoubleType>(edgeModel2Name, rp, TriangleEdgeModel::DisplayType::SCALAR, this->GetSelfPtr());
}

//// Need to figure out the deleter situation from sub models
//// Perhaps a Delete SubModels method??????

template <typename DoubleType>
void TriangleEdgeFromNodeModel<DoubleType>::calcTriangleEdgeScalarValues() const
{
  const Region &reg = GetRegion();

#if 0
  const Device &dev = *reg.GetDevice();
  const size_t dimension = dev.GetDimension();
  dsAssert(dimension == 2, "UNEXPECTED");
#endif

  const ConstNodeModelPtr nmp = reg.GetNodeModel(nodeModelName);
  dsAssert(nmp.get(), "UNEXPECTED");


  const ConstTriangleEdgeModelPtr temp1 = reg.GetTriangleEdgeModel(edgeModel1Name);
  dsAssert(temp1.get(), "UNEXPECTED");

  const ConstTriangleEdgeModelPtr temp2 = reg.GetTriangleEdgeModel(edgeModel2Name);
  dsAssert(temp2.get(), "UNEXPECTED");

  const Region::TriangleToConstEdgeList_t &ttelist = reg.GetTriangleToEdgeList();
  const ConstTriangleList &triangleList = reg.GetTriangleList();

  dsAssert(triangleList.size() == ttelist.size(), "UNEXPECTED");

  std::vector<DoubleType> ev0(3*triangleList.size());
  std::vector<DoubleType> ev1(3*triangleList.size());
  std::vector<DoubleType> ev2(3*triangleList.size());

  const NodeScalarList<DoubleType> &nsl = nmp->GetScalarValues<DoubleType>();

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
  std::const_pointer_cast<TriangleEdgeModel, const TriangleEdgeModel>(temp1)->SetValues(ev1);
  std::const_pointer_cast<TriangleEdgeModel, const TriangleEdgeModel>(temp2)->SetValues(ev2);
}

template <typename DoubleType>
void TriangleEdgeFromNodeModel<DoubleType>::Serialize(std::ostream &of) const
{
  of << "COMMAND element_from_node_model -device \"" << GetDeviceName() << "\" -region \"" << GetRegionName() << "\" -node_model \"" << nodeModelName << "\"";
}

template class TriangleEdgeFromNodeModel<double>;
#ifdef DEVSIM_EXTENDED_PRECISION
#include "Float128.hh"
template class TriangleEdgeFromNodeModel<float128>;
#endif

TriangleEdgeModelPtr CreateTriangleEdgeFromNodeModel(const std::string &en0, const std::string &en1, const std::string &en2, const std::string &nodemodel, RegionPtr rp)
{
  const bool use_extended = rp->UseExtendedPrecisionModels();
  return create_triangle_edge_model<TriangleEdgeFromNodeModel<double>, TriangleEdgeFromNodeModel<extended_type>>(use_extended, en0, en1, en2, nodemodel, rp);
}

