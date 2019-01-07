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

#include "Region.hh"
#include "Device.hh"
#include "Node.hh"
#include "Triangle.hh"
#include "Tetrahedron.hh"
#include "Edge.hh"
#include "NodeModel.hh"
#include "NodeSolution.hh"
#include "EdgeModel.hh"
#include "TriangleEdgeModel.hh"
#include "TetrahedronEdgeModel.hh"
#include "EquationHolder.hh"
#include "GeometryStream.hh"
#include "GradientField.hh"
#include "TriangleElementField.hh"
#include "TetrahedronElementField.hh"
#include "EdgeData.hh"
#include "MaterialDB.hh"
#include "GlobalData.hh"

#include "Interface.hh"
#include "InterfaceNodeModel.hh"

#include "dsAssert.hh"

#include <algorithm>
#include <vector>
#include <map>
#include <string>
#include <iterator>
namespace {
template <typename T> void deleteVectorPointers(std::vector<T *> &x)
{
    for (size_t i=0; i < x.size(); ++i)
    {
        delete x[i];
    }
}

template <typename T> void deleteMapPointers(std::map<std::string, T *> &x)
{
    typedef std::map<std::string, T *> mtype;

    typename mtype::iterator it = x.begin();
    for ( ; it != x.end(); ++it)
    {
        delete it->second;
    }
}

#if 0
template <typename T> void deleteMultiMapPointers(std::multimap<std::string, T *> &x)
{
    typedef std::map<std::string, T *> mtype;

    typename mtype::iterator it = x.begin();
    for ( ; it != x.end(); ++it)
    {
        delete it->second;
    }
}
#endif
}

namespace {
const Node *findNodeOppositeOfTriangleEdge(const Edge &edge, const Triangle &triangle)
{
  const Node * const h = edge.GetHead();
  const Node * const t = edge.GetTail();

  const Node *ret = nullptr;

  const ConstNodeList &tnl = triangle.GetNodeList();
  for (size_t i = 0; i < 3; ++i)
  {
    const Node *tnode = tnl[i];
    if ((tnode != h) && (tnode != t))
    {
      ret = tnode; 
      break;
    }
  }
  return ret;
}
}// anonymous namespace


template <typename DoubleType>
const GradientField<DoubleType> &Region::GetGradientField() const
{
  GradientField<DoubleType> *gradientField = GetGeometryField<DoubleType>().gradientField;
  if (!gradientField)
  {
    gradientField = new GradientField<DoubleType>(this);
  }
  return *gradientField;
}

template <typename DoubleType>
const TriangleElementField<DoubleType> &Region::GetTriangleElementField() const
{
  TriangleElementField<DoubleType> *triangleElementField = GetGeometryField<DoubleType>().triangleElementField;
  if (!triangleElementField)
  {
    triangleElementField = new TriangleElementField<DoubleType>(this);
  }
  return *triangleElementField;
}

template <typename DoubleType>
const TetrahedronElementField<DoubleType> &Region::GetTetrahedronElementField() const
{
  TetrahedronElementField<DoubleType> *tetrahedronElementField = GetGeometryField<DoubleType>().tetrahedronElementField;
  if (!tetrahedronElementField)
  {
    tetrahedronElementField = new TetrahedronElementField<DoubleType>(this);
  }
  return *tetrahedronElementField;
}

template <typename DoubleType>
const std::vector<Vector<DoubleType> > &Region::GetTriangleCenters() const
{
  return GetGeometryField<DoubleType>().triangleCenters;
}

template <typename DoubleType>
const std::vector<Vector<DoubleType> > &Region::GetTetrahedronCenters() const
{
  return GetGeometryField<DoubleType>().tetrahedronCenters;
}

template <typename DoubleType>
Region::GeometryField<DoubleType>::~GeometryField()
{
  delete gradientField;
  delete triangleElementField;
  delete tetrahedronElementField;
}

template <>
Region::GeometryField<double> &Region::GetGeometryField() const
{
  return GeometryField_double;
}

#ifdef DEVSIM_EXTENDED_PRECISION
template <>
Region::GeometryField<float128> &Region::GetGeometryField() const
{
  return GeometryField_float128;
}
#endif

Region::~Region()
{
#if 0
    deleteMapPointers(equationPointerMap);
    //// We are now using smart pointers
    deleteMapPointers(nodeModels);
    deleteMapPointers(edgeModels);
    deleteMapPointers(triangleEdgeModels);
    deleteMapPointers(tetrahedronEdgeModels);
#endif
    deleteVectorPointers(nodeList);
    deleteVectorPointers(edgeList);
    deleteVectorPointers(triangleList);
    deleteVectorPointers(tetrahedronList);

    for (size_t i = 0; i < tetrahedronToEdgeDataList.size(); ++i)
    {
      deleteVectorPointers(tetrahedronToEdgeDataList[i]);
    }
}

Region::Region(std::string regName, std::string mat, size_t d, ConstDevicePtr dp)
    : finalized(false), device(dp), relError(0.0), absError(0.0)
{
    dsAssert(!mat.empty(), "UNEXPECTED");
    materialName = mat;
    dsAssert(!regName.empty(), "UNEXPECTED");
    regionName = regName;
    dimension = d;
    dsAssert(dimension >=1 && dimension <= 3, "UNEXPECTED");
    nodeList.reserve(DEFAULT_NUMBER_NODES);
    edgeList.reserve(DEFAULT_NUMBER_NODES);

    if (dimension > 1)
        triangleList.reserve(DEFAULT_NUMBER_NODES);

    numequations = 0;
    baseeqnnum = size_t(-1);
}

bool Region::operator==(const Region &r) const
{
    return (this == &r);
}

const std::string &Region::GetDeviceName() const
{
  return GetDevice()->GetName();
}

// Note that a nd can have an index of 0
// Need to minimize reallocations by setting a default minimum size
// and letting mesher specify the number of nodes to be added

// A node has an index which is used as a sorting criteria
void Region::AddNode(const NodePtr &nd)
{
    dsAssert(!finalized, "UNEXPECTED");
    nodeList.push_back(nd);
}

void Region::AddNodeList(ConstNodeList &nl)
{
    dsAssert(!finalized, "UNEXPECTED");
    if (nodeList.empty())
    {
        nodeList = nl;
    }
    else
    {
        for (ConstNodeList::const_iterator it = nl.begin(); it != nl.end(); ++it)
        {
            nodeList.push_back(*it);
        }
    }
}

void Region::AddEdge(const EdgePtr &ep)
{
    dsAssert(!finalized, "UNEXPECTED");
    edgeList.push_back(ep);
}

void Region::AddEdgeList(ConstEdgeList &nl)
{
    dsAssert(!finalized, "UNEXPECTED");
    if (edgeList.empty())
    {
        edgeList = nl;
    }
    else
    {
        for (ConstEdgeList::const_iterator it = nl.begin(); it != nl.end(); ++it)
        {
            edgeList.push_back(*it);
        }
    }
}

void Region::AddTriangle(const TrianglePtr &ep)
{
    dsAssert(!finalized, "UNEXPECTED");
    triangleList.push_back(ep);
}

void Region::AddTriangleList(ConstTriangleList &nl)
{
    dsAssert(!finalized, "UNEXPECTED");
    if (triangleList.empty())
    {
        triangleList = nl;
    }
    else
    {
        for (ConstTriangleList::const_iterator it = nl.begin(); it != nl.end(); ++it)
        {
            triangleList.push_back(*it);
        }
    }
}

void Region::AddTetrahedron(const TetrahedronPtr &ep)
{
    dsAssert(!finalized, "UNEXPECTED");
    tetrahedronList.push_back(ep);
}

void Region::AddTetrahedronList(ConstTetrahedronList &nl)
{
    dsAssert(!finalized, "UNEXPECTED");
    if (tetrahedronList.empty())
    {
        tetrahedronList = nl;
    }
    else
    {
        for (ConstTetrahedronList::const_iterator it = nl.begin(); it != nl.end(); ++it)
        {
            tetrahedronList.push_back(*it);
        }
    }
}

/// Need to first make sure to set indexes on nodes
void Region::SetNodeIndexes()
{
  // Sort based on node index
  std::sort(nodeList.begin(), nodeList.end(), NodeCompIndex());
  // Swap trick to remove excess consumption
  std::vector<ConstNodePtr>(nodeList).swap(nodeList);
  // Number elements locally
  const size_t len = nodeList.size();
  for (size_t i=0; i < len; ++i) 
  {
    const_cast<NodePtr>(nodeList[i])->SetIndex(i);
  }
#if 0
  std::ostringstream os; 
  for (size_t i = 0; i < nodeList.size(); ++i)
  {
    os << "Node " << nodeList[i]->GetIndex() << "\n";
  }
  GeometryStream::WriteOut(OutputStream::OutputType::INFO, *this, os.str());
#endif
}

/// then edges
void Region::SetEdgeIndexes()
{
  // Sort based on node index
  std::sort(edgeList.begin(), edgeList.end(), EdgeCompIndex());
  // Swap trick to remove excess consumption
  std::vector<ConstEdgePtr>(edgeList).swap(edgeList);
  // Number elements locally
  const size_t elen = edgeList.size();
  for (size_t i=0; i < elen; ++i) 
  {
    const_cast<EdgePtr>(edgeList[i])->SetIndex(i);
  }
}

/// then triangles
void Region::SetTriangleIndexes()
{
  const size_t tlen = triangleList.size();
  for (size_t i=0; i < tlen; ++i) 
  {
    const_cast<TrianglePtr>(triangleList[i])->SetIndex(i);
  }
}

/// then tetrahedron
void Region::SetTetrahedronIndexes()
{
  const size_t tlen = tetrahedronList.size();
  for (size_t i=0; i < tlen; ++i) 
  {
    const_cast<TetrahedronPtr>(tetrahedronList[i])->SetIndex(i);
  }
}

/// Requires Node and Edge Indexes Set
void Region::CreateNodeToEdgeList()
{
  // make this a separate function
  // need to get all edges at this point
  nodeToEdgeList.clear();
  nodeToEdgeList.resize(nodeList.size());
  for (size_t i = 0; i < edgeList.size(); ++i)
  {
    const size_t nh = edgeList[i]->GetHead()->GetIndex();
    const size_t nt = edgeList[i]->GetTail()->GetIndex();
    nodeToEdgeList[nh].push_back(edgeList[i]); 
    nodeToEdgeList[nt].push_back(edgeList[i]); 
  }

  for (size_t i = 0; i < nodeToEdgeList.size(); ++i)
  {
    std::sort(nodeToEdgeList[i].begin(), nodeToEdgeList[i].end(), EdgeCompIndex());
  }
}

/// Requires Node Indexes to be set
void Region::CreateNodeToTriangleList()
{
  nodeToTriangleList.clear();
  nodeToTriangleList.resize(nodeList.size());
  for (size_t i = 0; i < triangleList.size(); ++i)
  {
    ConstTrianglePtr ctp = triangleList[i];
    const std::vector<ConstNodePtr> &nodes = ctp->GetNodeList();
    for (size_t j = 0; j < nodes.size(); ++j)
    {
      nodeToTriangleList[nodes[j]->GetIndex()].push_back(ctp);
    }
  }

  // triangle intersection below requires sorted vectors
  for (size_t i = 0; i < nodeToTriangleList.size(); ++i)
  {
    std::sort(nodeToTriangleList[i].begin(), nodeToTriangleList[i].end(), TriangleCompIndex());
  }
}

/// Requires Node Indexes to be set
/// Blatant ripoff of CreateNodeToTriangleList
void Region::CreateNodeToTetrahedronList()
{
  nodeToTetrahedronList.clear();
  nodeToTetrahedronList.resize(nodeList.size());
  for (size_t i = 0; i < tetrahedronList.size(); ++i)
  {
    ConstTetrahedronPtr ctp = tetrahedronList[i];
//    std::cerr << "ctp " << ctp->GetIndex() << "\n";
    const std::vector<ConstNodePtr> &nodes = ctp->GetNodeList();
//    dsAssert(nodes.size() == 4, "UNEXPECTED");
    for (size_t j = 0; j < nodes.size(); ++j)
    {
      nodeToTetrahedronList[nodes[j]->GetIndex()].push_back(ctp);
//      std::cerr << nodes[j]->GetIndex() << " " << j << " " << ctp->GetIndex() << "\n";
    }
  }

  // tetrahedron intersection below requires sorted vectors
  for (size_t i = 0; i < nodeToTetrahedronList.size(); ++i)
  {
    std::sort(nodeToTetrahedronList[i].begin(), nodeToTetrahedronList[i].end(), TetrahedronCompIndex());
  }
}

/// Requires Node, edge, and triangle indices to be set
/// The STL intersection requires to triangle indexes in
/// nodeToTriangleList to be sorted
void Region::CreateEdgeToTriangleList()
{
  edgeToTriangleList.clear();
  edgeToTriangleList.resize(edgeList.size());

  ConstTriangleList nout;

  for (size_t i = 0; i < edgeList.size(); ++i)
  {
    const size_t nh = edgeList[i]->GetHead()->GetIndex();
    const size_t nt = edgeList[i]->GetTail()->GetIndex();

    // need these to be sorted ranges so do sort above
    const ConstTriangleList &nht = GetNodeToTriangleList()[nh];
    const ConstTriangleList &ntt = GetNodeToTriangleList()[nt];

    nout.clear();
    //// Given:
    ////   list of triangles on head node of edge
    ////   list of triangles on tail node of edge
    ////   find all triangles connect to both nodes
    set_intersection(nht.begin(), nht.end(),
      ntt.begin(), ntt.end(),
      std::insert_iterator<ConstTriangleList>(nout, nout.begin()),
      TriangleCompIndex()
    );

    if (dimension == 2)
    {
      dsAssert(nout.size()==1 || nout.size()==2, "UNEXPECTED"); // only expect an edge to have up to 2 triangles
    }

    edgeToTriangleList[i] = nout;
  }
}

/// Ripoff of CreateEdgeToTriangleList
void Region::CreateEdgeToTetrahedronList()
{
  edgeToTetrahedronList.clear();
  edgeToTetrahedronList.resize(edgeList.size());

  ConstTetrahedronList nout;

  for (size_t i = 0; i < edgeList.size(); ++i)
  {
    const size_t nh = edgeList[i]->GetHead()->GetIndex();
    const size_t nt = edgeList[i]->GetTail()->GetIndex();

    // need these to be sorted ranges so do sort above
    const ConstTetrahedronList &nht = GetNodeToTetrahedronList()[nh];
    const ConstTetrahedronList &ntt = GetNodeToTetrahedronList()[nt];

    nout.clear();
    //// Given:
    ////   list of tetrahedrons on head node of edge
    ////   list of tetrahedrons on tail node of edge
    ////   find all tetrahedrons connect to both nodes
    set_intersection(nht.begin(), nht.end(),
      ntt.begin(), ntt.end(),
      std::insert_iterator<ConstTetrahedronList>(nout, nout.begin()),
      TetrahedronCompIndex()
    );

    edgeToTetrahedronList[i] = nout;
  }
}

/// Requires EdgeToTriangleList
/// Create reverse mapping
/// Enforces that triangleToEdgeList has node not on edge
/// For example:
// triangle.GetNodeList[0] is not on region.GetTriangleToEdgeList[0]
// This helps on element assembly
void Region::CreateTriangleToEdgeList()
{
  triangleToEdgeList.clear();
  triangleToEdgeList.resize(triangleList.size());

  for (size_t i = 0; i < triangleList.size(); ++i)
  {
    triangleToEdgeList[i].resize(3);
  }

  for (size_t i = 0; i < edgeList.size(); ++i)
  {
    ConstEdgePtr eptr = edgeList[i];
    const ConstTriangleList &tlist = GetEdgeToTriangleList()[i];
    for (ConstTriangleList::const_iterator tit = tlist.begin(); tit != tlist.end(); ++tit)
    {
      const size_t tindex = (*tit)->GetIndex();

      const ConstNodeList &nl = (*tit)->GetNodeList();
      ConstEdgeList &el = triangleToEdgeList[tindex];

      for (size_t j = 0; j < 3; ++j)
      {
        ConstNodePtr np = nl[j];
        if (!((np == eptr->GetHead()) || (np == eptr->GetTail())))
        {
          el[j] = eptr;
          break;
        }
      }
    }
  }
}

//// Ripoff of CreateTriangleToEdgeList
//// but with significant modifications
//// no guarantee of the edge ordering on the tetrahedron
//// just depends on the order that the edges are visited
void Region::CreateTetrahedronToEdgeDataList()
{
  tetrahedronToEdgeDataList.clear();
  tetrahedronToEdgeDataList.resize(tetrahedronList.size());

#if 0
  for (size_t i = 0; i < tetrahedronList.size(); ++i)
  {
    tetrahedronToEdgeList[i].resize(6);
  }
#endif

  for (size_t i = 0; i < edgeToTetrahedronList.size(); ++i)
  {
    ConstEdgePtr eptr = edgeList[i];
    const ConstTetrahedronList &tlist = GetEdgeToTetrahedronList()[i];
    for (ConstTetrahedronList::const_iterator tit = tlist.begin(); tit != tlist.end(); ++tit)
    {
      const size_t teindex = (*tit)->GetIndex();

      ConstEdgeDataList &el = tetrahedronToEdgeDataList[teindex];

      ConstTriangleList &trl = tetrahedronToTriangleList[teindex];

      EdgeData *edata = new EdgeData();
      edata->edge = eptr;
      const size_t eindex = eptr->GetIndex();
      size_t trindex = 0;
      for (size_t j = 0; j < trl.size(); ++j)
      {
        const Triangle &triangle = *trl[j];
        ConstEdgeList  &triangleEdgeList = triangleToEdgeList[triangle.GetIndex()];
        for (size_t k = 0; k < 3; ++k)
        {
          const size_t teindex = triangleEdgeList[k]->GetIndex();
          if (teindex == eindex)
          {
            edata->triangle[trindex] = trl[j];
            edata->triangle_index[trindex] = j;
            edata->nodeopp[trindex] = findNodeOppositeOfTriangleEdge(*eptr, triangle);
            ++trindex;
            break;
          }
        }
      }
      dsAssert(trindex == 2, "UNEXPECTED");
      el.push_back(edata);
#if 0
      const ConstNodeList &nl = (*tit)->GetNodeList();
      for (size_t j = 0; j < 6; ++j)
      {
        ConstNodePtr np = nl[j];
        if (!((np == eptr->GetHead()) || (np == eptr->GetTail())))
        {
          el[j] = eptr;
          break;
        }
      }
#endif
    }
  }
}

/// Requires Node, edge, triangle, and tetrahedron indices to be set
void Region::CreateTriangleToTetrahedronList()
{
  triangleToTetrahedronList.clear();
  triangleToTetrahedronList.resize(triangleList.size());

  ConstTetrahedronList nout0;
  ConstTetrahedronList nout1;

  for (size_t i = 0; i < triangleList.size(); ++i)
  {
    const Triangle &triangle = *triangleList[i];
    const ConstNodeList &cnl = triangle.GetNodeList();

    const size_t n0 = cnl[0]->GetIndex();
    const size_t n1 = cnl[1]->GetIndex();
    const size_t n2 = cnl[2]->GetIndex();

    // need these to be sorted ranges so do sort above
    const ConstTetrahedronList &nt0 = GetNodeToTetrahedronList()[n0];
    const ConstTetrahedronList &nt1 = GetNodeToTetrahedronList()[n1];
    const ConstTetrahedronList &nt2 = GetNodeToTetrahedronList()[n2];

    nout0.clear();
    nout1.clear();

    set_intersection(nt0.begin(), nt0.end(),
      nt1.begin(), nt1.end(),
      std::insert_iterator<ConstTetrahedronList>(nout0, nout0.begin()),
      TetrahedronCompIndex()
    );

    set_intersection(nout0.begin(), nout0.end(),
      nt2.begin(), nt2.end(),
      std::insert_iterator<ConstTetrahedronList>(nout1, nout1.begin()),
      TetrahedronCompIndex()
    );

    dsAssert(nout1.size()==1 || nout1.size()==2, "UNEXPECTED"); // only expect triangle to have up to 1 tetrahedron

    triangleToTetrahedronList[i] = nout1;
  }
}

void Region::CreateTetrahedronToTriangleList()
{
  tetrahedronToTriangleList.clear();
  tetrahedronToTriangleList.resize(tetrahedronList.size());

  for (size_t i = 0; i < tetrahedronList.size(); ++i)
  {
    tetrahedronToTriangleList[i].resize(4);
  }

  for (size_t i = 0; i < triangleList.size(); ++i)
  {
    ConstTrianglePtr tptr = triangleList[i];
    const ConstTetrahedronList &tlist = triangleToTetrahedronList[i];
    for (ConstTetrahedronList::const_iterator tit = tlist.begin(); tit != tlist.end(); ++tit)
    {
      const size_t tindex = (*tit)->GetIndex();

      const ConstNodeList &nl = (*tit)->GetNodeList();

      const ConstNodeList &trnl = (tptr)->GetNodeList();

      ConstTriangleList &el = tetrahedronToTriangleList[tindex];
      for (size_t j = 0; j < 4; ++j)
      {
        ConstNodePtr np = nl[j];
        if (!((np == trnl[0]) || (np == trnl[1]) || (np == trnl[2])))
        {
          el[j] = tptr;
          break;
        }
      }
    }
  }
}

void Region::SetTriangleCenters()
{
#ifdef DEVSIM_EXTENDED_PRECISION
  auto &triangleCenters_float128 = GetGeometryField<float128>().triangleCenters;
  auto &triangleCenters_double = GetGeometryField<double>().triangleCenters;
  triangleCenters_float128.resize(triangleList.size());
  triangleCenters_double.resize(triangleList.size());
  for (size_t i = 0; i < triangleList.size(); ++i)
  {
    Vector<float128> &center = triangleCenters_float128[i];
    center = GetCenter<float128>(*triangleList[i]);
    triangleCenters_double[i] = Vector<double>(static_cast<double>(center.Getx()), static_cast<double>(center.Gety()), static_cast<double>(center.Getz()));
  }
#else
  auto &triangleCenters_double = GetGeometryField<double>().triangleCenters;
  triangleCenters_double.resize(triangleList.size());
  for (size_t i = 0; i < triangleList.size(); ++i)
  {
    triangleCenters_double[i] = GetCenter<double>(*triangleList[i]);
  }
#endif
}

void Region::SetTetrahedronCenters()
{
#ifdef DEVSIM_EXTENDED_PRECISION
  auto &tetrahedronCenters_float128 = GetGeometryField<float128>().tetrahedronCenters;
  auto &tetrahedronCenters_double = GetGeometryField<double>().tetrahedronCenters;
  tetrahedronCenters_float128.resize(tetrahedronList.size());
  tetrahedronCenters_double.resize(tetrahedronList.size());
  for (size_t i = 0; i < tetrahedronList.size(); ++i)
  {
    Vector<float128> &center = tetrahedronCenters_float128[i];
    center = GetCenter<float128>(*tetrahedronList[i]);
    tetrahedronCenters_double[i] = Vector<double>(static_cast<double>(center.Getx()), static_cast<double>(center.Gety()), static_cast<double>(center.Getz()));
  }
#else
  auto &tetrahedronCenters_double = GetGeometryField<double>().tetrahedronCenters;
  tetrahedronCenters_double.resize(tetrahedronList.size());
  for (size_t i = 0; i < tetrahedronList.size(); ++i)
  {
    tetrahedronCenters_double[i] = GetCenter<double>(*tetrahedronList[i]);
  }
#endif
}

//Performs the sort when we are done adding nodes and edges
void Region::FinalizeMesh()
{
  SetNodeIndexes();

  SetEdgeIndexes();

  SetTriangleIndexes();

  SetTetrahedronIndexes();

  CreateNodeToEdgeList();

  if (!triangleList.empty())
  {
    CreateNodeToTriangleList();
    CreateEdgeToTriangleList();
    CreateTriangleToEdgeList();
    SetTriangleCenters();
  }

  if (!tetrahedronList.empty())
  {
#if 0
    for (size_t i = 0; i < tetrahedronList.size(); ++i)
    {
      std:: cerr << "HELP " << i;
      const ConstNodeList &cnl = tetrahedronList[i]->GetNodeList();
      for (size_t j = 0; j < 4; ++j)
      {
        std::cerr << " " << cnl[j]->GetIndex();
      }
      std::cerr << "\n";
    }
#endif
    CreateNodeToTetrahedronList();
    CreateEdgeToTetrahedronList();
    CreateTriangleToTetrahedronList();
    CreateTetrahedronToTriangleList();
    CreateTetrahedronToEdgeDataList();
    SetTetrahedronCenters();
  }

  finalized = true;
}


ConstNodeModelPtr Region::GetNodeModel(const std::string &nm) const
{
    NodeModelPtr em;
    NodeModelList_t::const_iterator it = nodeModels.find(nm);
    if (nodeModels.end() != it)
    {
        em = it->second;
    }
//    dsAssert(em != nullptr, "UNEXPECTED");
    return em;
}

ConstEdgeModelPtr Region::GetEdgeModel(const std::string &nm) const
{
    EdgeModelPtr em;
    EdgeModelList_t::const_iterator it = edgeModels.find(nm);
    if (edgeModels.end() != it)
    {
        em = it->second;
    }
//    dsAssert(em != nullptr, "UNEXPECTED");
    return em;
}

ConstTriangleEdgeModelPtr Region::GetTriangleEdgeModel(const std::string &nm) const
{
    TriangleEdgeModelPtr em;
    TriangleEdgeModelList_t::const_iterator it = triangleEdgeModels.find(nm);
    if (triangleEdgeModels.end() != it)
    {
        em = it->second;
    }
//    dsAssert(em != nullptr, "UNEXPECTED");
    return em;
}

ConstTetrahedronEdgeModelPtr Region::GetTetrahedronEdgeModel(const std::string &nm) const
{
    TetrahedronEdgeModelPtr em;
    TetrahedronEdgeModelList_t::const_iterator it = tetrahedronEdgeModels.find(nm);
    if (tetrahedronEdgeModels.end() != it)
    {
        em = it->second;
    }
//    dsAssert(em != nullptr, "UNEXPECTED");
    return em;
}


void Region::DeleteNodeModel(const std::string &nm)
{
  NodeModelList_t::iterator it = nodeModels.find(nm);
  if (nodeModels.end() != it)
  {
    //// We should be the only one with a shared pointer
    dsAssert((it->second).unique(), "UNEXPECTED");
    UnregisterCallback(nm);
    nodeModels.erase(it);
    //// Anything depending on this model is notified it is out of date
    //// However, it is not removed as a dependency for that model
    this->SignalCallbacks(nm);
  }
}

void Region::DeleteEdgeModel(const std::string &nm)
{
  EdgeModelList_t::iterator it = edgeModels.find(nm);
  if (edgeModels.end() != it)
  {
    //// We should be the only one with a shared pointer
    dsAssert((it->second).unique(), "UNEXPECTED");
    UnregisterCallback(nm);
    edgeModels.erase(it);
    //// Anything depending on this model is notified it is out of date
    //// However, it is not removed as a dependency for that model
    this->SignalCallbacks(nm);
  }
}

void Region::DeleteTriangleEdgeModel(const std::string &nm)
{
  TriangleEdgeModelList_t::iterator it = triangleEdgeModels.find(nm);
  if (triangleEdgeModels.end() != it)
  {
    //// We should be the only one with a shared pointer
    dsAssert((it->second).unique(), "UNEXPECTED");
    UnregisterCallback(nm);
    triangleEdgeModels.erase(it);
    //// Anything depending on this model is notified it is out of date
    //// However, it is not removed as a dependency for that model
    this->SignalCallbacks(nm);
  }
}

void Region::DeleteTetrahedronEdgeModel(const std::string &nm)
{
  TetrahedronEdgeModelList_t::iterator it = tetrahedronEdgeModels.find(nm);
  if (tetrahedronEdgeModels.end() != it)
  {
    //// We should be the only one with a shared pointer
    dsAssert((it->second).unique(), "UNEXPECTED");
    UnregisterCallback(nm);
    tetrahedronEdgeModels.erase(it);
    //// Anything depending on this model is notified it is out of date
    //// However, it is not removed as a dependency for that model
    this->SignalCallbacks(nm);
  }
}

NodeModelPtr Region::AddNodeModel(NodeModel *nmp)
{
    NodeModelPtr ret;

    const std::string &nm = nmp->GetName();
    if (nodeModels.count(nm))
    {
        dsAssert(nodeModels[nm].unique(), "UNEXPECTED");
        //// TODO: what happens when there is a dependency on this model???
        std::ostringstream os; 
        os << "Replacing Node Model " << nm << " in region " << regionName
                  << " of material " << materialName << "\n";
        GeometryStream::WriteOut(OutputStream::OutputType::INFO, *this, os.str());
        ret = NodeModelPtr(nmp);
        nodeModels[nm] = ret;
    }
    else if (edgeModels.count(nm))
    {
        std::ostringstream os;
        os << "Cannot replace Edge Model " << nm << " in region " << regionName
                  << " of material " << materialName << " with Node Model of the same name\n";
        GeometryStream::WriteOut(OutputStream::OutputType::FATAL, *this, os.str());
    }
    else if (triangleEdgeModels.count(nm))
    {
        std::ostringstream os;
        os << "Cannot replace Triangle Edge Model " << nm << " in region " << regionName
                  << " of material " << materialName << " with Node Model of the same name\n";
        GeometryStream::WriteOut(OutputStream::OutputType::FATAL, *this, os.str());
    }
    else if (tetrahedronEdgeModels.count(nm))
    {
        std::ostringstream os;
        os << "Cannot replace Tetrahedron Edge Model " << nm << " in region " << regionName
                  << " of material " << materialName << " with Node Model of the same name\n";
        GeometryStream::WriteOut(OutputStream::OutputType::FATAL, *this, os.str());
    }
    else
    {
      ret = NodeModelPtr(nmp);
      nodeModels[nm] = ret;
    }

    return ret;
}

EdgeModelPtr Region::AddEdgeModel(EdgeModel *emp)
{
    EdgeModelPtr ret;
    const std::string &nm = emp->GetName();
    if (edgeModels.count(nm))
    {
        dsAssert(edgeModels[nm].unique(), "UNEXPECTED");
        std::ostringstream os; 
        os << "Replacing Edge Model " << nm << " in region " << regionName
                  << " of material " << materialName << "\n";
        GeometryStream::WriteOut(OutputStream::OutputType::INFO, *this, os.str());
        ret = EdgeModelPtr(emp);
        edgeModels[nm] = ret;
    }
    else if (nodeModels.count(nm))
    {
        std::ostringstream os;
        os << "Cannot replace Node Model " << nm << " in region " << regionName
                  << " of material " << materialName << " with Edge Model of the same name\n";
        GeometryStream::WriteOut(OutputStream::OutputType::FATAL, *this, os.str());
    }
    else if (triangleEdgeModels.count(nm))
    {
        std::ostringstream os;
        os << "Cannot replace Triangle Edge Model " << nm << " in region " << regionName
                  << " of material " << materialName << " with Edge Model of the same name\n";
        GeometryStream::WriteOut(OutputStream::OutputType::FATAL, *this, os.str());
    }
    else if (tetrahedronEdgeModels.count(nm))
    {
        std::ostringstream os;
        os << "Cannot replace Tetrahedron Edge Model " << nm << " in region " << regionName
                  << " of material " << materialName << " with Edge Model of the same name\n";
        GeometryStream::WriteOut(OutputStream::OutputType::FATAL, *this, os.str());
    }
    else
    {
      ret = EdgeModelPtr(emp);
      edgeModels[nm] = ret;
    }
    return ret;
}

TriangleEdgeModelPtr Region::AddTriangleEdgeModel(TriangleEdgeModel *emp)
{
    TriangleEdgeModelPtr ret;
    const std::string &nm = emp->GetName();
    if (triangleEdgeModels.count(nm))
    {
        dsAssert(triangleEdgeModels[nm].unique(), "UNEXPECTED");
        std::ostringstream os; 
        os << "Replacing Triangle Edge Model " << nm << " in region " << regionName
                  << " of material " << materialName << "\n";
        GeometryStream::WriteOut(OutputStream::OutputType::INFO, *this, os.str());
        ret = TriangleEdgeModelPtr(emp);
        triangleEdgeModels[nm] = ret;
    }
    else if (nodeModels.count(nm))
    {
        std::ostringstream os;
        os << "Cannot replace Node Model " << nm << " in region " << regionName
                  << " of material " << materialName << " with Triangle Edge Model of the same name\n";
        GeometryStream::WriteOut(OutputStream::OutputType::FATAL, *this, os.str());
    }
    else if (edgeModels.count(nm))
    {
        std::ostringstream os;
        os << "Cannot replace Edge Model " << nm << " in region " << regionName
                  << " of material " << materialName << " with Triangle Edge Model of the same name\n";
        GeometryStream::WriteOut(OutputStream::OutputType::FATAL, *this, os.str());
    }
    else if (tetrahedronEdgeModels.count(nm))
    {
        std::ostringstream os;
        os << "Cannot replace Tetrahedron Edge Model " << nm << " in region " << regionName
                  << " of material " << materialName << " with Triangle Edge Model of the same name\n";
        GeometryStream::WriteOut(OutputStream::OutputType::FATAL, *this, os.str());
    }
    else
    {
      ret = TriangleEdgeModelPtr(emp);
      triangleEdgeModels[nm] = ret;
    }
    return ret;
}

TetrahedronEdgeModelPtr Region::AddTetrahedronEdgeModel(TetrahedronEdgeModel *emp)
{
    TetrahedronEdgeModelPtr ret;
    const std::string &nm = emp->GetName();
    if (tetrahedronEdgeModels.count(nm))
    {
        dsAssert(tetrahedronEdgeModels[nm].unique(), "UNEXPECTED");
        std::ostringstream os; 
        os << "Replacing Tetrahedron Edge Model " << nm << " in region " << regionName
                  << " of material " << materialName << "\n";
        GeometryStream::WriteOut(OutputStream::OutputType::INFO, *this, os.str());
        ret = TetrahedronEdgeModelPtr(emp);
        tetrahedronEdgeModels[nm] = ret;
    }
    else if (nodeModels.count(nm))
    {
        std::ostringstream os;
        os << "Cannot replace Node Model " << nm << " in region " << regionName
                  << " of material " << materialName << " with Tetrahedron Edge Model of the same name\n";
        GeometryStream::WriteOut(OutputStream::OutputType::FATAL, *this, os.str());
    }
    else if (edgeModels.count(nm))
    {
        std::ostringstream os;
        os << "Cannot replace Edge Model " << nm << " in region " << regionName
                  << " of material " << materialName << " with Tetrahedron Edge Model of the same name\n";
        GeometryStream::WriteOut(OutputStream::OutputType::FATAL, *this, os.str());
    }
    else if (triangleEdgeModels.count(nm))
    {
        std::ostringstream os;
        os << "Cannot replace Triangle Edge Model " << nm << " in region " << regionName
                  << " of material " << materialName << " with Tetrahedron Edge Model of the same name\n";
        GeometryStream::WriteOut(OutputStream::OutputType::FATAL, *this, os.str());
    }
    else
    {
      ret = TetrahedronEdgeModelPtr(emp);
      tetrahedronEdgeModels[nm] = ret;
    }
    return ret;
}


/*
 * No differentation between NodeModels or EdgeModels
 */
void Region::RegisterCallback(const std::string &mod, const std::string &dep)
{
    DependencyMap[mod].insert(dep);
}

void Region::UnregisterCallback(const std::string &mod)
{
    // Model does not release itself as dependency
    DependencyMap_t::iterator it = DependencyMap.find(mod);
    if (it != DependencyMap.end())
    {
        DependencyMap.erase(it);
    }
}

/*
 * Does not yet handle cyclic dependencies
 * Does not mark the original dependency as being old
 */
void Region::SignalCallbacks(const std::string &str)
{
  typedef std::set<std::string> list_t; 
  list_t list; 
  DependencyMap_t::iterator it = DependencyMap.begin();
  const DependencyMap_t::iterator end = DependencyMap.end();
  for ( ; it != end; ++it)
  {
    // If this one has a dependency on str
    if ((*it).second.count(str))
    {
      // put it in the set to be notified
      list.insert(it->first);
    }
  }

  list_t::iterator lit = list.begin();
  const list_t::iterator lend = list.end();
  for ( ; lit != lend; ++lit)
  {
    dsAssert(!(nodeModels.count(*lit) && edgeModels.count(*lit) && triangleEdgeModels.count(*lit) && tetrahedronEdgeModels.count(*lit)), "UNEXPECTED");
    if (nodeModels.count(*lit))
    {
      NodeModelPtr nmp = nodeModels[*lit];
      if ((nmp->IsUpToDate()))
      {
        nmp->MarkOld();
//        This is what MarkOld does
//        this->SignalCallbacks(*lit);
      }
    }
    else if (edgeModels.count(*lit))
    {
      EdgeModelPtr emp = edgeModels[*lit];
      if ((emp->IsUpToDate()))
      {
        emp->MarkOld();
//        This is what MarkOld does
//        this->SignalCallbacks(*lit);
      }
    }
    else if (triangleEdgeModels.count(*lit))
    {
      TriangleEdgeModelPtr temp = triangleEdgeModels[*lit];
      if ((temp->IsUpToDate()))
      {
        temp->MarkOld();
//        This is what MarkOld does
//        this->SignalCallbacks(*lit);
      }
    }
    else if (tetrahedronEdgeModels.count(*lit))
    {
      TetrahedronEdgeModelPtr temp = tetrahedronEdgeModels[*lit];
      if ((temp->IsUpToDate()))
      {
        temp->MarkOld();
//        This is what MarkOld does
//        this->SignalCallbacks(*lit);
      }
    }
  }

  GetDevice()->SignalCallbacksOnInterface(str, this);
}

// number equations by order they are entered
void Region::AddEquation(EquationHolder &eq)
{

  const std::string nm  = eq.GetName();
  const std::string var = eq.GetVariable();

  if (equationPointerMap.count(nm))
  {
    EquationHolder &oeq = equationPointerMap[nm];
    if (oeq == eq)
    {
      std::ostringstream os; 
      os << "Warning: Will not replace equation with itself.\n"
          "Region: " << this->GetName() << ", Equation: " << nm <<
          ", New Variable: " << var << "\n";
      GeometryStream::WriteOut(OutputStream::OutputType::INFO, *this, os.str());
    }
    else
    {
      if (oeq.GetVariable() != var)
      {
        std::ostringstream os; 
        os << "Warning: Adding a new equation by the same name with a different variable will remove mapping to other variable.\n"
            "Region: " << this->GetName() << ", Equation: " << nm << ", Old variable: " << oeq.GetVariable() << ", New Variable: " << var << "\n";
        GeometryStream::WriteOut(OutputStream::OutputType::INFO, *this, os.str());

        variableEquationMap.erase(var);
        variableEquationMap[var] = nm;
      }
      else
      {
        std::ostringstream os; 
        os << "Warning: Replacing equation with equation of the same name.\n"
            "Region: " << this->GetName() << ", Equation: " << nm << ", Variable: " << var << "\n";
        GeometryStream::WriteOut(OutputStream::OutputType::INFO, *this, os.str());
      }

      equationPointerMap[nm] = eq;
        /// the equationIndexMap doesn't change
    }
  }
  else
  {
    if (variableEquationMap.count(var))
    {
      const std::string oenm = variableEquationMap[var];
      std::ostringstream os; 
      os << "ERROR: Can't create equation if its variable is already being used\n"
          << "New Equation: " << nm << ", Old Equation: " << oenm << ", Variable: "  << var << "\n";
      GeometryStream::WriteOut(OutputStream::OutputType::FATAL, *this, os.str());
    }
    else
    {
      const size_t a = equationPointerMap.size();
      equationPointerMap[nm] = eq;
      equationIndexMap[nm]   = a;
      variableEquationMap[var] = nm;
      numequations = equationPointerMap.size();
      dsAssert(numequations == (a+1), "UNEXPECTED");
    }
  }
}

//// Deletes the equation and its associated variable
//// Decrements the equation index for all other equations
void Region::DeleteEquation(EquationHolder &eq)
{
  // Replace this with a warning
  const std::string nm  = eq.GetName();
  dsAssert(equationPointerMap.count(nm) != 0, "UNEXPECTED");
  dsAssert(equationIndexMap.count(nm) != 0, "UNEXPECTED");

  const std::string var = eq.GetVariable();
  dsAssert(variableEquationMap.count(var) != 0, "UNEXPECTED");

  /// At this point, the matrix will totally change.  We need to signify this somehow.
  size_t a = equationIndexMap[nm];
  equationPointerMap.erase(nm);
  equationIndexMap.erase(nm);
  variableEquationMap.erase(var);

  EquationIndMap_t::iterator it = equationIndexMap.begin();
  for ( ; it != equationIndexMap.end(); ++it)
  {
    if (it->second > a)
    {
      it->second -= 1;
    }
  }
  numequations = equationPointerMap.size();
}

EquationPtrMap_t &Region::GetEquationPtrList()
{
  return equationPointerMap;
}

const EquationPtrMap_t &Region::GetEquationPtrList() const
{
  return equationPointerMap;
}

// return size_t(-1) if doesn't exist
size_t Region::GetEquationIndex(const std::string &nm) const
{
    size_t a = size_t(-1); // not found
    EquationIndMap_t::const_iterator it=equationIndexMap.find(nm);
    if (it != equationIndexMap.end())
    {
        a = (it->second);
    }
    return a;
}

std::string Region::GetEquationNameFromVariable(const std::string &nm) const
{
    std::string a; // not found
    VariableEqnMap_t::const_iterator it=variableEquationMap.find(nm);
    if (it != variableEquationMap.end())
    {
        a = (it->second);
    }
    return a;
}

size_t Region::GetEquationNumber(size_t equation_index, ConstNodePtr np) const
{
    dsAssert(equation_index < numequations, "UNEXPECTED");
    dsAssert(baseeqnnum != size_t(-1), "UNEXPECTED");
    dsAssert(numequations != size_t(-1), "UNEXPECTED");
    const size_t num =  baseeqnnum + equation_index * GetNumberNodes() + np->GetIndex();
//    const size_t num =  baseeqnnum + equation_index + np->GetIndex() * numequations;
    return num;
}

void Region::SetBaseEquationNumber(size_t x)
{
    baseeqnnum = x;
}

size_t Region::GetBaseEquationNumber() const
{
    return baseeqnnum;
}

size_t Region::GetMaxEquationNumber() const
{
    size_t s = baseeqnnum + numequations * nodeList.size() - 1;
#if 0
    os << GetName() << " has base equation number of " << baseeqnnum << "\n";
    os << GetName() << " has max equation number of " << s << "\n";
#endif
    return s;
}

size_t Region::GetNumberEquations() const
{
    return numequations;
}

ConstDevicePtr Region::GetDevice() const
{
    dsAssert(device != nullptr, "UNEXPECTED");
    return device;
}

std::vector<std::string> Region::GetVariableList() const
{
    std::vector<std::string> vlist;
    VariableEqnMap_t::const_iterator it = variableEquationMap.begin();
    while (it != variableEquationMap.end())
    {
        vlist.push_back(it->first);
        ++it;
    }
    return vlist;
}

template <typename DoubleType>
void Region::Update(const std::vector<DoubleType> &result)
{
        absError = 0.0;
        relError = 0.0;

        if (!numequations)
        {
            return;
        }

        const EquationPtrMap_t &ep = GetEquationPtrList();
        EquationPtrMap_t::const_iterator eit = ep.begin();
        const EquationPtrMap_t::const_iterator eend = ep.end();
        for ( ; eit != eend; ++eit)
        {
            const std::string eqname = eit->first;
            const EquationHolder &eqptr = eit->second;
            const std::string var = eqptr.GetVariable();

            NodeModelPtr nm = std::const_pointer_cast<NodeModel, const NodeModel>(GetNodeModel(var));
            dsAssert(nm.get(), "UNEXPECTED");

            eqptr.Update(*nm, result);

            DoubleType rerr = eqptr.GetRelError<DoubleType>();
            DoubleType aerr = eqptr.GetAbsError<DoubleType>();

            absError += aerr;
            relError += rerr;

#if 0
            os << GetName() << " " << eqname << " " << var
                      << " " << aerr << " " << rerr << "\n";
#endif
        }
}

template <typename DoubleType>
void Region::ACUpdate(const std::vector<std::complex<DoubleType> > &result)
{
        if (!numequations)
        {
            return;
        }

        const EquationPtrMap_t &ep = GetEquationPtrList();
        EquationPtrMap_t::const_iterator eit = ep.begin();
        const EquationPtrMap_t::const_iterator eend = ep.end();
        for ( ; eit != eend; ++eit)
        {
            const std::string eqname = eit->first;
            const EquationHolder &eqptr = eit->second;
            const std::string var = eqptr.GetVariable();

            NodeModelPtr nm = std::const_pointer_cast<NodeModel, const NodeModel>(GetNodeModel(var));
            dsAssert(nm.get(), "UNEXPECTED");

            eqptr.ACUpdate(*nm, result);
        }
}

template <typename DoubleType>
void Region::NoiseUpdate(const std::string &output, const std::vector<PermutationEntry> &permvec, const std::vector<std::complex<DoubleType> > &result)
{
        if (!numequations)
        {
            return;
        }

        const EquationPtrMap_t &ep = GetEquationPtrList();
        EquationPtrMap_t::const_iterator eit = ep.begin();
        const EquationPtrMap_t::const_iterator eend = ep.end();
        for ( ; eit != eend; ++eit)
        {
            const std::string eqname = eit->first;
            const EquationHolder &eqptr = eit->second;

            eqptr.NoiseUpdate(output, permvec, result);
        }
}

template <typename DoubleType>
void Region::Assemble(dsMath::RealRowColValueVec<DoubleType> &m, dsMath::RHSEntryVec<DoubleType> &v, dsMathEnum::WhatToLoad w, dsMathEnum::TimeMode t)
{
    if (numequations)
    {
      const EquationPtrMap_t &ep = GetEquationPtrList();
      for (auto it : ep)
      {
        (it).second.Assemble(m, v, w, t);
      }
    }
}

void Region::BackupSolutions(const std::string &suffix)
{
  const std::vector<std::string> &vlist = GetVariableList();
  for (std::vector<std::string>::const_iterator it = vlist.begin(); it != vlist.end(); ++it)
  {
    ConstNodeModelPtr nm = GetNodeModel(*it);
    dsAssert(nm.get(), std::string("Node Solution: \"") + *it + "\" not available" );
    std::string bname = (*it) + suffix;
    NodeModelPtr bnm = std::const_pointer_cast<NodeModel, const NodeModel>(GetNodeModel(bname));
    if (!bnm)
    {
      if (std::dynamic_pointer_cast<const NodeSolution<extended_type>>(nm))
      {
        bnm = CreateNodeSolution(bname, this);
      }
      else if (std::dynamic_pointer_cast<const NodeSolution<double>>(nm))
      {
        bnm = CreateNodeSolution(bname, this);
      }
      else
      {
        dsAssert(false, std::string("Node Model: \"") + *it + "\" is not a Node Solution" );
      }
    }

    if (std::dynamic_pointer_cast<NodeSolution<extended_type>>(bnm))
    {
      bnm->SetValues(nm->GetScalarValues<extended_type>());
    }
    else if (std::dynamic_pointer_cast<NodeSolution<double>>(bnm))
    {
      bnm->SetValues(nm->GetScalarValues<double>());
    }
  }
}

void Region::RestoreSolutions(const std::string &suffix)
{
  const std::vector<std::string> &vlist = GetVariableList();
  for (std::vector<std::string>::const_iterator it = vlist.begin(); it != vlist.end(); ++it)
  {
    NodeModelPtr nm = std::const_pointer_cast<NodeModel, const NodeModel>(GetNodeModel(*it));
    dsAssert(nm.get(), "UNEXPECTED");
    std::string bname = (*it) + suffix;
    NodeModelPtr bnm = std::const_pointer_cast<NodeModel, const NodeModel>(GetNodeModel(bname));
    dsAssert(bnm.get(), "UNEXPECTED");

    if (std::dynamic_pointer_cast<NodeSolution<double>>(nm))
    {
      nm->SetValues(bnm->GetScalarValues<double>());
    }
#ifdef DEVSIM_EXTENDED_PRECISION
    else if (std::dynamic_pointer_cast<NodeSolution<float128>>(nm))
    {
      nm->SetValues(bnm->GetScalarValues<float128>());
    }
#endif
    else
    {
      dsAssert(0, "UNEXPECTED");
    }
  }
}

size_t Region::GetEdgeIndexOnTriangle(const Triangle &t, ConstEdgePtr ep) const
{

  size_t index = size_t(-1);

  size_t tindex = t.GetIndex();

  const TriangleToConstEdgeList_t &ttelist = GetTriangleToEdgeList(); 
  const ConstEdgeList &elist   = ttelist[tindex];

  for (size_t i = 0; i < 3; ++i)
  {
    if (elist[i] == ep)
    {
      index = i;
      break;
    }
  }
  return index;
}

size_t Region::GetEdgeIndexOnTetrahedron(const Tetrahedron &t, ConstEdgePtr ep) const
{

  size_t index = size_t(-1);

  size_t tindex = t.GetIndex();

  const TetrahedronToConstEdgeDataList_t &ttelist = GetTetrahedronToEdgeDataList(); 
  const ConstEdgeDataList &elist   = ttelist[tindex];

  for (size_t i = 0; i < 6; ++i)
  {
    if (elist[i]->edge == ep)
    {
      index = i;
      break;
    }
  }
  return index;
}

void Region::SetMaterial(const std::string &new_material)
{
  //// Our global parameter db has precedence
  GlobalData &gd = GlobalData::GetInstance();
  MaterialDB &md = MaterialDB::GetInstance();
  
  const MaterialDB::ParameterDataMap_t &old_data = md.GetOpenEntries(materialName);

  //// We can no longer rely on anything based on our old material name
  for (MaterialDB::ParameterDataMap_t::const_iterator it = old_data.begin(); it != old_data.end(); ++it)
  {
    const std::string &parameter_name = it->first;
    if (!(gd.GetDBEntryOnRegion(this, parameter_name).first))
    {
      this->SignalCallbacks(parameter_name);
    }
  }

  const MaterialDB::ParameterDataMap_t &global_data = md.GetOpenEntries("global");

  //// Check for global data.  If old material did not define it, then signal callback
  //// The next model evaluation will detect if it is available from the db
  for (MaterialDB::ParameterDataMap_t::const_iterator it = global_data.begin(); it != global_data.end(); ++it)
  {
    const std::string &parameter_name = it->first;
    if (!(gd.GetDBEntryOnRegion(this, parameter_name).first))
    {
      if (!old_data.count(parameter_name))
      {
        this->SignalCallbacks(parameter_name);
      }
    }
  }

  //// Mark all interface models as being out of date
  //// contact models already belong to region
  //// Should do this when parameters change as well
  const Device::InterfaceList_t &iml = GetDevice()->GetInterfaceList();
  for (Device::InterfaceList_t::const_iterator it = iml.begin(); it != iml.end(); ++it)
  {
    Interface *interface = it->second;
    if ((interface->GetRegion0() == this) || (interface->GetRegion1() == this))
    {
        const Interface::NameToInterfaceNodeModelMap_t &iml = interface->GetInterfaceNodeModelList();
        for (Interface::NameToInterfaceNodeModelMap_t::const_iterator it = iml.begin(); it != iml.end(); ++it)
        {
          it->second->MarkOld();
        }
    }
  }

  materialName = new_material;
}


std::string Region::GetNodeVolumeModel() const
{
  const GlobalData &ginst = GlobalData::GetInstance();

  GlobalData::DBEntry_t dbent = ginst.GetDBEntryOnRegion(this, "node_volume_model");

  dsAssert(dbent.first, "node_volume_model not specified\n");

  return dbent.second.GetString();
}

std::string Region::GetEdgeCoupleModel() const
{
  const GlobalData &ginst = GlobalData::GetInstance();

  GlobalData::DBEntry_t dbent = ginst.GetDBEntryOnRegion(this, "edge_couple_model");

  dsAssert(dbent.first, "edge_couple_model not specified\n");

  return dbent.second.GetString();
}

std::string Region::GetElementEdgeCoupleModel() const
{
  const GlobalData &ginst = GlobalData::GetInstance();

  GlobalData::DBEntry_t dbent = ginst.GetDBEntryOnRegion(this, "element_edge_couple_model");

  dsAssert(dbent.first, "element_edge_couple_model not specified\n");

  return dbent.second.GetString();
}

std::string Region::GetElementNode0VolumeModel() const
{
  const GlobalData &ginst = GlobalData::GetInstance();

  GlobalData::DBEntry_t dbent = ginst.GetDBEntryOnRegion(this, "element_node0_volume_model");

  dsAssert(dbent.first, "element_node0_volume_model not specified\n");

  return dbent.second.GetString();
}

std::string Region::GetElementNode1VolumeModel() const
{
  const GlobalData &ginst = GlobalData::GetInstance();

  GlobalData::DBEntry_t dbent = ginst.GetDBEntryOnRegion(this, "element_node1_volume_model");

  dsAssert(dbent.first, "element_node1_volume_model not specified\n");

  return dbent.second.GetString();
}

std::string Region::GetEdgeNode0VolumeModel() const
{
  const GlobalData &ginst = GlobalData::GetInstance();

  GlobalData::DBEntry_t dbent = ginst.GetDBEntryOnRegion(this, "edge_node0_volume_model");

  dsAssert(dbent.first, "edge_node0_volume_model not specified\n");

  return dbent.second.GetString();
}

std::string Region::GetEdgeNode1VolumeModel() const
{
  const GlobalData &ginst = GlobalData::GetInstance();

  GlobalData::DBEntry_t dbent = ginst.GetDBEntryOnRegion(this, "edge_node1_volume_model");

  dsAssert(dbent.first, "edge_node1_volume_model not specified\n");

  return dbent.second.GetString();
}

template <>
ModelExprDataCachePtr<double> Region::GetModelExprDataCache()
{
  return modelExprDataCache_double.lock();
}

template <>
void Region::SetModelExprDataCache(ModelExprDataCachePtr<double> p)
{
  modelExprDataCache_double = p;
}

#ifdef DEVSIM_EXTENDED_PRECISION
template <>
ModelExprDataCachePtr<float128> Region::GetModelExprDataCache()
{
  return modelExprDataCache_float128.lock();
}

template <>
void Region::SetModelExprDataCache(ModelExprDataCachePtr<float128> p)
{
  modelExprDataCache_float128 = p;
}
#endif


ConstEdgePtr Region::FindEdge(ConstNodePtr nh, ConstNodePtr nt) const
{
  ConstEdgePtr ret = nullptr;

  ConstEdgeList nout;
  // need these to be sorted ranges so do sort above
  const ConstEdgeList &nht = GetNodeToEdgeList()[nh->GetIndex()];
  const ConstEdgeList &ntt = GetNodeToEdgeList()[nt->GetIndex()];

  set_intersection(nht.begin(), nht.end(),
    ntt.begin(), ntt.end(),
    std::insert_iterator<ConstEdgeList>(nout, nout.begin()),
    EdgeCompIndex()
  );

  if (!nout.empty())
  {
    ret = nout[0];
  }

  return ret;
}

ConstTrianglePtr Region::FindTriangle(ConstNodePtr n0, ConstNodePtr n1, ConstNodePtr n2) const
{
  ConstTrianglePtr ret = nullptr;

  // need these to be sorted ranges so do sort above
  const ConstTriangleList &tl0 = GetNodeToTriangleList()[n0->GetIndex()];
  const ConstTriangleList &tl1 = GetNodeToTriangleList()[n1->GetIndex()];
  const ConstTriangleList &tl2 = GetNodeToTriangleList()[n2->GetIndex()];

  ConstTriangleList nout1;
  ConstTriangleList nout2;

  set_intersection(tl0.begin(), tl0.end(),
    tl1.begin(), tl1.end(),
    std::insert_iterator<ConstTriangleList>(nout1, nout1.begin()),
    TriangleCompIndex()
  );

  set_intersection(nout1.begin(), nout1.end(),
    tl2.begin(), tl2.end(),
    std::insert_iterator<ConstTriangleList>(nout2, nout2.begin()),
    TriangleCompIndex()
  );

  if (!nout2.empty())
  {
    ret = nout2[0];
  }

  return ret;
}

ConstTetrahedronPtr Region::FindTetrahedron(ConstNodePtr n0, ConstNodePtr n1, ConstNodePtr n2, ConstNodePtr n3) const
{
  ConstTetrahedronPtr ret = nullptr;

  // need these to be sorted ranges so do sort above
  const ConstTetrahedronList &tl0 = GetNodeToTetrahedronList()[n0->GetIndex()];
  const ConstTetrahedronList &tl1 = GetNodeToTetrahedronList()[n1->GetIndex()];
  const ConstTetrahedronList &tl2 = GetNodeToTetrahedronList()[n2->GetIndex()];
  const ConstTetrahedronList &tl3 = GetNodeToTetrahedronList()[n3->GetIndex()];

  ConstTetrahedronList nout1;
  ConstTetrahedronList nout2;
  ConstTetrahedronList nout3;

  set_intersection(tl0.begin(), tl0.end(),
    tl1.begin(), tl1.end(),
    std::insert_iterator<ConstTetrahedronList>(nout1, nout1.begin()),
    TetrahedronCompIndex()
  );

  set_intersection(nout1.begin(), nout1.end(),
    tl2.begin(), tl2.end(),
    std::insert_iterator<ConstTetrahedronList>(nout2, nout2.begin()),
    TetrahedronCompIndex()
  );

  set_intersection(nout2.begin(), nout2.end(),
    tl3.begin(), tl3.end(),
    std::insert_iterator<ConstTetrahedronList>(nout3, nout3.begin()),
    TetrahedronCompIndex()
  );

  if (!nout3.empty())
  {
    ret = nout3[0];
  }

  return ret;
}

bool Region::UseExtendedPrecisionType(const std::string &t) const
{
  bool ret = false;
#ifdef DEVSIM_EXTENDED_PRECISION
  const GlobalData &ginst = GlobalData::GetInstance();
  GlobalData::DBEntry_t dbent = ginst.GetDBEntryOnRegion(this, t);
  if (dbent.first)
  {
    const auto &x = dbent.second.GetBoolean();
    if (x.first)
    {
      ret = x.second;
    }
  }
#endif
  return ret;
}

bool Region::UseExtendedPrecisionModels() const
{
  return UseExtendedPrecisionType("extended_model");
}

bool Region::UseExtendedPrecisionEquations() const
{
  return UseExtendedPrecisionType("extended_equation");
}


#define DBLTYPE double
#include "RegionInstantiate.cc"
#undef DBLTYPE
#ifdef DEVSIM_EXTENDED_PRECISION
#define DBLTYPE float128
#include "RegionInstantiate.cc"
#endif

