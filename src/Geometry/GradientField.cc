/***
DEVSIM
Copyright 2013 DEVSIM LLC

SPDX-License-Identifier: Apache-2.0
***/

#include "GradientField.hh"
#include "DenseMatrix.hh"
#include "Region.hh"
#include "NodeModel.hh"
#include "dsAssert.hh"
#include "Triangle.hh"
#include "Tetrahedron.hh"
#include "Node.hh"

template <typename DoubleType>
GradientField<DoubleType>::~GradientField()
{
  for (typename std::vector<dsMath::RealDenseMatrix<DoubleType> *>::iterator it = dense_mats_.begin(); it != dense_mats_.end(); ++it)
  {
    delete *it;
  }
}

template <typename DoubleType>
GradientField<DoubleType>::GradientField(const Region *r) : myregion_(r)
{
}

template <typename DoubleType>
void GradientField<DoubleType>::CalcMatrices2d() const
{
  dsAssert(GetRegion().GetDimension() == 2, "UNEXPECTED");
  //// TODO:Check for FPE's
  //// Assert dimension is 2!
  //// Assert fields exist
  ConstNodeModelPtr ux = myregion_->GetNodeModel("x");
  ConstNodeModelPtr uy = myregion_->GetNodeModel("y");

  dsAssert(ux.get(), "UNEXPECTED");
  dsAssert(uy.get(), "UNEXPECTED");

  const NodeScalarList<DoubleType> &xvec = ux->GetScalarValues<DoubleType>();
  const NodeScalarList<DoubleType> &yvec = uy->GetScalarValues<DoubleType>();


  const ConstTriangleList &tlist = myregion_->GetTriangleList();

  dense_mats_.resize(tlist.size());

  for (ConstTriangleList::const_iterator ti = tlist.begin(); ti != tlist.end(); ++ti)
  {
    const Triangle &triangle = **ti;
    const size_t triangleIndex = triangle.GetIndex();

    auto *dmp = new dsMath::RealDenseMatrix<DoubleType>(3);
    dsMath::RealDenseMatrix<DoubleType> &M = *dmp;

    const std::vector<ConstNodePtr> &nl = triangle.GetNodeList();

    const size_t ni0 = nl[0]->GetIndex();
    const DoubleType x0 = xvec[ni0];
    const DoubleType y0 = yvec[ni0];
    for (size_t r = 1; r < 3; ++r)
    {
      const size_t nir = nl[r]->GetIndex();
      const DoubleType xr = xvec[nir] - x0;
      const DoubleType yr = yvec[nir] - y0;

      M(r, 0) = xr;
      M(r, 1) = yr;
      M(r, 2) = 1.0;
    }
    M(0, 2) = 1.0;
    M.LUFactor();

    dense_mats_[triangleIndex] = dmp;

  }
}

template <typename DoubleType>
void GradientField<DoubleType>::CalcMatrices3d() const
{
  dsAssert(GetRegion().GetDimension() == 3, "UNEXPECTED");
  //// TODO:Check for FPE's
  //// Assert dimension is 2!
  //// Assert fields exist
  ConstNodeModelPtr ux = myregion_->GetNodeModel("x");
  ConstNodeModelPtr uy = myregion_->GetNodeModel("y");
  ConstNodeModelPtr uz = myregion_->GetNodeModel("y");

  dsAssert(ux.get(), "UNEXPECTED");
  dsAssert(uy.get(), "UNEXPECTED");
  dsAssert(uz.get(), "UNEXPECTED");

  const NodeScalarList<DoubleType> &xvec = ux->GetScalarValues<DoubleType>();
  const NodeScalarList<DoubleType> &yvec = uy->GetScalarValues<DoubleType>();
  const NodeScalarList<DoubleType> &zvec = uz->GetScalarValues<DoubleType>();


  const ConstTetrahedronList &tlist = myregion_->GetTetrahedronList();

  dense_mats_.resize(tlist.size());

  for (ConstTetrahedronList::const_iterator ti = tlist.begin(); ti != tlist.end(); ++ti)
  {
    const Tetrahedron &tetrahedron = **ti;
    const size_t tetrahedronIndex = tetrahedron.GetIndex();

    dsMath::RealDenseMatrix<DoubleType> *dmp = new dsMath::RealDenseMatrix<DoubleType>(3);
    dsMath::RealDenseMatrix<DoubleType> &M = *dmp;

    const std::vector<ConstNodePtr> &nl = tetrahedron.GetNodeList();

    const size_t ni0 = nl[0]->GetIndex();
    const DoubleType x0 = xvec[ni0];
    const DoubleType y0 = yvec[ni0];
    const DoubleType z0 = zvec[ni0];
    for (size_t r = 1; r < 4; ++r)
    {
      const size_t nir = nl[r]->GetIndex();
      const DoubleType xr = xvec[nir] - x0;
      const DoubleType yr = yvec[nir] - y0;
      const DoubleType zr = zvec[nir] - z0;

      M(r-1, 0) = xr;
      M(r-1, 1) = yr;
      M(r-1, 2) = zr;
    }
    M.LUFactor();

    dense_mats_[tetrahedronIndex] = dmp;

  }
}

template <typename DoubleType>
Vector<DoubleType> GradientField<DoubleType>::GetGradient(const Triangle &triangle, const NodeModel &nm) const
{
  if (dense_mats_.empty())
  {
    CalcMatrices2d();
  }

  const NodeScalarList<DoubleType> &nvals = nm.GetScalarValues<DoubleType>();

  const size_t triangleIndex = triangle.GetIndex();
  dsMath::RealDenseMatrix<DoubleType> &M = *dense_mats_[triangleIndex];

  const std::vector<ConstNodePtr> &nl = triangle.GetNodeList();

  thread_local std::vector<DoubleType> B(3);

  for (size_t i = 0; i < 3; ++i)
  {
    B[i] = nvals[nl[i]->GetIndex()];
  }

  bool info = M.Solve(B.data());

  if (info)
  {
    return Vector<DoubleType>(B[0], B[1], B[2]);
  }
  else
  {
    return Vector<DoubleType>(0,0,0);
    //// This is due to the inf result from a bad factorization
  }
}

template <typename DoubleType>
Vector<DoubleType> GradientField<DoubleType>::GetGradient(const Tetrahedron &tetrahedron, const NodeModel &nm) const
{
  if (dense_mats_.empty())
  {
    CalcMatrices3d();
  }

  const NodeScalarList<DoubleType> &nvals = nm.GetScalarValues<DoubleType>();

  const size_t tetrahedronIndex = tetrahedron.GetIndex();
  dsMath::RealDenseMatrix<DoubleType> &M = *dense_mats_[tetrahedronIndex];

  const std::vector<ConstNodePtr> &nl = tetrahedron.GetNodeList();

  thread_local std::vector<DoubleType> B(3);
  const DoubleType nv0 = nvals[nl[0]->GetIndex()];

  for (size_t i = 1; i < 4; ++i)
  {
    const DoubleType nvr = nvals[nl[i]->GetIndex()] - nv0;
    B[i-1] = nvr;
  }
  bool info = M.Solve(B.data());

  if (info)
  {
    return Vector<DoubleType>(B[0], B[1], B[2]);
  }
  else
  {
    return Vector<DoubleType>(0,0,0);
    //// This is due to the inf result from a bad factorization
  }
}

template class GradientField<double>;
#ifdef DEVSIM_EXTENDED_PRECISION
#include "Float128.hh"
template class GradientField<float128>;
#endif

