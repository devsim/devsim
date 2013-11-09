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

#include "Equation.hh"
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

#include <cmath>

const double Equation::defminError = 1.0e-10;

//// TODO: make this parameter based
void Equation::setMinError(double m)
{
    minError = m;
}

double Equation::GetMinError() const
{
    return minError;
}

void Equation::setAbsError(double a)
{
    absError = a;
}

void Equation::setRelError(double r)
{
    relError = r;
}

double Equation::GetAbsError() const
{
    return absError;
}

double Equation::GetRelError() const
{
    return relError;
}

Equation::Equation(const std::string &nm, RegionPtr rp, const std::string &var, UpdateType ut)
    : myname(nm), myregion(rp), variable(var), absError(0.0), relError(0.0), minError(defminError), updateType(ut)
{
    rp->AddEquation(this);
}

Equation::~Equation()
{
}

// At this level, we can probably control which row we wish to assemble
void Equation::Assemble(dsMath::RealRowColValueVec &m, dsMath::RHSEntryVec &v, dsMathEnum::WhatToLoad w, dsMathEnum::TimeMode t)
{
    // get noncontact node list from Region
    // assemble each row individually
    // or better yet send exclusion list of rows off to the matrix class
    DerivedAssemble(m, v, w, t);
}

void Equation::Update(NodeModel &nm, const std::vector<double> &rhs)
{
    UpdateValues(nm, rhs);
}

void Equation::ACUpdate(NodeModel &nm, const std::vector<std::complex<double> > &rhs)
{
    ACUpdateValues(nm, rhs);
}

void Equation::NoiseUpdate(const std::string &nm, const std::vector<size_t> &permvec, const std::vector<std::complex<double> > &rhs)
{
    NoiseUpdateValues(nm, permvec, rhs);
}

void Equation::LogSolutionUpdate(const NodeScalarList &ovals, NodeScalarList &upds, NodeScalarList &nvals)
{
  for (size_t i = 0; i < ovals.size(); ++i)
  {
      double upd = upds[i];
      if ( fabs(upd) > 0.0259 )
      {
          const double sign = (upd > 0.0) ? 1.0 : -1.0;
          upd = sign*0.0259*log(1+fabs(upd)/0.0259);
      }

      const double oval = ovals[i];

      const double nval = upd + oval;

      upds[i] = upd;

      nvals[i] = nval;
      
  }
}

void Equation::PositiveSolutionUpdate(const NodeScalarList &ovals, NodeScalarList &upds, NodeScalarList &nvals)
{
  for (size_t i = 0; i < ovals.size(); ++i)
  {
      const double oval = ovals[i];

      double upd = upds[i];

      double nval = upd + oval;

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
        dsErrors::SolutionVariableNonPositive(reg, myname, GetVariable(), oval, OutputStream::FATAL);
      }

      upds[i] = upd;
      nvals[i] = nval;
  }
}

void Equation::DefaultSolutionUpdate(const NodeScalarList &ovals, NodeScalarList &upds, NodeScalarList &nvals)
{
  for (size_t i = 0; i < ovals.size(); ++i)
  {
      const double upd = upds[i];

      const double oval = ovals[i];

      const double nval = upd + oval;

      nvals[i] = nval;
  }
}

void Equation::DefaultUpdate(NodeModel &nm, const std::vector<double> &result)
{
    const Region &reg = *myregion;

    const size_t ind = reg.GetEquationIndex(myname);
//    dsAssert(ind != size_t(-1), "UNEXPECTED");
    if (ind == size_t(-1))
    {
      dsErrors::MissingEquationIndex(reg, myname, "", OutputStream::FATAL) ;
    }

    const ConstNodeList &nl = myregion->GetNodeList();

    ConstNodeList::const_iterator nit = nl.begin();
    const ConstNodeList::const_iterator nend = nl.end();

    const NodeScalarList &ovals = nm.GetScalarValues();
    NodeScalarList upds(ovals.size());

    for ( ; nit != nend; ++nit)
    {
        const size_t eqrow     = reg.GetEquationNumber(ind, *nit);
        const size_t nodeindex = (*nit)->GetIndex();
        upds[nodeindex]  = result[eqrow];
    }

    NodeScalarList nvals(ovals.size());

    /// Could probably pass this as a functor class in the future
    if (updateType == LOGDAMP)
    {
      LogSolutionUpdate(ovals, upds, nvals);
    }
    else if (updateType == POSITIVE)
    {
      PositiveSolutionUpdate(ovals, upds, nvals);
    }
    else if (updateType == DEFAULT)
    {
      DefaultSolutionUpdate(ovals, upds, nvals);
    }
    else
    {
      dsAssert(0, "UNEXPECTED");
    }

    nm.SetValues(nvals);

    double aerr = 0.0;
    double rerr = 0.0;

    for (size_t i = 0; i < upds.size(); ++i)
    {
        const double n1 = fabs(upds[i]);

        if (n1 > aerr)
        {
          aerr = n1;
        }
//      aerr += n1;

        const double n2 = fabs(nvals[i]);

        const double nrerror =  n1 / (n2 + minError); 

        if (nrerror > rerr)
        {
            rerr = nrerror;
        }

    }

    setAbsError(aerr);
    setRelError(rerr);
//    dsErrors::EquationMathErrorInfo(*this, rerr, aerr, OutputStream::INFO);
}

//// TODO: Make sure noise and ac kept in sync
void Equation::DefaultACUpdate(NodeModel &nm, const std::vector<std::complex<double> > &result)
{
    const std::string &realnodemodel = nm.GetRealName();
    const std::string &imagnodemodel = nm.GetImagName();

    const size_t ind = myregion->GetEquationIndex(myname);
//    dsAssert(ind != size_t(-1), "UNEXPECTED");
    if (ind == size_t(-1))
    {
      dsErrors::MissingEquationIndex(*myregion, myname, "", OutputStream::FATAL) ;
    }

    const ConstNodeList &nl = myregion->GetNodeList();

    NodeModelPtr rnm = std::const_pointer_cast<NodeModel, const NodeModel>(myregion->GetNodeModel(realnodemodel));
    NodeModelPtr inm = std::const_pointer_cast<NodeModel, const NodeModel>(myregion->GetNodeModel(imagnodemodel));

    if (!rnm)
    {
        dsErrors::CreateModelOnRegion(*myregion, realnodemodel, OutputStream::INFO);
        rnm = NodeSolution::CreateNodeSolution(realnodemodel, myregion);
    }

    if (!inm)
    {
        dsErrors::CreateModelOnRegion(*myregion, imagnodemodel, OutputStream::INFO);
        inm = NodeSolution::CreateNodeSolution(imagnodemodel, myregion);
    }

    NodeScalarList realout(nl.size());
    NodeScalarList imagout(nl.size());

    ConstNodeList::const_iterator nit = nl.begin();
    const ConstNodeList::const_iterator nend = nl.end();
    for ( ; nit != nend; ++nit)
    {
        const size_t eqindex = (*nit)->GetIndex();
        const size_t eqrow = myregion->GetEquationNumber(ind, *nit);

        const std::complex<double> upd  = result[eqrow];

        realout[eqindex] = upd.real();
        imagout[eqindex] = upd.imag();
    }

    rnm->SetValues(realout);
    inm->SetValues(imagout);
}

void Equation::DefaultNoiseUpdate(const std::string &outputname, const std::vector<size_t> &permvec, const std::vector<std::complex<double> > &result)
{
    const std::string &realnodemodel = GetNoiseRealName(outputname);
    const std::string &imagnodemodel = GetNoiseImagName(outputname);

    const size_t ind = myregion->GetEquationIndex(myname);
//    dsAssert(ind != size_t(-1), "UNEXPECTED");
    if (ind == size_t(-1))
    {
      dsErrors::MissingEquationIndex(*myregion, myname, "", OutputStream::FATAL) ;
    }

    const ConstNodeList &nl = myregion->GetNodeList();

    NodeModelPtr rnm = std::const_pointer_cast<NodeModel, const NodeModel>(myregion->GetNodeModel(realnodemodel));
    NodeModelPtr inm = std::const_pointer_cast<NodeModel, const NodeModel>(myregion->GetNodeModel(imagnodemodel));

    if (!rnm)
    {
        dsErrors::CreateModelOnRegion(*myregion, realnodemodel, OutputStream::INFO);
        rnm = NodeSolution::CreateNodeSolution(realnodemodel, myregion);
    }

    if (!inm)
    {
        dsErrors::CreateModelOnRegion(*myregion, imagnodemodel, OutputStream::INFO);
        inm = NodeSolution::CreateNodeSolution(imagnodemodel, myregion);
    }

    NodeScalarList realout(nl.size());
    NodeScalarList imagout(nl.size());

    ConstNodeList::const_iterator nit = nl.begin();
    const ConstNodeList::const_iterator nend = nl.end();
    for ( ; nit != nend; ++nit)
    {
        const size_t eqrow = permvec[myregion->GetEquationNumber(ind, *nit)];

        if (eqrow != size_t(-1))
        {
          const size_t eqindex = (*nit)->GetIndex();
          const std::complex<double> upd  = result[eqrow];
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
      new VectorGradient(myregion, realnodemodel, VectorGradient::AVOIDZERO);
    }
    if (!myregion->GetNodeModel(xiname))
    {
      new VectorGradient(myregion, imagnodemodel, VectorGradient::AVOIDZERO);
    }
}

void Equation::EdgeAssembleRHS(dsMath::RHSEntryVec &v, const EdgeScalarData &eflux)
{
    const Region &r = GetRegion();
    const size_t eqindex0 = r.GetEquationIndex(myname);
//    dsAssert(eqindex0 != size_t(-1), "UNEXPECTED");
    if (eqindex0 == size_t(-1))
    {
      dsErrors::MissingEquationIndex(r, myname, "", OutputStream::FATAL) ;
    }

    const ConstEdgeList &el = r.GetEdgeList();
    for (size_t i = 0 ; i < el.size(); ++i)
    {
        const ConstNodeList &nl = el[i]->GetNodeList();
        const size_t row0 = r.GetEquationNumber(eqindex0, nl[0]);
        const size_t row1 = r.GetEquationNumber(eqindex0, nl[1]);

        const double rhsval = eflux[i];

        v.push_back(std::make_pair(row0,  rhsval));
        v.push_back(std::make_pair(row1, -rhsval));
    }
}

/// should have one assembly routine per shape
/// Other variants should handle case if ElementNodeVolume is used on 3d element
void Equation::TriangleEdgeAssembleRHS(dsMath::RHSEntryVec &v, const TriangleEdgeScalarData &teflux, const double n0_sign, const double n1_sign)
{
  const Region &r = GetRegion();
  const size_t eqindex0 = r.GetEquationIndex(myname);
//  dsAssert(eqindex0 != size_t(-1), "UNEXPECTED");
  if (eqindex0 == size_t(-1))
  {
    dsErrors::MissingEquationIndex(r, myname, "", OutputStream::FATAL) ;
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

      const double rhsval = teflux[3*i + j];

      v.push_back(std::make_pair(row0,  n0_sign * rhsval));
      v.push_back(std::make_pair(row1,  n1_sign * rhsval));
    }
  }
}

void Equation::TetrahedronEdgeAssembleRHS(dsMath::RHSEntryVec &v, const TetrahedronEdgeScalarData &teflux, const double n0_sign, const double n1_sign)
{
  const Region &r = GetRegion();
  const size_t eqindex0 = r.GetEquationIndex(myname);
//  dsAssert(eqindex0 != size_t(-1), "UNEXPECTED");
  if (eqindex0 == size_t(-1))
  {
    dsErrors::MissingEquationIndex(r, myname, "", OutputStream::FATAL) ;
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

      const double rhsval = teflux[6*i + j];

      v.push_back(std::make_pair(row0,  n0_sign * rhsval));
      v.push_back(std::make_pair(row1,  n1_sign * rhsval));
    }
  }
}

std::string Equation::GetNoiseRealName(const std::string &outname)
{
  std::string out(outname); 
  out += "_";
  out += myname;
  out += "_";
  out += "real";
  return out;
}

std::string Equation::GetNoiseImagName(const std::string &outname)
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
void Equation::SymmetricEdgeAssembleJacobian(dsMath::RealRowColValueVec &m, const EdgeScalarData &eder, const std::string &var)
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
        const double ederval = eder[i];
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
void Equation::UnSymmetricEdgeAssembleJacobian(dsMath::RealRowColValueVec &m, const EdgeScalarData &eder0, const EdgeScalarData &eder1, const std::string &var)
{
    const Region &r = GetRegion();
    const size_t eqindex0 = r.GetEquationIndex(myname);
//    dsAssert(eqindex0 != size_t(-1), "UNEXPECTED");
    if (eqindex0 == size_t(-1))
    {
      dsErrors::MissingEquationIndex(r, myname, "", OutputStream::FATAL) ;
    }

    const size_t eqindex1 = r.GetEquationIndex(r.GetEquationNameFromVariable(var));
//    dsAssert(eqindex1 != size_t(-1), "UNEXPECTED");
    if (eqindex1 == size_t(-1))
    {
      dsErrors::MissingEquationIndex(r, myname, var, OutputStream::FATAL) ;
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

        const double ederval0 = eder0[i];
        const double ederval1 = eder1[i];

        /// Here we account for the fact stuff moving toward row1 has opposite sign
        m.push_back(dsMath::RealRowColVal(row0, col0, ederval0));
        m.push_back(dsMath::RealRowColVal(row1, col1, -ederval1));
        m.push_back(dsMath::RealRowColVal(row0, col1, ederval1));
        m.push_back(dsMath::RealRowColVal(row1, col0, -ederval0));
    }
}

/// Inserts the entries into the matrix for the case when the sensitivity is different with respect to the same variable at the nodes opposite of the edge
/// eder0 is derivative wrt first node
/// eder1 is derivative wrt second node
/// eder2 is derivative wrt third node
void Equation::UnSymmetricTriangleEdgeAssembleJacobian(dsMath::RealRowColValueVec &m, const TriangleEdgeScalarData &eder0, const TriangleEdgeScalarData &eder1, const TriangleEdgeScalarData &eder2, const std::string &var, const double n0_sign, const double n1_sign)
{
  const Region &r = GetRegion();
  const size_t eqindex0 = r.GetEquationIndex(myname);
//  dsAssert(eqindex0 != size_t(-1), "UNEXPECTED");
  if (eqindex0 == size_t(-1))
  {
    dsErrors::MissingEquationIndex(r, myname, "", OutputStream::FATAL) ;
  }

  const size_t eqindex1 = r.GetEquationIndex(r.GetEquationNameFromVariable(var));
//  dsAssert(eqindex1 != size_t(-1), "UNEXPECTED");
  if (eqindex1 == size_t(-1))
  {
    dsErrors::MissingEquationIndex(r, myname, var, OutputStream::FATAL) ;
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
      const double ederval0 = eder0[eindex];
      const double ederval1 = eder1[eindex];
      const double ederval2 = eder2[eindex];

      /// Here we account for the fact stuff moving toward row1 has opposite sign
      m.push_back(dsMath::RealRowColVal(row0, col0,  n0_sign * ederval0));
      m.push_back(dsMath::RealRowColVal(row1, col1,  n1_sign * ederval1));
      m.push_back(dsMath::RealRowColVal(row0, col1,  n0_sign * ederval1));
      m.push_back(dsMath::RealRowColVal(row1, col0,  n1_sign * ederval0));

      /// This is true as long as we are projected along the unit vector
      m.push_back(dsMath::RealRowColVal(row0, col2,  n0_sign * ederval2));
      m.push_back(dsMath::RealRowColVal(row1, col2,  n1_sign * ederval2));
    }
  }
}

void Equation::UnSymmetricTetrahedronEdgeAssembleJacobian(dsMath::RealRowColValueVec &m, const TetrahedronEdgeScalarData &eder0, const TetrahedronEdgeScalarData &eder1, const TetrahedronEdgeScalarData &eder2, const TetrahedronEdgeScalarData &eder3, const std::string &var, const double n0_sign, const double n1_sign)
{
  const Region &r = GetRegion();
  const size_t eqindex0 = r.GetEquationIndex(myname);
//  dsAssert(eqindex0 != size_t(-1), "UNEXPECTED");
  if (eqindex0 == size_t(-1))
  {
    dsErrors::MissingEquationIndex(r, myname, "", OutputStream::FATAL) ;
  }

  const size_t eqindex1 = r.GetEquationIndex(r.GetEquationNameFromVariable(var));
//  dsAssert(eqindex1 != size_t(-1), "UNEXPECTED");
  if (eqindex1 == size_t(-1))
  {
    dsErrors::MissingEquationIndex(r, myname, var, OutputStream::FATAL) ;
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
      const double ederval0 = eder0[eindex];
      const double ederval1 = eder1[eindex];
      const double ederval2 = eder2[eindex];
      const double ederval3 = eder3[eindex];

      /// Here we account for the fact stuff moving toward row1 has opposite sign
      m.push_back(dsMath::RealRowColVal(row0, col0,  n0_sign * ederval0));
      m.push_back(dsMath::RealRowColVal(row1, col1,  n1_sign * ederval1));
      m.push_back(dsMath::RealRowColVal(row0, col1,  n0_sign * ederval1));
      m.push_back(dsMath::RealRowColVal(row1, col0,  n1_sign * ederval0));

      /// This is true as long as we are projected along the unit vector
      m.push_back(dsMath::RealRowColVal(row0, col2,  n0_sign * ederval2));
      m.push_back(dsMath::RealRowColVal(row1, col2,  n1_sign * ederval2));
      m.push_back(dsMath::RealRowColVal(row0, col3,  n0_sign * ederval3));
      m.push_back(dsMath::RealRowColVal(row1, col3,  n1_sign * ederval3));
    }
  }
}

/// Not scaled by node volume
void Equation::NodeAssembleRHS(dsMath::RHSEntryVec &v, const NodeScalarData &nrhs)
{
    // var is the name of the variable we are differentiating with respect to.  It is therefore the column and our equation is the row

    const Region &r = GetRegion();
    const size_t eqindex0 = r.GetEquationIndex(myname);

//    dsAssert(eqindex0 != size_t(-1), "UNEXPECTED");
    if (eqindex0 == size_t(-1))
    {
      dsErrors::MissingEquationIndex(r, myname, "", OutputStream::FATAL) ;
    }

    const ConstNodeList &nl = r.GetNodeList();
    for (size_t i = 0; i < nl.size(); ++i)
    {
        const size_t row1 = r.GetEquationNumber(eqindex0, nl[i]);
        const double rhsval = nrhs.GetScalarList()[i];
        // Note that the sign is reversed
        v.push_back(std::make_pair(row1, rhsval));
    }
}

void Equation::NodeAssembleJacobian(dsMath::RealRowColValueVec &m, const NodeScalarData &nder, const std::string &var)
{
    // var is the name of the variable we are differentiating with respect to.  It is therefore the column and our equation is the row

    const Region &r = GetRegion();
    const size_t eqindex0 = r.GetEquationIndex(myname);
//    dsAssert(eqindex0 != size_t(-1), "UNEXPECTED");

    if (eqindex0 == size_t(-1))
    {
      dsErrors::MissingEquationIndex(r, myname, "", OutputStream::FATAL) ;
    }

    const size_t eqindex1 = r.GetEquationIndex(r.GetEquationNameFromVariable(var));
//    dsAssert(eqindex1 != size_t(-1), "UNEXPECTED");

    if (eqindex1 == size_t(-1))
    {
      dsErrors::MissingEquationIndex(r, myname, var, OutputStream::FATAL) ;
    }

    const ConstNodeList &nl = r.GetNodeList();
    for (size_t i = 0; i < nl.size(); ++i)
    {
        const size_t row1 = r.GetEquationNumber(eqindex0, nl[i]);
        const size_t row2 = r.GetEquationNumber(eqindex1, nl[i]);

        const double derval = nder.GetScalarList()[i];
        m.push_back(dsMath::RealRowColVal(row1, row2, derval));
    }
}

void Equation::NodeVolumeAssemble(const std::string &model, dsMath::RealRowColValueVec &m, dsMath::RHSEntryVec &v, dsMathEnum::WhatToLoad w)
{
  const std::string &volume = GetRegion().GetNodeVolumeModel();

  NodeVolumeAssemble(model, m, v, w, volume);
}

void Equation::NodeVolumeAssemble(const std::string &model, dsMath::RealRowColValueVec &m, dsMath::RHSEntryVec &v, dsMathEnum::WhatToLoad w, const std::string &node_volume)
{
  const Region &r = GetRegion();

  VariableList_t vlist = r.GetVariableList();

  ConstNodeModelPtr nm = r.GetNodeModel(model);
//    dsAssert(nm != NULL, "UNEXPECTED");
  if (!nm)
  {
    dsErrors::MissingEquationModel(r, myname, model, dsErrors::ModelInfo::NODE, OutputStream::FATAL);
  }

  ConstNodeModelPtr nvol = r.GetNodeModel(node_volume);
//  dsAssert(nvol, "UNEXPECTED");

  if (!nvol)
  {
    dsErrors::MissingEquationModel(r, myname, node_volume, dsErrors::ModelInfo::NODE, OutputStream::FATAL);
  }

  if ((w == dsMathEnum::RHS) || (w == dsMathEnum::MATRIXANDRHS))
  {
    // Integrate with volume and charge
    NodeScalarData nrhs(*nvol);
    nrhs *= *nm;
    NodeAssembleRHS(v, nrhs);
  }
  else if (w == dsMathEnum::MATRIXONLY)
  {
  }
  else
  {
    dsAssert(0, "UNEXPECTED");
  }

  if ((w == dsMathEnum::MATRIXONLY) || (w == dsMathEnum::MATRIXANDRHS))
  {
    VariableList_t::iterator it = vlist.begin();
    for (; it != vlist.end(); ++it)
    {
      const std::string var(*it);

      std::string dermodel = GetDerivativeModelName(model, var);

      ConstNodeModelPtr ndm = r.GetNodeModel(dermodel);

      if (!ndm)
      {
        dsErrors::MissingEquationModel(r, myname, dermodel, dsErrors::ModelInfo::NODE, OutputStream::VERBOSE1);
      }
      else
      {
        NodeScalarData nder(*nvol);
        nder *= *ndm;
        NodeAssembleJacobian(m, nder, var);
      }
    }
  }
  else if (w == dsMathEnum::RHS)
  {
  }
  else
  {
    dsAssert(0, "UNEXPECTED");
  }
}

void Equation::EdgeCoupleAssemble(const std::string &model, dsMath::RealRowColValueVec &m, dsMath::RHSEntryVec &v, dsMathEnum::WhatToLoad w)
{
  const std::string &couple = GetRegion().GetEdgeCoupleModel();

  EdgeCoupleAssemble(model, m, v, w, couple);
}

//// Handles the case for Unsymmetric Jacobian Assembly
void Equation::EdgeCoupleAssemble(const std::string &model, dsMath::RealRowColValueVec &m, dsMath::RHSEntryVec &v, dsMathEnum::WhatToLoad w, const std::string &edge_couple)
{
    const Region &r = GetRegion();

    VariableList_t vlist = r.GetVariableList();

    ConstEdgeModelPtr ef = r.GetEdgeModel(model);
    if (!ef)
    {
      dsErrors::MissingEquationModel(r, myname, model, dsErrors::ModelInfo::EDGE, OutputStream::FATAL);
    }

    ConstEdgeModelPtr ec = r.GetEdgeModel(edge_couple);
    if (!ec)
    {
      dsErrors::MissingEquationModel(r, myname, edge_couple, dsErrors::ModelInfo::EDGE, OutputStream::FATAL);
    }

    if ((w == dsMathEnum::RHS) || (w == dsMathEnum::MATRIXANDRHS))
    {
        EdgeScalarData eflux = EdgeScalarData(*ef);
        eflux *= *ec;
        EdgeAssembleRHS(v, eflux);
    }
    else if (w == dsMathEnum::MATRIXONLY)
    {
    }
    else
    {
      dsAssert(0, "UNEXPECTED");
    }

    if ((w == dsMathEnum::MATRIXONLY) || (w == dsMathEnum::MATRIXANDRHS))
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
          dsErrors::MissingEquationModel(r, myname, dermodel0, dsErrors::ModelInfo::EDGE, OutputStream::VERBOSE1);
          dsErrors::MissingEquationModel(r, myname, dermodel1, dsErrors::ModelInfo::EDGE, OutputStream::VERBOSE1);
        }
        else
        {
            /// Both models must exist
//              dsAssert(em0, "UNEXPECTED");
//              dsAssert(em1, "UNEXPECTED");
          if (!em0)
          {
            dsErrors::MissingEquationModel(r, myname, dermodel0, dsErrors::ModelInfo::EDGE, OutputStream::FATAL);
          }

          if (!em1)
          {
            dsErrors::MissingEquationModel(r, myname, dermodel1, dsErrors::ModelInfo::EDGE, OutputStream::FATAL);
          }

          EdgeScalarData eder0 = EdgeScalarData(*em0);
          EdgeScalarData eder1 = EdgeScalarData(*em1);

          eder0 *= *ec; /// integrate wrt volume
          eder1 *= *ec; /// integrate wrt volume

          UnSymmetricEdgeAssembleJacobian(m, eder0, eder1, var);
        }
      }
    }
    else if (w == dsMathEnum::RHS)
    {
    }
    else
    {
      dsAssert(0, "UNEXPECTED");
    }
}

void Equation::TriangleEdgeCoupleAssemble(const std::string &model, dsMath::RealRowColValueVec &m, dsMath::RHSEntryVec &v, dsMathEnum::WhatToLoad w, const std::string &edge_couple, const double n0_sign, const double n1_sign)
{
  const Region &r = GetRegion();

  VariableList_t vlist = r.GetVariableList();

  ConstTriangleEdgeModelPtr ef = r.GetTriangleEdgeModel(model);
  if (!ef)
  {
    dsErrors::MissingEquationModel(r, myname, model, dsErrors::ModelInfo::ELEMENTEDGE, OutputStream::FATAL);
  }
//    dsAssert(ef != NULL, "UNEXPECTED");

  ConstTriangleEdgeModelPtr ec = r.GetTriangleEdgeModel(edge_couple);
  if (!ec)
  {
    dsErrors::MissingEquationModel(r, myname, edge_couple, dsErrors::ModelInfo::ELEMENTEDGE, OutputStream::FATAL);
  }
//    dsAssert(ec != NULL, "UNEXPECTED");

  if ((w == dsMathEnum::RHS) || (w == dsMathEnum::MATRIXANDRHS))
  {
    TriangleEdgeScalarData eflux = TriangleEdgeScalarData(*ef);
    eflux *= *ec;
    TriangleEdgeAssembleRHS(v, eflux, n0_sign, n1_sign);
  }
  else if (w == dsMathEnum::MATRIXONLY)
  {
  }
  else
  {
    dsAssert(0, "UNEXPECTED");
  }

  if ((w == dsMathEnum::MATRIXONLY) || (w == dsMathEnum::MATRIXANDRHS))
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
        dsErrors::MissingEquationModel(r, myname, dermodel0, dsErrors::ModelInfo::ELEMENTEDGE, OutputStream::VERBOSE1);
        dsErrors::MissingEquationModel(r, myname, dermodel1, dsErrors::ModelInfo::ELEMENTEDGE, OutputStream::VERBOSE1);
        dsErrors::MissingEquationModel(r, myname, dermodel2, dsErrors::ModelInfo::ELEMENTEDGE, OutputStream::VERBOSE1);
      }
      else
      {
        if (!em0)
        {
          dsErrors::MissingEquationModel(r, myname, dermodel0, dsErrors::ModelInfo::ELEMENTEDGE, OutputStream::FATAL);
        }
        if (!em1)
        {
          dsErrors::MissingEquationModel(r, myname, dermodel1, dsErrors::ModelInfo::ELEMENTEDGE, OutputStream::FATAL);
        }
        if (!em2)
        {
          dsErrors::MissingEquationModel(r, myname, dermodel2, dsErrors::ModelInfo::ELEMENTEDGE, OutputStream::FATAL);
        }

        TriangleEdgeScalarData eder0 = TriangleEdgeScalarData(*em0);
        TriangleEdgeScalarData eder1 = TriangleEdgeScalarData(*em1);
        TriangleEdgeScalarData eder2 = TriangleEdgeScalarData(*em2);

        eder0 *= *ec; /// integrate wrt volume
        eder1 *= *ec; /// integrate wrt volume
        eder2 *= *ec; /// integrate wrt volume

        UnSymmetricTriangleEdgeAssembleJacobian(m, eder0, eder1, eder2, var, n0_sign, n1_sign);
      }
    }
  }
  else if (w == dsMathEnum::RHS)
  {
  }
  else
  {
    dsAssert(0, "UNEXPECTED");
  }
}

// TODO:"REVIEW CODE"
void Equation::TetrahedronEdgeCoupleAssemble(const std::string &model, dsMath::RealRowColValueVec &m, dsMath::RHSEntryVec &v, dsMathEnum::WhatToLoad w, const std::string &edge_couple, const double n0_sign, const double n1_sign)
{
  const Region &r = GetRegion();

  VariableList_t vlist = r.GetVariableList();

  ConstTetrahedronEdgeModelPtr ef = r.GetTetrahedronEdgeModel(model);
  if (!ef)
  {
    dsErrors::MissingEquationModel(r, myname, model, dsErrors::ModelInfo::ELEMENTEDGE, OutputStream::FATAL);
  }
//  dsAssert(ef != NULL, "UNEXPECTED");

  ConstTetrahedronEdgeModelPtr ec = r.GetTetrahedronEdgeModel(edge_couple);
  if (!ec)
  {
    dsErrors::MissingEquationModel(r, myname, edge_couple, dsErrors::ModelInfo::ELEMENTEDGE, OutputStream::FATAL);
  }
//    dsAssert(ec != NULL, "UNEXPECTED");

  if ((w == dsMathEnum::RHS) || (w == dsMathEnum::MATRIXANDRHS))
  {
    TetrahedronEdgeScalarData eflux = TetrahedronEdgeScalarData(*ef);
    eflux *= *ec;
    TetrahedronEdgeAssembleRHS(v, eflux, n0_sign, n1_sign);
  }
  else if (w == dsMathEnum::MATRIXONLY)
  {
  }
  else
  {
    dsAssert(0, "UNEXPECTED");
  }

  if ((w == dsMathEnum::MATRIXONLY) || (w == dsMathEnum::MATRIXANDRHS))
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
        dsErrors::MissingEquationModel(r, myname, dermodel0, dsErrors::ModelInfo::ELEMENTEDGE, OutputStream::VERBOSE1);
        dsErrors::MissingEquationModel(r, myname, dermodel1, dsErrors::ModelInfo::ELEMENTEDGE, OutputStream::VERBOSE1);
        dsErrors::MissingEquationModel(r, myname, dermodel2, dsErrors::ModelInfo::ELEMENTEDGE, OutputStream::VERBOSE1);
        dsErrors::MissingEquationModel(r, myname, dermodel3, dsErrors::ModelInfo::ELEMENTEDGE, OutputStream::VERBOSE1);
      }
      else
      {
        if (!em0)
        {
          dsErrors::MissingEquationModel(r, myname, dermodel0, dsErrors::ModelInfo::ELEMENTEDGE, OutputStream::FATAL);
        }
        if (!em1)
        {
          dsErrors::MissingEquationModel(r, myname, dermodel1, dsErrors::ModelInfo::ELEMENTEDGE, OutputStream::FATAL);
        }
        if (!em2)
        {
          dsErrors::MissingEquationModel(r, myname, dermodel2, dsErrors::ModelInfo::ELEMENTEDGE, OutputStream::FATAL);
        }
        if (!em3)
        {
          dsErrors::MissingEquationModel(r, myname, dermodel3, dsErrors::ModelInfo::ELEMENTEDGE, OutputStream::FATAL);
        }

        TetrahedronEdgeScalarData eder0 = TetrahedronEdgeScalarData(*em0);
        TetrahedronEdgeScalarData eder1 = TetrahedronEdgeScalarData(*em1);
        TetrahedronEdgeScalarData eder2 = TetrahedronEdgeScalarData(*em2);
        TetrahedronEdgeScalarData eder3 = TetrahedronEdgeScalarData(*em3);

        eder0 *= *ec; /// integrate wrt volume
        eder1 *= *ec; /// integrate wrt volume
        eder2 *= *ec; /// integrate wrt volume
        eder3 *= *ec; /// integrate wrt volume

        UnSymmetricTetrahedronEdgeAssembleJacobian(m, eder0, eder1, eder2, eder3, var, n0_sign, n1_sign);
      }
    }
  }
  else if (w == dsMathEnum::RHS)
  {
  }
  else
  {
      dsAssert(0, "UNEXPECTED");
  }
}

void Equation::ElementEdgeCoupleAssemble(const std::string &model, dsMath::RealRowColValueVec &m, dsMath::RHSEntryVec &v, dsMathEnum::WhatToLoad w)
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

void Equation::ElementNodeVolumeAssemble(const std::string &model, dsMath::RealRowColValueVec &m, dsMath::RHSEntryVec &v, dsMathEnum::WhatToLoad w)
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

std::string Equation::GetDerivativeModelName(const std::string &model, const std::string &var)
{
  std::string ret = model;
  ret += ":";
  ret += var;
  return ret;
}

void Equation::DevsimSerialize(std::ostream &of) const
{
  of << "begin_equation \"" << this->GetName() << "\"\n";
  this->Serialize(of);
  of << "\nend_equation\n\n";
}

