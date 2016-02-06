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

#include "NodePosition.hh"
#include "NodeSolution.hh"
#include "Region.hh"
#include "Node.hh"
#include "Vector.hh"

NodePosition::~NodePosition()
{
}

NodePosition::NodePosition(RegionPtr rp)
    : NodeModel("x", rp, NodeModel::SCALAR)
{
  yposition = NodeSolution::CreateNodeSolution("y", rp, this->GetSelfPtr());
  zposition = NodeSolution::CreateNodeSolution("z", rp, this->GetSelfPtr());
  node_index = NodeSolution::CreateNodeSolution("node_index", rp, this->GetSelfPtr());
  coordinate_index = NodeSolution::CreateNodeSolution("coordinate_index", rp, this->GetSelfPtr());
}

void NodePosition::calcNodeScalarValues() const
{
    const ConstNodeList &nl = GetRegion().GetNodeList();
    std::vector<double> nx(nl.size());
    std::vector<double> ny(nl.size());
    std::vector<double> nz(nl.size());
    std::vector<double> ni(nl.size());
    std::vector<double> ci(nl.size());
    for (size_t i = 0; i < nx.size(); ++i)
    {
        const Vector &pos = nl[i]->Position();
        nx[i] = pos.Getx();
        ny[i] = pos.Gety();
        nz[i] = pos.Getz();
        ni[i] = i;
        ci[i] = nl[i]->GetCoordinate().GetIndex();
    }
    SetValues(nx);
    yposition.lock()->SetValues(ny);
    zposition.lock()->SetValues(nz);
    node_index.lock()->SetValues(ni);
    coordinate_index.lock()->SetValues(ci);
}

void NodePosition::setInitialValues()
{
    DefaultInitializeValues();
}

void NodePosition::Serialize(std::ostream &of) const
{
  SerializeBuiltIn(of);
}

