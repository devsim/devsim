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

#ifndef MODEL_EXPR_DATA_HH
#define MODEL_EXPR_DATA_HH
#include <memory>
#include <vector>

namespace Eqo {
class EquationObject;
typedef std::shared_ptr<EquationObject> EqObjPtr;
}

template <typename T, typename U> class ScalarData;

class NodeModel;

using ConstNodeModelPtr = std::shared_ptr<const NodeModel>;

template<typename T>
using NodeScalarData = ScalarData<NodeModel, T>;

class EdgeModel;

using ConstEdgeModelPtr = std::shared_ptr<const EdgeModel>;

template<typename T>
using EdgeScalarData = ScalarData<EdgeModel, T>;

class TriangleEdgeModel;

using ConstTriangleEdgeModelPtr = std::shared_ptr<const TriangleEdgeModel>;

template<typename T>
using TriangleEdgeScalarData = ScalarData<TriangleEdgeModel, T>;

class TetrahedronEdgeModel;

using ConstTetrahedronEdgeModelPtr = std::shared_ptr<const TetrahedronEdgeModel>;

template<typename T>
using TetrahedronEdgeScalarData = ScalarData<TetrahedronEdgeModel, T>;

class Region;

namespace MEE {
template <typename DoubleType>
class ScalarValuesType {
  public:
  ScalarValuesType() : vals(0), uval(0.0), isuniform(false), length(0) {}
  ScalarValuesType(const std::vector<DoubleType> &v) : vals(&v), uval(0.0), isuniform(false) {length = v.size();}

  ScalarValuesType(DoubleType v, size_t len) : vals(0), uval(v), isuniform(true), length(len) {}

  bool IsUniform() const
  {
    return isuniform;
  }

  const std::vector<DoubleType> &GetVector() const {
    return *vals;
  }

  DoubleType GetScalar() const {
    return uval;
  }

  size_t GetLength() const {
    return length;
  }

  private:
    const std::vector<DoubleType> *vals;
    DoubleType                     uval;
    bool isuniform;
    size_t                     length;
};

enum class datatype {
  NODEDATA=0,
  EDGEDATA,
  TRIANGLEEDGEDATA,
  TETRAHEDRONEDGEDATA,
  DOUBLE,
  INVALID
};

extern const char *const datatypename[];

template<typename T>
using nodeScalarData_ptr = std::shared_ptr<NodeScalarData<T> >;

template<typename T>
using edgeScalarData_ptr = std::shared_ptr<EdgeScalarData<T> >;

template<typename T>
using triangleEdgeScalarData_ptr = std::shared_ptr<TriangleEdgeScalarData<T> >;

template<typename T>
using tetrahedronEdgeScalarData_ptr = std::shared_ptr<TetrahedronEdgeScalarData<T> >;

template <typename DoubleType>
class ModelExprData {
    public:
        explicit ModelExprData(const Region *r = nullptr) : val(0.0), type(datatype::INVALID), reg(r) {};
        ModelExprData(DoubleType x, const Region *r);
        ModelExprData(ConstNodeModelPtr x, const Region *r);
        ModelExprData(ConstEdgeModelPtr x, const Region *r);
        ModelExprData(ConstTriangleEdgeModelPtr x, const Region *r);
        ModelExprData(ConstTetrahedronEdgeModelPtr x, const Region *r);
        ModelExprData(const NodeScalarData<DoubleType> &, const Region *);
        ModelExprData(const EdgeScalarData<DoubleType> &, const Region *);
        ModelExprData(const TriangleEdgeScalarData<DoubleType> &, const Region *);
        ModelExprData(const TetrahedronEdgeScalarData<DoubleType> &, const Region *);

        ModelExprData(const ModelExprData &);
        ModelExprData &operator=(const ModelExprData &);

        template <typename T>
        ModelExprData<DoubleType> &op_equal(const ModelExprData<DoubleType> &, const T &);
        ModelExprData<DoubleType> &operator*=(const ModelExprData &);
        ModelExprData<DoubleType> &operator+=(const ModelExprData &);

        // test or will get undefined data
        DoubleType GetDoubleValue() const
        {
            return val;
        }
        datatype GetType() const {
            return type;
        }


        ScalarValuesType<DoubleType> GetScalarValues() const;

        void convertToTriangleEdgeData();
        void convertToTetrahedronEdgeData();

    private:
        template <typename T>
        void node_op_equal(const ModelExprData &, const T &);
        template <typename T>
        void edge_op_equal(const ModelExprData &, const T &);
        template <typename T>
        void triangle_edge_op_equal(const ModelExprData &, const T &);
        template <typename T>
        void tetrahedron_edge_op_equal(const ModelExprData &, const T &);
        template <typename T>
        void double_op_equal(const ModelExprData &, const T &);

        /// use member function pointer to make sure that we don't do
        /// redundant operations
/** Address this in the future when it makes sense to do so
        typedef ModelExprData &(ModelExprData::*selfopptr)(ModelExprData &);
        ModelExprData &DoSelfOp(const ModelExprData &, selfopptr);
*/

//      void morph();
        typedef ModelExprData<DoubleType> &(ModelExprData<DoubleType>::*selfopptr)(ModelExprData<DoubleType> &);

        // These are the types of data we can keep
        nodeScalarData_ptr<DoubleType>            nodeScalarData;
        edgeScalarData_ptr<DoubleType>            edgeScalarData;
        triangleEdgeScalarData_ptr<DoubleType>    triangleEdgeScalarData;
        tetrahedronEdgeScalarData_ptr<DoubleType> tetrahedronEdgeScalarData;
        DoubleType   val;

        datatype type;
        const    Region *          reg;
};
}
#endif

