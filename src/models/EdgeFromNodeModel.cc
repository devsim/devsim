/***
DEVSIM
Copyright 2013 Devsim LLC

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
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


EdgeFromNodeModel::~EdgeFromNodeModel()
{
  // We can't do this, it causes seg fault if the region is already gone
#if 0
  dsErrors::ModelModelDeletion(GetRegion(), edgeModel1Name, dsErrors::ModelInfo::EDGE, GetName(), dsErrors::ModelInfo::EDGE, OutputStream::INFO);
  ///// This is EVIL
//// TODO: revisit the deletion issue.
  const_cast<Region &>(GetRegion()).DeleteEdgeModel(edgeModel1Name);
#endif
}

EdgeFromNodeModel::EdgeFromNodeModel(const std::string &edgemodel0, const std::string &edgemodel1, const std::string &nodemodel, const RegionPtr rp)
    :
        EdgeModel(edgemodel0, rp, EdgeModel::SCALAR), // The name of this node model
        nodeModelName(nodemodel),
        edgeModel1Name(edgemodel1)
{
    // The other model is going to be set from our model
//// TODO: issues, use soft references by name here, just like we do other node models
    node1EdgeModel = EdgeSubModel::CreateEdgeSubModel(edgemodel1, rp, EdgeModel::SCALAR, this->GetSelfPtr());
    dsAssert(!node1EdgeModel.expired(), "UNEXPECTED");

//// This is caught by the api
    dsAssert(rp->GetNodeModel(nodemodel).get(), "UNEXPECTED");
    RegisterCallback(nodeModelName);
#if 0
    os << "creating EdgeFromNodeModelModel " << edgemodel0 << " with parent " << rp->GetNodeModel(nodemodel)->GetName() << "\n";
#endif
}

void EdgeFromNodeModel::calcEdgeScalarValues() const
{
#if 0
        os << "updating EdgeFromNodeModel " << GetName() << " from parent " << nodeModelName << "\n";
#endif
    Region *rp = const_cast<Region *>(&this->GetRegion());
    ConstNodeModelPtr nm = rp->GetNodeModel(nodeModelName);

    if (!nm)
    {
      dsErrors::MissingModelModelDependency(*rp, nodeModelName, dsErrors::ModelInfo::NODE, GetName(),      dsErrors::ModelInfo::EDGE, OutputStream::ERROR);
      dsErrors::MissingModelModelDependency(*rp, nodeModelName, dsErrors::ModelInfo::NODE, edgeModel1Name, dsErrors::ModelInfo::EDGE, OutputStream::FATAL);
    }

    const ConstEdgeModelPtr em1 = rp->GetEdgeModel(edgeModel1Name);
    if (!em1)
    {
      dsErrors::ReviveContainer(*rp, edgeModel1Name, dsErrors::ModelInfo::EDGE, GetName(), dsErrors::ModelInfo::EDGE, OutputStream::INFO);
      node1EdgeModel = EdgeSubModel::CreateEdgeSubModel(edgeModel1Name, rp, EdgeModel::SCALAR, this->GetConstSelfPtr());
    }
    else if (node1EdgeModel.expired())
    {
      dsErrors::ChangedModelModelDependency(*rp, nodeModelName, dsErrors::ModelInfo::NODE, GetName(),      dsErrors::ModelInfo::EDGE, OutputStream::ERROR);
      dsErrors::ChangedModelModelDependency(*rp, nodeModelName, dsErrors::ModelInfo::NODE, edgeModel1Name, dsErrors::ModelInfo::EDGE, OutputStream::FATAL);
    }


    EdgeScalarList e0;
    EdgeScalarList e1;
    createEdgeModelsFromNodeModel(nm->GetScalarValues(), *rp, e0, e1);
    this->SetValues(e0);
    std::const_pointer_cast<EdgeModel, const EdgeModel>(node1EdgeModel.lock())->SetValues(e1);
}

void EdgeFromNodeModel::Serialize(std::ostream &of) const
{
  of << "COMMAND edge_from_node_model -device \"" << GetDeviceName() << "\" -region \"" << GetRegionName() << "\" -node_model \"" << nodeModelName << "\"";
}

