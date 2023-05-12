/***
DEVSIM
Copyright 2013 DEVSIM LLC

SPDX-License-Identifier: Apache-2.0
***/

#include "AtContactNode.hh"
#include "Contact.hh"
#include "Region.hh"
#include "Device.hh"
#include "Node.hh"
#include "GeometryStream.hh"

template <typename DoubleType>
AtContactNode<DoubleType>::AtContactNode(RegionPtr rp)
    : NodeModel("AtContactNode", rp, NodeModel::DisplayType::SCALAR)
{
    RegisterCallback("@@@ContactChange");
}

template <typename DoubleType>
void AtContactNode<DoubleType>::calcNodeScalarValues() const
{
    const Region &region = GetRegion();

    const std::string &rname = region.GetName();

    //// TODO: a region should probably own this
    const Device::ContactList_t &cl = region.GetDevice()->GetContactList();

    const ConstNodeList &nl = region.GetNodeList();
    std::vector<DoubleType> nv(nl.size());

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
            GeometryStream::WriteOut(OutputStream::OutputType::INFO, os.str());
#endif

            const size_t index = (*jt)->GetIndex();
            nv[index] += 1.0;
        }
    }
    SetValues(nv);
}

template <typename DoubleType>
void AtContactNode<DoubleType>::setInitialValues()
{
    DefaultInitializeValues();
}

template <typename DoubleType>
void AtContactNode<DoubleType>::Serialize(std::ostream &of) const
{
  SerializeBuiltIn(of);
}

template class AtContactNode<double>;
#ifdef DEVSIM_EXTENDED_PRECISION
#include "Float128.hh"
template class AtContactNode<float128>;
#endif


