/***
DEVSIM
Copyright 2013 DEVSIM LLC

SPDX-License-Identifier: Apache-2.0
***/

#include "Tetrahedron.hh"
#include "Node.hh"
#include "dsAssert.hh"
#include "DenseMatrix.hh"
#include "OutputStream.hh"
#include <cmath>
#include <array>
//const double Tetrahedron::EPSILON=1.0e-20;
Tetrahedron::Tetrahedron(size_t ind, ConstNodePtr n0, ConstNodePtr n1, ConstNodePtr n2, ConstNodePtr n3) : nodes(4)
{
    index = ind;
    nodes[0]=n0;
    nodes[1]=n1;
    nodes[2]=n2;
    nodes[3]=n3;
}

const std::vector<ConstNodePtr> &Tetrahedron::GetFENodeList() const
{
  if (!fe_nodes.empty())
  {
    return fe_nodes;
  }
  fe_nodes.resize(4);
  for (size_t i = 0; i < 4; ++i)
  {
    fe_nodes[i] = nodes[i];
  }

  std::array<Vector<double>, 4> positions;
  for (size_t i = 0; i < 4; ++i)
  {
    positions[i] = ConvertPosition<double>(nodes[i]->Position());
  }

  std::array<Vector<double>, 3> vecs;
  for (size_t i = 0; i < 3; ++i)
  {
    vecs[i] = positions[i+1] - positions[i];
  }
  const auto &cp = vecs[1].cross_prod(vecs[2]);
  const auto &dp = vecs[0].dot_prod(cp);
  // is this sign correct for output normal
  if (dp > 0.0)
  {
    std::swap(fe_nodes[2], fe_nodes[3]);
  }

  return fe_nodes;
}


template <typename DoubleType>
Vector<DoubleType> GetTetrahedronCenter(const std::vector<ConstNodePtr> &nodes)
{
  const Vector<DoubleType> &v0 = ConvertPosition<DoubleType>(nodes[0]->Position());

  thread_local std::vector<Vector<DoubleType> > vecs(3);
  thread_local std::vector<DoubleType> B(3);

  for (size_t i = 0; i < 3; ++i)
  {
    Vector<DoubleType> &v = vecs[i];
    v = ConvertPosition<DoubleType>(nodes[i + 1]->Position());
    v -= v0;
    B[i] = 0.5*dot_prod(v, v);
  }


  dsMath::RealDenseMatrix<DoubleType> M(3);
  for (size_t i = 0; i < 3; ++i)
  {
    const Vector<DoubleType> &v = vecs[i];
    M(i, 0) = v.Getx();
    M(i, 1) = v.Gety();
    M(i, 2) = v.Getz();
  }

  const bool ok = M.LUFactor();
  if (!ok)
  {
    OutputStream::WriteOut(OutputStream::OutputType::FATAL, "BAD TETRAHEDRON");
  }

  const bool info = M.Solve(B.data());
  if (!info)
  {
    OutputStream::WriteOut(OutputStream::OutputType::FATAL, "BAD TETRAHEDRON");
  }

  Vector<DoubleType> ret(B[0], B[1], B[2]);
  ret += v0;
  return ret;
}

template <typename DoubleType>
Vector<DoubleType> GetCenter(const Tetrahedron &tet)
{
  const std::vector<ConstNodePtr> &nodes = tet.GetNodeList();

  return GetTetrahedronCenter<DoubleType>(nodes);
}


template Vector<double> GetTetrahedronCenter(const std::vector<ConstNodePtr> &nodes);
template Vector<double> GetCenter(const Tetrahedron &tet);

#ifdef DEVSIM_EXTENDED_PRECISION
#include "Float128.hh"
template Vector<float128> GetTetrahedronCenter(const std::vector<ConstNodePtr> &nodes);
template Vector<float128> GetCenter(const Tetrahedron &tet);
#endif

