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

#include "TriangleElementField.hh"
#include "DenseMatrix.hh"
#include "EdgeModel.hh"
#include "Region.hh"
#include "Triangle.hh"
#include "Edge.hh"
#include "TriangleEdgeModel.hh"
#include "dsAssert.hh"

TriangleElementFieldMatrixHolder::TriangleElementFieldMatrixHolder()
{
  for (size_t i = 0; i < 3; ++i)
  {
    mats[i] = NULL;
  }
}

TriangleElementFieldMatrixHolder::~TriangleElementFieldMatrixHolder()
{
  for (size_t i = 0; i < 3; ++i)
  {
    delete mats[i];
  }
}

const size_t TriangleElementField::row0_[3] = {0, 0, 1};
const size_t TriangleElementField::row1_[3] = {1, 2, 2};

TriangleElementField::~TriangleElementField()
{
}

TriangleElementField::TriangleElementField(const Region *r) : myregion_(r)
{
}

void TriangleElementField::CalcMatrices() const
{
  dsAssert(myregion_->GetDimension() == 2, "UNEXPECTED");
  //// TODO:Check for FPE's
  //// Assert fields exist
  ConstEdgeModelPtr ux = myregion_->GetEdgeModel("unitx");
  ConstEdgeModelPtr uy = myregion_->GetEdgeModel("unity");

  dsAssert(ux, "UNEXPECTED");
  dsAssert(uy, "UNEXPECTED");

  const EdgeScalarList &xvec = ux->GetScalarValues();
  const EdgeScalarList &yvec = uy->GetScalarValues();


//  const ConstTriangleList &tlist = myregion_->GetTriangleList();

  const Region::TriangleToConstEdgeList_t &ttelist = myregion_->GetTriangleToEdgeList();

  ///// Need to handle 01, 02, 12 combinations
  dense_mats_.resize(ttelist.size());

  for (size_t tindex = 0; tindex < ttelist.size(); ++tindex)
  {

    const std::vector<ConstEdgePtr> &el = ttelist[tindex];

//    size_t eindex[3];
    double sx[3];
    double sy[3];
    for (size_t i = 0; i < 3; ++i)
    {
      const size_t eindex = (el[i])->GetIndex();
      sx[i] = xvec[eindex];
      sy[i] = yvec[eindex];
    }

    for (size_t i = 0; i < 3; ++i)
    {
      dsMath::RealDenseMatrix *dmp = new dsMath::RealDenseMatrix(2);
      dsMath::RealDenseMatrix &M = *dmp;

      const size_t r0 = row0_[i];
      M(0, 0) = sx[r0];
      M(0, 1) = sy[r0];

      const size_t r1 = row1_[i];
      M(1, 0) = sx[r1];
      M(1, 1) = sy[r1];

      bool info = M.LUFactor();
      dsAssert(info, "UNEXPECTED");

      dense_mats_[tindex].mats[i] = dmp;
    }
  }
}

std::vector<Vector> TriangleElementField::GetTriangleElementField(const Triangle &triangle, const TriangleEdgeModel &em) const
{
  const size_t triangleIndex = triangle.GetIndex();

  const TriangleEdgeScalarList &evals = em.GetScalarValues();

  std::vector<double> edgedata(3);

  for (size_t i = 0; i < 3; ++i)
  {
    const size_t edgeIndex = 3*triangleIndex + i;
    edgedata[i] = evals[edgeIndex];
  }

  return GetTriangleElementField(triangle, edgedata);

}

std::vector<Vector> TriangleElementField::GetTriangleElementField(const Triangle &triangle, const EdgeModel &em) const
{
  const size_t triangleIndex = triangle.GetIndex();
  const Region::TriangleToConstEdgeList_t &ttelist = myregion_->GetTriangleToEdgeList();
  const std::vector<ConstEdgePtr> &el = ttelist[triangleIndex];

  const EdgeScalarList &evals = em.GetScalarValues();

  std::vector<double> edgedata(3);

  for (size_t i = 0; i < 3; ++i)
  {
    const size_t edgeIndex = el[i]->GetIndex();
    edgedata[i] = evals[edgeIndex];
  }

  return GetTriangleElementField(triangle, edgedata);

}

//// This is an abstraction so we can call the same routine for either element edge or regular edge data
std::vector<Vector> TriangleElementField::GetTriangleElementField(const Triangle &triangle, const std::vector<double> &edgedata) const
{

  std::vector<Vector> ret(3);

  if (dense_mats_.empty())
  {
    CalcMatrices();
  }

  const size_t triangleIndex = triangle.GetIndex();

  ConstTriangleEdgeModelPtr eec = myregion_->GetTriangleEdgeModel("ElementEdgeCouple");
  dsAssert(eec, "UNEXPECTED");
  const TriangleEdgeScalarList &ecouple = eec->GetScalarValues();

  std::vector<double> B(2);

  //// These are the components projected onto the element
  std::vector<Vector>  results(3);
  std::vector<double>  edgeweights(3);

  //// populate edgeweights
  for (size_t mi = 0; mi < 3; ++mi)
  {
    //// Get the appropriate edge couples
    //// note that the index correspondes to the edge, not the mindex
    edgeweights[mi] = ecouple[3*triangleIndex + mi];

    //// Calculate the values
    B[0] = edgedata[row0_[mi]];
    B[1] = edgedata[row1_[mi]];

    dsMath::RealDenseMatrix &M = *dense_mats_[triangleIndex].mats[mi];

    bool info = M.Solve(B);
    dsAssert(info, "UNEXPECTED");

    //// this is the combination of edge i and edge j
    results[mi] = Vector(B[0], B[1], 0.0);
  }

  //// This is the edge index being filled in
  for (size_t i = 0; i < 3; ++i)
  {
    double weight = 0.0;

    for (size_t j = 0; j < 3; ++j)
    {
      if (i == j)
      {
        continue;
      }

      const size_t mi= i + j - 1;
      ret[i] += results[mi] * edgeweights[j];
      weight += edgeweights[j];
    }
    dsAssert(weight != 0.0, "UNEXPECTED");
    ret[i] /= weight;
  }

  return ret;
}

//// eindex is the desired edge in (0, 1, 2), nindex is the derivative node we are evaluating
//// em0 is the @n0
//// em1 is the @n1
std::vector<std::vector<Vector> > TriangleElementField::GetTriangleElementField(const Triangle &triangle, const EdgeModel &em0, const EdgeModel &em1) const
{
  /// ordered by [nindex][eindex]
  std::vector<std::vector<Vector> > ret(3);
  for (size_t i = 0; i < 3; ++i)
  {
    ret[i].resize(3);
  }

  if (dense_mats_.empty())
  {
    CalcMatrices();
  }

  const EdgeScalarList &evals0 = em0.GetScalarValues();
  const EdgeScalarList &evals1 = em1.GetScalarValues();

  const size_t triangleIndex = triangle.GetIndex();

  ConstTriangleEdgeModelPtr eec = myregion_->GetTriangleEdgeModel("ElementEdgeCouple");
  dsAssert(eec, "UNEXPECTED");
  const TriangleEdgeScalarList &ecouple = eec->GetScalarValues();

  const Region::TriangleToConstEdgeList_t &ttelist = myregion_->GetTriangleToEdgeList();

  const std::vector<ConstEdgePtr> &el = ttelist[triangleIndex];

  const std::vector<ConstNodePtr> &nl = triangle.GetNodeList();

  std::vector<double> B(2);
  std::vector<double>  edgeweights(3);

  //// The first index is which node derivative
  //// The edge index is which edge pair
  std::vector<std::vector<Vector > > results(3);
  for (size_t i = 0; i < 3; ++i)
  {
    results[i].resize(3);
  }

  for (size_t mi = 0; mi < 3; ++mi)
  {
    edgeweights[mi] = ecouple[3*triangleIndex + mi];

    const Edge * const edge0 = el[row0_[mi]];
    const Edge * const edge1 = el[row1_[mi]];

    const size_t ri0 = edge0->GetIndex();
    const size_t ri1 = edge1->GetIndex();
    for (size_t ni = 0; ni < 3; ++ni)
    {
      //// this is the node we are taking the derivative with respect to
      const Node * const np = nl[ni];

      double ev0 = 0.0;
      if (edge0->GetHead() == np)
      {
        ev0 = evals0[ri0];
      }
      else if (edge0->GetTail() == np)
      {
        ev0 = evals1[ri0];
      }

      double ev1 = 0.0;
      if (edge1->GetHead() == np)
      {
        ev1 = evals0[ri1];
      }
      else if (edge1->GetTail() == np)
      {
        ev1 = evals1[ri1];
      }

      B[0] = ev0;
      B[1] = ev1;

      dsMath::RealDenseMatrix &M = *dense_mats_[triangleIndex].mats[mi];
      bool info = M.Solve(B);
      dsAssert(info, "UNEXPECTED");

      results[ni][mi] = Vector(B[0], B[1], 0.0);
    }
  }

  for (size_t ni = 0; ni < 3; ++ni)
  {
    const Node * const np = nl[ni];

    for (size_t i = 0; i < 3; ++i)
    {

      size_t out_index = 2;
      //// this is the output index
      if (el[i]->GetHead() == np)
      {
        out_index = 0;
      }
      else if (el[i]->GetTail() == np)
      {
        out_index = 1;
      }

      double weight = 0.0;

      for (size_t j = 0; j < 3; ++j)
      {
        if (i == j)
        {
          continue;
        }

        const size_t mi = i + j - 1;

        ret[out_index][i] += results[ni][mi] * edgeweights[j];
        weight += edgeweights[j];
      }
      ret[out_index][i] /= weight;
    }
  }

  return ret;
}

