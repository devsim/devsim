/***
DEVSIM
Copyright 2013 DEVSIM LLC

SPDX-License-Identifier: Apache-2.0
***/

#include "TriangleElementField.hh"
#include "DenseMatrix.hh"
#include "EdgeModel.hh"
#include "Region.hh"
#include "Triangle.hh"
#include "Edge.hh"
#include "TriangleEdgeModel.hh"
#include "dsAssert.hh"
#include <array>

template <typename DoubleType>
TriangleElementFieldMatrixHolder<DoubleType>::TriangleElementFieldMatrixHolder()
{
  for (size_t i = 0; i < 3; ++i)
  {
    mats[i] = nullptr;
  }
}

template <typename DoubleType>
TriangleElementFieldMatrixHolder<DoubleType>::~TriangleElementFieldMatrixHolder()
{
  for (size_t i = 0; i < 3; ++i)
  {
    delete mats[i];
  }
}

// position = i + j - 1
template <typename DoubleType>
const size_t TriangleElementField<DoubleType>::row0_[3] = {0, 0, 1};
template <typename DoubleType>
const size_t TriangleElementField<DoubleType>::row1_[3] = {1, 2, 2};

// this can also be calculates as 3 - (i + j)
template <typename DoubleType>
const size_t TriangleElementField<DoubleType>::node_shared_[3] = {2, 1, 0};

template <typename DoubleType>
TriangleElementField<DoubleType>::~TriangleElementField()
{
}

template <typename DoubleType>
TriangleElementField<DoubleType>::TriangleElementField(const Region *r) : myregion_(r)
{
}


template <typename DoubleType>
void TriangleElementField<DoubleType>::PopulateEdgeData(const Triangle &triangle, const EdgeModel &em, std::vector<DoubleType> &edgedata) const
{
  const size_t triangleIndex = triangle.GetIndex();
  const Region::TriangleToConstEdgeList_t &ttelist = myregion_->GetTriangleToEdgeList();
  const std::vector<ConstEdgePtr> &el = ttelist[triangleIndex];

  const EdgeScalarList<DoubleType> &evals = em.GetScalarValues<DoubleType>();

  for (size_t i = 0; i < 3; ++i)
  {
    const size_t edgeIndex = el[i]->GetIndex();
    edgedata[i] = evals[edgeIndex];
  }
}

template <typename DoubleType>
void TriangleElementField<DoubleType>::CalcMatrices() const
{
  dsAssert(myregion_->GetDimension() == 2, "UNEXPECTED");
  //// TODO:Check for FPE's
  //// Assert fields exist
  ConstEdgeModelPtr ux = myregion_->GetEdgeModel("unitx");
  ConstEdgeModelPtr uy = myregion_->GetEdgeModel("unity");

  dsAssert(ux.get(), "UNEXPECTED");
  dsAssert(uy.get(), "UNEXPECTED");

  const EdgeScalarList<DoubleType> &xvec = ux->GetScalarValues<DoubleType>();
  const EdgeScalarList<DoubleType> &yvec = uy->GetScalarValues<DoubleType>();


//  const ConstTriangleList &tlist = myregion_->GetTriangleList();

  const Region::TriangleToConstEdgeList_t &ttelist = myregion_->GetTriangleToEdgeList();

  ///// Need to handle 01, 02, 12 combinations
  dense_mats_.resize(ttelist.size());

  for (size_t tindex = 0; tindex < ttelist.size(); ++tindex)
  {

    const std::vector<ConstEdgePtr> &el = ttelist[tindex];

//    size_t eindex[3];
    DoubleType sx[3];
    DoubleType sy[3];
    for (size_t i = 0; i < 3; ++i)
    {
      const size_t eindex = (el[i])->GetIndex();
      sx[i] = xvec[eindex];
      sy[i] = yvec[eindex];
    }

    for (size_t i = 0; i < 3; ++i)
    {
      dsMath::RealDenseMatrix<DoubleType> *dmp = new dsMath::RealDenseMatrix<DoubleType>(2);
      dsMath::RealDenseMatrix<DoubleType> &M = *dmp;

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

template <typename DoubleType>
void TriangleElementField<DoubleType>::GetTriangleElementField(const Triangle &triangle, const TriangleEdgeModel &eec, const TriangleEdgeModel &em, TriangleElementField<DoubleType>::EdgeVectors_t &result) const
{
  const size_t triangleIndex = triangle.GetIndex();

  const TriangleEdgeScalarList<DoubleType> &evals = em.GetScalarValues<DoubleType>();

  thread_local std::vector<DoubleType> edgedata(3);
  for (size_t i = 0; i < 3; ++i)
  {
    const size_t edgeIndex = 3*triangleIndex + i;
    edgedata[i] = evals[edgeIndex];
  }

  GetTriangleElementField(triangle, eec, edgedata, result);

}

template <typename DoubleType>
void TriangleElementField<DoubleType>::GetTriangleElementField(const Triangle &triangle, const TriangleEdgeModel &eec, const EdgeModel &em, TriangleElementField<DoubleType>::EdgeVectors_t &result) const
{
  thread_local std::vector<DoubleType> edgedata(3);
  PopulateEdgeData(triangle, em, edgedata);

  GetTriangleElementField(triangle, eec, edgedata, result);

}

template <typename DoubleType>
const typename TriangleElementField<DoubleType>::EdgePairVectors_t &TriangleElementField<DoubleType>::GetEdgePairVectors(const Triangle &triangle, const std::vector<DoubleType> &edgedata) const
{
  if (dense_mats_.empty())
  {
    CalcMatrices();
  }

  const size_t triangleIndex = triangle.GetIndex();
  thread_local typename TriangleElementField<DoubleType>::EdgePairVectors_t results;
  thread_local std::array<DoubleType, 2> B;

  for (size_t mi = 0; mi < 3; ++mi)
  {
    //// Calculate the values
    B[0] = edgedata[row0_[mi]];
    B[1] = edgedata[row1_[mi]];

    dsMath::RealDenseMatrix<DoubleType> &M = *dense_mats_[triangleIndex].mats[mi];

    bool info = M.Solve(B.data());
    dsAssert(info, "UNEXPECTED");

    //// this is the combination of edge i and edge j
    results[mi] = Vector<DoubleType>(B[0], B[1], 0.0);
  }


  return results;
}

template <typename DoubleType>
void TriangleElementField<DoubleType>::GetFieldPairs(const Triangle &triangle, const TriangleEdgeModel &eec, const std::vector<DoubleType> &edgedata, TriangleElementField<DoubleType>::TuplePairs_t &tuple_pairs) const
{
  const size_t triangleIndex = triangle.GetIndex();

  const TriangleEdgeScalarList<DoubleType> &ecouple = eec.GetScalarValues<DoubleType>();

  const auto &results = GetEdgePairVectors(triangle, edgedata);
  //// These are the components projected onto the element
  const DoubleType * const edgeweights = &ecouple[3*triangleIndex];
  const auto &ttelist = myregion_->GetTriangleToEdgeList();
  const auto &el = ttelist[triangleIndex];
  const auto &nl = triangle.GetNodeList();

  auto &node_based = std::get<0>(tuple_pairs);
  auto &weights = std::get<1>(tuple_pairs);

  for (size_t mi = 0; mi < 3; ++mi)
  {
    const size_t i = row0_[mi];
    const size_t j = row1_[mi];

    const auto eli = el[i];
    const auto elj = el[j];
    const auto np = nl[node_shared_[mi]];

    size_t np_i = 0;
    size_t np_j = 0;

    if (eli->GetHead() == np)
    {
      np_i = 0;
    }
    else if (eli->GetTail() == np)
    {
      np_i = 1;
    }
    else
    {
      dsAssert(false, "UNEXPECTED");
    }

    if (elj->GetHead() == np)
    {
      np_j = 0;
    }
    else if (elj->GetTail() == np)
    {
      np_j = 1;
    }
    else
    {
      dsAssert(false, "UNEXPECTED");
    }

    node_based[i][np_i] = results[mi];
    node_based[j][np_j] = results[mi];

    weights[i][np_i] = edgeweights[j];
    weights[j][np_j] = edgeweights[i];
  }
}

template <typename DoubleType>
void TriangleElementField<DoubleType>::GetTriangleElementFieldPairs(const Triangle &triangle, const TriangleEdgeModel &eec, const EdgeModel& em, TriangleElementField<DoubleType>::EdgeVectors_t &result0, TriangleElementField<DoubleType>::EdgeVectors_t &result1) const
{
  thread_local std::vector<DoubleType> edgedata(3);
  PopulateEdgeData(triangle, em, edgedata);

  thread_local TriangleElementField<DoubleType>::TuplePairs_t tuple_pairs;
  GetFieldPairs(triangle, eec, edgedata, tuple_pairs);
  auto &node_based = std::get<0>(tuple_pairs);

  for (size_t i = 0; i < 3; ++i)
  {
    result0[i] = node_based[i][0];
    result1[i] = node_based[i][1];
  }
}

//// This is an abstraction so we can call the same routine for either element edge or regular edge data
template <typename DoubleType>
void TriangleElementField<DoubleType>::GetTriangleElementField(const Triangle &triangle, const TriangleEdgeModel &eec, const std::vector<DoubleType> &edgedata, TriangleElementField<DoubleType>::EdgeVectors_t &result) const
{
  thread_local TriangleElementField<DoubleType>::TuplePairs_t tuple_pairs;
  GetFieldPairs(triangle, eec, edgedata, tuple_pairs);
  auto &node_based = std::get<0>(tuple_pairs);
  auto &weights = std::get<1>(tuple_pairs);

  static const Vector<DoubleType> ZeroVector;
  result.fill(ZeroVector);
  for (size_t i = 0; i < 3; ++i)
  {
    DoubleType weight = 0.0;
    for (size_t ni = 0; ni < 2; ++ni)
    {
      result[i] += node_based[i][ni] * weights[i][ni];
      weight += weights[i][ni];
    }
    dsAssert(weight != 0.0, "UNEXPECTED");
    result[i] /= weight;
  }
}

template <typename DoubleType>
const typename TriangleElementField<DoubleType>::DerivativeEdgePairVectors_t &TriangleElementField<DoubleType>::GetDerivativeEdgePairVectors(const Triangle &triangle, const std::vector<DoubleType> &evals0, const std::vector<DoubleType> &evals1) const
{
  if (dense_mats_.empty())
  {
    CalcMatrices();
  }

  const size_t triangleIndex = triangle.GetIndex();
  const auto &ttelist = myregion_->GetTriangleToEdgeList();
  const auto &el = ttelist[triangleIndex];
  const auto &nl = triangle.GetNodeList();

  thread_local typename TriangleElementField<DoubleType>::DerivativeEdgePairVectors_t results;

  thread_local std::array<DoubleType, 2> B;

  //// The first index is which node derivative
  //// The edge index is which edge pair
  for (size_t mi = 0; mi < 3; ++mi)
  {

    const Edge * const edge0 = el[row0_[mi]];
    const Edge * const edge1 = el[row1_[mi]];

    const size_t ri0 = edge0->GetIndex();
    const size_t ri1 = edge1->GetIndex();
    for (size_t ni = 0; ni < 3; ++ni)
    {
      //// this is the node we are taking the derivative with respect to
      const Node * const np = nl[ni];

      DoubleType ev0 = 0.0;
      if (edge0->GetHead() == np)
      {
        ev0 = evals0[ri0];
      }
      else if (edge0->GetTail() == np)
      {
        ev0 = evals1[ri0];
      }

      DoubleType ev1 = 0.0;
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

      dsMath::RealDenseMatrix<DoubleType> &M = *dense_mats_[triangleIndex].mats[mi];
      bool info = M.Solve(B.data());
      dsAssert(info, "UNEXPECTED");

      results[ni][mi] = Vector<DoubleType>(B[0], B[1], 0.0);
    }
  }

  return results;
}

template <typename DoubleType>
void TriangleElementField<DoubleType>::GetDerivativeFieldPairs(const Triangle &triangle, const TriangleEdgeModel &eec, const EdgeModel &em0, const EdgeModel &em1, TriangleElementField<DoubleType>::DerivativeTuplePairs_t &derivative_tuple_pairs) const
{

  const EdgeScalarList<DoubleType> &evals0 = em0.GetScalarValues<DoubleType>();
  const EdgeScalarList<DoubleType> &evals1 = em1.GetScalarValues<DoubleType>();

  const size_t triangleIndex = triangle.GetIndex();
  const auto &ttelist = myregion_->GetTriangleToEdgeList();
  const auto &el = ttelist[triangleIndex];
  const auto &nl = triangle.GetNodeList();

  const auto &results = GetDerivativeEdgePairVectors(triangle, evals0, evals1);

  const TriangleEdgeScalarList<DoubleType> &ecouple = eec.GetScalarValues<DoubleType>();

  const DoubleType * const edgeweights = &ecouple[3*triangleIndex];

  auto &node_based = std::get<0>(derivative_tuple_pairs);
  auto &weights = std::get<1>(derivative_tuple_pairs);

  for (size_t ni = 0; ni < 3; ++ni)
  {
    // this is the derivative node
    const Node * const nd = nl[ni];

    for (size_t mi = 0; mi < 3; ++mi)
    {
      const size_t i = row0_[mi];
      const size_t j = row1_[mi];
      const auto eli = el[i];
      const auto elj = el[j];
      const auto np = nl[node_shared_[mi]];

      // this is the output derivative index
      size_t der_i = 2;
      if (el[i]->GetHead() == nd)
      {
        der_i = 0;
      }
      else if (el[i]->GetTail() == nd)
      {
        der_i = 1;
      }

      size_t der_j = 2;
      if (el[j]->GetHead() == nd)
      {
        der_j = 0;
      }
      else if (el[j]->GetTail() == nd)
      {
        der_j = 1;
      }

      size_t np_i = 0;
      size_t np_j = 0;

      // this is to handle the common node
      if (eli->GetHead() == np)
      {
        np_i = 0;
      }
      else if (eli->GetTail() == np)
      {
        np_i = 1;
      }
      else
      {
        dsAssert(false, "UNEXPECTED");
      }

      if (elj->GetHead() == np)
      {
        np_j = 0;
      }
      else if (elj->GetTail() == np)
      {
        np_j = 1;
      }
      else
      {
        dsAssert(false, "UNEXPECTED");
      }

      node_based[der_i][i][np_i] = results[ni][mi];
      node_based[der_j][j][np_j] = results[ni][mi];

      weights[der_i][i][np_i] = edgeweights[j];
      weights[der_j][j][np_j] = edgeweights[i];
    }
  }
}


//// eindex is the desired edge in (0, 1, 2), nindex is the derivative node we are evaluating
//// em0 is the @n0
//// em1 is the @n1
template <typename DoubleType>
void TriangleElementField<DoubleType>::GetTriangleElementField(const Triangle &triangle, const TriangleEdgeModel &eec, const EdgeModel &em0, const EdgeModel &em1, TriangleElementField<DoubleType>::DerivativeEdgeVectors_t &result) const
{
  /// result ordered by [nindex][eindex]

  thread_local TriangleElementField<DoubleType>::DerivativeTuplePairs_t derivative_tuple_pairs;
  GetDerivativeFieldPairs(triangle, eec, em0, em1, derivative_tuple_pairs);
  auto &node_based = std::get<0>(derivative_tuple_pairs);
  auto &weights = std::get<1>(derivative_tuple_pairs);

  static const Vector<DoubleType> ZeroVector;

  // this as separate function
  for (size_t nd = 0; nd < 3; ++nd)
  {
    result[nd].fill(ZeroVector);
    for (size_t i = 0; i < 3; ++i)
    {
      DoubleType weight = 0.0;
      for (size_t ni = 0; ni < 2; ++ni)
      {
        result[nd][i] += node_based[nd][i][ni] * weights[nd][i][ni];
        weight += weights[nd][i][ni];
      }
      dsAssert(weight != 0.0, "UNEXPECTED");
      result[nd][i] /= weight;
    }
  }
}

template <typename DoubleType>
void TriangleElementField<DoubleType>::GetTriangleElementFieldPairs(const Triangle &triangle, const TriangleEdgeModel &eec, const EdgeModel &em0, const EdgeModel &em1, TriangleElementField<DoubleType>::DerivativeEdgeVectors_t &result0, TriangleElementField<DoubleType>::DerivativeEdgeVectors_t &result1) const
{
  /// result ordered by [nindex][eindex]

  thread_local TriangleElementField<DoubleType>::DerivativeTuplePairs_t derivative_tuple_pairs;
  GetDerivativeFieldPairs(triangle, eec, em0, em1, derivative_tuple_pairs);
  auto &node_based = std::get<0>(derivative_tuple_pairs);

  // this as separate function
  for (size_t nd = 0; nd < 3; ++nd)
  {
    for (size_t i = 0; i < 3; ++i)
    {
      result0[nd][i] = node_based[nd][i][0];
      result1[nd][i] = node_based[nd][i][1];
    }
  }
}

template class TriangleElementField<double>;
#ifdef DEVSIM_EXTENDED_PRECISION
#include "Float128.hh"
template class TriangleElementField<float128>;
#endif

