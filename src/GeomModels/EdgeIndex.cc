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

#include "EdgeIndex.hh"
#include "EdgeModel.hh"
#include "Vector.hh"
#include "Region.hh"
#include "Edge.hh"
#include "Node.hh"

EdgeIndex::EdgeIndex(RegionPtr rp) :
EdgeModel("edge_index", rp, EdgeModel::SCALAR)
{
}


void EdgeIndex::calcEdgeScalarValues() const
{
    const ConstEdgeList &el = GetRegion().GetEdgeList();
    std::vector<double> ev(el.size());
    for (size_t i = 0; i < ev.size(); ++i)
    {
        ev[i] = calcEdgeIndex(el[i]);
    }
    SetValues(ev);
}

double EdgeIndex::calcEdgeIndex(ConstEdgePtr ep) const
{
    const double val = ep->GetIndex();
    return val;
}

void EdgeIndex::Serialize(std::ostream &of) const
{
  SerializeBuiltIn(of);
}

