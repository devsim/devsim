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

#ifndef TETRAHEDRON_ELEMENT_FIELD_HH
#define TETRAHEDRON_ELEMENT_FIELD_HH

class Vector;
#include <vector>
#include <cstddef>

class Region;
class Tetrahedron;
class EdgeModel;
class TetrahedronEdgeModel;

namespace dsMath {
template <typename T> class DenseMatrix;
typedef DenseMatrix<double> RealDenseMatrix;
}

struct TetrahedronElementFieldMatrixHolder {
  TetrahedronElementFieldMatrixHolder();
  ~TetrahedronElementFieldMatrixHolder();

  //// 3 edge indexes, 1 for each of the four tetrahedron nodes
  //// The edge index is the order in the edge data list
  size_t edgeIndexes[4][3];
  //// One dense matrix for each Tetrahedron node
  dsMath::RealDenseMatrix *mats[4];
};

class TetrahedronElementField {

  public:
    explicit TetrahedronElementField (const Region *);

    ~TetrahedronElementField();

    //// Gets the weighted average for a given edge index (0, 1, 2)
    std::vector<Vector> GetTetrahedronElementField(const Tetrahedron &, const TetrahedronEdgeModel &) const;
    std::vector<Vector> GetTetrahedronElementField(const Tetrahedron &, const EdgeModel &) const;
    std::vector<Vector> GetTetrahedronElementField(const Tetrahedron &, const std::vector<double> &) const;
    //// Gets the weighted average for a given edge index (0, 1, 2) w.r.t. given node index (0, 1, 2)
    std::vector<std::vector<Vector> > GetTetrahedronElementField(const Tetrahedron &, const EdgeModel &, const EdgeModel &) const;

  private:
    TetrahedronElementField();
    TetrahedronElementField(const TetrahedronElementField &);
    TetrahedronElementField  &operator=(TetrahedronElementField &);

    void CalcMatrices() const;
    const Region *myregion_;

    typedef std::vector<TetrahedronElementFieldMatrixHolder> dense_matrix_vec_t;
    mutable dense_matrix_vec_t dense_mats_;

};
#endif
