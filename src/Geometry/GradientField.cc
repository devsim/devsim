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

#include "GradientField.hh"
#include "DenseMatrix.hh"
#include "Region.hh"
#include "NodeModel.hh"
#include "dsAssert.hh"
#include "Triangle.hh"
#include "Tetrahedron.hh"
#include "Node.hh"

GradientField::~GradientField()
{
  for (std::vector<dsMath::RealDenseMatrix *>::iterator it = dense_mats_.begin(); it != dense_mats_.end(); ++it)
  {
    delete *it;
  }
}

GradientField::GradientField(const Region *r) : myregion_(r)
{
}

void GradientField::CalcMatrices2d() const
{
  dsAssert(GetRegion().GetDimension() == 2, "UNEXPECTED");
  //// TODO:Check for FPE's
  //// Assert dimension is 2!
  //// Assert fields exist
  ConstNodeModelPtr ux = myregion_->GetNodeModel("x");
  ConstNodeModelPtr uy = myregion_->GetNodeModel("y");

  dsAssert(ux, "UNEXPECTED");
  dsAssert(uy, "UNEXPECTED");

  const NodeScalarList &xvec = ux->GetScalarValues();
  const NodeScalarList &yvec = uy->GetScalarValues();


  const ConstTriangleList &tlist = myregion_->GetTriangleList();

  dense_mats_.resize(tlist.size());

  for (ConstTriangleList::const_iterator ti = tlist.begin(); ti != tlist.end(); ++ti)
  {
    const Triangle &triangle = **ti;
    const size_t triangleIndex = triangle.GetIndex();

    dsMath::RealDenseMatrix *dmp = new dsMath::RealDenseMatrix(3);
    dsMath::RealDenseMatrix &M = *dmp;

    const std::vector<ConstNodePtr> &nl = triangle.GetNodeList();

    const size_t ni0 = nl[0]->GetIndex();
    const double x0 = xvec[ni0];
    const double y0 = yvec[ni0];
    for (size_t r = 1; r < 3; ++r)
    {
      const size_t nir = nl[r]->GetIndex();
      const double xr = xvec[nir] - x0;
      const double yr = yvec[nir] - y0;

      M(r, 0) = xr;
      M(r, 1) = yr;
      M(r, 2) = 1.0;
    }
    M(0, 2) = 1.0;
    M.LUFactor();

    dense_mats_[triangleIndex] = dmp;
    
  }
}

void GradientField::CalcMatrices3d() const
{
  dsAssert(GetRegion().GetDimension() == 3, "UNEXPECTED");
  //// TODO:Check for FPE's
  //// Assert dimension is 2!
  //// Assert fields exist
  ConstNodeModelPtr ux = myregion_->GetNodeModel("x");
  ConstNodeModelPtr uy = myregion_->GetNodeModel("y");
  ConstNodeModelPtr uz = myregion_->GetNodeModel("y");

  dsAssert(ux, "UNEXPECTED");
  dsAssert(uy, "UNEXPECTED");
  dsAssert(uz, "UNEXPECTED");

  const NodeScalarList &xvec = ux->GetScalarValues();
  const NodeScalarList &yvec = uy->GetScalarValues();
  const NodeScalarList &zvec = uz->GetScalarValues();


  const ConstTetrahedronList &tlist = myregion_->GetTetrahedronList();

  dense_mats_.resize(tlist.size());

  for (ConstTetrahedronList::const_iterator ti = tlist.begin(); ti != tlist.end(); ++ti)
  {
    const Tetrahedron &tetrahedron = **ti;
    const size_t tetrahedronIndex = tetrahedron.GetIndex();

    dsMath::RealDenseMatrix *dmp = new dsMath::RealDenseMatrix(3);
    dsMath::RealDenseMatrix &M = *dmp;

    const std::vector<ConstNodePtr> &nl = tetrahedron.GetNodeList();

    const size_t ni0 = nl[0]->GetIndex();
    const double x0 = xvec[ni0];
    const double y0 = yvec[ni0];
    const double z0 = zvec[ni0];
    for (size_t r = 1; r < 4; ++r)
    {
      const size_t nir = nl[r]->GetIndex();
      const double xr = xvec[nir] - x0;
      const double yr = yvec[nir] - y0;
      const double zr = zvec[nir] - z0;

      M(r-1, 0) = xr;
      M(r-1, 1) = yr;
      M(r-1, 2) = zr;
    }
    M.LUFactor();

    dense_mats_[tetrahedronIndex] = dmp;
    
  }
}

Vector GradientField::GetGradient(const Triangle &triangle, const NodeModel &nm) const
{
  if (dense_mats_.empty())
  {
    CalcMatrices2d();
  }

  const NodeScalarList &nvals = nm.GetScalarValues();

  const size_t triangleIndex = triangle.GetIndex();
  dsMath::RealDenseMatrix &M = *dense_mats_[triangleIndex];

  const std::vector<ConstNodePtr> &nl = triangle.GetNodeList();

  static std::vector<double> B(3);
  for (size_t i = 0; i < 3; ++i)
  {
    B[i] = nvals[nl[i]->GetIndex()];
  }
  bool info = M.Solve(B);

  if (info)
  {
    return Vector(B[0], B[1], B[2]);
  }
  else
  {
    return Vector(0,0,0);
    //// This is due to the inf result from a bad factorization
  }
}

// TODO:"REVIEW, TEST 3D CODE"
Vector GradientField::GetGradient(const Tetrahedron &tetrahedron, const NodeModel &nm) const
{
  if (dense_mats_.empty())
  {
    CalcMatrices3d();
  }

  const NodeScalarList &nvals = nm.GetScalarValues();

  const size_t tetrahedronIndex = tetrahedron.GetIndex();
  dsMath::RealDenseMatrix &M = *dense_mats_[tetrahedronIndex];

  const std::vector<ConstNodePtr> &nl = tetrahedron.GetNodeList();

  static std::vector<double> B(3);
  const double nv0 = nvals[nl[0]->GetIndex()];

  for (size_t i = 1; i < 4; ++i)
  {
    const double nvr = nvals[nl[i]->GetIndex()] - nv0;
    B[i-1] = nvr;
  }
  bool info = M.Solve(B);

  if (info)
  {
    return Vector(B[0], B[1], B[2]);
  }
  else
  {
    return Vector(0,0,0);
    //// This is due to the inf result from a bad factorization
  }
}

