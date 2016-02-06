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

#include "VectorGradient.hh"
#include "NodeSolution.hh"
#include "Region.hh"
#include "NodeScalarData.hh"
#include "dsAssert.hh"
#include "Vector.hh"
#include "Triangle.hh"
#include "Tetrahedron.hh"
#include "Node.hh"
#include "Edge.hh"
#include "EdgeModel.hh"
#include "EdgeScalarData.hh"
#include "GradientField.hh"
#include <complex>


const char *VectorGradient::CalcTypeString[] = {
  "default",
  "avoidzero",
  NULL
};

VectorGradient::VectorGradient(RegionPtr rp, const std::string &name, CalcType ct)
    : NodeModel(name + "_gradx", rp, NodeModel::SCALAR), parentname_(name),  calctype_(ct)
{
    /// This is the scalar we are taking the gradient of
    RegisterCallback(name);

    RegisterCallback("EdgeInverseLength");
    RegisterCallback("unitx");
    //// unitx calculates unity and unitz
    
    const size_t dimension = GetRegion().GetDimension();
    if (dimension == 2)
    {
      RegisterCallback("unity");
      yfield_ = NodeSolution::CreateNodeSolution(name + "_grady", rp, this->GetSelfPtr());
    }
    else if (dimension == 3)
    {
      RegisterCallback("unity");
      RegisterCallback("unitz");
      yfield_ = NodeSolution::CreateNodeSolution(name + "_grady", rp, this->GetSelfPtr());
      zfield_ = NodeSolution::CreateNodeSolution(name + "_gradz", rp, this->GetSelfPtr());
    }
}


void VectorGradient::calc1d() const
{
  const Region &r = GetRegion();
  ConstNodeModelPtr scalarField = r.GetNodeModel(parentname_);
  ConstEdgeModelPtr edgeSign    = r.GetEdgeModel("unitx");
  ConstEdgeModelPtr edgeInv     = r.GetEdgeModel("EdgeInverseLength");
  dsAssert(scalarField.get(), "UNEXPECTED");
  dsAssert(edgeSign.get(), "UNEXPECTED");
  dsAssert(edgeInv.get(), "UNEXPECTED");

  const ConstEdgeList &el = r.GetEdgeList();

  EdgeScalarData vgrad(*edgeSign);
  vgrad *= *edgeInv;

  const NodeScalarList &nsl = scalarField->GetScalarValues();

  std::vector<size_t> zlist(el.size(), 1);

  for (size_t i = 0; i < el.size(); ++i)
  {
    const Edge &edge = *el[i];
    const double s0 = nsl[edge.GetHead()->GetIndex()];
    const double s1 = nsl[edge.GetTail()->GetIndex()];

    vgrad[i] *= (s1 - s0);

    if ((calctype_ == AVOIDZERO) && ((s0 == 0.0) || (s1 == 0.0)))
    {
      zlist[i] = 0;
    }
  }

  NodeScalarList vx(nsl.size());
  const Region::NodeToConstEdgeList_t &ntelist = r.GetNodeToEdgeList();
  for (size_t i = 0; i < vx.size(); ++i)
  {
    const ConstEdgeList &el = ntelist[i];
    size_t count = 0;
    double val = 0.0;
    for (ConstEdgeList::const_iterator eit = el.begin(); eit != el.end(); ++eit)
    {
      const size_t eindex = (*eit)->GetIndex();
      if (zlist[eindex] != 0)
      {
        val += vgrad[eindex];
        ++count;
      }
      if (count != 0)
      {
        val /= count;
        vx[i] = val;
      }
    }
  }

  this->SetValues(vx);
}

void VectorGradient::calc2d() const
{

  const Region &r = GetRegion();

  ConstNodeModelPtr scalarField = r.GetNodeModel(parentname_);
  dsAssert(scalarField.get(), "UNEXPECTED");

  const NodeScalarList &nsl = scalarField->GetScalarValues();

  NodeScalarList vx(nsl.size());
  NodeScalarList vy(nsl.size());

  const GradientField &gfield = r.GetGradientField();
  const ConstTriangleList &tl = GetRegion().GetTriangleList();
  std::vector<size_t> zlist(tl.size(), 1);

  std::vector<std::complex<double> > tvals(tl.size());

  for (ConstTriangleList::const_iterator tit = tl.begin(); tit != tl.end(); ++tit)
  {
    Vector grad = gfield.GetGradient(**tit, *scalarField);
    tvals[(*tit)->GetIndex()] = std::complex<double>(grad.Getx(), grad.Gety());
  }

  const Region::NodeToConstTriangleList_t &nttlist = r.GetNodeToTriangleList();
  if (calctype_ == AVOIDZERO)
  {
    for (size_t i = 0; i < nsl.size(); ++i)
    {
      if (nsl[i] == 0.0)
      {
        const ConstTriangleList &tlist = nttlist[i];
        for (ConstTriangleList::const_iterator tit = tlist.begin(); tit != tlist.end(); ++tit)
        {
          zlist[(*tit)->GetIndex()] = 0;
        }
      }
    }
  }

  for (size_t i = 0; i < nsl.size(); ++i)
  {
    std::complex<double> val = 0.0;
    size_t count = 0;

    const ConstTriangleList &tlist = nttlist[i];
    for (ConstTriangleList::const_iterator tit = tlist.begin(); tit != tlist.end(); ++tit)
    {
      size_t tindex = (*tit)->GetIndex();

      if (zlist[tindex] != 0)
      {
        val += tvals[tindex];
        ++count;
      }
    }
    if (count != 0)
    {
      val /= static_cast<double>(count);
      vx[i] = val.real();
      vy[i] = val.imag();
    }
  }

  SetValues(vx);
  yfield_.lock()->SetValues(vy);
}

// TODO:"REVIEW, TEST CODE"
void VectorGradient::calc3d() const
{

  const Region &r = GetRegion();

  ConstNodeModelPtr scalarField = r.GetNodeModel(parentname_);
  dsAssert(scalarField.get(), "UNEXPECTED");

  const NodeScalarList &nsl = scalarField->GetScalarValues();

  NodeScalarList vx(nsl.size());
  NodeScalarList vy(nsl.size());
  NodeScalarList vz(nsl.size());

  const GradientField &gfield = r.GetGradientField();
  const ConstTetrahedronList &tl = GetRegion().GetTetrahedronList();
  std::vector<size_t> zlist(tl.size(), 1);

  std::vector<Vector> tvals(tl.size());

  for (ConstTetrahedronList::const_iterator tit = tl.begin(); tit != tl.end(); ++tit)
  {
    Vector grad = gfield.GetGradient(**tit, *scalarField);
    tvals[(*tit)->GetIndex()] = Vector(grad.Getx(), grad.Gety(), grad.Getz());
  }

  const Region::NodeToConstTetrahedronList_t &nttlist = r.GetNodeToTetrahedronList();
  if (calctype_ == AVOIDZERO)
  {
    for (size_t i = 0; i < nsl.size(); ++i)
    {
      if (nsl[i] == 0.0)
      {
        const ConstTetrahedronList &tlist = nttlist[i];
        for (ConstTetrahedronList::const_iterator tit = tlist.begin(); tit != tlist.end(); ++tit)
        {
          zlist[(*tit)->GetIndex()] = 0;
        }
      }
    }
  }

  for (size_t i = 0; i < nsl.size(); ++i)
  {
    Vector val(0);
    size_t count = 0;

    const ConstTetrahedronList &tlist = nttlist[i];
    for (ConstTetrahedronList::const_iterator tit = tlist.begin(); tit != tlist.end(); ++tit)
    {
      size_t tindex = (*tit)->GetIndex();

      if (zlist[tindex] != 0)
      {
        val += tvals[tindex];
        ++count;
      }
    }
    if (count != 0)
    {
      val /= count;
      vx[i] = val.Getx();
      vy[i] = val.Gety();
      vz[i] = val.Getz();
    }
  }

  SetValues(vx);
  yfield_.lock()->SetValues(vy);
  zfield_.lock()->SetValues(vz);
}

void VectorGradient::calcNodeScalarValues() const
{
  const Region &r = GetRegion();
  const size_t dimension = r.GetDimension();

  if (dimension == 1)
  {
    calc1d();
  }
  else if (dimension == 2)
  {
    calc2d();
  }
  else if (dimension == 3)
  {
    calc3d();
  }
  else
  {
    dsAssert(0, "UNEXPECTED");
  }

}

void VectorGradient::setInitialValues()
{
    DefaultInitializeValues();
}

void VectorGradient::Serialize(std::ostream &of) const
{
  
  of << "COMMAND vector_gradient -device \"" << GetDeviceName() << "\" -region \"" << GetRegionName() << "\" -calc_type \"" << CalcTypeString[calctype_] << "\" -node_model \"" << parentname_ << "\"";
}

