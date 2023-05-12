/***
DEVSIM
Copyright 2013 DEVSIM LLC

SPDX-License-Identifier: Apache-2.0
***/

#include "EdgeFromNodeModel.hh"
#include "EdgeSubModel.hh"
#include "EdgeScalarData.hh"
#include "NodeModel.hh"
#include "Region.hh"
#include "Edge.hh"
#include "Vector.hh"
#include "dsAssert.hh"
#include "GeometryStream.hh"
#include "ModelErrors.hh"

#include <sstream>


template <typename DoubleType>
EdgeFromNodeModel<DoubleType>::~EdgeFromNodeModel()
{
  // We can't do this, it causes seg fault if the region is already gone
#if 0
  dsErrors::ModelModelDeletion(GetRegion(), edgeModel1Name, dsErrors::ModelInfo::EDGE, GetName(), dsErrors::ModelInfo::EDGE, OutputStream::OutputType::INFO);
  ///// This is EVIL
//// TODO: revisit the deletion issue.
  const_cast<Region &>(GetRegion()).DeleteEdgeModel(edgeModel1Name);
#endif
}

template <typename DoubleType>
EdgeFromNodeModel<DoubleType>::EdgeFromNodeModel(const std::string &edgemodel0, const std::string &edgemodel1, const std::string &nodemodel, const RegionPtr rp)
    :
        EdgeModel(edgemodel0, rp, EdgeModel::DisplayType::SCALAR), // The name of this node model
        nodeModelName(nodemodel),
        edgeModel1Name(edgemodel1)
{
    // The other model is going to be set from our model
//// TODO: issues, use soft references by name here, just like we do other node models
    node1EdgeModel = EdgeSubModel<DoubleType>::CreateEdgeSubModel(edgemodel1, rp, EdgeModel::DisplayType::SCALAR, this->GetSelfPtr());
    dsAssert(!node1EdgeModel.expired(), "UNEXPECTED");

//// This is caught by the api
    dsAssert(rp->GetNodeModel(nodemodel).get(), "UNEXPECTED");
    RegisterCallback(nodeModelName);
#if 0
    os << "creating EdgeFromNodeModelModel " << edgemodel0 << " with parent " << rp->GetNodeModel(nodemodel)->GetName() << "\n";
#endif
}

template <typename DoubleType>
void EdgeFromNodeModel<DoubleType>::calcEdgeScalarValues() const
{
#if 0
        os << "updating EdgeFromNodeModel " << GetName() << " from parent " << nodeModelName << "\n";
#endif
    Region *rp = const_cast<Region *>(&this->GetRegion());
    ConstNodeModelPtr nm = rp->GetNodeModel(nodeModelName);

    if (!nm)
    {
      dsErrors::MissingModelModelDependency(*rp, nodeModelName, dsErrors::ModelInfo::NODE, GetName(),      dsErrors::ModelInfo::EDGE, OutputStream::OutputType::ERROR);
      dsErrors::MissingModelModelDependency(*rp, nodeModelName, dsErrors::ModelInfo::NODE, edgeModel1Name, dsErrors::ModelInfo::EDGE, OutputStream::OutputType::FATAL);
      return;
    }

    const ConstEdgeModelPtr em1 = rp->GetEdgeModel(edgeModel1Name);
    if (!em1)
    {
      dsErrors::ReviveContainer(*rp, edgeModel1Name, dsErrors::ModelInfo::EDGE, GetName(), dsErrors::ModelInfo::EDGE, OutputStream::OutputType::INFO);
      node1EdgeModel = EdgeSubModel<DoubleType>::CreateEdgeSubModel(edgeModel1Name, rp, EdgeModel::DisplayType::SCALAR, this->GetConstSelfPtr());
    }
    else if (node1EdgeModel.expired())
    {
      dsErrors::ChangedModelModelDependency(*rp, nodeModelName, dsErrors::ModelInfo::NODE, GetName(),      dsErrors::ModelInfo::EDGE, OutputStream::OutputType::ERROR);
      dsErrors::ChangedModelModelDependency(*rp, nodeModelName, dsErrors::ModelInfo::NODE, edgeModel1Name, dsErrors::ModelInfo::EDGE, OutputStream::OutputType::FATAL);
      return;
    }


    EdgeScalarList<DoubleType> e0;
    EdgeScalarList<DoubleType> e1;
    createEdgeModelsFromNodeModel(nm->GetScalarValues<DoubleType>(), *rp, e0, e1);
    this->SetValues(e0);
    std::const_pointer_cast<EdgeModel, const EdgeModel>(node1EdgeModel.lock())->SetValues(e1);
}

template <typename DoubleType>
void EdgeFromNodeModel<DoubleType>::Serialize(std::ostream &of) const
{
  of << "COMMAND edge_from_node_model -device \"" << GetDeviceName() << "\" -region \"" << GetRegionName() << "\" -node_model \"" << nodeModelName << "\"";
}

template class EdgeFromNodeModel<double>;
#ifdef DEVSIM_EXTENDED_PRECISION
#include "Float128.hh"
template class EdgeFromNodeModel<float128>;
#endif

EdgeModelPtr CreateEdgeFromNodeModel(const std::string &edgemodel0, const std::string &edgemodel1, const std::string &nodemodel, const RegionPtr rp)
{
  const bool use_extended = rp->UseExtendedPrecisionModels();
  return create_edge_model<EdgeFromNodeModel<double>, EdgeFromNodeModel<extended_type>>(use_extended, edgemodel0, edgemodel1, nodemodel, rp);
}

