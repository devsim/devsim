/***
DEVSIM
Copyright 2013 DEVSIM LLC

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
template <typename T>
using RealDenseMatrix = DenseMatrix<T>;
}

template <typename DoubleType>
class GradientField {

  public:
    explicit GradientField (const Region *);

    ~GradientField();

    Vector<DoubleType>   GetGradient(const Triangle &, const NodeModel &) const;

    Vector<DoubleType>   GetGradient(const Tetrahedron &, const NodeModel &) const;

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

    typedef std::vector<dsMath::RealDenseMatrix<DoubleType> *> dense_matrix_vec_t;
    mutable dense_matrix_vec_t dense_mats_;
};
#endif

