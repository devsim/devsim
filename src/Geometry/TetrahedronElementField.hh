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

#ifndef TETRAHEDRON_ELEMENT_FIELD_HH
#define TETRAHEDRON_ELEMENT_FIELD_HH

template <typename T> class Vector;
#include <array>
#include <cstddef>
#include <vector>

class Region;
class Tetrahedron;

class EdgeModel;

class TetrahedronEdgeModel;

namespace dsMath {
template <typename T> class DenseMatrix;
template <typename T> using RealDenseMatrix = DenseMatrix<T>;
} // namespace dsMath

template <typename DoubleType> struct TetrahedronElementFieldMatrixHolder {
  TetrahedronElementFieldMatrixHolder();
  ~TetrahedronElementFieldMatrixHolder();

  //// 3 edge indexes, 1 for each of the four tetrahedron nodes
  //// The edge index is the order in the edge data list
  size_t edgeIndexes[4][3];
  //// One dense matrix for each Tetrahedron node
  dsMath::RealDenseMatrix<DoubleType> *mats[4];
};

template <typename DoubleType> class TetrahedronElementField {

public:
  explicit TetrahedronElementField(const Region *);

  ~TetrahedronElementField();

  //// Gets the weighted average for a given edge index (0, 1, 2)
  typedef std::array<Vector<DoubleType>, 6> EdgeVectors_t;
  typedef std::array<EdgeVectors_t, 4> DerivativeEdgeVectors_t;
  void GetTetrahedronElementField(const Tetrahedron &,
                                  const TetrahedronEdgeModel &,
                                  EdgeVectors_t &) const;
  void GetTetrahedronElementField(const Tetrahedron &, const EdgeModel &,
                                  EdgeVectors_t &) const;
  void GetTetrahedronElementField(const Tetrahedron &, const EdgeModel &,
                                  const EdgeModel &,
                                  DerivativeEdgeVectors_t &) const;
  void GetTetrahedronElementFieldPairs(const Tetrahedron &, const EdgeModel &,
                                       EdgeVectors_t &, EdgeVectors_t &) const;
  void GetTetrahedronElementFieldPairs(const Tetrahedron &, const EdgeModel &,
                                       const EdgeModel &,
                                       DerivativeEdgeVectors_t &,
                                       DerivativeEdgeVectors_t &) const;

private:
  void GetTetrahedronElementField(const Tetrahedron &,
                                  const std::vector<DoubleType> &,
                                  EdgeVectors_t &) const;
  typedef std::array<Vector<DoubleType>, 4> NodeVectors_t;
  typedef std::array<std::array<Vector<DoubleType>, 4>, 4>
      DerivativeNodeVectors_t;
  const NodeVectors_t &GetNodeVectors(const Tetrahedron &,
                                      const std::vector<DoubleType> &) const;
  const DerivativeNodeVectors_t &
  GetDerivativeNodeVectors(const Tetrahedron &, const std::vector<DoubleType> &,
                           const std::vector<DoubleType> &) const;

  typedef std::array<Vector<DoubleType>, 2> VectorPair_t;
  typedef std::array<VectorPair_t, 6> VectorPairs_t;
  typedef std::array<VectorPairs_t, 4> DerivativeVectorPairs_t;

  void GetFieldPairs(const Tetrahedron &, const std::vector<DoubleType> &,
                     VectorPairs_t &) const;
  void GetDerivativeFieldPairs(const Tetrahedron &, const EdgeModel &,
                               const EdgeModel &,
                               DerivativeVectorPairs_t &) const;

  void PopulateEdgeData(const Tetrahedron &, const EdgeModel &,
                        std::vector<DoubleType> &) const;

  TetrahedronElementField();
  TetrahedronElementField(const TetrahedronElementField &);
  TetrahedronElementField &operator=(TetrahedronElementField &);

  void CalcMatrices() const;
  const Region *myregion_;

  typedef std::vector<TetrahedronElementFieldMatrixHolder<DoubleType>>
      dense_matrix_vec_t;
  mutable dense_matrix_vec_t dense_mats_;
};
#endif
