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

Vector<double> GetCenter(const std::vector<ConstNodePtr> &nodes)
{
  const Vector<double> &v0 = nodes[0]->Position();

  static std::vector<Vector<double> > vecs(3);
  static std::vector<double> B(3);

  for (size_t i = 0; i < 3; ++i)
  {
    Vector<double> &v = vecs[i];
    v = nodes[i + 1]->Position();
    v -= v0;
    B[i] = 0.5*dot_prod(v, v);
  }


  dsMath::RealDenseMatrix M(3);
  for (size_t i = 0; i < 3; ++i)
  {
    const Vector<double> &v = vecs[i];
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

  Vector<double> ret(B[0], B[1], B[2]);
  ret += v0;
  return ret;
}

Vector<double> GetCenter(const Tetrahedron &tet)
{
  const std::vector<ConstNodePtr> &nodes = tet.GetNodeList();

  return GetCenter(nodes);
}

