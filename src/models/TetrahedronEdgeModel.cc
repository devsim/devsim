/***
DEVSIM
Copyright 2013 DEVSIM LLC

SPDX-License-Identifier: Apache-2.0
***/

#include "TetrahedronEdgeModel.hh"
#include "Region.hh"
#include "FPECheck.hh"
#include "Device.hh"
#include "dsAssert.hh"
#include "EdgeData.hh"
#include "Edge.hh"
#include "Node.hh"
#include "GeometryStream.hh"
#include "TetrahedronEdgeScalarData.hh"
#include "Tetrahedron.hh"



const char *TetrahedronEdgeModel::DisplayTypeString[] = {
  "nodisplay",
  "scalar",
  nullptr
};

TetrahedronEdgeModel::~TetrahedronEdgeModel()
{
#if 0
    myregion->UnregisterCallback(name);
#endif
}

bool TetrahedronEdgeModel::IsZero() const
{
  CalculateValues();
  return model_data.IsUniform() && model_data.IsZero();
}

bool TetrahedronEdgeModel::IsOne() const
{
  CalculateValues();
  return model_data.IsUniform() && model_data.IsOne();
}

TetrahedronEdgeModel::TetrahedronEdgeModel(const std::string &nm, const RegionPtr rp, TetrahedronEdgeModel::DisplayType dt)
    : name(nm),
      myregion(rp),
      uptodate(false),
      inprocess(false),
      displayType(dt),
      model_data(rp->GetNumberTetrahedrons() * 6)
{
  myself = rp->AddTetrahedronEdgeModel(this);
}

void TetrahedronEdgeModel::CalculateValues() const
{
  FPECheck::ClearFPE();
  if (!uptodate)
  {
    inprocess = true;
    try
    {
        this->calcTetrahedronEdgeScalarValues();
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
const TetrahedronEdgeScalarList<DoubleType> & TetrahedronEdgeModel::GetScalarValues() const
{
  CalculateValues();

  model_data.expand_uniform();

  return model_data.GetValues<DoubleType>();
}

template <typename DoubleType>
void TetrahedronEdgeModel::SetValues(const TetrahedronEdgeScalarList<DoubleType> &nv)
{
  model_data.set_values(nv);

  MarkOld();
  uptodate = true;
}

template <typename DoubleType>
void TetrahedronEdgeModel::SetValues(const TetrahedronEdgeScalarList<DoubleType> &nv) const
{
  const_cast<TetrahedronEdgeModel *>(this)->SetValues(nv);
}

template <typename DoubleType>
void TetrahedronEdgeModel::SetValues(const DoubleType &v) const
{
  const_cast<TetrahedronEdgeModel *>(this)->SetValues(v);
}

template <typename DoubleType>
void TetrahedronEdgeModel::SetValues(const DoubleType &v)
{
  model_data.SetUniformValue(v);

  MarkOld();
  uptodate = true;
}

void TetrahedronEdgeModel::MarkOld()
{
  uptodate = false;
  myregion->SignalCallbacks(name);
}

void TetrahedronEdgeModel::MarkOld() const
{
  const_cast<TetrahedronEdgeModel *>(this)->MarkOld();
}

void TetrahedronEdgeModel::RegisterCallback(const std::string &nm)
{
    myregion->RegisterCallback(name, nm);
}

template <typename DoubleType>
void TetrahedronEdgeModel::GetScalarValuesOnNodes(TetrahedronEdgeModel::InterpolationType interpolation_type, std::vector<DoubleType> &values) const
{
  const Region &region = this->GetRegion();

  const TetrahedronEdgeScalarList<DoubleType> &esl = this->GetScalarValues<DoubleType>();

  const size_t number_nodes = region.GetNumberNodes();
  const size_t number_tetrahedron = region.GetNumberTetrahedrons();

  dsAssert(esl.size() == 6*number_tetrahedron, "UNEXPECTED");

  values.clear();
  values.resize(number_nodes);

  const Region::TetrahedronToConstEdgeDataList_t &ttcedl = region.GetTetrahedronToEdgeDataList();

  if ((interpolation_type == TetrahedronEdgeModel::InterpolationType::AVERAGE) || (interpolation_type == TetrahedronEdgeModel::InterpolationType::SUM))
  {
    /// Does a straight average on the whole
    std::vector<size_t> count(number_nodes);
    for (size_t i = 0; i < number_tetrahedron; ++i)
    {
      const ConstEdgeDataList &cel = ttcedl[i];
      for (size_t j = 0; j < 6; ++j)
      {
        const Edge &edge = *((*cel[j]).edge);
        size_t ni0 = edge.GetHead()->GetIndex();
        size_t ni1 = edge.GetTail()->GetIndex();
        const DoubleType val = esl[6*i + j];
        values[ni0] += val;
        values[ni1] += val;
        ++count[ni0];
        ++count[ni1];
      }
    }

    if (interpolation_type == TetrahedronEdgeModel::InterpolationType::AVERAGE)
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
  else if (interpolation_type == TetrahedronEdgeModel::InterpolationType::COUPLE)
  {
    ConstTetrahedronEdgeModelPtr couple_ptr = region.GetTetrahedronEdgeModel("ElementEdgeCouple");
    dsAssert(couple_ptr.get(), "UNEXPECTED");
    const TetrahedronEdgeScalarList<DoubleType> &element_edge_couples = couple_ptr->GetScalarValues<DoubleType>();

    std::vector<DoubleType> scales(number_nodes);

    for (size_t i = 0; i < number_tetrahedron; ++i)
    {
      const ConstEdgeDataList &cel = ttcedl[i];
      for (size_t j = 0; j < 3; ++j)
      {
        const Edge &edge = *((*cel[j]).edge);
        const size_t ni0 = edge.GetHead()->GetIndex();
        const size_t ni1 = edge.GetTail()->GetIndex();
        const size_t index = 6*i + j;
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
void TetrahedronEdgeModel::GetScalarValuesOnElements(std::vector<DoubleType> &ret) const
{
  const Region &region = this->GetRegion();

  const TetrahedronEdgeScalarList<DoubleType> &esl = this->GetScalarValues<DoubleType>();

//  const size_t number_nodes = region.GetNumberNodes();
  const size_t number_tetrahedron = region.GetNumberTetrahedrons();

//  dsAssert(esl.size() == 6*number_tetrahedron, "UNEXPECTED");

  ret.clear();
  ret.resize(number_tetrahedron);

  const DoubleType scale = static_cast<DoubleType>(1.0) / static_cast<DoubleType>(6.0);
  size_t mindex = 0;
  for (size_t i = 0; i < number_tetrahedron; ++i)
  {
    DoubleType &value = ret[i];
    value += esl[mindex++];
    value += esl[mindex++];
    value += esl[mindex++];
    value += esl[mindex++];
    value += esl[mindex++];
    value += esl[mindex++];
    value *= scale;
  }
}

bool TetrahedronEdgeModel::IsUniform() const
{
  //// We cannot know if we are uniform unless we are uptodate!
  if (!uptodate)
  {
    CalculateValues();
  }

  return model_data.IsUniform();
}

template <typename DoubleType>
const DoubleType &TetrahedronEdgeModel::GetUniformValue() const
{
  return model_data.GetUniformValue<DoubleType>();
}

void TetrahedronEdgeModel::SerializeBuiltIn(std::ostream &of) const
{
  of << "BUILTIN";
}

void TetrahedronEdgeModel::DevsimSerialize(std::ostream &of) const
{
  of << "begin_tetrahedron_edge_model \"" << this->GetName() << "\"\n";
  this->Serialize(of);
  of << "\nend_tetrahedron_edge_model\n\n";
}

const std::string &TetrahedronEdgeModel::GetDeviceName() const
{
  return GetRegion().GetDevice()->GetName();
}

const std::string &TetrahedronEdgeModel::GetRegionName() const
{
  return GetRegion().GetName();
}

template <typename DoubleType>
EdgeScalarList<DoubleType> TetrahedronEdgeModel::GetValuesOnEdges() const
{
  const TetrahedronEdgeScalarData<DoubleType> &tec = TetrahedronEdgeScalarData<DoubleType>(*this);

  const ConstEdgeList &edge_list = GetRegion().GetEdgeList();
  std::vector<DoubleType> ev(edge_list.size());

  for (size_t i = 0; i < edge_list.size(); ++i)
  {
    const ConstTetrahedronList &tl = GetRegion().GetEdgeToTetrahedronList()[i];
    ConstTetrahedronList::const_iterator it = tl.begin();
    const ConstTetrahedronList::const_iterator itend = tl.end();
    DoubleType vol = 0.0;
    for ( ; it != itend; ++it)
    {
      const size_t tindex = (**it).GetIndex();

      const size_t eindex = GetRegion().GetEdgeIndexOnTetrahedron(**it, edge_list[i]);

      vol += tec[6*tindex + eindex];
    }
    ev[i] = vol;
  }

  return ev;
}

#define DBLTYPE double
#include "TetrahedronEdgeModelInstantiate.cc"

#ifdef DEVSIM_EXTENDED_PRECISION
#undef  DBLTYPE
#define DBLTYPE float128
#include "Float128.hh"
#include "TetrahedronEdgeModelInstantiate.cc"
#endif

