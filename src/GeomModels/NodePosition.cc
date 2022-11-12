/***
DEVSIM
Copyright 2013 DEVSIM LLC

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

template <typename DoubleType>
NodePosition<DoubleType>::~NodePosition()
{
}

template <typename DoubleType>
NodePosition<DoubleType>::NodePosition(RegionPtr rp)
    : NodeModel("x", rp, NodeModel::DisplayType::SCALAR)
{
  yposition = CreateNodeSolution("y", rp, NodeModel::DisplayType::SCALAR, this->GetSelfPtr());
  zposition = CreateNodeSolution("z", rp, NodeModel::DisplayType::SCALAR, this->GetSelfPtr());
  node_index = CreateNodeSolution("node_index", rp, NodeModel::DisplayType::SCALAR, this->GetSelfPtr());
  coordinate_index = CreateNodeSolution("coordinate_index", rp, NodeModel::DisplayType::SCALAR, this->GetSelfPtr());
}

template <typename DoubleType>
void NodePosition<DoubleType>::calcNodeScalarValues() const
{
    const ConstNodeList &nl = GetRegion().GetNodeList();
    std::vector<DoubleType> nx(nl.size());
    std::vector<DoubleType> ny(nl.size());
    std::vector<DoubleType> nz(nl.size());
    std::vector<DoubleType> ni(nl.size());
    std::vector<DoubleType> ci(nl.size());
    for (size_t i = 0; i < nx.size(); ++i)
    {
        const auto &pos = nl[i]->Position();
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

template <typename DoubleType>
void NodePosition<DoubleType>::setInitialValues()
{
    DefaultInitializeValues();
}

template <typename DoubleType>
void NodePosition<DoubleType>::Serialize(std::ostream &of) const
{
  SerializeBuiltIn(of);
}

template class NodePosition<double>;
#ifdef DEVSIM_EXTENDED_PRECISION
#include "Float128.hh"
template class NodePosition<float128>;
#endif

