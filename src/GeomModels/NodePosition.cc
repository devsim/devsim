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
    if (rp->GetDimension() > 1)
    {
        yposition = NodeSolution::CreateNodeSolution("y", rp, this->GetSelfPtr());
    }

    if (rp->GetDimension() > 2)
    {
        zposition = NodeSolution::CreateNodeSolution("z", rp, this->GetSelfPtr());
    }
}

void NodePosition::calcNodeScalarValues() const
{
    const ConstNodeList &nl = GetRegion().GetNodeList();
    std::vector<double> nx(nl.size());
    std::vector<double> ny(nl.size());
    std::vector<double> nz(nl.size());
    for (size_t i = 0; i < nx.size(); ++i)
    {
        const Vector &pos = nl[i]->Position();
        nx[i] = pos.Getx();
        ny[i] = pos.Gety();
        nz[i] = pos.Getz();
    }
    SetValues(nx);
    if (GetRegion().GetDimension() > 1)
    {
        yposition.lock()->SetValues(ny);
    }

    if (GetRegion().GetDimension() > 2)
    {
        zposition.lock()->SetValues(nz);
    }
}

void NodePosition::setInitialValues()
{
    DefaultInitializeValues();
}

void NodePosition::Serialize(std::ostream &of) const
{
  SerializeBuiltIn(of);
}

