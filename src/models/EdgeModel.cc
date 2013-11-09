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

#include "EdgeModel.hh"
#include "Device.hh"
#include "Region.hh"
#include "Edge.hh"
#include "Contact.hh"
#include "Node.hh"
#include "dsAssert.hh"
#include "FPECheck.hh"
#include "Vector.hh"
#include "GeometryStream.hh"
#include <cmath>

#include <algorithm>

const char *EdgeModel::DisplayTypeString[] = {
  "nodisplay",
  "scalar",
  "vector",
  NULL
};


EdgeModel::~EdgeModel()
{
#if 0
    myregion->UnregisterCallback(name);
#endif
}

EdgeModel::EdgeModel(const std::string &nm, const RegionPtr rp, EdgeModel::DisplayType dt, const ContactPtr cp)
    : name(nm),
      myregion(rp),
      mycontact(cp),
      uptodate(false),
      inprocess(false),
      isuniform(true),
      uniform_value(0.0),
      displayType(dt)
{ 
  myself = rp->AddEdgeModel(this);
  length = GetRegion().GetNumberEdges();

  if (mycontact)
  {
    isuniform = false;
  }
}

void EdgeModel::CalculateValues() const
{
  FPECheck::ClearFPE();
  if (!uptodate)
  {
    inprocess = true;
    this->calcEdgeScalarValues();
    uptodate = true;
    inprocess = false;
  }

  if (FPECheck::CheckFPE())
  {
    std::ostringstream os;
    os << "There was a floating point exception of type \"" << FPECheck::getFPEString() << "\"  while evaluating the edge model " << name
    << " on Device: " << GetRegion().GetDevice()->GetName() << " on Region: " << GetRegion().GetName() << "\n";
    FPECheck::ClearFPE();
    GeometryStream::WriteOut(OutputStream::FATAL, GetRegion(), os.str().c_str());
  }
}

const EdgeScalarList & EdgeModel::GetScalarValues() const
{
  CalculateValues();

  if (isuniform)
  {
    dsAssert(!mycontact, "UNEXPECTED");
    values.clear();
    values.resize(length, uniform_value);
  }

  return values;
}

///// IF provided in general in the expression parser, WARN about missing derivatives
NodeScalarList EdgeModel::GetScalarValuesOnNodes() const
{
//  dsAssert(this->GetDisplayType() == EdgeModel::SCALAR, "UNEXPECTED");

  const Region &region = this->GetRegion();

  const EdgeScalarList &esl = this->GetScalarValues();

  const size_t number_nodes = region.GetNumberNodes();
  const size_t number_edges = region.GetNumberEdges();

  NodeScalarList values(number_nodes);

  /// Does a straight average on the whole
  std::vector<size_t> count(number_nodes);

  const ConstEdgeList &cel = region.GetEdgeList();
  for (size_t i = 0; i < number_edges; ++i)
  {
    const Edge &edge = *cel[i];
    const double val = esl[i];
    size_t ni0 = edge.GetHead()->GetIndex();
    size_t ni1 = edge.GetTail()->GetIndex();
    values[ni0] += val;
    values[ni1] += val;
    ++count[ni0];
    ++count[ni1];
  }
  for (size_t i = 0; i < number_nodes; ++i)
  {
    const size_t c = count[i];
    if (c != 0)
    {
      values[i] /= c;
    }
  }

  return values;
}

///// IF provided in general in the expression parser, WARN about missing derivatives
NodeVectorList EdgeModel::GetVectorValuesOnNodes() const
{
//  dsAssert(this->GetDisplayType() == EdgeModel::VECTOR, "UNEXPECTED");

  const Region &region = this->GetRegion();

  const EdgeScalarList &esl = this->GetScalarValues();

  const size_t number_nodes = region.GetNumberNodes();
  const size_t number_edges = region.GetNumberEdges();

  /// these are the unit vectors on each edge
  EdgeVectorList edge_scaling(number_edges);
  /// these are the accumulated values on each edge
  NodeVectorList node_values(number_nodes);
  /// these are how we divide out the values based on how much they are pointing in our direction
  NodeVectorList node_scaling(number_nodes);


  const size_t dimension = region.GetDimension();

  {
    ConstEdgeModelPtr emx = region.GetEdgeModel("unitx");
    dsAssert(emx.get(), "UNEXPECTED");
    const EdgeScalarList &ex = emx->GetScalarValues();
    for (size_t i = 0; i < number_edges; ++i)
    {
      edge_scaling[i].Setx(ex[i]);
    }
  }

  if (dimension > 1)
  {
    ConstEdgeModelPtr emy = region.GetEdgeModel("unity");
    dsAssert(emy.get(), "UNEXPECTED");
    const EdgeScalarList &ey = emy->GetScalarValues();
    for (size_t i = 0; i < number_edges; ++i)
    {
      edge_scaling[i].Sety(ey[i]);
    }
  }

  if (dimension > 2)
  {
    ConstEdgeModelPtr emz = region.GetEdgeModel("unitz");
    dsAssert(emz.get(), "UNEXPECTED");
    const EdgeScalarList &ez = emz->GetScalarValues();
    for (size_t i = 0; i < number_edges; ++i)
    {
      edge_scaling[i].Setz(ez[i]);
    }
  }


  const ConstEdgeList &cel = region.GetEdgeList();

  for (size_t i = 0; i < number_edges; ++i)
  {
    const Edge &edge = *cel[i];
    //// this is the value to be projected
    const double val = esl[i];
    size_t ni0 = edge.GetHead()->GetIndex();
    size_t ni1 = edge.GetTail()->GetIndex();

    //// this is the unit vector
    const Vector &escal = edge_scaling[i];
    //// project the edge quantity onto the unit vector
    const Vector vval   = val * escal;

    /// This is the scaling based on the direction we are pointing in
    const Vector sval = Vector(fabs(escal.Getx()), fabs(escal.Gety()), fabs(escal.Getz()));

    //// Now we must actually weight each direction
    const Vector nval = Vector(vval.Getx()*sval.Getx(), vval.Gety()*sval.Gety(), vval.Getz()*sval.Getz());

    node_values[ni0]  += nval;
    node_scaling[ni0] += sval;
    node_values[ni1]  += nval;
    node_scaling[ni1] += sval;
  }
  for (size_t i = 0; i < number_nodes; ++i)
  {
    const Vector &scal = node_scaling[i];

    Vector vval = node_values[i];

    if (scal.Getx() > 0.0)
    {
      vval.Setx( vval.Getx() / scal.Getx());
    }
    else
    {
      vval.Setx(0.0);
    }

    if (scal.Gety() > 0.0)
    {
      vval.Sety( vval.Gety() / scal.Gety());
    }
    else
    {
      vval.Sety(0.0);
    }

    if (scal.Getz() > 0.0)
    {
      vval.Setz( vval.Getz() / scal.Getz());
    }
    else
    {
      vval.Setz(0.0);
    }

    node_values[i] = vval;
  }

  /// TODO: optimize by reusing edge scaling vector on the region (e.g., getEdgeScalingVector)

  return node_values;
}

void EdgeModel::SetValues(const EdgeScalarList &nv) const
{
  const_cast<EdgeModel *>(this)->SetValues(nv);
}

void EdgeModel::SetValues(const EdgeScalarList &nv)
{
  if (mycontact)
  {
    values.clear();
    values.resize(length);
    for (std::vector<size_t>::iterator it = atcontact.begin(); it != atcontact.end(); ++it)
    {
      values[*it] = nv[*it];
    }
  }
  else
  {
    values = nv;
  }

  isuniform = false;
  uniform_value = 0.0;

  MarkOld();
  uptodate = true;
}

void EdgeModel::SetValues(const double &v) const
{
  const_cast<EdgeModel *>(this)->SetValues(v);
}

void EdgeModel::SetValues(const double &v)
{
  if (mycontact)
  {
    values.clear();
    values.resize(length);
    for (std::vector<size_t>::iterator it = atcontact.begin(); it != atcontact.end(); ++it)
    {
      values[*it] = v;
    }
  }
  else
  { 
    isuniform = true;
    uniform_value = v;
    values.clear();
  }

  MarkOld();
  uptodate = true;
}


void EdgeModel::MarkOld()
{
  uptodate = false;
  myregion->SignalCallbacks(name);
}

void EdgeModel::MarkOld() const
{
  const_cast<EdgeModel *>(this)->MarkOld();
}

void EdgeModel::RegisterCallback(const std::string &nm)
{
  myregion->RegisterCallback(name, nm);
}

const std::vector<size_t> & EdgeModel::GetContactIndexes() const
{
  if ((mycontact) && (atcontact.empty()))
  {
    const Region::NodeToConstEdgeList_t &nte = myregion->GetNodeToEdgeList();

    const ConstNodeList_t &cnodes = mycontact->GetNodes();
    for (ConstNodeList_t::const_iterator it = cnodes.begin(); it != cnodes.end(); ++it)
    {
      const ConstEdgeList &el  = nte[(*it)->GetIndex()];
      for (ConstEdgeList::const_iterator jt = el.begin(); jt != el.end(); ++jt)
      {
        atcontact.push_back((*jt)->GetIndex());
      }
    }
    std::sort(atcontact.begin(), atcontact.end());
  }

  return atcontact;
}

bool EdgeModel::IsUniform() const
{
  //// We cannot know if we are uniform unless we are uptodate!
  if (!uptodate)
  {
    CalculateValues();
  }

  return isuniform;
}

double EdgeModel::GetUniformValue() const
{
  dsAssert(isuniform, "UNEXPECTED");
  return uniform_value;
}

void EdgeModel::SerializeBuiltIn(std::ostream &of) const
{
  of << "BUILTIN";
}

void EdgeModel::DevsimSerialize(std::ostream &of) const
{
  of << "begin_edge_model \"" << this->GetName() << "\"\n";
  this->Serialize(of);
  of << "\nend_edge_model\n\n";
}

void EdgeModel::SetContact(const ContactPtr cp)
{
  mycontact = cp;
  atcontact.clear();
}

const std::string &EdgeModel::GetDeviceName() const
{
  return GetRegion().GetDevice()->GetName();
}

const std::string &EdgeModel::GetRegionName() const
{
  return GetRegion().GetName();
}

const std::string EdgeModel::GetContactName() const
{
  std::string ret;
  if (mycontact)
  {
  ret = mycontact->GetName();
  }
  return ret;
}

