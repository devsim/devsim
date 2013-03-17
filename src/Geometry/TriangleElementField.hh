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
