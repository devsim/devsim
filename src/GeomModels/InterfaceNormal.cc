/***
DEVSIM
Copyright 2013 DEVSIM LLC

SPDX-License-Identifier: Apache-2.0
***/

#include "InterfaceNormal.hh"
#include "EdgeSubModel.hh"
#include "NodeModel.hh"
#include "EdgeModel.hh"
#include "Region.hh"
#include "Device.hh"
#include "Interface.hh"
#include "Edge.hh"
#include "Node.hh"

#include "dsAssert.hh"
#include <limits>


template <typename DoubleType>
InterfaceNormal<DoubleType>::InterfaceNormal(const std::string &iname, const std::string &idistname, const std::string &normx, const std::string &normy, const std::string &normz, RegionPtr rp)
    : EdgeModel(idistname, rp, EdgeModel::DisplayType::SCALAR), interface_name(iname)
{
  const size_t dimension = GetRegion().GetDimension();

  if (dimension == 1)
  {
  }
  else if (dimension == 2)
  {
    normal_x = EdgeSubModel<DoubleType>::CreateEdgeSubModel(normx, rp, EdgeModel::DisplayType::SCALAR, this->GetSelfPtr());
    normal_y = EdgeSubModel<DoubleType>::CreateEdgeSubModel(normy, rp, EdgeModel::DisplayType::SCALAR, this->GetSelfPtr());

    RegisterCallback("NSurfaceNormal_x");
    RegisterCallback("NSurfaceNormal_y");
  }
  else if (dimension == 3)
  {
    //// Actually this is off the tetrahedron, not the triangle
//    RegisterCallback("ElementNodeVolume");
    normal_x = EdgeSubModel<DoubleType>::CreateEdgeSubModel(normx, rp, EdgeModel::DisplayType::SCALAR, this->GetSelfPtr());
    normal_y = EdgeSubModel<DoubleType>::CreateEdgeSubModel(normy, rp, EdgeModel::DisplayType::SCALAR, this->GetSelfPtr());
    normal_z = EdgeSubModel<DoubleType>::CreateEdgeSubModel(normz, rp, EdgeModel::DisplayType::SCALAR, this->GetSelfPtr());

    RegisterCallback("NSurfaceNormal_x");
    RegisterCallback("NSurfaceNormal_y");
    RegisterCallback("NSurfaceNormal_z");
  }

  RegisterCallback("SurfaceArea");
  RegisterCallback("@@@InterfaceChange");
}

template <typename DoubleType>
void InterfaceNormal<DoubleType>::calcEdgeScalarValues() const {
  const Device       &device = *GetRegion().GetDevice();
  const size_t dimension = GetRegion().GetDimension();

  dsAssert(dimension != 1, "UNEXPECTED");

  const InterfacePtr iptr = device.GetInterface(interface_name);
  dsAssert(iptr != 0, "MISSING INTERFACE");

  Interface::ConstNodeList_t cnl;
  if (&GetRegion() == iptr->GetRegion0())
  {
    cnl = iptr->GetNodes0();
  }
  else if (&GetRegion() == iptr->GetRegion1())
  {
    cnl = iptr->GetNodes1();
  }
  else
  {
    dsAssert(0, "MISSING REGION FOR INTERFACE");
  }

  //// vector of distances and resulting normals
  const ConstEdgeList &el = GetRegion().GetEdgeList();
  std::vector<DoubleType> distances(el.size(), std::numeric_limits<DoubleType>().max());
  std::vector<Vector<DoubleType>> normals(el.size());
  std::vector<Vector<DoubleType>> edgecenters(el.size());

  for (size_t i = 0; i < el.size(); ++i)
  {
    const Edge &edge = *el[i];
    edgecenters[i] = static_cast<DoubleType>(0.5) * (ConvertVector<DoubleType>(edge.GetHead()->GetCoordinate().Position()) + ConvertVector<DoubleType>(edge.GetTail()->GetCoordinate().Position()));
  }

  ConstNodeModelPtr nx;
  ConstNodeModelPtr ny;
  ConstNodeModelPtr nz;

  NodeScalarList<DoubleType> nxv;
  NodeScalarList<DoubleType> nyv;
  NodeScalarList<DoubleType> nzv;

  if (dimension == 2)
  {
    nx = GetRegion().GetNodeModel("NSurfaceNormal_x");
    ny = GetRegion().GetNodeModel("NSurfaceNormal_y");

    dsAssert(nx.get(), "UNEXPECTED");
    dsAssert(ny.get(), "UNEXPECTED");

    nxv = nx->GetScalarValues<DoubleType>();
    nyv = ny->GetScalarValues<DoubleType>();
    nzv.resize(nyv.size());
  }
  else if (dimension == 3)
  {
    nx = GetRegion().GetNodeModel("NSurfaceNormal_x");
    ny = GetRegion().GetNodeModel("NSurfaceNormal_y");
    nz = GetRegion().GetNodeModel("NSurfaceNormal_z");

    dsAssert(nx.get(), "UNEXPECTED");
    dsAssert(ny.get(), "UNEXPECTED");
    dsAssert(nz.get(), "UNEXPECTED");

    nxv = nx->GetScalarValues<DoubleType>();
    nyv = ny->GetScalarValues<DoubleType>();
    nzv = nz->GetScalarValues<DoubleType>();
  }



  for (Interface::ConstNodeList_t::const_iterator it = cnl.begin(); it != cnl.end(); ++it)
  {
    const Node   &node   = **it;
    const Vector<DoubleType> &inp = ConvertVector<DoubleType>(node.GetCoordinate().Position());

    const size_t node_index = node.GetIndex();

    Vector<DoubleType> normal(nxv[node_index], nyv[node_index], nzv[node_index]);

    for (size_t i = 0; i < el.size(); ++i)
    {
      //// t is the vector from the edge center to the surface node normal
      Vector<DoubleType> t(inp);
      t -= edgecenters[i];

      const DoubleType &dist = dot_prod(t,t);

      DoubleType &d = distances[i];
      if (dist < d)
      {
        d = dist;
        if (dot_prod(t, normal) < 0.0)
        {
          normals[i] = -normal;
        }
        else
        {
          normals[i] =  normal;
        }
      }
    }
  }

  for (size_t i = 0; i < el.size(); ++i)
  {
    distances[i] = sqrt(distances[i]);
  }

  SetValues(distances);

  EdgeScalarList<DoubleType> esl(el.size());
  if (!normal_x.expired())
  {
    for (size_t i = 0; i < el.size(); ++i)
    {
      esl[i] = normals[i].Getx();
    }
    normal_x.lock()->SetValues(esl);
  }
  if (!normal_y.expired())
  {
    for (size_t i = 0; i < el.size(); ++i)
    {
      esl[i] = normals[i].Gety();
    }
    normal_y.lock()->SetValues(esl);
  }
  if (!normal_z.expired())
  {
    for (size_t i = 0; i < el.size(); ++i)
    {
      esl[i] = normals[i].Getz();
    }
    normal_z.lock()->SetValues(esl);
  }

}

template <typename DoubleType>
void InterfaceNormal<DoubleType>::Serialize(std::ostream &of) const
{
  of << "COMMAND interface_normal_model -device \"" << GetDeviceName() << "\" -region \"" << GetRegionName() << "\" -interface \"" << interface_name << "\"";
}

template class InterfaceNormal<double>;
#ifdef DEVSIM_EXTENDED_PRECISION
#include "Float128.hh"
template class InterfaceNormal<float128>;
#endif

EdgeModelPtr CreateInterfaceNormal(const std::string &iname, const std::string &idistname, const std::string &normx, const std::string &normy, const std::string &normz, RegionPtr rp)
{
  const bool use_extended = rp->UseExtendedPrecisionModels();
  return create_edge_model<InterfaceNormal<double>, InterfaceNormal<extended_type>>(use_extended, iname, idistname, normx, normy, normz, rp);
}

