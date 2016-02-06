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

#ifndef TRIANGLE_ELEMENT_FIELD_HH
#define TRIANGLE_ELEMENT_FIELD_HH

class Vector;
#include <vector>
#include <cstddef>

class Region;
class Triangle;
class EdgeModel;
class TriangleEdgeModel;

namespace dsMath {
template <typename T> class DenseMatrix;
typedef DenseMatrix<double> RealDenseMatrix;
}

struct TriangleElementFieldMatrixHolder {
  TriangleElementFieldMatrixHolder();
  ~TriangleElementFieldMatrixHolder();

  dsMath::RealDenseMatrix *mats[3];
};

class TriangleElementField {

  public:
    explicit TriangleElementField (const Region *);

    ~TriangleElementField();

    std::vector<Vector> GetTriangleElementField(const Triangle &, const TriangleEdgeModel &) const;
    std::vector<Vector> GetTriangleElementField(const Triangle &, const EdgeModel &) const;
    std::vector<Vector> GetTriangleElementField(const Triangle &, const std::vector<double> &) const;
    std::vector<std::vector<Vector> > GetTriangleElementField(const Triangle &, const EdgeModel &, const EdgeModel &) const;

  private:
    TriangleElementField();
    TriangleElementField(const TriangleElementField &);
    TriangleElementField  &operator=(TriangleElementField &);

    void CalcMatrices() const;
    const Region *myregion_;

    typedef std::vector<TriangleElementFieldMatrixHolder> dense_matrix_vec_t;
    ///// Need to handle 01, 02, 12 combinations
    ///// Indexing should be (i + j - 1) assuming symmetry
    mutable dense_matrix_vec_t dense_mats_;

    ///// Corresponds to the edge position in triangle to edge list
    static const size_t row0_[3];
    static const size_t row1_[3];

};
#endif
