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


InterfaceNormal::InterfaceNormal(const std::string &iname, const std::string &idistname, const std::string &normx, const std::string &normy, const std::string &normz, RegionPtr rp)
    : EdgeModel(idistname, rp, EdgeModel::SCALAR), interface_name(iname)
{
  const size_t dimension = GetRegion().GetDimension();

  if (dimension == 1)
  {
  }
  else if (dimension == 2)
  {
    //// TODO: make sure to handle delete model
    //// perhaps this model is a dependent model and can't be deleted
    normal_x = EdgeSubModel::CreateEdgeSubModel(normx, rp, EdgeModel::SCALAR, this->GetSelfPtr());
    normal_y = EdgeSubModel::CreateEdgeSubModel(normy, rp, EdgeModel::SCALAR, this->GetSelfPtr());

    RegisterCallback("NSurfaceNormal_x");
    RegisterCallback("NSurfaceNormal_y");
  }
  else if (dimension == 3)
  {
    //// Actually this is off the tetrahedron, not the triangle
//    RegisterCallback("ElementNodeVolume");
    normal_x = EdgeSubModel::CreateEdgeSubModel(normx, rp, EdgeModel::SCALAR, this->GetSelfPtr());
    normal_y = EdgeSubModel::CreateEdgeSubModel(normy, rp, EdgeModel::SCALAR, this->GetSelfPtr());
    normal_z = EdgeSubModel::CreateEdgeSubModel(normz, rp, EdgeModel::SCALAR, this->GetSelfPtr());

    RegisterCallback("NSurfaceNormal_x");
    RegisterCallback("NSurfaceNormal_y");
    RegisterCallback("NSurfaceNormal_z");
  }

  RegisterCallback("SurfaceArea");
  RegisterCallback("@@@InterfaceChange");
}

void InterfaceNormal::calcEdgeScalarValues() const {
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
  std::vector<double> distances(el.size(), std::numeric_limits<double>().max());
  std::vector<Vector> normals(el.size());
  std::vector<Vector> edgecenters(el.size());

  for (size_t i = 0; i < el.size(); ++i)
  {
    const Edge &edge = *el[i];
    edgecenters[i] = 0.5 * (edge.GetHead()->GetCoordinate().Position() + edge.GetTail()->GetCoordinate().Position());
  }

  ConstNodeModelPtr nx;
  ConstNodeModelPtr ny;
  ConstNodeModelPtr nz;

  NodeScalarList nxv;
  NodeScalarList nyv;
  NodeScalarList nzv;

  if (dimension == 2)
  {
    nx = GetRegion().GetNodeModel("NSurfaceNormal_x");
    ny = GetRegion().GetNodeModel("NSurfaceNormal_y");

    /// TODO: put missing model messages here
    dsAssert(nx.get(), "UNEXPECTED");
    dsAssert(ny.get(), "UNEXPECTED");

    nxv = nx->GetScalarValues();
    nyv = ny->GetScalarValues();
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

    nxv = nx->GetScalarValues();
    nyv = ny->GetScalarValues();
    nzv = nz->GetScalarValues();
  }

  

  for (Interface::ConstNodeList_t::const_iterator it = cnl.begin(); it != cnl.end(); ++it)
  {
    const Node   &node   = **it;
    const Vector &inp    = node.GetCoordinate().Position();
  
    const size_t node_index = node.GetIndex();

    Vector normal(nxv[node_index], nyv[node_index], nzv[node_index]);

    for (size_t i = 0; i < el.size(); ++i)
    {
      //// t is the vector from the edge center to the surface node normal
      Vector t(inp);
      t -= edgecenters[i];

      const double &dist = dot_prod(t,t);

      double &d = distances[i];
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

  EdgeScalarList esl(el.size());
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

void InterfaceNormal::Serialize(std::ostream &of) const
{
  of << "COMMAND interface_normal_model -device \"" << GetDeviceName() << "\" -region \"" << GetRegionName() << "\" -interface \"" << interface_name << "\"";
}

