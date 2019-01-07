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

#include "Equation.hh"
#include "EquationHolder.hh"
#include "Region.hh"

#include "Node.hh"
#include "NodeModel.hh"
#include "NodeSolution.hh"
#include "NodeScalarData.hh"

#include "Edge.hh"
#include "EdgeModel.hh"
#include "EdgeScalarData.hh"
#include "EdgeData.hh"

#include "Triangle.hh"

#include "MatrixEntries.hh"

#include "GeometryStream.hh"

#include "VectorGradient.hh"

#include "dsAssert.hh"

#include "EquationErrors.hh"

#include "Permutation.hh"

#include <cmath>
using std::abs;

template <typename DoubleType>
const DoubleType Equation<DoubleType>::defminError = 1.0e-10;

template <typename DoubleType>
void Equation<DoubleType>::setMinError(DoubleType m)
{
    minError = m;
}

template <typename DoubleType>
DoubleType Equation<DoubleType>::GetMinError() const
{
    return minError;
}

template <typename DoubleType>
void Equation<DoubleType>::setAbsError(DoubleType a)
{
    absError = a;
}

template <typename DoubleType>
void Equation<DoubleType>::setRelError(DoubleType r)
{
    relError = r;
}

template <typename DoubleType>
DoubleType Equation<DoubleType>::GetAbsError() const
{
    return absError;
}

template <typename DoubleType>
DoubleType Equation<DoubleType>::GetRelError() const
{
    return relError;
}

template <typename DoubleType>
Equation<DoubleType>::Equation(const std::string &nm, RegionPtr rp, const std::string &var, EquationEnum::UpdateType ut)
    : myname(nm), myregion(rp), variable(var), absError(0.0), relError(0.0), minError(defminError), updateType(ut)
{
    EquationHolder ptr(this);
    rp->AddEquation(ptr);
}

template <typename DoubleType>
Equation<DoubleType>::~Equation()
{
}

// At this level, we can probably control which row we wish to assemble
template <typename DoubleType>
void Equation<DoubleType>::Assemble(dsMath::RealRowColValueVec<DoubleType> &m, dsMath::RHSEntryVec<DoubleType> &v, dsMathEnum::WhatToLoad w, dsMathEnum::TimeMode t)
{
    // get noncontact node list from Region
    // assemble each row individually
    // or better yet send exclusion list of rows off to the matrix class
    DerivedAssemble(m, v, w, t);
}

template <typename DoubleType>
void Equation<DoubleType>::Update(NodeModel &nm, const std::vector<DoubleType> &rhs)
{
    UpdateValues(nm, rhs);
}

template <typename DoubleType>
void Equation<DoubleType>::ACUpdate(NodeModel &nm, const std::vector<std::complex<DoubleType> > &rhs)
{
    ACUpdateValues(nm, rhs);
}

template <typename DoubleType>
void Equation<DoubleType>::NoiseUpdate(const std::string &nm, const std::vector<PermutationEntry> &permvec, const std::vector<std::complex<DoubleType> > &rhs)
{
    NoiseUpdateValues(nm, permvec, rhs);
}

template <typename DoubleType>
void Equation<DoubleType>::LogSolutionUpdate(const NodeScalarList<DoubleType> &ovals, NodeScalarList<DoubleType> &upds, NodeScalarList<DoubleType> &nvals)
{
  for (size_t i = 0; i < ovals.size(); ++i)
  {
      DoubleType upd = upds[i];
      if ( abs(upd) > 0.0259 )
      {
          const DoubleType sign = (upd > 0.0) ? 1.0 : -1.0;
          upd = sign*0.0259*log(1+abs(upd)/0.0259);
      }

      const DoubleType oval = ovals[i];

      const DoubleType nval = upd + oval;

      upds[i] = upd;

      nvals[i] = nval;
      
  }
}

template <typename DoubleType>
void Equation<DoubleType>::PositiveSolutionUpdate(const NodeScalarList<DoubleType> &ovals, NodeScalarList<DoubleType> &upds, NodeScalarList<DoubleType> &nvals)
{
  for (size_t i = 0; i < ovals.size(); ++i)
  {
      const DoubleType oval = ovals[i];

      DoubleType upd = upds[i];

      DoubleType nval = upd + oval;

      if (oval > 0)
      {
        if (nval <= 0.0)
        {
          nval = 0.001 * oval;
          if (nval <= 0.0)
          {
            nval  = 0.5 * oval;

            if (nval <= 0.0)
            {
              nval = oval;
            }
          }

          upd  = nval - oval;
        }
#if 0
        while ((upd + oval) <=  0.0)
        {
            upd *= 0.5;
        }
#endif
      }
      else
      {
        upd = 0;
        const Region &reg = *myregion;
        dsErrors::SolutionVariableNonPositive(reg, myname, GetVariable(), oval, OutputStream::OutputType::FATAL);
      }

      upds[i] = upd;
      nvals[i] = nval;
  }
}

template <typename DoubleType>
void Equation<DoubleType>::DefaultSolutionUpdate(const NodeScalarList<DoubleType> &ovals, NodeScalarList<DoubleType> &upds, NodeScalarList<DoubleType> &nvals)
{
  for (size_t i = 0; i < ovals.size(); ++i)
  {
      const DoubleType upd = upds[i];

      const DoubleType oval = ovals[i];

      const DoubleType nval = upd + oval;

      nvals[i] = nval;
  }
}

template <typename DoubleType>
void Equation<DoubleType>::DefaultUpdate(NodeModel &nm, const std::vector<DoubleType> &result)
{
    const Region &reg = *myregion;

    const size_t ind = reg.GetEquationIndex(myname);
//    dsAssert(ind != size_t(-1), "UNEXPECTED");
    if (ind == size_t(-1))
    {
      dsErrors::MissingEquationIndex(reg, myname, "", OutputStream::OutputType::FATAL) ;
    }

    const ConstNodeList &nl = myregion->GetNodeList();

    ConstNodeList::const_iterator nit = nl.begin();
    const ConstNodeList::const_iterator nend = nl.end();

    const NodeScalarList<DoubleType> &ovals = nm.GetScalarValues<DoubleType>();
    NodeScalarList<DoubleType> upds(ovals.size());

    for ( ; nit != nend; ++nit)
    {
        const size_t eqrow     = reg.GetEquationNumber(ind, *nit);
        const size_t nodeindex = (*nit)->GetIndex();
        upds[nodeindex]  = result[eqrow];
    }

    NodeScalarList<DoubleType> nvals(ovals.size());

    /// Could probably pass this as a functor class in the future
    if (updateType == EquationEnum::LOGDAMP)
    {
      LogSolutionUpdate(ovals, upds, nvals);
    }
    else if (updateType == EquationEnum::POSITIVE)
    {
      PositiveSolutionUpdate(ovals, upds, nvals);
    }
    else if (updateType == EquationEnum::DEFAULT)
    {
      DefaultSolutionUpdate(ovals, upds, nvals);
    }
    else
    {
      dsAssert(0, "UNEXPECTED");
    }

    nm.SetValues(nvals);

    DoubleType aerr = 0.0;
    DoubleType rerr = 0.0;

    for (size_t i = 0; i < upds.size(); ++i)
    {
        const DoubleType n1 = abs(upds[i]);

        if (n1 > aerr)
        {
          aerr = n1;
        }
//      aerr += n1;

        const DoubleType n2 = abs(nvals[i]);

        const DoubleType nrerror =  n1 / (n2 + minError); 

        if (nrerror > rerr)
        {
            rerr = nrerror;
        }

    }

    setAbsError(aerr);
    setRelError(rerr);
//    dsErrors::EquationMathErrorInfo(*this, rerr, aerr, OutputStream::OutputType::INFO);
}

//// TODO: Make sure noise and ac kept in sync
template <typename DoubleType>
void Equation<DoubleType>::DefaultACUpdate(NodeModel &nm, const std::vector<std::complex<DoubleType> > &result)
{
    const std::string &realnodemodel = nm.GetRealName();
    const std::string &imagnodemodel = nm.GetImagName();

    const size_t ind = myregion->GetEquationIndex(myname);
//    dsAssert(ind != size_t(-1), "UNEXPECTED");
    if (ind == size_t(-1))
    {
      dsErrors::MissingEquationIndex(*myregion, myname, "", OutputStream::OutputType::FATAL) ;
    }

    const ConstNodeList &nl = myregion->GetNodeList();

    NodeModelPtr rnm = std::const_pointer_cast<NodeModel, const NodeModel>(myregion->GetNodeModel(realnodemodel));
    NodeModelPtr inm = std::const_pointer_cast<NodeModel, const NodeModel>(myregion->GetNodeModel(imagnodemodel));

    if (!rnm)
    {
        dsErrors::CreateModelOnRegion(*myregion, realnodemodel, OutputStream::OutputType::INFO);
        rnm = CreateNodeSolution(realnodemodel, myregion);
    }

    if (!inm)
    {
        dsErrors::CreateModelOnRegion(*myregion, imagnodemodel, OutputStream::OutputType::INFO);
        inm = CreateNodeSolution(imagnodemodel, myregion);
    }

    NodeScalarList<DoubleType> realout(nl.size());
    NodeScalarList<DoubleType> imagout(nl.size());

    ConstNodeList::const_iterator nit = nl.begin();
    const ConstNodeList::const_iterator nend = nl.end();
    for ( ; nit != nend; ++nit)
    {
        const size_t eqindex = (*nit)->GetIndex();
        const size_t eqrow = myregion->GetEquationNumber(ind, *nit);

        const std::complex<DoubleType> upd  = result[eqrow];

        realout[eqindex] = upd.real();
        imagout[eqindex] = upd.imag();
    }

    rnm->SetValues(realout);
    inm->SetValues(imagout);
}

template <typename DoubleType>
void Equation<DoubleType>::DefaultNoiseUpdate(const std::string &outputname, const std::vector<PermutationEntry> &permvec, const std::vector<std::complex<DoubleType> > &result)
{
    const std::string &realnodemodel = GetNoiseRealName(outputname);
    const std::string &imagnodemodel = GetNoiseImagName(outputname);

    const size_t ind = myregion->GetEquationIndex(myname);
//    dsAssert(ind != size_t(-1), "UNEXPECTED");
    if (ind == size_t(-1))
    {
      dsErrors::MissingEquationIndex(*myregion, myname, "", OutputStream::OutputType::FATAL) ;
    }

    const ConstNodeList &nl = myregion->GetNodeList();

    NodeModelPtr rnm = std::const_pointer_cast<NodeModel, const NodeModel>(myregion->GetNodeModel(realnodemodel));
    NodeModelPtr inm = std::const_pointer_cast<NodeModel, const NodeModel>(myregion->GetNodeModel(imagnodemodel));

    if (!rnm)
    {
        dsErrors::CreateModelOnRegion(*myregion, realnodemodel, OutputStream::OutputType::INFO);
        rnm = CreateNodeSolution(realnodemodel, myregion);
    }

    if (!inm)
    {
        dsErrors::CreateModelOnRegion(*myregion, imagnodemodel, OutputStream::OutputType::INFO);
        inm = CreateNodeSolution(imagnodemodel, myregion);
    }

    NodeScalarList<DoubleType> realout(nl.size());
    NodeScalarList<DoubleType> imagout(nl.size());

    ConstNodeList::const_iterator nit = nl.begin();
    const ConstNodeList::const_iterator nend = nl.end();
    for ( ; nit != nend; ++nit)
    {
        // TODO: we need to make sure we handle when the equation was copied, KeepCopy() == true
        const size_t eqrow = permvec[myregion->GetEquationNumber(ind, *nit)].GetRow();

        if (eqrow != size_t(-1))
        {
          const size_t eqindex = (*nit)->GetIndex();
          const std::complex<DoubleType> upd  = result[eqrow];
          realout[eqindex] = upd.real();
          imagout[eqindex] = upd.imag();
        }
    }

    rnm->SetValues(realout);
    inm->SetValues(imagout);

    const std::string xrname = realnodemodel + "_gradx";
    const std::string xiname = imagnodemodel + "_gradx";
    if (!myregion->GetNodeModel(xrname))
    {
      CreateVectorGradient(myregion, realnodemodel, VectorGradientEnum::AVOIDZERO);
    }
    if (!myregion->GetNodeModel(xiname))
    {
      CreateVectorGradient(myregion, imagnodemodel, VectorGradientEnum::AVOIDZERO);
    }
}

template <typename DoubleType>
void Equation<DoubleType>::EdgeAssembleRHS(dsMath::RHSEntryVec<DoubleType> &v, const EdgeScalarData<DoubleType> &eflux, const DoubleType n0_sign, const DoubleType n1_sign)
{
    const Region &r = GetRegion();
    const size_t eqindex0 = r.GetEquationIndex(myname);
//    dsAssert(eqindex0 != size_t(-1), "UNEXPECTED");
    if (eqindex0 == size_t(-1))
    {
      dsErrors::MissingEquationIndex(r, myname, "", OutputStream::OutputType::FATAL) ;
    }

    const ConstEdgeList &el = r.GetEdgeList();
    for (size_t i = 0 ; i < el.size(); ++i)
    {
        const ConstNodeList &nl = el[i]->GetNodeList();
        const size_t row0 = r.GetEquationNumber(eqindex0, nl[0]);
        const size_t row1 = r.GetEquationNumber(eqindex0, nl[1]);

        const DoubleType rhsval = eflux[i];

        v.push_back(std::make_pair(row0,  n0_sign*rhsval));
        v.push_back(std::make_pair(row1,  n1_sign*rhsval));
    }
}

/// should have one assembly routine per shape
/// Other variants should handle case if ElementNodeVolume is used on 3d element
template <typename DoubleType>
void Equation<DoubleType>::TriangleEdgeAssembleRHS(dsMath::RHSEntryVec<DoubleType> &v, const TriangleEdgeScalarData<DoubleType> &teflux, const DoubleType n0_sign, const DoubleType n1_sign)
{
  const Region &r = GetRegion();
  const size_t eqindex0 = r.GetEquationIndex(myname);
//  dsAssert(eqindex0 != size_t(-1), "UNEXPECTED");
  if (eqindex0 == size_t(-1))
  {
    dsErrors::MissingEquationIndex(r, myname, "", OutputStream::OutputType::FATAL) ;
  }

  const Region::TriangleToConstEdgeList_t &ttelist = r.GetTriangleToEdgeList();
  for (size_t i = 0 ; i < ttelist.size(); ++i)
  {
    const ConstEdgeList &el = ttelist[i];
    for (size_t j = 0; j < el.size(); ++j)
    {
      const ConstNodeList &nl = el[j]->GetNodeList();

      const size_t row0 = r.GetEquationNumber(eqindex0, nl[0]);
      const size_t row1 = r.GetEquationNumber(eqindex0, nl[1]);

      const DoubleType rhsval = teflux[3*i + j];

      v.push_back(std::make_pair(row0,  n0_sign * rhsval));
      v.push_back(std::make_pair(row1,  n1_sign * rhsval));
    }
  }
}

template <typename DoubleType>
void Equation<DoubleType>::TetrahedronEdgeAssembleRHS(dsMath::RHSEntryVec<DoubleType> &v, const TetrahedronEdgeScalarData<DoubleType> &teflux, const DoubleType n0_sign, const DoubleType n1_sign)
{
  const Region &r = GetRegion();
  const size_t eqindex0 = r.GetEquationIndex(myname);
//  dsAssert(eqindex0 != size_t(-1), "UNEXPECTED");
  if (eqindex0 == size_t(-1))
  {
    dsErrors::MissingEquationIndex(r, myname, "", OutputStream::OutputType::FATAL) ;
  }

  const Region::TetrahedronToConstEdgeDataList_t &ttelist = r.GetTetrahedronToEdgeDataList();
  for (size_t i = 0 ; i < ttelist.size(); ++i)
  {
    const ConstEdgeDataList &edgeDataList = ttelist[i];
    for (size_t j = 0; j < edgeDataList.size(); ++j)
    {
      const EdgeData &edgeData = *edgeDataList[j];
      const Edge     &edge     = *edgeData.edge;
      const ConstNodeList &nl = edge.GetNodeList();

      const size_t row0 = r.GetEquationNumber(eqindex0, nl[0]);
      const size_t row1 = r.GetEquationNumber(eqindex0, nl[1]);

      const DoubleType rhsval = teflux[6*i + j];

      v.push_back(std::make_pair(row0,  n0_sign * rhsval));
      v.push_back(std::make_pair(row1,  n1_sign * rhsval));
    }
  }
}

template <typename DoubleType>
std::string Equation<DoubleType>::GetNoiseRealName(const std::string &outname)
{
  std::string out(outname); 
  out += "_";
  out += myname;
  out += "_";
  out += "real";
  return out;
}

template <typename DoubleType>
std::string Equation<DoubleType>::GetNoiseImagName(const std::string &outname)
{
  std::string out(outname); 
  out += "_";
  out += myname;
  out += "_";
  out += "imag";
  return out;
}

#if 0
//// Not currently being used for our generic cases
/// Inserts the entries into the matrix for you, currently does not scale by edge couple
template <typename DoubleType>
void Equation<DoubleType>::SymmetricEdgeAssembleJacobian(dsMath::RealRowColValueVec<DoubleType> &m, const EdgeScalarData<DoubleType> &eder, const std::string &var)
{
    const Region &r = GetRegion();
    const size_t eqindex0 = r.GetEquationIndex(myname);
    dsAssert(eqindex0 != size_t(-1), "UNEXPECTED");
    const size_t eqindex1 = r.GetEquationIndex(r.GetEquationNameFromVariable(var));
    dsAssert(eqindex1 != size_t(-1), "UNEXPECTED");
    // assemble the edge components to rhs first
    const ConstEdgeList &el = r.GetEdgeList();
    for (size_t i = 0 ; i < el.size(); ++i)
    {
        const ConstNodeList &nl = el[i]->GetNodeList();
        const size_t row0 = r.GetEquationNumber(eqindex0, nl[0]);
        const size_t col0 = r.GetEquationNumber(eqindex1, nl[0]);
        const size_t row1 = r.GetEquationNumber(eqindex0, nl[1]);
        const size_t col1 = r.GetEquationNumber(eqindex1, nl[1]);

        // Assemble the Jacobian integrating over the surface area
        const DoubleType ederval = eder[i];
        m.push_back(dsMath::RowColVal(row0, col0, ederval));
        m.push_back(dsMath::RowColVal(row1, col1, ederval));
        m.push_back(dsMath::RowColVal(row0, col1, -ederval));
        m.push_back(dsMath::RowColVal(row1, col0, -ederval));
    }
}
#endif

/// Inserts the entries into the matrix for the case when the sensitivity is different with respect to the same variable at the nodes opposite of the edge
/// eder0 is derivative wrt first node
/// eder1 is derivative wrt second node
template <typename DoubleType>
void Equation<DoubleType>::UnSymmetricEdgeAssembleJacobian(dsMath::RealRowColValueVec<DoubleType> &m, const EdgeScalarData<DoubleType> &eder0, const EdgeScalarData<DoubleType> &eder1, const std::string &var, const DoubleType n0_sign, const DoubleType n1_sign)
{
    const Region &r = GetRegion();
    const size_t eqindex0 = r.GetEquationIndex(myname);
//    dsAssert(eqindex0 != size_t(-1), "UNEXPECTED");
    if (eqindex0 == size_t(-1))
    {
      dsErrors::MissingEquationIndex(r, myname, "", OutputStream::OutputType::FATAL) ;
    }

    const size_t eqindex1 = r.GetEquationIndex(r.GetEquationNameFromVariable(var));
//    dsAssert(eqindex1 != size_t(-1), "UNEXPECTED");
    if (eqindex1 == size_t(-1))
    {
      dsErrors::MissingEquationIndex(r, myname, var, OutputStream::OutputType::FATAL) ;
    }

    // assemble the edge components to rhs first
    const ConstEdgeList &el = r.GetEdgeList();
    for (size_t i = 0 ; i < el.size(); ++i)
    {
        const ConstNodeList &nl = el[i]->GetNodeList();
        const size_t row0 = r.GetEquationNumber(eqindex0, nl[0]);
        const size_t col0 = r.GetEquationNumber(eqindex1, nl[0]);
        const size_t row1 = r.GetEquationNumber(eqindex0, nl[1]);
        const size_t col1 = r.GetEquationNumber(eqindex1, nl[1]);

        const DoubleType ederval0 = eder0[i];
        const DoubleType ederval1 = eder1[i];

        /// Here we account for the fact stuff moving toward row1 has opposite sign
        m.push_back(dsMath::RealRowColVal<DoubleType>(row0, col0, n0_sign * ederval0));
        m.push_back(dsMath::RealRowColVal<DoubleType>(row1, col1, n1_sign * ederval1));
        m.push_back(dsMath::RealRowColVal<DoubleType>(row0, col1, n0_sign * ederval1));
        m.push_back(dsMath::RealRowColVal<DoubleType>(row1, col0, n1_sign * ederval0));
    }
}

/// Inserts the entries into the matrix for the case when the sensitivity is different with respect to the same variable at the nodes opposite of the edge
/// eder0 is derivative wrt first node
/// eder1 is derivative wrt second node
/// eder2 is derivative wrt third node
template <typename DoubleType>
void Equation<DoubleType>::UnSymmetricTriangleEdgeAssembleJacobian(dsMath::RealRowColValueVec<DoubleType> &m, const TriangleEdgeScalarData<DoubleType> &eder0, const TriangleEdgeScalarData<DoubleType> &eder1, const TriangleEdgeScalarData<DoubleType> &eder2, const std::string &var, const DoubleType n0_sign, const DoubleType n1_sign)
{
  const Region &r = GetRegion();
  const size_t eqindex0 = r.GetEquationIndex(myname);
//  dsAssert(eqindex0 != size_t(-1), "UNEXPECTED");
  if (eqindex0 == size_t(-1))
  {
    dsErrors::MissingEquationIndex(r, myname, "", OutputStream::OutputType::FATAL) ;
  }

  const size_t eqindex1 = r.GetEquationIndex(r.GetEquationNameFromVariable(var));
//  dsAssert(eqindex1 != size_t(-1), "UNEXPECTED");
  if (eqindex1 == size_t(-1))
  {
    dsErrors::MissingEquationIndex(r, myname, var, OutputStream::OutputType::FATAL) ;
  }

  const Region::TriangleToConstEdgeList_t &ttelist = r.GetTriangleToEdgeList();
  const ConstTriangleList &triangleList = r.GetTriangleList();
  dsAssert(triangleList.size() == ttelist.size(), "UNEXPECTED");

  for (size_t i = 0 ; i < ttelist.size(); ++i)
  {
    // assemble the edge components to rhs first
    const ConstEdgeList &el = ttelist[i];

    const Triangle &triangle = *triangleList[i];

    const ConstNodeList &tnl = triangle.GetNodeList();

    for (size_t j = 0 ; j < el.size(); ++j)
    {
      const ConstNodeList &nl = el[j]->GetNodeList();

      const Node * const node0 = nl[0];
      const Node * const node1 = nl[1];

      //// we are guaranteed that the node is across from the edge
      const Node * const node2 = tnl[j];

      const size_t row0 = r.GetEquationNumber(eqindex0, node0);
      const size_t col0 = r.GetEquationNumber(eqindex1, node0);
      const size_t row1 = r.GetEquationNumber(eqindex0, node1);
      const size_t col1 = r.GetEquationNumber(eqindex1, node1);

      const size_t col2 = r.GetEquationNumber(eqindex1, node2);

      const size_t eindex = 3 * i + j;
      const DoubleType ederval0 = eder0[eindex];
      const DoubleType ederval1 = eder1[eindex];
      const DoubleType ederval2 = eder2[eindex];

      /// Here we account for the fact stuff moving toward row1 has opposite sign
      m.push_back(dsMath::RealRowColVal<DoubleType>(row0, col0,  n0_sign * ederval0));
      m.push_back(dsMath::RealRowColVal<DoubleType>(row1, col1,  n1_sign * ederval1));
      m.push_back(dsMath::RealRowColVal<DoubleType>(row0, col1,  n0_sign * ederval1));
      m.push_back(dsMath::RealRowColVal<DoubleType>(row1, col0,  n1_sign * ederval0));

      /// This is true as long as we are projected along the unit vector
      m.push_back(dsMath::RealRowColVal<DoubleType>(row0, col2,  n0_sign * ederval2));
      m.push_back(dsMath::RealRowColVal<DoubleType>(row1, col2,  n1_sign * ederval2));
    }
  }
}

template <typename DoubleType>
void Equation<DoubleType>::UnSymmetricTetrahedronEdgeAssembleJacobian(dsMath::RealRowColValueVec<DoubleType> &m, const TetrahedronEdgeScalarData<DoubleType> &eder0, const TetrahedronEdgeScalarData<DoubleType> &eder1, const TetrahedronEdgeScalarData<DoubleType> &eder2, const TetrahedronEdgeScalarData<DoubleType> &eder3, const std::string &var, const DoubleType n0_sign, const DoubleType n1_sign)
{
  const Region &r = GetRegion();
  const size_t eqindex0 = r.GetEquationIndex(myname);
//  dsAssert(eqindex0 != size_t(-1), "UNEXPECTED");
  if (eqindex0 == size_t(-1))
  {
    dsErrors::MissingEquationIndex(r, myname, "", OutputStream::OutputType::FATAL) ;
  }

  const size_t eqindex1 = r.GetEquationIndex(r.GetEquationNameFromVariable(var));
//  dsAssert(eqindex1 != size_t(-1), "UNEXPECTED");
  if (eqindex1 == size_t(-1))
  {
    dsErrors::MissingEquationIndex(r, myname, var, OutputStream::OutputType::FATAL) ;
  }

  const Region::TetrahedronToConstEdgeDataList_t &ttelist = r.GetTetrahedronToEdgeDataList();
  const ConstTetrahedronList &tetrahedronList = r.GetTetrahedronList();
  dsAssert(tetrahedronList.size() == ttelist.size(), "UNEXPECTED");


  for (size_t i = 0 ; i < ttelist.size(); ++i)
  {
    // assemble the edge components to rhs first
    const ConstEdgeDataList &edgeDataList = ttelist[i];

    for (size_t j = 0 ; j < edgeDataList.size(); ++j)
    {
      const EdgeData &edgeData = *edgeDataList[j];
      const Edge &edge = *(edgeData.edge);

      const ConstNodeList &nl = edge.GetNodeList();

      const Node * const node0 = nl[0];
      const Node * const node1 = nl[1];

      //// we are guaranteed that the node is across from the edge
      const Node * const node2 = edgeData.nodeopp[0];
      const Node * const node3 = edgeData.nodeopp[1];

      const size_t row0 = r.GetEquationNumber(eqindex0, node0);
      const size_t col0 = r.GetEquationNumber(eqindex1, node0);
      const size_t row1 = r.GetEquationNumber(eqindex0, node1);
      const size_t col1 = r.GetEquationNumber(eqindex1, node1);

      const size_t col2 = r.GetEquationNumber(eqindex1, node2);
      const size_t col3 = r.GetEquationNumber(eqindex1, node3);

      const size_t eindex = 6 * i + j;
      const DoubleType ederval0 = eder0[eindex];
      const DoubleType ederval1 = eder1[eindex];
      const DoubleType ederval2 = eder2[eindex];
      const DoubleType ederval3 = eder3[eindex];

      /// Here we account for the fact stuff moving toward row1 has opposite sign
      m.push_back(dsMath::RealRowColVal<DoubleType>(row0, col0,  n0_sign * ederval0));
      m.push_back(dsMath::RealRowColVal<DoubleType>(row1, col1,  n1_sign * ederval1));
      m.push_back(dsMath::RealRowColVal<DoubleType>(row0, col1,  n0_sign * ederval1));
      m.push_back(dsMath::RealRowColVal<DoubleType>(row1, col0,  n1_sign * ederval0));

      /// This is true as long as we are projected along the unit vector
      m.push_back(dsMath::RealRowColVal<DoubleType>(row0, col2,  n0_sign * ederval2));
      m.push_back(dsMath::RealRowColVal<DoubleType>(row1, col2,  n1_sign * ederval2));
      m.push_back(dsMath::RealRowColVal<DoubleType>(row0, col3,  n0_sign * ederval3));
      m.push_back(dsMath::RealRowColVal<DoubleType>(row1, col3,  n1_sign * ederval3));
    }
  }
}

/// Not scaled by node volume
template <typename DoubleType>
void Equation<DoubleType>::NodeAssembleRHS(dsMath::RHSEntryVec<DoubleType> &v, const NodeScalarData<DoubleType> &nrhs)
{
    // var is the name of the variable we are differentiating with respect to.  It is therefore the column and our equation is the row

    const Region &r = GetRegion();
    const size_t eqindex0 = r.GetEquationIndex(myname);

//    dsAssert(eqindex0 != size_t(-1), "UNEXPECTED");
    if (eqindex0 == size_t(-1))
    {
      dsErrors::MissingEquationIndex(r, myname, "", OutputStream::OutputType::FATAL) ;
    }

    const ConstNodeList &nl = r.GetNodeList();
    for (size_t i = 0; i < nl.size(); ++i)
    {
        const size_t row1 = r.GetEquationNumber(eqindex0, nl[i]);
        const DoubleType rhsval = nrhs.GetScalarList()[i];
        // Note that the sign is reversed
        v.push_back(std::make_pair(row1, rhsval));
    }
}

template <typename DoubleType>
void Equation<DoubleType>::NodeAssembleJacobian(dsMath::RealRowColValueVec<DoubleType> &m, const NodeScalarData<DoubleType> &nder, const std::string &var)
{
    // var is the name of the variable we are differentiating with respect to.  It is therefore the column and our equation is the row

    const Region &r = GetRegion();
    const size_t eqindex0 = r.GetEquationIndex(myname);
//    dsAssert(eqindex0 != size_t(-1), "UNEXPECTED");

    if (eqindex0 == size_t(-1))
    {
      dsErrors::MissingEquationIndex(r, myname, "", OutputStream::OutputType::FATAL) ;
    }

    const size_t eqindex1 = r.GetEquationIndex(r.GetEquationNameFromVariable(var));
//    dsAssert(eqindex1 != size_t(-1), "UNEXPECTED");

    if (eqindex1 == size_t(-1))
    {
      dsErrors::MissingEquationIndex(r, myname, var, OutputStream::OutputType::FATAL) ;
    }

    const ConstNodeList &nl = r.GetNodeList();
    for (size_t i = 0; i < nl.size(); ++i)
    {
        const size_t row1 = r.GetEquationNumber(eqindex0, nl[i]);
        const size_t row2 = r.GetEquationNumber(eqindex1, nl[i]);

        const DoubleType derval = nder.GetScalarList()[i];
        m.push_back(dsMath::RealRowColVal<DoubleType>(row1, row2, derval));
    }
}

template <typename DoubleType>
void Equation<DoubleType>::NodeVolumeAssemble(const std::string &model, dsMath::RealRowColValueVec<DoubleType> &m, dsMath::RHSEntryVec<DoubleType> &v, dsMathEnum::WhatToLoad w)
{
  const std::string &volume = GetRegion().GetNodeVolumeModel();

  NodeVolumeAssemble(model, m, v, w, volume);
}

template <typename DoubleType>
void Equation<DoubleType>::NodeVolumeAssemble(const std::string &model, dsMath::RealRowColValueVec<DoubleType> &m, dsMath::RHSEntryVec<DoubleType> &v, dsMathEnum::WhatToLoad w, const std::string &node_volume)
{
  const Region &r = GetRegion();

  VariableList_t vlist = r.GetVariableList();

  ConstNodeModelPtr nm = r.GetNodeModel(model);
//    dsAssert(nm != nullptr, "UNEXPECTED");
  if (!nm)
  {
    dsErrors::MissingEquationModel(r, myname, model, dsErrors::ModelInfo::NODE, OutputStream::OutputType::FATAL);
  }

  ConstNodeModelPtr nvol = r.GetNodeModel(node_volume);
//  dsAssert(nvol, "UNEXPECTED");

  if (!nvol)
  {
    dsErrors::MissingEquationModel(r, myname, node_volume, dsErrors::ModelInfo::NODE, OutputStream::OutputType::FATAL);
  }

  if ((w == dsMathEnum::WhatToLoad::RHS) || (w == dsMathEnum::WhatToLoad::MATRIXANDRHS))
  {
    // Integrate with volume and charge
    NodeScalarData<DoubleType> nrhs(*nvol);
    nrhs.times_equal_model(*nm);
    NodeAssembleRHS(v, nrhs);
  }
  else if (w == dsMathEnum::WhatToLoad::MATRIXONLY)
  {
  }
  else
  {
    dsAssert(0, "UNEXPECTED");
  }

  if ((w == dsMathEnum::WhatToLoad::MATRIXONLY) || (w == dsMathEnum::WhatToLoad::MATRIXANDRHS))
  {
    VariableList_t::iterator it = vlist.begin();
    for (; it != vlist.end(); ++it)
    {
      const std::string var(*it);

      std::string dermodel = GetDerivativeModelName(model, var);

      ConstNodeModelPtr ndm = r.GetNodeModel(dermodel);

      if (!ndm)
      {
        dsErrors::MissingEquationModel(r, myname, dermodel, dsErrors::ModelInfo::NODE, OutputStream::OutputType::VERBOSE1);
      }
      else
      {
        NodeScalarData<DoubleType> nder(*nvol);
        nder.times_equal_model(*ndm);
        NodeAssembleJacobian(m, nder, var);
      }
    }
  }
  else if (w == dsMathEnum::WhatToLoad::RHS)
  {
  }
  else
  {
    dsAssert(0, "UNEXPECTED");
  }
}

template <typename DoubleType>
void Equation<DoubleType>::EdgeCoupleAssemble(const std::string &model, dsMath::RealRowColValueVec<DoubleType> &m, dsMath::RHSEntryVec<DoubleType> &v, dsMathEnum::WhatToLoad w)
{
  const std::string &couple = GetRegion().GetEdgeCoupleModel();

  EdgeCoupleAssemble(model, m, v, w, couple, 1.0, -1.0);
}

template <typename DoubleType>
void Equation<DoubleType>::EdgeNodeVolumeAssemble(const std::string &model, dsMath::RealRowColValueVec<DoubleType> &m, dsMath::RHSEntryVec<DoubleType> &v, dsMathEnum::WhatToLoad w)
{
  const std::string &node0model = GetRegion().GetEdgeNode0VolumeModel();
  const std::string &node1model = GetRegion().GetEdgeNode1VolumeModel();

  if (node0model == node1model)
  {
    EdgeCoupleAssemble(model, m, v, w, node0model, 1.0, 1.0);
  }
  else
  {
    /// necessary for cylindrical coordinate system in 2d
    EdgeCoupleAssemble(model, m, v, w, node0model, 1.0, 0.0);
    EdgeCoupleAssemble(model, m, v, w, node1model, 0.0, 1.0);
  }
}


//// Handles the case for Unsymmetric Jacobian Assembly
template <typename DoubleType>
void Equation<DoubleType>::EdgeCoupleAssemble(const std::string &model, dsMath::RealRowColValueVec<DoubleType> &m, dsMath::RHSEntryVec<DoubleType> &v, dsMathEnum::WhatToLoad w, const std::string &edge_couple, const DoubleType n0_sign, const DoubleType n1_sign)
{
    const Region &r = GetRegion();

    VariableList_t vlist = r.GetVariableList();

    ConstEdgeModelPtr ef = r.GetEdgeModel(model);
    if (!ef)
    {
      dsErrors::MissingEquationModel(r, myname, model, dsErrors::ModelInfo::EDGE, OutputStream::OutputType::FATAL);
    }

    ConstEdgeModelPtr ec = r.GetEdgeModel(edge_couple);
    if (!ec)
    {
      dsErrors::MissingEquationModel(r, myname, edge_couple, dsErrors::ModelInfo::EDGE, OutputStream::OutputType::FATAL);
    }

    if ((w == dsMathEnum::WhatToLoad::RHS) || (w == dsMathEnum::WhatToLoad::MATRIXANDRHS))
    {
        EdgeScalarData<DoubleType> eflux = EdgeScalarData<DoubleType>(*ef);
        eflux.times_equal_model(*ec);
        EdgeAssembleRHS(v, eflux, n0_sign, n1_sign);
    }
    else if (w == dsMathEnum::WhatToLoad::MATRIXONLY)
    {
    }
    else
    {
      dsAssert(0, "UNEXPECTED");
    }

    if ((w == dsMathEnum::WhatToLoad::MATRIXONLY) || (w == dsMathEnum::WhatToLoad::MATRIXANDRHS))
    {
      VariableList_t::iterator it = vlist.begin();
      for ( ; it != vlist.end(); ++it)
      {
        const std::string var(*it);

        std::string dermodel0 = GetDerivativeModelName(model, var);
        std::string dermodel1 = dermodel0;

        //// Ultimately this needs to be a function too
        dermodel0 += "@n0";
        dermodel1 += "@n1";

        ConstEdgeModelPtr em0 = r.GetEdgeModel(dermodel0);
        ConstEdgeModelPtr em1 = r.GetEdgeModel(dermodel1);

        if ((!em0) && (!em1))
        {
          dsErrors::MissingEquationModel(r, myname, dermodel0, dsErrors::ModelInfo::EDGE, OutputStream::OutputType::VERBOSE1);
          dsErrors::MissingEquationModel(r, myname, dermodel1, dsErrors::ModelInfo::EDGE, OutputStream::OutputType::VERBOSE1);
        }
        else
        {
            /// Both models must exist
//              dsAssert(em0, "UNEXPECTED");
//              dsAssert(em1, "UNEXPECTED");
          if (!em0)
          {
            dsErrors::MissingEquationModel(r, myname, dermodel0, dsErrors::ModelInfo::EDGE, OutputStream::OutputType::FATAL);
          }

          if (!em1)
          {
            dsErrors::MissingEquationModel(r, myname, dermodel1, dsErrors::ModelInfo::EDGE, OutputStream::OutputType::FATAL);
          }

          EdgeScalarData<DoubleType> eder0 = EdgeScalarData<DoubleType>(*em0);
          EdgeScalarData<DoubleType> eder1 = EdgeScalarData<DoubleType>(*em1);

          eder0.times_equal_model(*ec); /// integrate wrt volume
          eder1.times_equal_model(*ec); /// integrate wrt volume

          UnSymmetricEdgeAssembleJacobian(m, eder0, eder1, var, n0_sign, n1_sign);
        }
      }
    }
    else if (w == dsMathEnum::WhatToLoad::RHS)
    {
    }
    else
    {
      dsAssert(0, "UNEXPECTED");
    }
}

template <typename DoubleType>
void Equation<DoubleType>::TriangleEdgeCoupleAssemble(const std::string &model, dsMath::RealRowColValueVec<DoubleType> &m, dsMath::RHSEntryVec<DoubleType> &v, dsMathEnum::WhatToLoad w, const std::string &edge_couple, const DoubleType n0_sign, const DoubleType n1_sign)
{
  const Region &r = GetRegion();

  VariableList_t vlist = r.GetVariableList();

  ConstTriangleEdgeModelPtr ef = r.GetTriangleEdgeModel(model);
  if (!ef)
  {
    dsErrors::MissingEquationModel(r, myname, model, dsErrors::ModelInfo::ELEMENTEDGE, OutputStream::OutputType::FATAL);
  }
//    dsAssert(ef != nullptr, "UNEXPECTED");

  ConstTriangleEdgeModelPtr ec = r.GetTriangleEdgeModel(edge_couple);
  if (!ec)
  {
    dsErrors::MissingEquationModel(r, myname, edge_couple, dsErrors::ModelInfo::ELEMENTEDGE, OutputStream::OutputType::FATAL);
  }
//    dsAssert(ec != nullptr, "UNEXPECTED");

  if ((w == dsMathEnum::WhatToLoad::RHS) || (w == dsMathEnum::WhatToLoad::MATRIXANDRHS))
  {
    TriangleEdgeScalarData<DoubleType> eflux = TriangleEdgeScalarData<DoubleType>(*ef);
    eflux.times_equal_model(*ec);
    TriangleEdgeAssembleRHS(v, eflux, n0_sign, n1_sign);
  }
  else if (w == dsMathEnum::WhatToLoad::MATRIXONLY)
  {
  }
  else
  {
    dsAssert(0, "UNEXPECTED");
  }

  if ((w == dsMathEnum::WhatToLoad::MATRIXONLY) || (w == dsMathEnum::WhatToLoad::MATRIXANDRHS))
  {
    VariableList_t::iterator it = vlist.begin();
    for ( ; it != vlist.end(); ++it)
    {
      const std::string var(*it);

      std::string dermodel0 = GetDerivativeModelName(model, var);
      std::string dermodel1 = dermodel0;
      std::string dermodel2 = dermodel0;

      //// Ultimately this needs to be a function too
      dermodel0 += "@en0";
      dermodel1 += "@en1";
      dermodel2 += "@en2";

      ConstTriangleEdgeModelPtr em0 = r.GetTriangleEdgeModel(dermodel0);
      ConstTriangleEdgeModelPtr em1 = r.GetTriangleEdgeModel(dermodel1);
      ConstTriangleEdgeModelPtr em2 = r.GetTriangleEdgeModel(dermodel2);

      if ((!em0) && (!em1) && (!em2))
      {
        dsErrors::MissingEquationModel(r, myname, dermodel0, dsErrors::ModelInfo::ELEMENTEDGE, OutputStream::OutputType::VERBOSE1);
        dsErrors::MissingEquationModel(r, myname, dermodel1, dsErrors::ModelInfo::ELEMENTEDGE, OutputStream::OutputType::VERBOSE1);
        dsErrors::MissingEquationModel(r, myname, dermodel2, dsErrors::ModelInfo::ELEMENTEDGE, OutputStream::OutputType::VERBOSE1);
      }
      else
      {
        if (!em0)
        {
          dsErrors::MissingEquationModel(r, myname, dermodel0, dsErrors::ModelInfo::ELEMENTEDGE, OutputStream::OutputType::FATAL);
        }
        if (!em1)
        {
          dsErrors::MissingEquationModel(r, myname, dermodel1, dsErrors::ModelInfo::ELEMENTEDGE, OutputStream::OutputType::FATAL);
        }
        if (!em2)
        {
          dsErrors::MissingEquationModel(r, myname, dermodel2, dsErrors::ModelInfo::ELEMENTEDGE, OutputStream::OutputType::FATAL);
        }

        TriangleEdgeScalarData<DoubleType> eder0 = TriangleEdgeScalarData<DoubleType>(*em0);
        TriangleEdgeScalarData<DoubleType> eder1 = TriangleEdgeScalarData<DoubleType>(*em1);
        TriangleEdgeScalarData<DoubleType> eder2 = TriangleEdgeScalarData<DoubleType>(*em2);

        eder0.times_equal_model(*ec); /// integrate wrt volume
        eder1.times_equal_model(*ec); /// integrate wrt volume
        eder2.times_equal_model(*ec); /// integrate wrt volume

        UnSymmetricTriangleEdgeAssembleJacobian(m, eder0, eder1, eder2, var, n0_sign, n1_sign);
      }
    }
  }
  else if (w == dsMathEnum::WhatToLoad::RHS)
  {
  }
  else
  {
    dsAssert(0, "UNEXPECTED");
  }
}

template <typename DoubleType>
void Equation<DoubleType>::TetrahedronEdgeCoupleAssemble(const std::string &model, dsMath::RealRowColValueVec<DoubleType> &m, dsMath::RHSEntryVec<DoubleType> &v, dsMathEnum::WhatToLoad w, const std::string &edge_couple, const DoubleType n0_sign, const DoubleType n1_sign)
{
  const Region &r = GetRegion();

  VariableList_t vlist = r.GetVariableList();

  ConstTetrahedronEdgeModelPtr ef = r.GetTetrahedronEdgeModel(model);
  if (!ef)
  {
    dsErrors::MissingEquationModel(r, myname, model, dsErrors::ModelInfo::ELEMENTEDGE, OutputStream::OutputType::FATAL);
  }
//  dsAssert(ef != nullptr, "UNEXPECTED");

  ConstTetrahedronEdgeModelPtr ec = r.GetTetrahedronEdgeModel(edge_couple);
  if (!ec)
  {
    dsErrors::MissingEquationModel(r, myname, edge_couple, dsErrors::ModelInfo::ELEMENTEDGE, OutputStream::OutputType::FATAL);
  }
//    dsAssert(ec != nullptr, "UNEXPECTED");

  if ((w == dsMathEnum::WhatToLoad::RHS) || (w == dsMathEnum::WhatToLoad::MATRIXANDRHS))
  {
    TetrahedronEdgeScalarData<DoubleType> eflux = TetrahedronEdgeScalarData<DoubleType>(*ef);
    eflux.times_equal_model(*ec);
    TetrahedronEdgeAssembleRHS(v, eflux, n0_sign, n1_sign);
  }
  else if (w == dsMathEnum::WhatToLoad::MATRIXONLY)
  {
  }
  else
  {
    dsAssert(0, "UNEXPECTED");
  }

  if ((w == dsMathEnum::WhatToLoad::MATRIXONLY) || (w == dsMathEnum::WhatToLoad::MATRIXANDRHS))
  {
    VariableList_t::iterator it = vlist.begin();
    for ( ; it != vlist.end(); ++it)
    {
      const std::string var(*it);

      std::string dermodel0 = GetDerivativeModelName(model, var);
      std::string dermodel1 = dermodel0;
      std::string dermodel2 = dermodel0;
      std::string dermodel3 = dermodel0;

      //// Ultimately this needs to be a function too
      dermodel0 += "@en0";
      dermodel1 += "@en1";
      dermodel2 += "@en2";
      dermodel3 += "@en3";

      ConstTetrahedronEdgeModelPtr em0 = r.GetTetrahedronEdgeModel(dermodel0);
      ConstTetrahedronEdgeModelPtr em1 = r.GetTetrahedronEdgeModel(dermodel1);
      ConstTetrahedronEdgeModelPtr em2 = r.GetTetrahedronEdgeModel(dermodel2);
      ConstTetrahedronEdgeModelPtr em3 = r.GetTetrahedronEdgeModel(dermodel3);

      if ((!em0) && (!em1) && (!em2) && (!em3))
      {
        dsErrors::MissingEquationModel(r, myname, dermodel0, dsErrors::ModelInfo::ELEMENTEDGE, OutputStream::OutputType::VERBOSE1);
        dsErrors::MissingEquationModel(r, myname, dermodel1, dsErrors::ModelInfo::ELEMENTEDGE, OutputStream::OutputType::VERBOSE1);
        dsErrors::MissingEquationModel(r, myname, dermodel2, dsErrors::ModelInfo::ELEMENTEDGE, OutputStream::OutputType::VERBOSE1);
        dsErrors::MissingEquationModel(r, myname, dermodel3, dsErrors::ModelInfo::ELEMENTEDGE, OutputStream::OutputType::VERBOSE1);
      }
      else
      {
        if (!em0)
        {
          dsErrors::MissingEquationModel(r, myname, dermodel0, dsErrors::ModelInfo::ELEMENTEDGE, OutputStream::OutputType::FATAL);
        }
        if (!em1)
        {
          dsErrors::MissingEquationModel(r, myname, dermodel1, dsErrors::ModelInfo::ELEMENTEDGE, OutputStream::OutputType::FATAL);
        }
        if (!em2)
        {
          dsErrors::MissingEquationModel(r, myname, dermodel2, dsErrors::ModelInfo::ELEMENTEDGE, OutputStream::OutputType::FATAL);
        }
        if (!em3)
        {
          dsErrors::MissingEquationModel(r, myname, dermodel3, dsErrors::ModelInfo::ELEMENTEDGE, OutputStream::OutputType::FATAL);
        }

        TetrahedronEdgeScalarData<DoubleType> eder0 = TetrahedronEdgeScalarData<DoubleType>(*em0);
        TetrahedronEdgeScalarData<DoubleType> eder1 = TetrahedronEdgeScalarData<DoubleType>(*em1);
        TetrahedronEdgeScalarData<DoubleType> eder2 = TetrahedronEdgeScalarData<DoubleType>(*em2);
        TetrahedronEdgeScalarData<DoubleType> eder3 = TetrahedronEdgeScalarData<DoubleType>(*em3);

        eder0.times_equal_model(*ec); /// integrate wrt volume
        eder1.times_equal_model(*ec); /// integrate wrt volume
        eder2.times_equal_model(*ec); /// integrate wrt volume
        eder3.times_equal_model(*ec); /// integrate wrt volume

        UnSymmetricTetrahedronEdgeAssembleJacobian(m, eder0, eder1, eder2, eder3, var, n0_sign, n1_sign);
      }
    }
  }
  else if (w == dsMathEnum::WhatToLoad::RHS)
  {
  }
  else
  {
      dsAssert(0, "UNEXPECTED");
  }
}

template <typename DoubleType>
void Equation<DoubleType>::ElementEdgeCoupleAssemble(const std::string &model, dsMath::RealRowColValueVec<DoubleType> &m, dsMath::RHSEntryVec<DoubleType> &v, dsMathEnum::WhatToLoad w)
{

  const std::string &couple = GetRegion().GetElementEdgeCoupleModel();

  const size_t dimension = GetRegion().GetDimension();
  if (dimension == 2)
  {
    TriangleEdgeCoupleAssemble(model, m, v, w, couple, 1.0, -1.0);
  }
  else if (dimension == 3)
  {
    TetrahedronEdgeCoupleAssemble(model, m, v, w, couple, 1.0, -1.0);
  }
}

template <typename DoubleType>
void Equation<DoubleType>::ElementNodeVolumeAssemble(const std::string &model, dsMath::RealRowColValueVec<DoubleType> &m, dsMath::RHSEntryVec<DoubleType> &v, dsMathEnum::WhatToLoad w)
{
  const size_t dimension = GetRegion().GetDimension();

  const std::string &node0model = GetRegion().GetElementNode0VolumeModel();
  const std::string &node1model = GetRegion().GetElementNode1VolumeModel();

  if (dimension == 2)
  {
    if (node0model == node1model)
    {
      TriangleEdgeCoupleAssemble(model, m, v, w, node0model, 1.0, 1.0);
    }
    else
    {
      /// necessary for cylindrical coordinate system in 2d
      TriangleEdgeCoupleAssemble(model, m, v, w, node0model, 1.0, 0.0);
      TriangleEdgeCoupleAssemble(model, m, v, w, node1model, 0.0, 1.0);
    }
  }
  else if (dimension == 3)
  {
    if (node0model == node1model)
    {
      TetrahedronEdgeCoupleAssemble(model, m, v, w, node0model, 1.0, 1.0);
    }
    else
    {
      TetrahedronEdgeCoupleAssemble(model, m, v, w, node0model, 1.0, 0.0);
      TetrahedronEdgeCoupleAssemble(model, m, v, w, node1model, 0.0, 1.0);
    }
  }
}

template <typename DoubleType>
std::string Equation<DoubleType>::GetDerivativeModelName(const std::string &model, const std::string &var)
{
  std::string ret = model;
  ret += ":";
  ret += var;
  return ret;
}

template <typename DoubleType>
void Equation<DoubleType>::DevsimSerialize(std::ostream &of) const
{
  of << "begin_equation \"" << this->GetName() << "\"\n";
  this->Serialize(of);
  of << "\nend_equation\n\n";
}

template <typename DoubleType>
void Equation<DoubleType>::GetCommandOptions(std::map<std::string, ObjectHolder> &omap) const
{
  this->GetCommandOptions_Impl(omap);
}

template class Equation<double>;
#ifdef DEVSIM_EXTENDED_PRECISION
#include "Float128.hh"
template class Equation<float128>;
#endif

