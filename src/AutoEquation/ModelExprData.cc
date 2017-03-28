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
const char * const ModelExprData::datatypename[] = {
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
ModelExprData::ScalarValuesType ModelExprData::GetScalarValues() const
{
  ScalarValuesType ret;

  if (type == NODEDATA)
  {
    if (nodeScalarData->IsUniform())
    {
      ret = ScalarValuesType(nodeScalarData->GetUniformValue(), nodeScalarData->GetLength());
    }
    else
    {
      ret = ScalarValuesType(nodeScalarData->GetScalarList());
    }
  }
  else if (type == EDGEDATA)
  {
    if (edgeScalarData->IsUniform())
    {
      ret = ScalarValuesType(edgeScalarData->GetUniformValue(), edgeScalarData->GetLength());
    }
    else
    {
      ret = ScalarValuesType(edgeScalarData->GetScalarList());
    }
  }
  else if (type == TRIANGLEEDGEDATA)
  {
    if (triangleEdgeScalarData->IsUniform())
    {
      ret = ScalarValuesType(triangleEdgeScalarData->GetUniformValue(), triangleEdgeScalarData->GetLength());
    }
    else
    {
      ret = ScalarValuesType(triangleEdgeScalarData->GetScalarList());
    }
  }
  else if (type == TETRAHEDRONEDGEDATA)
  {
    if (tetrahedronEdgeScalarData->IsUniform())
    {
      ret = ScalarValuesType(tetrahedronEdgeScalarData->GetUniformValue(), tetrahedronEdgeScalarData->GetLength());
    }
    else
    {
      ret = ScalarValuesType(tetrahedronEdgeScalarData->GetScalarList());
    }
  }
  else
  {
    dsAssert(false, "UNEXPECTED");
    //// this data is invalid and will crash
  }
  return ret;
}

ModelExprData::ModelExprData(const NodeScalarData &x, const Region *r) : val(0.0), type(NODEDATA), reg(r)
{
  NodeScalarData *foo = new NodeScalarData(x);
  nodeScalarData = std::shared_ptr<NodeScalarData>(foo);
};

ModelExprData::ModelExprData(const EdgeScalarData &x, const Region *r) : val(0.0), type(EDGEDATA), reg(r)
{
    EdgeScalarData *foo = new EdgeScalarData(x);
    edgeScalarData = std::shared_ptr<EdgeScalarData>(foo);
};

ModelExprData::ModelExprData(const TriangleEdgeScalarData &x, const Region *r) : val(0.0), type(TRIANGLEEDGEDATA), reg(r)
{
  TriangleEdgeScalarData *foo = new TriangleEdgeScalarData(x);
  triangleEdgeScalarData = std::shared_ptr<TriangleEdgeScalarData>(foo);
};

ModelExprData::ModelExprData(const TetrahedronEdgeScalarData &x, const Region *r) : val(0.0), type(TETRAHEDRONEDGEDATA), reg(r)
{
  TetrahedronEdgeScalarData *foo = new TetrahedronEdgeScalarData(x);
  tetrahedronEdgeScalarData = std::shared_ptr<TetrahedronEdgeScalarData>(foo);
};


ModelExprData &ModelExprData::operator=(const ModelExprData &x)
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
ModelExprData::ModelExprData(const ModelExprData &x) : nodeScalarData(x.nodeScalarData), edgeScalarData(x.edgeScalarData), triangleEdgeScalarData(x.triangleEdgeScalarData), tetrahedronEdgeScalarData(x.tetrahedronEdgeScalarData), val(x.val), type(x.type), reg(x.reg)
{
}

ModelExprData::ModelExprData(double x, const Region *r)
  : val(x), type(DOUBLE), reg(r)
{
}

ModelExprData::ModelExprData(ConstNodeModelPtr x, const Region *r)
  : val(0.0), type(NODEDATA), reg(r)
{
  //// TODO: optimize for uniform values
  NodeScalarData *n = new NodeScalarData(*x);
  nodeScalarData = nodeScalarData_ptr(n);
}

ModelExprData::ModelExprData(ConstEdgeModelPtr x, const Region *r)
  : val(0.0), type(EDGEDATA), reg(r)
{
  EdgeScalarData *e = new EdgeScalarData(*x);
  edgeScalarData = edgeScalarData_ptr(e);
}

ModelExprData::ModelExprData(ConstTriangleEdgeModelPtr x, const Region *r)
  : val(0.0), type(TRIANGLEEDGEDATA), reg(r)
{
  TriangleEdgeScalarData *e = new TriangleEdgeScalarData(*x);
  triangleEdgeScalarData = triangleEdgeScalarData_ptr(e);
}

ModelExprData::ModelExprData(ConstTetrahedronEdgeModelPtr x, const Region *r)
  : val(0.0), type(TETRAHEDRONEDGEDATA), reg(r)
{
  TetrahedronEdgeScalarData *e = new TetrahedronEdgeScalarData(*x);
  tetrahedronEdgeScalarData = tetrahedronEdgeScalarData_ptr(e);
}


namespace {
///// TODO: This functionality should be available elsewhere
///// Remove Region.hh include from above when this is done
void convertEdgeToTriangleEdgeData(const Region &region, const std::vector<double> &edgeData, std::vector<double> &triangleData)
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

void convertEdgeToTetrahedronEdgeData(const Region &region, const std::vector<double> &edgeData, std::vector<double> &tetrahedronData)
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

void ModelExprData::convertToTriangleEdgeData()
{
  //// TODO: check for uniform value to simplify
  if (type == EDGEDATA)
  {
    if (edgeScalarData->IsUniform())
    {
      const double val = edgeScalarData->GetUniformValue();
      size_t       len = 3 * reg->GetTriangleList().size();
      
      type = TRIANGLEEDGEDATA;
      TriangleEdgeScalarData *e = new TriangleEdgeScalarData(val, len);
      triangleEdgeScalarData = triangleEdgeScalarData_ptr(e);
      edgeScalarData.reset();
    }
    else
    {
      std::vector<double> out;
      const std::vector<double> &evals = edgeScalarData->GetScalarList();
      convertEdgeToTriangleEdgeData(*reg, evals, out);
      if (!out.empty())
      {
        type = TRIANGLEEDGEDATA;
        TriangleEdgeScalarData *e = new TriangleEdgeScalarData(out);
        triangleEdgeScalarData = triangleEdgeScalarData_ptr(e);
        edgeScalarData.reset();
      }
    }
  }
}

void ModelExprData::convertToTetrahedronEdgeData()
{
  //// TODO: check for uniform value to simplify
  if (type == EDGEDATA)
  {
    if (edgeScalarData->IsUniform())
    {
      const double val = edgeScalarData->GetUniformValue();
      size_t       len = 6 * reg->GetTriangleList().size();
      
      type = TETRAHEDRONEDGEDATA;
      TetrahedronEdgeScalarData *e = new TetrahedronEdgeScalarData(val, len);
      tetrahedronEdgeScalarData = tetrahedronEdgeScalarData_ptr(e);
      edgeScalarData.reset();
    }
    else
    {
      std::vector<double> out;
      const std::vector<double> &evals = edgeScalarData->GetScalarList();
      convertEdgeToTetrahedronEdgeData(*reg, evals, out);
      if (!out.empty())
      {
        type = TETRAHEDRONEDGEDATA;
        TetrahedronEdgeScalarData *e = new TetrahedronEdgeScalarData(out);
        tetrahedronEdgeScalarData = tetrahedronEdgeScalarData_ptr(e);
        edgeScalarData.reset();
      }
    }
  }
}

template<typename T>
void ModelExprData::node_op_equal(const ModelExprData &other, const T &func)
{
  if (!nodeScalarData.unique())
  {
    nodeScalarData = nodeScalarData_ptr(new NodeScalarData(*nodeScalarData));
  }

  if (other.type == DOUBLE)
  {
    nodeScalarData->op_equal(other.val, func);
  }
  else if (other.type == NODEDATA)
  {
    nodeScalarData->op_equal(*other.nodeScalarData, func);
  }
  else
  {
    type = INVALID;
  }
}

template<typename T>
void ModelExprData::edge_op_equal(const ModelExprData &other, const T &func)
{
  if (!edgeScalarData.unique())
  {
    edgeScalarData = edgeScalarData_ptr(new EdgeScalarData(*edgeScalarData));
  }

  if (other.type == DOUBLE)
  {
    edgeScalarData->op_equal(other.val, func);
  }
  else if (other.type == EDGEDATA)
  {
    edgeScalarData->op_equal(*other.edgeScalarData, func);
  }
  else if (other.type == TRIANGLEEDGEDATA)
  {
    convertToTriangleEdgeData();
    triangleEdgeScalarData->op_equal(*other.triangleEdgeScalarData, func);
  }
  else if (other.type == TETRAHEDRONEDGEDATA)
  {
    convertToTetrahedronEdgeData();
    tetrahedronEdgeScalarData->op_equal(*other.tetrahedronEdgeScalarData, func);
  }
  else
  {
    type = INVALID;
  }
}

template<typename T>
void ModelExprData::triangle_edge_op_equal(const ModelExprData &other, const T &func)
{
  if (!triangleEdgeScalarData.unique())
  {
    triangleEdgeScalarData = triangleEdgeScalarData_ptr(new TriangleEdgeScalarData(*triangleEdgeScalarData));
  }

  if (other.type == DOUBLE)
  {
    triangleEdgeScalarData->op_equal(other.val, func);
  }
  else if (other.type == TRIANGLEEDGEDATA)
  {
    triangleEdgeScalarData->op_equal(*other.triangleEdgeScalarData, func);
  }
  else if (other.type == EDGEDATA)
  {
    ModelExprData temp(other);
    temp.convertToTriangleEdgeData();
    triangleEdgeScalarData->op_equal(*temp.triangleEdgeScalarData, func);
  }
  else
  {
      type = INVALID;
  }
}

template<typename T>
void ModelExprData::tetrahedron_edge_op_equal(const ModelExprData &other, const T &func)
{
  if (!tetrahedronEdgeScalarData.unique())
  {
    tetrahedronEdgeScalarData = tetrahedronEdgeScalarData_ptr(new TetrahedronEdgeScalarData(*tetrahedronEdgeScalarData));
  }

  if (other.type == DOUBLE)
  {
    tetrahedronEdgeScalarData->op_equal(other.val, func);
  }
  else if (other.type == TETRAHEDRONEDGEDATA)
  {
    tetrahedronEdgeScalarData->op_equal(*other.tetrahedronEdgeScalarData, func);
  }
  else if (other.type == EDGEDATA)
  {
    ModelExprData temp(other);
    temp.convertToTetrahedronEdgeData();
    tetrahedronEdgeScalarData->op_equal(*temp.tetrahedronEdgeScalarData, func);
  }
  else
  {
      type = INVALID;
  }
}

template<typename T>
void ModelExprData::double_op_equal(const ModelExprData &other, const T &func)
{
  if (other.type == DOUBLE)
  {
    func(val, other.val);
  }
  else if (other.type == NODEDATA)
  {
    NodeScalarData *x = new NodeScalarData(val, other.nodeScalarData->GetLength()); 
    nodeScalarData = nodeScalarData_ptr(x);
    nodeScalarData->op_equal(*other.nodeScalarData, func);
    type = NODEDATA;
  }
  else if (other.type == EDGEDATA)
  {
    EdgeScalarData *x = new EdgeScalarData(val, other.edgeScalarData->GetLength()); 
    edgeScalarData = edgeScalarData_ptr(x);
    edgeScalarData->op_equal(*other.edgeScalarData, func);
    type = EDGEDATA;
  }
  else if (other.type == TRIANGLEEDGEDATA)
  {
    TriangleEdgeScalarData *x = new TriangleEdgeScalarData(val, other.triangleEdgeScalarData->GetLength()); 
    triangleEdgeScalarData = triangleEdgeScalarData_ptr(x);
    triangleEdgeScalarData->op_equal(*other.triangleEdgeScalarData, func);
    type = TRIANGLEEDGEDATA;
  }
  else if (other.type == TETRAHEDRONEDGEDATA)
  {
    TetrahedronEdgeScalarData *x = new TetrahedronEdgeScalarData(val, other.tetrahedronEdgeScalarData->GetLength()); 
    tetrahedronEdgeScalarData = tetrahedronEdgeScalarData_ptr(x);
    tetrahedronEdgeScalarData->op_equal(*other.tetrahedronEdgeScalarData, func);
    type = TETRAHEDRONEDGEDATA;
  }
  else
  {
    type = INVALID;
  }
}

template<typename T>
ModelExprData &ModelExprData::op_equal(const ModelExprData &other, const T &func)
{
  switch (type) {
    case DOUBLE:
            double_op_equal(other, func);
            break;
    case NODEDATA:
            node_op_equal(other, func);
            break;
    case EDGEDATA:
            edge_op_equal(other, func);
            break;
    case TRIANGLEEDGEDATA:
            triangle_edge_op_equal(other, func);
            break;
    case TETRAHEDRONEDGEDATA:
            tetrahedron_edge_op_equal(other, func);
            break;
    case INVALID:
    default:
            break;
  }


  return *this;
}

ModelExprData &ModelExprData::operator+=(const ModelExprData &other)
{
  this->op_equal(other, ScalarDataHelper::plus_equal());
  return *this;
}

ModelExprData &ModelExprData::operator*=(const ModelExprData &other)
{
  this->op_equal(other, ScalarDataHelper::times_equal());
  return *this;
}
}

