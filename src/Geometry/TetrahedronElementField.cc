/***
DEVSIM
Copyright 2013 DEVSIM LLC

SPDX-License-Identifier: Apache-2.0
***/

#include "TetrahedronElementField.hh"
#include "DenseMatrix.hh"
#include "EdgeModel.hh"
#include "Region.hh"
#include "Tetrahedron.hh"
#include "Edge.hh"
#include "TetrahedronEdgeModel.hh"
#include "dsAssert.hh"
#include "EdgeData.hh"
#include "Triangle.hh"
#include <array>

template <typename DoubleType>
TetrahedronElementFieldMatrixHolder<DoubleType>::TetrahedronElementFieldMatrixHolder()
{
  for (size_t i = 0; i < 4; ++i)
  {
    mats[i] = nullptr;
    for (size_t j = 0; j < 3; ++j)
    {
      edgeIndexes[i][j] = 0;
    }
  }
}

template <typename DoubleType>
TetrahedronElementFieldMatrixHolder<DoubleType>::~TetrahedronElementFieldMatrixHolder()
{
  for (size_t i = 0; i < 4; ++i)
  {
    delete mats[i];
  }
}

template <typename DoubleType>
TetrahedronElementField<DoubleType>::~TetrahedronElementField()
{
}

template <typename DoubleType>
TetrahedronElementField<DoubleType>::TetrahedronElementField(const Region *r) : myregion_(r)
{
}

template <typename DoubleType>
void TetrahedronElementField<DoubleType>::PopulateEdgeData(const Tetrahedron &tetrahedron, const EdgeModel &em, std::vector<DoubleType> &edgedata) const
{
  const size_t tetrahedronIndex = tetrahedron.GetIndex();
  const Region::TetrahedronToConstEdgeDataList_t &ttelist = myregion_->GetTetrahedronToEdgeDataList();
  const ConstEdgeDataList &edgeDataList = ttelist[tetrahedronIndex];

  const EdgeScalarList<DoubleType> &evals = em.GetScalarValues<DoubleType>();

  for (size_t i = 0; i < 6; ++i)
  {
    const size_t edgeIndex = (edgeDataList[i]->edge)->GetIndex();
    edgedata[i] = evals[edgeIndex];
  }
}

//// The matrices we develop are for each of the 4 nodes on the Tetrahedron
//// Each node has 3 edges connected to it
template <typename DoubleType>
void TetrahedronElementField<DoubleType>::CalcMatrices() const
{
  dsAssert(myregion_->GetDimension() == 3, "UNEXPECTED");
  //// TODO:Check for FPE's
  //// Assert fields exist
  ConstEdgeModelPtr ux = myregion_->GetEdgeModel("unitx");
  ConstEdgeModelPtr uy = myregion_->GetEdgeModel("unity");
  ConstEdgeModelPtr uz = myregion_->GetEdgeModel("unitz");

  dsAssert(ux.get(), "UNEXPECTED");
  dsAssert(uy.get(), "UNEXPECTED");
  dsAssert(uz.get(), "UNEXPECTED");

  const EdgeScalarList<DoubleType> &xvec = ux->GetScalarValues<DoubleType>();
  const EdgeScalarList<DoubleType> &yvec = uy->GetScalarValues<DoubleType>();
  const EdgeScalarList<DoubleType> &zvec = uz->GetScalarValues<DoubleType>();

//  const ConstTetrahedronList &tlist = myregion_->GetTetrahedronList();

  const ConstTetrahedronList &tetrahedronList = myregion_->GetTetrahedronList();

  const Region::TetrahedronToConstEdgeDataList_t &ttelist = myregion_->GetTetrahedronToEdgeDataList();

  ///// Need to handle 01, 02, 12 combinations
  dense_mats_.resize(tetrahedronList.size());

  for (size_t tindex = 0; tindex < tetrahedronList.size(); ++tindex)
  {
    const auto &tetrahedron = *tetrahedronList[tindex];

    const auto &edgeDataList = ttelist[tindex];

    const auto &nodeList = tetrahedron.GetNodeList();

    DoubleType sx[3];
    DoubleType sy[3];
    DoubleType sz[3];
    size_t edgeIndexes[3];

    //// for each node
    //// find the four edges connected
    size_t k = 0;
    for (size_t i = 0; i < nodeList.size(); ++i)
    {
      k = 0;
      const ConstNodePtr nodeptr = nodeList[i];
      for (size_t j = 0; j < edgeDataList.size(); ++j)
      {
        const Edge &edge = *(edgeDataList[j]->edge);
        if ((edge.GetHead() == nodeptr) || (edge.GetTail() == nodeptr))
        {
          const size_t eindex = edge.GetIndex();
          edgeIndexes[k] = j;
          sx[k] = xvec[eindex];
          sy[k] = yvec[eindex];
          sz[k] = zvec[eindex];
          ++k;
        }
      }
      //// We expect to find 3 edges connected to this node
      dsAssert(k == 3, "UNEXPECTED");

      dsMath::RealDenseMatrix<DoubleType> *dmp = new dsMath::RealDenseMatrix<DoubleType>(3);
      dsMath::RealDenseMatrix<DoubleType> &M = *dmp;

      for (size_t l = 0; l < 3; ++l)
      {
        M(l, 0) = sx[l];
        M(l, 1) = sy[l];
        M(l, 2) = sz[l];
      }

      bool info = M.LUFactor();
      dsAssert(info, "UNEXPECTED");
      dense_mats_[tindex].mats[i] = dmp;
      for (size_t m = 0; m < 3; ++m)
      {
        dense_mats_[tindex].edgeIndexes[i][m] = edgeIndexes[m];
      }
    }
  }
}

template <typename DoubleType>
void TetrahedronElementField<DoubleType>::GetTetrahedronElementField(const Tetrahedron &tetrahedron, const EdgeModel &em, TetrahedronElementField<DoubleType>::EdgeVectors_t &result) const
{
  thread_local std::vector<DoubleType> edgedata(6);
  PopulateEdgeData(tetrahedron, em, edgedata);
  GetTetrahedronElementField(tetrahedron, edgedata, result);

}

template <typename DoubleType>
void TetrahedronElementField<DoubleType>::GetTetrahedronElementField(const Tetrahedron &tetrahedron, const TetrahedronEdgeModel &em, TetrahedronElementField<DoubleType>::EdgeVectors_t &result) const
{
  const size_t tetrahedronIndex = tetrahedron.GetIndex();

  const TetrahedronEdgeScalarList<DoubleType> &evals = em.GetScalarValues<DoubleType>();

  thread_local std::vector<DoubleType> edgedata(6);
  for (size_t i = 0; i < 6; ++i)
  {
    const size_t edgeIndex = 6*tetrahedronIndex + i;
    edgedata[i] = evals[edgeIndex];
  }

  GetTetrahedronElementField(tetrahedron, edgedata, result);

}

template <typename DoubleType>
const typename TetrahedronElementField<DoubleType>::NodeVectors_t &TetrahedronElementField<DoubleType>::GetNodeVectors(const Tetrahedron &tetrahedron, const std::vector<DoubleType> &edgedata) const
{
  if (dense_mats_.empty())
  {
    CalcMatrices();
  }

  const size_t tetrahedronIndex = tetrahedron.GetIndex();

  //// this is the rhs vec
  thread_local std::array<DoubleType, 3> B;
  //// these are the values emanating from each node
  thread_local std::array<Vector<DoubleType>, 4> nodeVectors;

  // for each node
  for (size_t i = 0; i < 4; ++i)
  {
    dsMath::RealDenseMatrix<DoubleType> &M = *dense_mats_[tetrahedronIndex].mats[i];
    for (size_t j = 0; j < 3; ++j)
    {
      const size_t eindex = dense_mats_[tetrahedronIndex].edgeIndexes[i][j];
      ///// map local index to region edge index
      B[j] = edgedata[eindex];
    }

    bool info = M.Solve(B.data());
    dsAssert(info, "UNEXPECTED");

    //// This is the element field on one of the four nodes
    nodeVectors[i] = Vector<DoubleType>(B[0], B[1], B[2]);
  }

  return nodeVectors;
}
template <typename DoubleType>
void TetrahedronElementField<DoubleType>::GetFieldPairs(const Tetrahedron &tetrahedron, const std::vector<DoubleType> &edgedata, VectorPairs_t &node_based) const
{
  const auto &nodeVectors = GetNodeVectors(tetrahedron, edgedata);

  const size_t tetrahedronIndex = tetrahedron.GetIndex();
  const auto &nodeList = tetrahedron.GetNodeList();
  const auto &edgeDataList = myregion_->GetTetrahedronToEdgeDataList()[tetrahedronIndex];

  for (size_t i = 0; i < 4; ++i)
  {
    const auto *np = nodeList[i];
    const auto &val = nodeVectors[i];
    //// calculate the average on the edge
    for (size_t j = 0; j < 3; ++j)
    {
      const size_t eindex = dense_mats_[tetrahedronIndex].edgeIndexes[i][j];
      const auto *edge = edgeDataList[eindex]->edge;

      size_t nei;
      if (edge->GetHead() == np)
      {
        nei = 0;
      }
      else if (edge->GetTail() == np)
      {
        nei = 1;
      }
      else
      {
        nei = 0;
        dsAssert(false, "UNEXPECTED");
      }

      node_based[eindex][nei] = val;
    }
  }
}

template <typename DoubleType>
void TetrahedronElementField<DoubleType>::GetTetrahedronElementField(const Tetrahedron &tetrahedron, const std::vector<DoubleType> &edgedata, TetrahedronElementField<DoubleType>::EdgeVectors_t &result) const
{
  /// for each of the nodes
  thread_local VectorPairs_t node_based;
  GetFieldPairs(tetrahedron, edgedata, node_based);

  static const auto weight = static_cast<DoubleType>(0.5);
  for (size_t i = 0; i < 6; ++i)
  {
    auto &rentry = result[i];
    auto &nbentry = node_based[i];
    rentry = nbentry[0] + nbentry[1];
    rentry *= weight;
  }
}

template <typename DoubleType>
void TetrahedronElementField<DoubleType>::GetTetrahedronElementFieldPairs(const Tetrahedron &tetrahedron, const EdgeModel &em, TetrahedronElementField<DoubleType>::EdgeVectors_t &result0, TetrahedronElementField<DoubleType>::EdgeVectors_t &result1) const
{
  thread_local std::vector<DoubleType> edgedata(6);
  PopulateEdgeData(tetrahedron, em, edgedata);

  /// for each of the nodes
  thread_local VectorPairs_t node_based;
  GetFieldPairs(tetrahedron, edgedata, node_based);

  for (size_t i = 0; i < 6; ++i)
  {
    auto &nbentry = node_based[i];
    result0[i] = nbentry[0];
    result1[i] = nbentry[1];
  }
}

template <typename DoubleType>
const typename TetrahedronElementField<DoubleType>::DerivativeNodeVectors_t &TetrahedronElementField<DoubleType>::GetDerivativeNodeVectors(const Tetrahedron &tetrahedron, const std::vector<DoubleType> &evals0, const std::vector<DoubleType> &evals1) const
{

  if (dense_mats_.empty())
  {
    CalcMatrices();
  }

  const size_t tetrahedronIndex = tetrahedron.GetIndex();

  const auto &nodeList = tetrahedron.GetNodeList();
  const auto &ttelist = myregion_->GetTetrahedronToEdgeDataList();
  const auto &edgeDataList = ttelist[tetrahedronIndex];

  thread_local std::array<DoubleType, 3> B;

  thread_local typename TetrahedronElementField<DoubleType>::DerivativeNodeVectors_t nodeVectors;

  //thread_local std::array<std::array<Vector<DoubleType>, 4>, 4> nodeVectors;
  /// foreach node index
  for (size_t i = 0; i < 4; ++i)
  {
//    const ConstNodePtr node_i_ptr = nodeList[i];

    dsMath::RealDenseMatrix<DoubleType> &M = *dense_mats_[tetrahedronIndex].mats[i];
    // for each derivative index
    for (size_t j = 0; j < 4; ++j)
    {
      const ConstNodePtr node_j_ptr = nodeList[j];

      // for each of the 3 edges on the node
      for (size_t k = 0; k < 3; ++k)
      {
        const size_t eindex    = dense_mats_[tetrahedronIndex].edgeIndexes[i][k];
        ConstEdgePtr edge_ptr  = edgeDataList[eindex]->edge;
        const size_t edgeIndex = edge_ptr->GetIndex();

        ConstNodePtr nh = edge_ptr->GetHead();
        ConstNodePtr nt = edge_ptr->GetTail();

        DoubleType rhs = 0.0;
        if (nh == node_j_ptr)
        {
          rhs = evals0[edgeIndex];
        }
        else if (nt == node_j_ptr)
        {
          rhs = evals1[edgeIndex];
        }
        B[k] = rhs;
      }

      bool info = M.Solve(B.data());
      dsAssert(info, "UNEXPECTED");

      //// This is the element field on one of the four nodes
      nodeVectors[i][j] = Vector<DoubleType>(B[0], B[1], B[2]);
    }
  }
  return nodeVectors;
}
template <typename DoubleType>
void TetrahedronElementField<DoubleType>::GetDerivativeFieldPairs(const Tetrahedron &tetrahedron, const EdgeModel &em0, const EdgeModel &em1, TetrahedronElementField<DoubleType>::DerivativeVectorPairs_t &node_based_derivatives) const
{

  const EdgeScalarList<DoubleType> &evals0 = em0.GetScalarValues<DoubleType>();
  const EdgeScalarList<DoubleType> &evals1 = em1.GetScalarValues<DoubleType>();

  const size_t tetrahedronIndex = tetrahedron.GetIndex();

  const auto &nodeList = tetrahedron.GetNodeList();
  const auto &ttelist = myregion_->GetTetrahedronToEdgeDataList();
  const auto &edgeDataList = ttelist[tetrahedronIndex];

  const auto &nodeVectors = GetDerivativeNodeVectors(tetrahedron, evals0, evals1);

  /// for each of the nodes
  for (size_t i = 0; i < 4; ++i)
  {
    const auto *np = nodeList[i];
    /// for each of the derivatives
    for (size_t j = 0; j < 4; ++j)
    {
      // the node ordering on the tetrahedron, not the element edge
      const ConstNodePtr dnode = nodeList[j];

      const Vector<DoubleType> &val = nodeVectors[i][j];

      //// calculate the average on the edge
      for (size_t k = 0; k < 3; ++k)
      {
        const size_t eindex = dense_mats_[tetrahedronIndex].edgeIndexes[i][k];

        const EdgeData &edata  = *edgeDataList[eindex];
        ConstEdgePtr edge_ptr = edata.edge;

//        const size_t edgeIndex = edge_ptr->GetIndex();

        //// This is the node on either end
        const ConstNodePtr nh = edge_ptr->GetHead();
        const ConstNodePtr nt = edge_ptr->GetTail();

        size_t nd = 0;
        if (nh == dnode)
        {
          nd = 0;
        }
        else if (nt == dnode)
        {
          nd = 1;
        }
        else if (edata.nodeopp[0] == dnode)
        {
          nd = 2;
        }
        else if (edata.nodeopp[1] == dnode)
        {
          nd = 3;
        }
        else
        {
          dsAssert(false, "UNEXPECTED");
        }

        size_t nei = 0;
        if (nh == np)
        {
          nei = 0;
        }
        else if (nt == np)
        {
          nei = 1;
        }
        else
        {
          dsAssert(false, "UNEXPECTED");
        }

        node_based_derivatives[nd][eindex][nei] = val;
        //ret[nd][eindex][np] = val;
      }
    }
  }
}

//// em0 is the @n0
//// em1 is the @n1
//// return matrix is based as 2nd index as edge (0-5), 1st index as derivative w.r.t. node (0-3)
//// where 2 the first triangle node off edge and 3 is the 2nd triangle node off edge
//return as [nodeindex][edgeindex]
template <typename DoubleType>
void TetrahedronElementField<DoubleType>::GetTetrahedronElementField(const Tetrahedron &tetrahedron, const EdgeModel &em0, const EdgeModel &em1, TetrahedronElementField<DoubleType>::DerivativeEdgeVectors_t &result) const
{
  // result sized for the 4 derivative nodes
  // has 6 edges

  thread_local TetrahedronElementField<DoubleType>::DerivativeVectorPairs_t node_based_derivatives;
  GetDerivativeFieldPairs(tetrahedron, em0, em1, node_based_derivatives);

  static const auto weight = static_cast<DoubleType>(0.5);
  for (size_t i = 0; i < 4; ++i)
  {
    for (size_t j = 0; j < 6; ++j)
    {
      auto &rentry = result[i][j];
      auto &nbentry = node_based_derivatives[i][j];
      rentry = nbentry[0] + nbentry[1];
      rentry *= weight;
    }
  }
}

template <typename DoubleType>
void TetrahedronElementField<DoubleType>::GetTetrahedronElementFieldPairs(const Tetrahedron &tetrahedron, const EdgeModel &em0, const EdgeModel &em1, TetrahedronElementField<DoubleType>::DerivativeEdgeVectors_t &result0, TetrahedronElementField<DoubleType>::DerivativeEdgeVectors_t &result1) const
{
  thread_local TetrahedronElementField<DoubleType>::DerivativeVectorPairs_t node_based_derivatives;
  GetDerivativeFieldPairs(tetrahedron, em0, em1, node_based_derivatives);

  for (size_t i = 0; i < 4; ++i)
  {
    for (size_t j = 0; j < 6; ++j)
    {
      auto &nbentry = node_based_derivatives[i][j];
      result0[i][j] = nbentry[0];
      result1[i][j] = nbentry[1];
    }
  }
}

template class TetrahedronElementField<double>;
#ifdef DEVSIM_EXTENDED_PRECISION
#include "Float128.hh"
template class TetrahedronElementField<float128>;
#endif

