/***
DEVSIM
Copyright 2013 Devsim LLC

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
***/

#include "TriangleEdgeModel.hh"
#include "TriangleEdgeScalarData.hh"
#include "Region.hh"
#include "FPECheck.hh"
#include "Device.hh"
#include "dsAssert.hh"
#include "Edge.hh"
#include "Node.hh"
#include "GeometryStream.hh"
#include "Triangle.hh"


const char *TriangleEdgeModel::DisplayTypeString[] = {
  "nodisplay",
  "scalar",
  nullptr
};

TriangleEdgeModel::~TriangleEdgeModel()
{
#if 0
    myregion->UnregisterCallback(name);
#endif
}

bool TriangleEdgeModel::IsZero() const
{
  CalculateValues();
  return model_data.IsUniform() && model_data.IsZero();
}

bool TriangleEdgeModel::IsOne() const
{
  CalculateValues();
  return model_data.IsUniform() && model_data.IsOne();
}

TriangleEdgeModel::TriangleEdgeModel(const std::string &nm, const RegionPtr rp, TriangleEdgeModel::DisplayType dt)
    : name(nm),
      myregion(rp),
      uptodate(false),
      inprocess(false),
      displayType(dt),
      model_data(rp->GetNumberTriangles() * 3)
{ 
  myself = rp->AddTriangleEdgeModel(this);
}

void TriangleEdgeModel::CalculateValues() const
{
  FPECheck::ClearFPE();
  if (!uptodate)
  {
    inprocess = true;
    this->calcTriangleEdgeScalarValues();
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
const TriangleEdgeScalarList<DoubleType> & TriangleEdgeModel::GetScalarValues() const
{
  CalculateValues();

  model_data.expand_uniform();

  return model_data.GetValues<DoubleType>();
}

template <typename DoubleType>
void TriangleEdgeModel::SetValues(const TriangleEdgeScalarList<DoubleType> &nv)
{
  model_data.set_values(nv);

  MarkOld();
  uptodate = true;
}

template <typename DoubleType>
void TriangleEdgeModel::SetValues(const TriangleEdgeScalarList<DoubleType> &nv) const
{
  const_cast<TriangleEdgeModel *>(this)->SetValues(nv);
}

template <typename DoubleType>
void TriangleEdgeModel::SetValues(const DoubleType &v) const
{
  const_cast<TriangleEdgeModel *>(this)->SetValues(v);
}

template <typename DoubleType>
void TriangleEdgeModel::SetValues(const DoubleType &v)
{
  model_data.SetUniformValue(v);

  MarkOld();
  uptodate = true;
}

void TriangleEdgeModel::MarkOld()
{
  uptodate = false;
  myregion->SignalCallbacks(name);
}

void TriangleEdgeModel::MarkOld() const
{
  const_cast<TriangleEdgeModel *>(this)->MarkOld();
}

void TriangleEdgeModel::RegisterCallback(const std::string &nm)
{
  myregion->RegisterCallback(name, nm);
}

template <typename DoubleType>
void TriangleEdgeModel::GetScalarValuesOnNodes(TriangleEdgeModel::InterpolationType interpolation_type, std::vector<DoubleType> &values) const
{
  //// TODO: optimize for uniform and zero

  const Region &region = this->GetRegion();
  const TriangleEdgeScalarList<DoubleType> &esl = this->GetScalarValues<DoubleType>();


  const size_t number_nodes = region.GetNumberNodes();

  values.clear();
  values.resize(number_nodes);

  /// Does a straight average on the whole

  const Region::TriangleToConstEdgeList_t &ttcedl = region.GetTriangleToEdgeList();
  const size_t number_triangle = ttcedl.size();

  if ((interpolation_type == TriangleEdgeModel::InterpolationType::AVERAGE) || (interpolation_type == TriangleEdgeModel::InterpolationType::SUM))
  {
    std::vector<size_t> count(number_nodes);
    for (size_t i = 0; i < number_triangle; ++i)
    {
      const ConstEdgeList &cel = ttcedl[i];
      for (size_t j = 0; j < 3; ++j) 
      {
        const Edge &edge = *cel[j];
        const size_t ni0 = edge.GetHead()->GetIndex();
        const size_t ni1 = edge.GetTail()->GetIndex();
        const DoubleType val = esl[3*i + j];
        values[ni0] += val;
        values[ni1] += val;
        ++count[ni0];
        ++count[ni1];
      }
    }
    if (interpolation_type == TriangleEdgeModel::InterpolationType::AVERAGE)
    {
      for (size_t i = 0; i < number_nodes; ++i)
      {
        const size_t c = count[i];
        if (c != 0)
        {
          values[i] /= c;
        }
      }
    }
  }
  else if (interpolation_type == TriangleEdgeModel::InterpolationType::COUPLE)
  {
    ConstTriangleEdgeModelPtr couple_ptr = region.GetTriangleEdgeModel("ElementEdgeCouple");
    dsAssert(couple_ptr.get(), "UNEXPECTED");
    const TriangleEdgeScalarList<DoubleType> &element_edge_couples = couple_ptr->GetScalarValues<DoubleType>();

    std::vector<DoubleType> scales(number_nodes);

    for (size_t i = 0; i < number_triangle; ++i)
    {
      const ConstEdgeList &cel = ttcedl[i];
      for (size_t j = 0; j < 3; ++j) 
      {
        const Edge &edge = *cel[j];
        const size_t ni0 = edge.GetHead()->GetIndex();
        const size_t ni1 = edge.GetTail()->GetIndex();
        const size_t index = 3*i + j;
        /// The scale must be larger than 0
        const DoubleType scale = element_edge_couples[index];
        const DoubleType val   = scale * esl[index];
        values[ni0] += val;
        values[ni1] += val;
        scales[ni0] += scale;
        scales[ni1] += scale;
      }
    }
    for (size_t i = 0; i < number_nodes; ++i)
    {
      const DoubleType s = scales[i];
      if (s > 0.0)
      {
        values[i] /= s;
      }
      else
      {
        values[i] = 0.0;
      }
    }
  }
  else
  {
    dsAssert(0, "UNEXPECTED");
  }
}

template <typename DoubleType>
void TriangleEdgeModel::GetScalarValuesOnElements(std::vector<DoubleType> &ret) const
{
  const Region &region = this->GetRegion();
  const TriangleEdgeScalarList<DoubleType> &esl = this->GetScalarValues<DoubleType>();


  const size_t number_triangles = region.GetNumberTriangles();

  ret.clear();
  ret.resize(number_triangles);

  const DoubleType scale = static_cast<DoubleType>(1.0) / static_cast<DoubleType>(3.0);
  size_t mindex = 0;
  for (size_t i = 0; i < number_triangles; ++i)
  {
    DoubleType &value = ret[i];
    value += esl[mindex++];
    value += esl[mindex++];
    value += esl[mindex++];
    value *= scale;
  }
}

bool TriangleEdgeModel::IsUniform() const
{
  //// We cannot know if we are uniform unless we are uptodate!
  if (!uptodate)
  {
    CalculateValues();
  }

  return model_data.IsUniform();
}

template <typename DoubleType>
const DoubleType &TriangleEdgeModel::GetUniformValue() const
{
  return model_data.GetUniformValue<DoubleType>();
}

void TriangleEdgeModel::SerializeBuiltIn(std::ostream &of) const
{
  of << "BUILTIN";
}

void TriangleEdgeModel::DevsimSerialize(std::ostream &of) const
{
  of << "begin_triangle_edge_model \"" << this->GetName() << "\"\n";
  this->Serialize(of);
  of << "\nend_triangle_edge_model\n\n";
}

const std::string &TriangleEdgeModel::GetDeviceName() const
{
  return GetRegion().GetDevice()->GetName();
}

const std::string &TriangleEdgeModel::GetRegionName() const
{
  return GetRegion().GetName();
}

template <typename DoubleType>
EdgeScalarList<DoubleType> TriangleEdgeModel::GetValuesOnEdges() const
{
  const TriangleEdgeScalarData<DoubleType> &tec = TriangleEdgeScalarData<DoubleType>(*this);
  const ConstEdgeList &edge_list = GetRegion().GetEdgeList();

  NodeScalarList<DoubleType> ev(edge_list.size());

  for (size_t i = 0; i < edge_list.size(); ++i)
  {
    const ConstTriangleList &triangle_list = GetRegion().GetEdgeToTriangleList()[i];
    ConstTriangleList::const_iterator it = triangle_list.begin();
    const ConstTriangleList::const_iterator itend = triangle_list.end();
    DoubleType vol = 0.0;
    for ( ; it != itend; ++it)
    {
      const size_t tindex = (**it).GetIndex();

      const size_t eindex = GetRegion().GetEdgeIndexOnTriangle(**it, edge_list[i]);

      vol += tec[3*tindex + eindex];
    }
    ev[i] = vol;
  }
  return ev;
}

#define DBLTYPE double
#include "TriangleEdgeModelInstantiate.cc"

#ifdef DEVSIM_EXTENDED_PRECISION
#undef  DBLTYPE
#define DBLTYPE float128
#include "Float128.hh"
#include "TriangleEdgeModelInstantiate.cc"
#endif

