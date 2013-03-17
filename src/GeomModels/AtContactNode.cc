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

#include "AtContactNode.hh"
#include "Contact.hh"
#include "Region.hh"
#include "Device.hh"
#include "Node.hh"
#include "GeometryStream.hh"

AtContactNode::AtContactNode(RegionPtr rp)
    : NodeModel("AtContactNode", rp, NodeModel::SCALAR)
{
    RegisterCallback("@@@ContactChange");
}

void AtContactNode::calcNodeScalarValues() const
{
    const Region &region = GetRegion();

    const std::string &rname = region.GetName();

    //// TODO: a region should probably own this
    const Device::ContactList_t &cl = region.GetDevice()->GetContactList();

    const ConstNodeList &nl = region.GetNodeList();
    std::vector<double> nv(nl.size());

    Device::ContactList_t::const_iterator it = cl.begin();
    for ( ; it != cl.end(); ++it)
    {
        if ((it->second->GetRegion()->GetName()) != rname)
        {
            continue;
        }

        const ConstNodeList_t &cnodes = it->second->GetNodes();
        for (ConstNodeList_t::const_iterator jt = cnodes.begin(); jt != cnodes.end(); ++jt)
        {
    
#if 0
            std::ostringstream os; 
            os << region.GetName() << " Node  pointer " << *jt << "\n";
            GeometryStream::WriteOut(OutputStream::INFO, os.str());
#endif

            const size_t index = (*jt)->GetIndex();
            nv[index] += 1.0;
        }
    }
    SetValues(nv);
}

void AtContactNode::setInitialValues()
{
    DefaultInitializeValues();
}

void AtContactNode::Serialize(std::ostream &of) const
{
  SerializeBuiltIn(of);
}

