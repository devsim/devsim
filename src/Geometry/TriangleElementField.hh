/***
DEVSIM
Copyright 2013 DEVSIM LLC

SPDX-License-Identifier: Apache-2.0
***/

#ifndef TRIANGLE_ELEMENT_FIELD_HH
#define TRIANGLE_ELEMENT_FIELD_HH

template <typename T>
class Vector;
#include <vector>
#include <cstddef>
#include <array>

class Region;
class Triangle;

class EdgeModel;

class TriangleEdgeModel;

namespace dsMath {
template <typename T> class DenseMatrix;
template <typename T>
using RealDenseMatrix = DenseMatrix<T>;
}

template <typename DoubleType>
struct TriangleElementFieldMatrixHolder {
  TriangleElementFieldMatrixHolder();
  ~TriangleElementFieldMatrixHolder();

  dsMath::RealDenseMatrix<DoubleType> *mats[3];
};

template <typename DoubleType>
class TriangleElementField {

  public:
    explicit TriangleElementField (const Region *);

    ~TriangleElementField();

    typedef std::array<Vector<DoubleType>, 3> EdgeVectors_t;
    typedef std::array<EdgeVectors_t, 3> DerivativeEdgeVectors_t;

    void GetTriangleElementField(const Triangle &, const TriangleEdgeModel &, const TriangleEdgeModel &, EdgeVectors_t &) const;
    void GetTriangleElementField(const Triangle &, const TriangleEdgeModel &, const EdgeModel &, EdgeVectors_t &) const;
    void GetTriangleElementField(const Triangle &, const TriangleEdgeModel &, const EdgeModel &, const EdgeModel &, DerivativeEdgeVectors_t &) const;

    void GetTriangleElementFieldPairs(const Triangle &, const TriangleEdgeModel &, const EdgeModel &, EdgeVectors_t &, EdgeVectors_t &) const;
    void GetTriangleElementFieldPairs(const Triangle &, const TriangleEdgeModel &, const EdgeModel &, const EdgeModel &, DerivativeEdgeVectors_t &, DerivativeEdgeVectors_t &) const;

  private:
    typedef std::array<DoubleType, 2> WeightPair_t;
    typedef std::array<WeightPair_t, 3> WeightPairs_t;
    typedef std::array<Vector<DoubleType>, 2> VectorPair_t;
    typedef std::array<VectorPair_t, 3> VectorPairs_t;
    typedef std::tuple<VectorPairs_t, WeightPairs_t> TuplePairs_t;
    typedef std::array<WeightPairs_t, 3> DerivativeWeightPairs_t;
    typedef std::array<VectorPairs_t, 3> DerivativeVectorPairs_t;
    typedef std::tuple<DerivativeVectorPairs_t, DerivativeWeightPairs_t> DerivativeTuplePairs_t;

    typedef std::array<Vector<DoubleType>, 3> EdgePairVectors_t;
    typedef std::array<EdgePairVectors_t, 3> DerivativeEdgePairVectors_t;

    void GetFieldPairs(const Triangle &, const TriangleEdgeModel &, const std::vector<DoubleType> &, TuplePairs_t &) const;
    void GetTriangleElementField(const Triangle &, const TriangleEdgeModel &, const std::vector<DoubleType> &, EdgeVectors_t &) const;

    const EdgePairVectors_t &GetEdgePairVectors(const Triangle &, const std::vector<DoubleType> &) const;
    const DerivativeEdgePairVectors_t &GetDerivativeEdgePairVectors(const Triangle &, const std::vector<DoubleType> &, const std::vector<DoubleType> &) const;

    void GetDerivativeFieldPairs(const Triangle &, const TriangleEdgeModel &, const EdgeModel &, const EdgeModel &, DerivativeTuplePairs_t &) const;

    void PopulateEdgeData(const Triangle &, const EdgeModel &, std::vector<DoubleType> &) const;

    TriangleElementField();
    TriangleElementField(const TriangleElementField &);
    TriangleElementField  &operator=(TriangleElementField &);

    void CalcMatrices() const;
    const Region *myregion_;

    typedef std::vector<TriangleElementFieldMatrixHolder<DoubleType>> dense_matrix_vec_t;
    ///// Need to handle 01, 02, 12 combinations
    ///// Indexing should be (i + j - 1) assuming symmetry
    mutable dense_matrix_vec_t dense_mats_;

    ///// Corresponds to the edge position in triangle to edge list
    static const size_t row0_[3];
    static const size_t row1_[3];
    ///// the node shared of the indexed edge pair in the triangle node list
    static const size_t node_shared_[3];

};
#endif
