/***
DEVSIM
Copyright 2013 DEVSIM LLC

SPDX-License-Identifier: Apache-2.0
***/

#include "ModelExprData.hh"
#include "NodeScalarData.hh"
#include "EdgeScalarData.hh"
#include "TriangleEdgeScalarData.hh"
#include "TetrahedronEdgeScalarData.hh"
#include "NodeModel.hh"
#include "EdgeModel.hh"
#include "TriangleEdgeModel.hh"
#include "TetrahedronEdgeModel.hh"
#include "EdgeData.hh"
#include "dsAssert.hh"

#include "NodeScalarData.hh"
#include "EdgeScalarData.hh"
#include "TriangleEdgeScalarData.hh"
#include "TetrahedronEdgeScalarData.hh"

//// can remove all three when we move edge to triangle conversion to different place
#include "Region.hh"
#include "Triangle.hh"
#include "Edge.hh"
#include "Tetrahedron.hh"

namespace MEE {
const char * const datatypename[] = {
  "nodedata",
  "edgedata",
  "triangleedgedata",
  "tetrahedronedgedata",
// "nodemodel",
// "edgemodel",
// "triangleedgemodel",
// "tetrahedronedgemodel",
  "double",
  "invalid"
};

/// Undefined behavior if we have a double or invalid data
template <typename DoubleType>
ScalarValuesType<DoubleType> ModelExprData<DoubleType>::GetScalarValues() const
{
  ScalarValuesType<DoubleType> ret;

  if (type == datatype::NODEDATA)
  {
    if (nodeScalarData->IsUniform())
    {
      ret = ScalarValuesType<DoubleType>(nodeScalarData->GetUniformValue(), nodeScalarData->GetLength());
    }
    else
    {
      ret = ScalarValuesType<DoubleType>(nodeScalarData->GetScalarList());
    }
  }
  else if (type == datatype::EDGEDATA)
  {
    if (edgeScalarData->IsUniform())
    {
      ret = ScalarValuesType<DoubleType>(edgeScalarData->GetUniformValue(), edgeScalarData->GetLength());
    }
    else
    {
      ret = ScalarValuesType<DoubleType>(edgeScalarData->GetScalarList());
    }
  }
  else if (type == datatype::TRIANGLEEDGEDATA)
  {
    if (triangleEdgeScalarData->IsUniform())
    {
      ret = ScalarValuesType<DoubleType>(triangleEdgeScalarData->GetUniformValue(), triangleEdgeScalarData->GetLength());
    }
    else
    {
      ret = ScalarValuesType<DoubleType>(triangleEdgeScalarData->GetScalarList());
    }
  }
  else if (type == datatype::TETRAHEDRONEDGEDATA)
  {
    if (tetrahedronEdgeScalarData->IsUniform())
    {
      ret = ScalarValuesType<DoubleType>(tetrahedronEdgeScalarData->GetUniformValue(), tetrahedronEdgeScalarData->GetLength());
    }
    else
    {
      ret = ScalarValuesType<DoubleType>(tetrahedronEdgeScalarData->GetScalarList());
    }
  }
  else
  {
    dsAssert(false, "UNEXPECTED");
    //// this data is invalid and will crash
  }
  return ret;
}

template <typename DoubleType>
ModelExprData<DoubleType>::ModelExprData(const NodeScalarData<DoubleType> &x, const Region *r) : val(0.0), type(datatype::NODEDATA), reg(r)
{
  NodeScalarData<DoubleType> *foo = new NodeScalarData<DoubleType>(x);
  nodeScalarData = std::shared_ptr<NodeScalarData<DoubleType>>(foo);
};

template <typename DoubleType>
ModelExprData<DoubleType>::ModelExprData(const EdgeScalarData<DoubleType> &x, const Region *r) : val(0.0), type(datatype::EDGEDATA), reg(r)
{
    EdgeScalarData<DoubleType> *foo = new EdgeScalarData<DoubleType>(x);
    edgeScalarData = std::shared_ptr<EdgeScalarData<DoubleType>>(foo);
};

template <typename DoubleType>
ModelExprData<DoubleType>::ModelExprData(const TriangleEdgeScalarData<DoubleType> &x, const Region *r) : val(0.0), type(datatype::TRIANGLEEDGEDATA), reg(r)
{
  TriangleEdgeScalarData<DoubleType> *foo = new TriangleEdgeScalarData<DoubleType>(x);
  triangleEdgeScalarData = std::shared_ptr<TriangleEdgeScalarData<DoubleType>>(foo);
};

template <typename DoubleType>
ModelExprData<DoubleType>::ModelExprData(const TetrahedronEdgeScalarData<DoubleType> &x, const Region *r) : val(0.0), type(datatype::TETRAHEDRONEDGEDATA), reg(r)
{
  TetrahedronEdgeScalarData<DoubleType> *foo = new TetrahedronEdgeScalarData<DoubleType>(x);
  tetrahedronEdgeScalarData = std::shared_ptr<TetrahedronEdgeScalarData<DoubleType>>(foo);
};

template <typename DoubleType>
ModelExprData<DoubleType> &ModelExprData<DoubleType>::operator=(const ModelExprData<DoubleType> &x)
{
  // Since this class is not derived, we don't have to copy the base part
  if (&x != this)
  {
    nodeScalarData  = x.nodeScalarData;
    edgeScalarData  = x.edgeScalarData;
    triangleEdgeScalarData = x.triangleEdgeScalarData;
    tetrahedronEdgeScalarData = x.tetrahedronEdgeScalarData;
    val  = x.val;
    type = x.type;
    reg  = x.reg;
  }
  return *this;
}

/// need to make full copy
template <typename DoubleType>
ModelExprData<DoubleType>::ModelExprData(const ModelExprData &x) : nodeScalarData(x.nodeScalarData), edgeScalarData(x.edgeScalarData), triangleEdgeScalarData(x.triangleEdgeScalarData), tetrahedronEdgeScalarData(x.tetrahedronEdgeScalarData), val(x.val), type(x.type), reg(x.reg)
{
}

template <typename DoubleType>
ModelExprData<DoubleType>::ModelExprData(DoubleType x, const Region *r)
  : val(x), type(datatype::DOUBLE), reg(r)
{
}

template <typename DoubleType>
ModelExprData<DoubleType>::ModelExprData(ConstNodeModelPtr x, const Region *r)
  : val(0.0), type(datatype::NODEDATA), reg(r)
{
  NodeScalarData<DoubleType> *n = new NodeScalarData<DoubleType>(*x);
  nodeScalarData = nodeScalarData_ptr<DoubleType>(n);
}

template <typename DoubleType>
ModelExprData<DoubleType>::ModelExprData(ConstEdgeModelPtr x, const Region *r)
  : val(0.0), type(datatype::EDGEDATA), reg(r)
{
  EdgeScalarData<DoubleType> *e = new EdgeScalarData<DoubleType>(*x);
  edgeScalarData = edgeScalarData_ptr<DoubleType>(e);
}

template <typename DoubleType>
ModelExprData<DoubleType>::ModelExprData(ConstTriangleEdgeModelPtr x, const Region *r)
  : val(0.0), type(datatype::TRIANGLEEDGEDATA), reg(r)
{
  TriangleEdgeScalarData<DoubleType> *e = new TriangleEdgeScalarData<DoubleType>(*x);
  triangleEdgeScalarData = triangleEdgeScalarData_ptr<DoubleType>(e);
}

template <typename DoubleType>
ModelExprData<DoubleType>::ModelExprData(ConstTetrahedronEdgeModelPtr x, const Region *r)
  : val(0.0), type(datatype::TETRAHEDRONEDGEDATA), reg(r)
{
  TetrahedronEdgeScalarData<DoubleType> *e = new TetrahedronEdgeScalarData<DoubleType>(*x);
  tetrahedronEdgeScalarData = tetrahedronEdgeScalarData_ptr<DoubleType>(e);
}


namespace {
///// TODO: This functionality should be available elsewhere
///// Remove Region.hh include from above when this is done
template <typename DoubleType>
void convertEdgeToTriangleEdgeData(const Region &region, const std::vector<DoubleType> &edgeData, std::vector<DoubleType> &triangleData)
{
  const ConstTriangleList &ctl = region.GetTriangleList();
  triangleData.resize(3*ctl.size());

  const Region::TriangleToConstEdgeList_t &ttelist = region.GetTriangleToEdgeList();

  for (ConstTriangleList::const_iterator it = ctl.begin(); it != ctl.end(); ++it)
  {
    const Triangle &triangle = **it;
    ///// This should exactly match ordering in vector of triangles
    const size_t tindex = triangle.GetIndex();

    const ConstEdgeList &cel = ttelist[tindex];
    for (size_t i = 0; i < 3; ++i)
    {
      const size_t eindex = cel[i]->GetIndex();
      triangleData[3*tindex + i] = edgeData[eindex];
    }
  }
}

template <typename DoubleType>
void convertEdgeToTetrahedronEdgeData(const Region &region, const std::vector<DoubleType> &edgeData, std::vector<DoubleType> &tetrahedronData)
{
  const ConstTetrahedronList &ctl = region.GetTetrahedronList();
  tetrahedronData.resize(6*ctl.size());

  const Region::TetrahedronToConstEdgeDataList_t &ttelist = region.GetTetrahedronToEdgeDataList();

  for (ConstTetrahedronList::const_iterator it = ctl.begin(); it != ctl.end(); ++it)
  {
    const Tetrahedron &tetrahedron = **it;
    ///// This should exactly match ordering in vector of tetrahedrons
    const size_t tindex = tetrahedron.GetIndex();

    const ConstEdgeDataList &cel = ttelist[tindex];
    for (size_t i = 0; i < 6; ++i)
    {
      const size_t eindex = cel[i]->edge->GetIndex();
      tetrahedronData[6*tindex + i] = edgeData[eindex];
    }
  }
}
}

template <typename DoubleType>
void ModelExprData<DoubleType>::convertToTriangleEdgeData()
{
  //// TODO: check for uniform value to simplify
  if (type == datatype::EDGEDATA)
  {
    if (edgeScalarData->IsUniform())
    {
      const DoubleType val = edgeScalarData->GetUniformValue();
      size_t       len = 3 * reg->GetTriangleList().size();

      type = datatype::TRIANGLEEDGEDATA;
      TriangleEdgeScalarData<DoubleType> *e = new TriangleEdgeScalarData<DoubleType>(val, len);
      triangleEdgeScalarData = triangleEdgeScalarData_ptr<DoubleType>(e);
      edgeScalarData.reset();
    }
    else
    {
      std::vector<DoubleType> out;
      const std::vector<DoubleType> &evals = edgeScalarData->GetScalarList();
      convertEdgeToTriangleEdgeData(*reg, evals, out);
      if (!out.empty())
      {
        type = datatype::TRIANGLEEDGEDATA;
        TriangleEdgeScalarData<DoubleType> *e = new TriangleEdgeScalarData<DoubleType>(out);
        triangleEdgeScalarData = triangleEdgeScalarData_ptr<DoubleType>(e);
        edgeScalarData.reset();
      }
    }
  }
}

template <typename DoubleType>
void ModelExprData<DoubleType>::convertToTetrahedronEdgeData()
{
  if (type == datatype::EDGEDATA)
  {
    if (edgeScalarData->IsUniform())
    {
      const DoubleType val = edgeScalarData->GetUniformValue();
      size_t       len = 6 * reg->GetTriangleList().size();

      type = datatype::TETRAHEDRONEDGEDATA;
      TetrahedronEdgeScalarData<DoubleType> *e = new TetrahedronEdgeScalarData<DoubleType>(val, len);
      tetrahedronEdgeScalarData = tetrahedronEdgeScalarData_ptr<DoubleType>(e);
      edgeScalarData.reset();
    }
    else
    {
      std::vector<DoubleType> out;
      const std::vector<DoubleType> &evals = edgeScalarData->GetScalarList();
      convertEdgeToTetrahedronEdgeData(*reg, evals, out);
      if (!out.empty())
      {
        type = datatype::TETRAHEDRONEDGEDATA;
        TetrahedronEdgeScalarData<DoubleType> *e = new TetrahedronEdgeScalarData<DoubleType>(out);
        tetrahedronEdgeScalarData = tetrahedronEdgeScalarData_ptr<DoubleType>(e);
        edgeScalarData.reset();
      }
    }
  }
}

template <typename DoubleType> template <typename T>
void ModelExprData<DoubleType>::node_op_equal(const ModelExprData<DoubleType> &other, const T &func)
{
  if (!nodeScalarData.unique())
  {
    nodeScalarData = nodeScalarData_ptr<DoubleType>(new NodeScalarData<DoubleType>(*nodeScalarData));
  }

  if (other.type == datatype::DOUBLE)
  {
    nodeScalarData->op_equal_scalar(other.val, func);
  }
  else if (other.type == datatype::NODEDATA)
  {
    nodeScalarData->op_equal_data(*other.nodeScalarData, func);
  }
  else
  {
    type = datatype::INVALID;
  }
}

template <typename DoubleType> template <typename T>
void ModelExprData<DoubleType>::edge_op_equal(const ModelExprData &other, const T &func)
{
  if (!edgeScalarData.unique())
  {
    edgeScalarData = edgeScalarData_ptr<DoubleType>(new EdgeScalarData<DoubleType>(*edgeScalarData));
  }

  if (other.type == datatype::DOUBLE)
  {
    edgeScalarData->op_equal_scalar(other.val, func);
  }
  else if (other.type == datatype::EDGEDATA)
  {
    edgeScalarData->op_equal_data(*other.edgeScalarData, func);
  }
  else if (other.type == datatype::TRIANGLEEDGEDATA)
  {
    convertToTriangleEdgeData();
    triangleEdgeScalarData->op_equal_data(*other.triangleEdgeScalarData, func);
  }
  else if (other.type == datatype::TETRAHEDRONEDGEDATA)
  {
    convertToTetrahedronEdgeData();
    tetrahedronEdgeScalarData->op_equal_data(*other.tetrahedronEdgeScalarData, func);
  }
  else
  {
    type = datatype::INVALID;
  }
}

template <typename DoubleType> template <typename T>
void ModelExprData<DoubleType>::triangle_edge_op_equal(const ModelExprData &other, const T &func)
{
  if (!triangleEdgeScalarData.unique())
  {
    triangleEdgeScalarData = triangleEdgeScalarData_ptr<DoubleType>(new TriangleEdgeScalarData<DoubleType>(*triangleEdgeScalarData));
  }

  if (other.type == datatype::DOUBLE)
  {
    triangleEdgeScalarData->op_equal_scalar(other.val, func);
  }
  else if (other.type == datatype::TRIANGLEEDGEDATA)
  {
    triangleEdgeScalarData->op_equal_data(*other.triangleEdgeScalarData, func);
  }
  else if (other.type == datatype::EDGEDATA)
  {
    ModelExprData temp(other);
    temp.convertToTriangleEdgeData();
    triangleEdgeScalarData->op_equal_data(*temp.triangleEdgeScalarData, func);
  }
  else
  {
      type = datatype::INVALID;
  }
}

template <typename DoubleType> template <typename T>
void ModelExprData<DoubleType>::tetrahedron_edge_op_equal(const ModelExprData &other, const T &func)
{
  if (!tetrahedronEdgeScalarData.unique())
  {
    tetrahedronEdgeScalarData = tetrahedronEdgeScalarData_ptr<DoubleType>(new TetrahedronEdgeScalarData<DoubleType>(*tetrahedronEdgeScalarData));
  }

  if (other.type == datatype::DOUBLE)
  {
    tetrahedronEdgeScalarData->op_equal_scalar(other.val, func);
  }
  else if (other.type == datatype::TETRAHEDRONEDGEDATA)
  {
    tetrahedronEdgeScalarData->op_equal_data(*other.tetrahedronEdgeScalarData, func);
  }
  else if (other.type == datatype::EDGEDATA)
  {
    ModelExprData temp(other);
    temp.convertToTetrahedronEdgeData();
    tetrahedronEdgeScalarData->op_equal_data(*temp.tetrahedronEdgeScalarData, func);
  }
  else
  {
      type = datatype::INVALID;
  }
}

template <typename DoubleType> template <typename T>
void ModelExprData<DoubleType>::double_op_equal(const ModelExprData<DoubleType> &other, const T &func)
{
  if (other.type == datatype::DOUBLE)
  {
    func(val, other.val);
  }
  else if (other.type == datatype::NODEDATA)
  {
    NodeScalarData<DoubleType> *x = new NodeScalarData<DoubleType>(val, other.nodeScalarData->GetLength());
    nodeScalarData = nodeScalarData_ptr<DoubleType>(x);
    nodeScalarData->op_equal_data(*other.nodeScalarData, func);
    type = datatype::NODEDATA;
  }
  else if (other.type == datatype::EDGEDATA)
  {
    EdgeScalarData<DoubleType> *x = new EdgeScalarData<DoubleType>(val, other.edgeScalarData->GetLength());
    edgeScalarData = edgeScalarData_ptr<DoubleType>(x);
    edgeScalarData->op_equal_data(*other.edgeScalarData, func);
    type = datatype::EDGEDATA;
  }
  else if (other.type == datatype::TRIANGLEEDGEDATA)
  {
    TriangleEdgeScalarData<DoubleType> *x = new TriangleEdgeScalarData<DoubleType>(val, other.triangleEdgeScalarData->GetLength());
    triangleEdgeScalarData = triangleEdgeScalarData_ptr<DoubleType>(x);
    triangleEdgeScalarData->op_equal_data(*other.triangleEdgeScalarData, func);
    type = datatype::TRIANGLEEDGEDATA;
  }
  else if (other.type == datatype::TETRAHEDRONEDGEDATA)
  {
    TetrahedronEdgeScalarData<DoubleType> *x = new TetrahedronEdgeScalarData<DoubleType>(val, other.tetrahedronEdgeScalarData->GetLength());
    tetrahedronEdgeScalarData = tetrahedronEdgeScalarData_ptr<DoubleType>(x);
    tetrahedronEdgeScalarData->op_equal_data(*other.tetrahedronEdgeScalarData, func);
    type = datatype::TETRAHEDRONEDGEDATA;
  }
  else
  {
    type = datatype::INVALID;
  }
}

template <typename DoubleType> template <typename T>
ModelExprData<DoubleType> &ModelExprData<DoubleType>::op_equal(const ModelExprData<DoubleType> &other, const T &func)
{
  switch (type) {
    case datatype::DOUBLE:
            double_op_equal(other, func);
            break;
    case datatype::NODEDATA:
            node_op_equal(other, func);
            break;
    case datatype::EDGEDATA:
            edge_op_equal(other, func);
            break;
    case datatype::TRIANGLEEDGEDATA:
            triangle_edge_op_equal(other, func);
            break;
    case datatype::TETRAHEDRONEDGEDATA:
            tetrahedron_edge_op_equal(other, func);
            break;
    case datatype::INVALID:
    default:
            break;
  }


  return *this;
}

template <typename DoubleType>
ModelExprData<DoubleType> &ModelExprData<DoubleType>::operator+=(const ModelExprData<DoubleType> &other)
{
  this->op_equal(other, ScalarDataHelper::plus_equal<DoubleType>());
  return *this;
}

template <typename DoubleType>
ModelExprData<DoubleType> &ModelExprData<DoubleType>::operator*=(const ModelExprData<DoubleType> &other)
{
  this->op_equal(other, ScalarDataHelper::times_equal<DoubleType>());
  return *this;
}

//// Manual Template Instantiation
template class ModelExprData<double>;
#ifdef DEVSIM_EXTENDED_PRECISION
#include "Float128.hh"
template class ModelExprData<float128>;
#endif
}

