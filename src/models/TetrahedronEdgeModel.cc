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
  NULL
};

TetrahedronEdgeModel::~TetrahedronEdgeModel()
{
#if 0
    myregion->UnregisterCallback(name);
#endif
}

// TODO:"Make display type configurable"
TetrahedronEdgeModel::TetrahedronEdgeModel(const std::string &nm, const RegionPtr rp, TetrahedronEdgeModel::DisplayType dt)
    : name(nm),
      myregion(rp),
      uptodate(false),
      inprocess(false),
      isuniform(true),
      uniform_value(0.0),
      displayType(dt)
{ 
  myself = rp->AddTetrahedronEdgeModel(this);
  length = GetRegion().GetNumberTetrahedrons() * 6;
}

void TetrahedronEdgeModel::CalculateValues() const
{
  FPECheck::ClearFPE();
  if (!uptodate)
  {
    inprocess = true;
    this->calcTetrahedronEdgeScalarValues();
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

const TetrahedronEdgeScalarList & TetrahedronEdgeModel::GetScalarValues() const
{
  CalculateValues();

  if (isuniform)
  {
    values.clear();
    values.resize(length, uniform_value);
  }

  return values;
}

void TetrahedronEdgeModel::SetValues(const TetrahedronEdgeScalarList &nv)
{
  values = nv;

  isuniform = false;
  uniform_value = 0.0;

  MarkOld();
  uptodate = true;
}

void TetrahedronEdgeModel::SetValues(const TetrahedronEdgeScalarList &nv) const
{
  const_cast<TetrahedronEdgeModel *>(this)->SetValues(nv);
}

void TetrahedronEdgeModel::SetValues(const double &v) const
{
  const_cast<TetrahedronEdgeModel *>(this)->SetValues(v);
}

void TetrahedronEdgeModel::SetValues(const double &v)
{
  values.clear();
  isuniform = true;
  uniform_value = v;

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

void TetrahedronEdgeModel::GetScalarValuesOnNodes(TetrahedronEdgeModel::InterpolationType interpolation_type, std::vector<double> &values) const
{
  const Region &region = this->GetRegion();

  const TetrahedronEdgeScalarList &esl = this->GetScalarValues();

  const size_t number_nodes = region.GetNumberNodes();
  const size_t number_tetrahedron = region.GetNumberTetrahedrons();

  dsAssert(esl.size() == 6*number_tetrahedron, "UNEXPECTED");

  values.clear();
  values.resize(number_nodes);

  const Region::TetrahedronToConstEdgeDataList_t &ttcedl = region.GetTetrahedronToEdgeDataList();

  if ((interpolation_type == TetrahedronEdgeModel::AVERAGE) || (interpolation_type == TetrahedronEdgeModel::SUM))
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
        const double val = esl[6*i + j];
        values[ni0] += val;
        values[ni1] += val;
        ++count[ni0];
        ++count[ni1];
      }
    }

    if (interpolation_type == TetrahedronEdgeModel::AVERAGE)
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
  else if (interpolation_type == TetrahedronEdgeModel::COUPLE)
  {
    ConstTetrahedronEdgeModelPtr couple_ptr = region.GetTetrahedronEdgeModel("ElementEdgeCouple");
    dsAssert(couple_ptr, "UNEXPECTED");
    const TetrahedronEdgeScalarList &element_edge_couples = couple_ptr->GetScalarValues();

    std::vector<double> scales(number_nodes);

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
        const double scale = element_edge_couples[index];
        const double val   = scale * esl[index];
        values[ni0] += val;
        values[ni1] += val;
        scales[ni0] += scale;
        scales[ni1] += scale;
      }
    }
    for (size_t i = 0; i < number_nodes; ++i)
    {
      const double s = scales[i];
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

void TetrahedronEdgeModel::GetScalarValuesOnElements(std::vector<double> &ret) const
{
  const Region &region = this->GetRegion();

  const TetrahedronEdgeScalarList &esl = this->GetScalarValues();

//  const size_t number_nodes = region.GetNumberNodes();
  const size_t number_tetrahedron = region.GetNumberTetrahedrons();

//  dsAssert(esl.size() == 6*number_tetrahedron, "UNEXPECTED");

  ret.clear();
  ret.resize(number_tetrahedron);

  const double scale = 1.0 / 6.0;
  size_t mindex = 0;
  for (size_t i = 0; i < number_tetrahedron; ++i)
  {
    double &value = ret[i];
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

  return isuniform;
}

double TetrahedronEdgeModel::GetUniformValue() const
{
  dsAssert(isuniform, "UNEXPECTED");
  return uniform_value;
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

EdgeScalarList TetrahedronEdgeModel::GetValuesOnEdges() const
{
  const TetrahedronEdgeScalarData &tec = TetrahedronEdgeScalarData(*this);

  const ConstEdgeList &edge_list = GetRegion().GetEdgeList();
  std::vector<double> ev(edge_list.size());

  for (size_t i = 0; i < edge_list.size(); ++i)
  {
    const ConstTetrahedronList &tl = GetRegion().GetEdgeToTetrahedronList()[i];
    ConstTetrahedronList::const_iterator it = tl.begin();
    const ConstTetrahedronList::const_iterator itend = tl.end();
    double vol = 0.0;
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

