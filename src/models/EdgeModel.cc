/***
DEVSIM
Copyright 2013 DEVSIM LLC

SPDX-License-Identifier: Apache-2.0
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
using std::abs;
#include <algorithm>

const char *EdgeModel::DisplayTypeString[] = {
  "nodisplay",
  "scalar",
  "vector",
  nullptr
};

EdgeModel::~EdgeModel()
{
#if 0
    myregion->UnregisterCallback(name);
#endif
}

bool EdgeModel::IsZero() const
{
  CalculateValues();
  return model_data.IsUniform() && model_data.IsZero();
}

bool EdgeModel::IsOne() const
{
  CalculateValues();
  return model_data.IsUniform() && model_data.IsOne();
}

EdgeModel::EdgeModel(const std::string &nm, const RegionPtr rp, EdgeModel::DisplayType dt, const ContactPtr cp)
    : name(nm),
      myregion(rp),
      mycontact(cp),
      uptodate(false),
      inprocess(false),
      displayType(dt),
      model_data(rp->GetNumberEdges())
{
  myself = rp->AddEdgeModel(this);
}

void EdgeModel::CalculateValues() const
{
  FPECheck::ClearFPE();
  if (!uptodate)
  {
    inprocess = true;
    try
    {
        this->calcEdgeScalarValues();
    }
    catch (...)
    {
        inprocess = false;
        throw;
    }
    uptodate = true;
    inprocess = false;
  }

  if (FPECheck::CheckFPE())
  {
    std::ostringstream os;
    os << "There was a floating point exception of type \"" << FPECheck::getFPEString() << "\"  while evaluating the edge model " << name
    << " on Device: " << GetRegion().GetDevice()->GetName() << " on Region: " << GetRegion().GetName() << "\n";
    FPECheck::ClearFPE();
    GeometryStream::WriteOut(OutputStream::OutputType::FATAL, GetRegion(), os.str().c_str());
  }
}

template <typename DoubleType>
const EdgeScalarList<DoubleType> & EdgeModel::GetScalarValues() const
{
  CalculateValues();

  model_data.expand_uniform();

  return model_data.GetValues<DoubleType>();
}

///// IF provided in general in the expression parser, WARN about missing derivatives
template <typename DoubleType>
NodeScalarList<DoubleType> EdgeModel::GetScalarValuesOnNodes() const
{
//  dsAssert(this->GetDisplayType() == EdgeModel::DisplayType::SCALAR, "UNEXPECTED");

  const Region &region = this->GetRegion();

  const EdgeScalarList<DoubleType> &esl = this->GetScalarValues<DoubleType>();

  const size_t number_nodes = region.GetNumberNodes();
  const size_t number_edges = region.GetNumberEdges();

  NodeScalarList<DoubleType> values(number_nodes);

  /// Does a straight average on the whole
  std::vector<size_t> count(number_nodes);

  const ConstEdgeList &cel = region.GetEdgeList();
  for (size_t i = 0; i < number_edges; ++i)
  {
    const Edge &edge = *cel[i];
    const DoubleType val = esl[i];
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
template <typename DoubleType>
NodeVectorList<DoubleType> EdgeModel::GetVectorValuesOnNodes() const
{
//  dsAssert(this->GetDisplayType() == EdgeModel::VECTOR, "UNEXPECTED");

  const Region &region = this->GetRegion();

  const EdgeScalarList<DoubleType> &esl = this->GetScalarValues<DoubleType>();

  const size_t number_nodes = region.GetNumberNodes();
  const size_t number_edges = region.GetNumberEdges();

  /// these are the unit vectors on each edge
  EdgeVectorList<DoubleType> edge_scaling(number_edges);
  /// these are the accumulated values on each edge
  NodeVectorList<DoubleType> node_values(number_nodes);
  /// these are how we divide out the values based on how much they are pointing in our direction
  NodeVectorList<DoubleType> node_scaling(number_nodes);


  const size_t dimension = region.GetDimension();

  {
    ConstEdgeModelPtr emx = region.GetEdgeModel("unitx");
    dsAssert(emx.get(), "UNEXPECTED");
    const EdgeScalarList<DoubleType> &ex = emx->GetScalarValues<DoubleType>();
    for (size_t i = 0; i < number_edges; ++i)
    {
      edge_scaling[i].Setx(ex[i]);
    }
  }

  if (dimension > 1)
  {
    ConstEdgeModelPtr emy = region.GetEdgeModel("unity");
    dsAssert(emy.get(), "UNEXPECTED");
    const EdgeScalarList<DoubleType> &ey = emy->GetScalarValues<DoubleType>();
    for (size_t i = 0; i < number_edges; ++i)
    {
      edge_scaling[i].Sety(ey[i]);
    }
  }

  if (dimension > 2)
  {
    ConstEdgeModelPtr emz = region.GetEdgeModel("unitz");
    dsAssert(emz.get(), "UNEXPECTED");
    const EdgeScalarList<DoubleType> &ez = emz->GetScalarValues<DoubleType>();
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
    const DoubleType val = esl[i];
    size_t ni0 = edge.GetHead()->GetIndex();
    size_t ni1 = edge.GetTail()->GetIndex();

    //// this is the unit vector
    const Vector<DoubleType> &escal = edge_scaling[i];
    //// project the edge quantity onto the unit vector
    const Vector<DoubleType> vval   = val * escal;

    /// This is the scaling based on the direction we are pointing in
    const Vector<DoubleType> sval = Vector<DoubleType>(abs(escal.Getx()), abs(escal.Gety()), abs(escal.Getz()));

    //// Now we must actually weight each direction
    const Vector<DoubleType> nval = Vector<DoubleType>(vval.Getx()*sval.Getx(), vval.Gety()*sval.Gety(), vval.Getz()*sval.Getz());

    node_values[ni0]  += nval;
    node_scaling[ni0] += sval;
    node_values[ni1]  += nval;
    node_scaling[ni1] += sval;
  }
  for (size_t i = 0; i < number_nodes; ++i)
  {
    const Vector<DoubleType> &scal = node_scaling[i];

    Vector<DoubleType> vval = node_values[i];

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

template <typename DoubleType>
void EdgeModel::SetValues(const EdgeScalarList<DoubleType> &nv) const
{
  const_cast<EdgeModel *>(this)->SetValues(nv);
}

template <typename DoubleType>
void EdgeModel::SetValues(const EdgeScalarList<DoubleType> &nv)
{
  if (mycontact)
  {
    GetContactIndexes();
    model_data.set_indexes(atcontact, nv);
  }
  else
  {
    model_data.set_values(nv);
  }

  MarkOld();
  uptodate = true;
}

template <typename DoubleType>
void EdgeModel::SetValues(const DoubleType &v) const
{
  const_cast<EdgeModel *>(this)->SetValues(v);
}

template <typename DoubleType>
void EdgeModel::SetValues(const DoubleType &v)
{
  if (mycontact)
  {
    GetContactIndexes();
    model_data.set_indexes(atcontact, v);
  }
  else
  {
    model_data.SetUniformValue<DoubleType>(v);
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

  return model_data.IsUniform();
}

template <typename DoubleType>
const DoubleType &EdgeModel::GetUniformValue() const
{
  return model_data.GetUniformValue<DoubleType>();
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
  uptodate = false;
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

#define DBLTYPE double
#include "EdgeModelInstantiate.cc"

#ifdef DEVSIM_EXTENDED_PRECISION
#undef  DBLTYPE
#define DBLTYPE float128
#include "Float128.hh"
#include "EdgeModelInstantiate.cc"
#endif

