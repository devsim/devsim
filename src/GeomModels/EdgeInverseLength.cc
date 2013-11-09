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

#include "EdgeInverseLength.hh"
#include "EdgeModel.hh"
#include "Vector.hh"
#include "Region.hh"
#include "Edge.hh"
#include "dsAssert.hh"

EdgeInverseLength::EdgeInverseLength(RegionPtr rp) :
EdgeModel("EdgeInverseLength", rp, EdgeModel::SCALAR)
{
    RegisterCallback("EdgeLength");
}


void EdgeInverseLength::calcEdgeScalarValues() const
{
    ConstEdgeModelPtr elen = GetRegion().GetEdgeModel("EdgeLength");
    dsAssert(elen.get(), "UNEXPECTED");

    const EdgeScalarList &evals = elen->GetScalarValues();

    std::vector<double> ev(evals.size());


    for (size_t i = 0; i < ev.size(); ++i)
    {
        ev[i] = 1.0 / evals[i];
    }
    SetValues(ev);
}

void EdgeInverseLength::Serialize(std::ostream &of) const
{
  SerializeBuiltIn(of);
}

