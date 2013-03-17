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

#ifndef GRADIENT_FIELD_HH
#define GRADIENT_FIELD_HH
/*
 * This is owned by the region.  It will be evaluated the first time it is accessed through the region
 *
 * It holds all of the dense matrix structures with the LU factors for finding the gradient on a triangle
 */
#include "Vector.hh"

#include <vector>

class Region;
class Triangle;
class Tetrahedron;
class NodeModel;

namespace dsMath {
template <typename T> class DenseMatrix;
typedef DenseMatrix<double> RealDenseMatrix;
}

class GradientField {

  public:
    explicit GradientField (const Region *);

    ~GradientField();

    Vector   GetGradient(const Triangle &, const NodeModel &) const;

    Vector   GetGradient(const Tetrahedron &, const NodeModel &) const;

  private:
    const Region &GetRegion() const
    {
      return *myregion_;
    }
    GradientField();
    GradientField(const GradientField &);
    GradientField  &operator=(GradientField &);

    void CalcMatrices() const;
    void CalcMatrices2d() const;
    void CalcMatrices3d() const;
    const Region *myregion_;

    typedef std::vector<dsMath::RealDenseMatrix *> dense_matrix_vec_t;
    mutable dense_matrix_vec_t dense_mats_;
};
#endif
