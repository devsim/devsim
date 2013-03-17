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

#include "EdgeLength.hh"
#include "EdgeModel.hh"
#include "Vector.hh"
#include "Region.hh"
#include "Edge.hh"
#include "Node.hh"

EdgeLength::EdgeLength(RegionPtr rp) :
EdgeModel("EdgeLength", rp, EdgeModel::SCALAR)
{
}


void EdgeLength::calcEdgeScalarValues() const
{
    const ConstEdgeList &el = GetRegion().GetEdgeList();
    std::vector<double> ev(el.size());
    for (size_t i = 0; i < ev.size(); ++i)
    {
        ev[i] = calcEdgeLength(el[i]);
    }
    SetValues(ev);
}

double EdgeLength::calcEdgeLength(ConstEdgePtr ep) const
{
    Vector vm = ep->GetNodeList()[0]->Position();
    vm -= ep->GetNodeList()[1]->Position();
    const double val = vm.magnitude();
    return val;
}

void EdgeLength::Serialize(std::ostream &of) const
{
  SerializeBuiltIn(of);
}

