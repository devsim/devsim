/***
DEVSIM
Copyright 2013 DEVSIM LLC

SPDX-License-Identifier: Apache-2.0
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


const char *VectorGradientEnum::CalcTypeString[] = {
  "default",
  "avoidzero",
  nullptr
};

template <typename DoubleType>
VectorGradient<DoubleType>::VectorGradient(RegionPtr rp, const std::string &name, VectorGradientEnum::CalcType ct)
    : NodeModel(name + "_gradx", rp, NodeModel::DisplayType::SCALAR), parentname_(name),  calctype_(ct)
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
      yfield_ = CreateNodeSolution(name + "_grady", rp, NodeModel::DisplayType::SCALAR, this->GetSelfPtr());
    }
    else if (dimension == 3)
    {
      RegisterCallback("unity");
      RegisterCallback("unitz");
      yfield_ = CreateNodeSolution(name + "_grady", rp, NodeModel::DisplayType::SCALAR, this->GetSelfPtr());
      zfield_ = CreateNodeSolution(name + "_gradz", rp, NodeModel::DisplayType::SCALAR, this->GetSelfPtr());
    }
}


template <typename DoubleType>
void VectorGradient<DoubleType>::calc1d() const
{
  const Region &r = GetRegion();
  ConstNodeModelPtr scalarField = r.GetNodeModel(parentname_);
  ConstEdgeModelPtr edgeSign    = r.GetEdgeModel("unitx");
  ConstEdgeModelPtr edgeInv     = r.GetEdgeModel("EdgeInverseLength");
  dsAssert(scalarField.get(), "UNEXPECTED");
  dsAssert(edgeSign.get(), "UNEXPECTED");
  dsAssert(edgeInv.get(), "UNEXPECTED");

  const ConstEdgeList &el = r.GetEdgeList();

  EdgeScalarData<DoubleType> vgrad(*edgeSign);
  vgrad.times_equal_model(*edgeInv);

  const NodeScalarList<DoubleType> &nsl = scalarField->GetScalarValues<DoubleType>();

  std::vector<size_t> zlist(el.size(), 1);

  for (size_t i = 0; i < el.size(); ++i)
  {
    const Edge &edge = *el[i];
    const DoubleType s0 = nsl[edge.GetHead()->GetIndex()];
    const DoubleType s1 = nsl[edge.GetTail()->GetIndex()];

    vgrad[i] *= (s1 - s0);

    if ((calctype_ == VectorGradientEnum::AVOIDZERO) && ((s0 == 0.0) || (s1 == 0.0)))
    {
      zlist[i] = 0;
    }
  }

  NodeScalarList<DoubleType> vx(nsl.size());
  const Region::NodeToConstEdgeList_t &ntelist = r.GetNodeToEdgeList();
  for (size_t i = 0; i < vx.size(); ++i)
  {
    const ConstEdgeList &el = ntelist[i];
    size_t count = 0;
    DoubleType val = 0.0;
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

template <typename DoubleType>
void VectorGradient<DoubleType>::calc2d() const
{

  const Region &r = GetRegion();

  ConstNodeModelPtr scalarField = r.GetNodeModel(parentname_);
  dsAssert(scalarField.get(), "UNEXPECTED");

  const NodeScalarList<DoubleType> &nsl = scalarField->GetScalarValues<DoubleType>();

  NodeScalarList<DoubleType> vx(nsl.size());
  NodeScalarList<DoubleType> vy(nsl.size());

  const GradientField<DoubleType> &gfield = r.GetGradientField<DoubleType>();
  const ConstTriangleList &tl = GetRegion().GetTriangleList();
  std::vector<size_t> zlist(tl.size(), 1);

  std::vector<std::complex<DoubleType> > tvals(tl.size());

  for (ConstTriangleList::const_iterator tit = tl.begin(); tit != tl.end(); ++tit)
  {
    Vector<DoubleType> grad = gfield.GetGradient(**tit, *scalarField);
    tvals[(*tit)->GetIndex()] = std::complex<DoubleType>(grad.Getx(), grad.Gety());
  }

  const Region::NodeToConstTriangleList_t &nttlist = r.GetNodeToTriangleList();
  if (calctype_ == VectorGradientEnum::AVOIDZERO)
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
    std::complex<DoubleType> val;
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
      val /= static_cast<DoubleType>(count);
      vx[i] = val.real();
      vy[i] = val.imag();
    }
  }

  SetValues(vx);
  yfield_.lock()->SetValues(vy);
}

template <typename DoubleType>
void VectorGradient<DoubleType>::calc3d() const
{

  const Region &r = GetRegion();

  ConstNodeModelPtr scalarField = r.GetNodeModel(parentname_);
  dsAssert(scalarField.get(), "UNEXPECTED");

  const NodeScalarList<DoubleType> &nsl = scalarField->GetScalarValues<DoubleType>();

  NodeScalarList<DoubleType> vx(nsl.size());
  NodeScalarList<DoubleType> vy(nsl.size());
  NodeScalarList<DoubleType> vz(nsl.size());

  const GradientField<DoubleType> &gfield = r.GetGradientField<DoubleType>();
  const ConstTetrahedronList &tl = GetRegion().GetTetrahedronList();
  std::vector<size_t> zlist(tl.size(), 1);

  std::vector<Vector<DoubleType>> tvals(tl.size());

  for (ConstTetrahedronList::const_iterator tit = tl.begin(); tit != tl.end(); ++tit)
  {
    Vector<DoubleType> grad = gfield.GetGradient(**tit, *scalarField);
    tvals[(*tit)->GetIndex()] = Vector<DoubleType>(grad.Getx(), grad.Gety(), grad.Getz());
  }

  const Region::NodeToConstTetrahedronList_t &nttlist = r.GetNodeToTetrahedronList();
  if (calctype_ == VectorGradientEnum::AVOIDZERO)
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
    Vector<DoubleType> val(0);
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

template <typename DoubleType>
void VectorGradient<DoubleType>::calcNodeScalarValues() const
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

template <typename DoubleType>
void VectorGradient<DoubleType>::setInitialValues()
{
    DefaultInitializeValues();
}

template <typename DoubleType>
void VectorGradient<DoubleType>::Serialize(std::ostream &of) const
{

  of << "COMMAND vector_gradient -device \"" << GetDeviceName() << "\" -region \"" << GetRegionName() << "\" -calc_type \"" << VectorGradientEnum::CalcTypeString[calctype_] << "\" -node_model \"" << parentname_ << "\"";
}

template class VectorGradient<double>;
#ifdef DEVSIM_EXTENDED_PRECISION
#include "Float128.hh"
template class VectorGradient<float128>;
#endif

NodeModelPtr CreateVectorGradient(RegionPtr rp, const std::string &name, VectorGradientEnum::CalcType ct)
{
  const bool use_extended = rp->UseExtendedPrecisionModels();
  return create_node_model<VectorGradient<double>, VectorGradient<extended_type>>(use_extended, rp, name, ct);
}


