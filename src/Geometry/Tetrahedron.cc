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

#include "Tetrahedron.hh"
#include "Node.hh"
#include "dsAssert.hh"
#include "DenseMatrix.hh"
#include "OutputStream.hh"
#include <cmath>
//const double Tetrahedron::EPSILON=1.0e-20;
Tetrahedron::Tetrahedron(size_t ind, ConstNodePtr n0, ConstNodePtr n1, ConstNodePtr n2, ConstNodePtr n3) : nodes(4)
{
    index = ind;
    nodes[0]=n0;
    nodes[1]=n1;
    nodes[2]=n2;
    nodes[3]=n3;
}

Vector GetCenter(const std::vector<ConstNodePtr> &nodes)
{
  const Vector &v0 = nodes[0]->Position();

  static std::vector<Vector> vecs(3);
  static std::vector<double> B(3);

  for (size_t i = 0; i < 3; ++i)
  {
    Vector &v = vecs[i];
    v = nodes[i + 1]->Position();
    v -= v0;
    B[i] = 0.5*dot_prod(v, v);
  }


  dsMath::RealDenseMatrix M(3);
  for (size_t i = 0; i < 3; ++i)
  {
    const Vector &v = vecs[i];
    M(i, 0) = v.Getx();
    M(i, 1) = v.Gety();
    M(i, 2) = v.Getz();
  }

  const bool ok = M.LUFactor();
  if (!ok)
  {
    OutputStream::WriteOut(OutputStream::FATAL, "BAD TETRAHEDRON");
  }

  const bool info = M.Solve(B);
  if (!info)
  {
    OutputStream::WriteOut(OutputStream::FATAL, "BAD TETRAHEDRON");
  }

  Vector ret(B[0], B[1], B[2]);
  ret += v0;
  return ret;
}

Vector GetCenter(const Tetrahedron &tet)
{
  const std::vector<ConstNodePtr> &nodes = tet.GetNodeList();

  return GetCenter(nodes);
}

