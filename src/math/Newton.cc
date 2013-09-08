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

#include "Newton.hh"
#include "LinearSolver.hh"
#include "Permutation.hh"
#include "MatrixEntries.hh"
#include "GlobalData.hh"
#include "InstanceKeeper.hh"
#include "NodeKeeper.hh"
#include "CompressedMatrix.hh"
#include "SuperLUPreconditioner.hh"
#include "Device.hh"
#include "Region.hh"
#include "Equation.hh"
#include "OutputStream.hh"
#include "dsAssert.hh"

#include "BlockPreconditioner.hh"
#include "IterativeLinearSolver.hh"
#include "TimeData.hh"

#include "ObjectHolder.hh"
#include "Interpreter.hh"
#include "dsTimer.hh"

//// Take out when remove EquationMathErrorInfo
//#include "EquationErrors.hh"

#include <sstream>
#include <iomanip>
//#include <iostream>

namespace dsMath {

const double Newton::rhssign = -1.0;
const size_t Newton::DefaultMaxIter = 20;
const double Newton::DefaultAbsError  = 0.0;
const double Newton::DefaultRelError  = 0.0;
const double Newton::DefaultQRelError = 0.0;

size_t Newton::NumberEquationsAndSetDimension()
{
  GlobalData &gdata = GlobalData::GetInstance();
  size_t eqnnum = 0;

  dimension = 0;

  const GlobalData::DeviceList_t &dlist = gdata.GetDeviceList();
  {
    GlobalData::DeviceList_t::const_iterator dit  = dlist.begin();
    GlobalData::DeviceList_t::const_iterator dend = dlist.end();
    for ( ; dit != dend; ++dit)
    {
      std::ostringstream os; 

      std::string name = (dit->first);
      Device &dev =     *(dit->second);
      dev.SetBaseEquationNumber(eqnnum);
      /// Must enforce that we have at least one node (as this function assumes this is the case.
      /// TODO: this is a disaster waiting to happen
      const size_t maxnum = dev.CalcMaxEquationNumber();

      if (maxnum != size_t(-1))
      {
          os << "Device \"" << name << "\" has equations " << eqnnum << ":" << maxnum << "\n";
          eqnnum = maxnum + 1;
      }
      else
      {
          os << "Device \"" << name << "\" has no equations.\n";
      }
      OutputStream::WriteOut(OutputStream::INFO, os.str());

      if (dev.GetDimension() > dimension)
      {
        dimension = dev.GetDimension();
      }
    }
  }

  {
    NodeKeeper &nk = NodeKeeper::instance();
    if (nk.HaveNodes())
    {
      nk.SetNodeNumbers(eqnnum);
      size_t maxnum = nk.GetMaxEquationNumber();
#if 1
      std::ostringstream os; 
      os << "Circuit " << " has equations " << eqnnum << ":" << maxnum << "\n";
      OutputStream::WriteOut(OutputStream::INFO, os.str());
#endif
      eqnnum = maxnum + 1;
    }
  }

  return eqnnum;
}

void Newton::AssembleContactsAndInterfaces(RealRowColValueVec &mat, RHSEntryVec &rhs, permvec_t &permvec, Device &dev, dsMathEnum::WhatToLoad w, dsMathEnum::TimeMode t)
{
  PermutationMap         p;

  dev.ContactAssemble(mat, rhs, p, w, t);
  dev.InterfaceAssemble(mat, rhs, p, w, t);

  PermutationMap::iterator pit = p.begin();
  const PermutationMap::iterator pend = p.end();
  for (; pit != pend; ++pit)
  {
    permvec.at(pit->first) = (pit->second).GetRow();
  }
}

void Newton::AssembleBulk(RealRowColValueVec &mat, RHSEntryVec &rhs, Device &dev, dsMathEnum::WhatToLoad w, dsMathEnum::TimeMode t)
{
  dev.RegionAssemble(mat, rhs, dsMathEnum::MATRIXANDRHS, t);
}


void Newton::LoadMatrixAndRHSOnCircuit(RealRowColValueVec &mat, RHSEntryVec &rhs, dsMathEnum::WhatToLoad w, dsMathEnum::TimeMode t)
{
  InstanceKeeper &ik = InstanceKeeper::instance();
  NodeKeeper     &nk = NodeKeeper::instance();

  /// Nasty hack
  /// TODO: circuit elements should ask nodekeeper instead of node for equation number
  //        circuit elements store node names instead of circuit node pointers
#if 0
  size_t offset = nk.GetMinEquationNumber();
#endif

////// TODO: is the rhs valid at this point for ssac?
/////  Make sure it is not used

  if (t == dsMathEnum::DC)
  {
    NodeKeeper::Solution *sol = nk.GetSolution("dcop");
    dsAssert(sol != 0, "UNEXPECTED");
    if (w == dsMathEnum::MATRIXANDRHS)
    {
      ik.AssembleDCMatrix(mat, *sol, rhs);
    }
    else if (w == dsMathEnum::MATRIXONLY)
    {
      RHSEntryVec r;
      ik.AssembleDCMatrix(mat, *sol, r);
    }
    else if (w == dsMathEnum::RHS)
    {
      //// Make more efficient later
      RealRowColValueVec m;
      ik.AssembleDCMatrix(m, *sol, rhs);
    }
    else
    {
      dsAssert(0, "UNEXPECTED");
    }
  }
  else if (t == dsMathEnum::TIME)
  {
    NodeKeeper::Solution *sol = nk.GetSolution("dcop");
    dsAssert(sol != 0, "UNEXPECTED");
    if (w == dsMathEnum::MATRIXANDRHS)
    {
      ik.AssembleTRMatrix(&mat, *sol, rhs, 1.0);
    }
    else if (w == dsMathEnum::MATRIXONLY)
    {
      RHSEntryVec r;
      ik.AssembleTRMatrix(&mat, *sol, r, 1.0);
    }
    else if (w == dsMathEnum::RHS)
    {
      RealRowColValueVec *m = NULL;
      ik.AssembleTRMatrix(m, *sol, rhs, 1.0);
    }
    else
    {
      dsAssert(0, "UNEXPECTED");
    }
  }
  else
  {
    dsAssert(0, "UNEXPECTED");
  }
}

//#include <iostream>

template <typename T>
void Newton::LoadIntoMatrix(const RealRowColValueVec &rcv, Matrix &mat, T scl, size_t offset)
{
  for (RealRowColValueVec::const_iterator it = rcv.begin(); it != rcv.end(); ++it)
  {
    const size_t row = it->row + offset;
    const size_t col = it->col + offset;
    const T val = scl * it->val;
//    std::cerr << "RCV: " << row << "\t" << col << "\t" << val << "\n";
    mat.AddEntry(row, col, val);
  }
}

template <typename T>
void Newton::LoadIntoMatrixPermutated(const RealRowColValueVec &rcv, Matrix &mat, const permvec_t &permvec, T scl, size_t offset)
{
  for (RealRowColValueVec::const_iterator it = rcv.begin(); it != rcv.end(); ++it)
  {
    size_t row = permvec[it->row];
    const size_t col = it->col + offset;
    const T val = scl * it->val;
    if (row != size_t(-1))
    {
      row += offset;
      mat.AddEntry(row, col, val);
    }
  }
}

template <typename T>
void Newton::LoadIntoRHS(const RHSEntryVec &r, std::vector<T> &rhs, T scl, size_t offset)
{
  for (RHSEntryVec::const_iterator it = r.begin(); it != r.end(); ++it)
  {
    const size_t row = it->first + offset;
    const T val = scl * rhssign * it->second;
//    std::cerr << "RHS " << row << "\t" << val << "\n";
    rhs[row] += val;
  }
}

template <typename T>
void Newton::LoadIntoRHSPermutated(const RHSEntryVec &r, std::vector<T> &rhs, const permvec_t &permvec, T scl, size_t offset)
{
  for (RHSEntryVec::const_iterator it = r.begin(); it != r.end(); ++it)
  {
    size_t row = permvec[it->first];
    if (row != size_t(-1))
    {
      row += offset;
      const T val = scl * rhssign * it->second;
      rhs[row] += val;
    }
  }
}

template <typename T>
void Newton::LoadMatrixAndRHS(Matrix &matrix, std::vector<T> &rhs, permvec_t &permvec, dsMathEnum::WhatToLoad w, dsMathEnum::TimeMode t, T scl)
{
  dsTimer timer("LoadMatrixAndRHS");

  RHSEntryVec    v;
  RealRowColValueVec m;

  GlobalData &gdata = GlobalData::GetInstance();
  const GlobalData::DeviceList_t dlist = gdata.GetDeviceList();

  GlobalData::DeviceList_t::const_iterator dit  = dlist.begin();
  GlobalData::DeviceList_t::const_iterator dend = dlist.end();
  for ( ; dit != dend; ++dit)
  {
    Device &dev = *(dit->second);
    m.clear();
    v.clear();

    AssembleContactsAndInterfaces(m, v, permvec, dev, w, t);

    if (w != dsMathEnum::PERMUTATIONSONLY)
    {
      LoadIntoMatrix(m, matrix, scl);
      LoadIntoRHS(v, rhs, scl);

      m.clear();
      v.clear();
      AssembleBulk(m, v, dev, w, t);
      LoadIntoMatrixPermutated(m, matrix, permvec, scl);
      LoadIntoRHSPermutated(v, rhs, permvec, scl);
    }
  }

  if (w != dsMathEnum::PERMUTATIONSONLY)
  {
    NodeKeeper &nk = NodeKeeper::instance();
    if (nk.HaveNodes())
    {
      size_t offset = nk.GetMinEquationNumber();
      m.clear();
      v.clear();
      LoadMatrixAndRHSOnCircuit(m, v, w, t);
      LoadIntoMatrix(m, matrix, scl, offset);
      LoadIntoRHS(v, rhs, scl, offset);
    }

    m.clear();
    v.clear();
    AssembleTclEquations(m, v, w, t);
    LoadIntoMatrixPermutated(m, matrix, permvec, scl);
    LoadIntoRHSPermutated(v, rhs, permvec, scl);
  }
}

//// This function must be skipped for noise
void Newton::LoadCircuitRHSAC(ComplexDoubleVec_t &rhs)
{

  typedef std::vector<std::pair<size_t, std::complex<double> > > ComplexRHSEntryVec_t;
  ComplexRHSEntryVec_t cv;

  std::ostringstream os; 
  NodeKeeper &nk = NodeKeeper::instance();
  if (nk.HaveNodes())
  {
    InstanceKeeper &ik = InstanceKeeper::instance();
    ik.assembleACRHS(cv);

    if (cv.empty())
    {
      os << "WARN: no rhs source for small-signal ac\n";
    }

    size_t offset = nk.GetMinEquationNumber();

    for (ComplexRHSEntryVec_t::iterator it = cv.begin(); it != cv.end(); ++it)
    {
      rhs[it->first + offset] = it->second;
    }
  }
  OutputStream::WriteOut(OutputStream::INFO, os.str());
}

void Newton::LoadMatrixAndRHSAC(Matrix &matrix, ComplexDoubleVec_t &rhs, permvec_t &permvec, double frequency)
{
  const ComplexDouble_t jOmega  = 2.0 * M_PI * ComplexDouble_t(0,1.0) * frequency;

  DoubleVec_t        r(rhs.size());
  ComplexDoubleVec_t c(rhs.size());
  LoadMatrixAndRHS(matrix, r, permvec, dsMathEnum::PERMUTATIONSONLY, dsMathEnum::DC,   1.0);
  LoadMatrixAndRHS(matrix, r, permvec, dsMathEnum::MATRIXONLY, dsMathEnum::DC,   1.0);
  LoadMatrixAndRHS(matrix, c, permvec, dsMathEnum::MATRIXONLY, dsMathEnum::TIME, jOmega);
}

void Newton::RestoreSolutions()
{
  GlobalData &gdata = GlobalData::GetInstance();

  const GlobalData::DeviceList_t &dlist = gdata.GetDeviceList();
  {
    GlobalData::DeviceList_t::const_iterator dit  = dlist.begin();
    GlobalData::DeviceList_t::const_iterator dend = dlist.end();
    for ( ; dit != dend; ++dit)
    {
      std::ostringstream os; 

      std::string name = (dit->first);
      Device &dev =     *(dit->second);
      //// Transient may have other backup suffixes
      dev.RestoreSolutions("_prev");
    }
  }

  {
    NodeKeeper &nk = NodeKeeper::instance();
    if (nk.HaveNodes())
    {
      nk.CopySolution("dcop_prev", "dcop");
    }
  }
}

//// TODO: refactor so we are not so explicit
void Newton::BackupSolutions()
{
  GlobalData &gdata = GlobalData::GetInstance();

  const GlobalData::DeviceList_t &dlist = gdata.GetDeviceList();
  {
    GlobalData::DeviceList_t::const_iterator dit  = dlist.begin();
    GlobalData::DeviceList_t::const_iterator dend = dlist.end();
    for ( ; dit != dend; ++dit)
    {
      std::ostringstream os; 

      std::string name = (dit->first);
      Device &dev =     *(dit->second);
      //// Transient may have other backup suffixes
      dev.BackupSolutions("_prev");
    }
  }

  {
    NodeKeeper &nk = NodeKeeper::instance();
    if (nk.HaveNodes())
    {
      nk.InitializeSolution("dcop_prev");
      nk.CopySolution("dcop", "dcop_prev");
    }
  }
}

//// TODO: call backups to handle time steps
//// TODO: return information to tclapi
bool Newton::Solve(LinearSolver &itermethod, TimeMethods::TimeMethod_t timemethod, double tdelta, double a0, double a1, double a2, double b0, double b1, double b2)
{

  NodeKeeper &nk = NodeKeeper::instance();
  GlobalData &gdata = GlobalData::GetInstance();
  const GlobalData::DeviceList_t      &dlist = gdata.GetDeviceList();

  const size_t numeqns = NumberEquationsAndSetDimension();

  if (nk.HaveNodes())
  {
    nk.InitializeSolution("dcop");
  }

  {
    std::ostringstream os; 
    os << "number of equations " << numeqns << "\n";
    OutputStream::WriteOut(OutputStream::INFO, os.str());
  }

  std::auto_ptr<Matrix> matrix;
  std::auto_ptr<Preconditioner> preconditioner;

  if (dynamic_cast<IterativeLinearSolver *>(&itermethod))
  {
    matrix = std::auto_ptr<Matrix>(new CompressedMatrix(numeqns));
    preconditioner = std::auto_ptr<Preconditioner>(new BlockPreconditioner(numeqns, Preconditioner::NOTRANS));
  }
  else
  {
    matrix = std::auto_ptr<Matrix>(new CompressedMatrix(numeqns));
    preconditioner = std::auto_ptr<Preconditioner>(new SuperLUPreconditioner(numeqns, Preconditioner::NOTRANS, Preconditioner::FULL));
  }

  DoubleVec_t rhs(numeqns);

  bool converged = false;

  BackupSolutions();

  /////
  ///// Permutation vector
  /////
  permvec_t permvec(numeqns);
  for (size_t i = 0; i < permvec.size(); ++i)
  {
    permvec[i] = i;
  }

  DoubleVec_t result(numeqns);

  DoubleVec_t rhs_constant(numeqns);
  if ((timemethod != TimeMethods::DCONLY) && (timemethod != TimeMethods::INTEGRATEDC))
  {
    TimeData &tinst = TimeData::GetInstance();
    if (a1 != 0.0)
    {
      tinst.AssembleQ(TimeData::TM0, a1, rhs_constant);
    }
    if (a2 != 0.0)
    {
      tinst.AssembleQ(TimeData::TM1, a2, rhs_constant);
    }

    if (b1 != 0.0)
    {
      tinst.AssembleI(TimeData::TM0, b1, rhs_constant);
    }
    if (b2 != 0.0)
    {
      tinst.AssembleI(TimeData::TM1, b2, rhs_constant);
    }
  }
  

  LoadMatrixAndRHS(*matrix, rhs, permvec, dsMathEnum::PERMUTATIONSONLY, dsMathEnum::DC, 1.0);

  int divergence_count = 0;
  double last_rel_err = 0.0;
  double last_abs_err = 0.0;
  for (int iter = 0; (iter < maxiter) && (!converged) && (divergence_count < 5); ++iter)
  {
    std::ostringstream os; 

    rhs = rhs_constant;

//        std::cerr << "Begin Load Matrix\n";
    /// This is the resistive portion (always assembled
    if (timemethod == TimeMethods::DCONLY)
    {
      LoadMatrixAndRHS(*matrix, rhs, permvec, dsMathEnum::MATRIXANDRHS, dsMathEnum::DC, 1.0);
    }
    else
    {
      LoadMatrixAndRHS(*matrix, rhs, permvec, dsMathEnum::MATRIXANDRHS, dsMathEnum::DC, 1.0);

      /// This assembles the time derivative current 
      if (a0 != 0.0)
      {
        LoadMatrixAndRHS(*matrix, rhs, permvec, dsMathEnum::MATRIXANDRHS, dsMathEnum::TIME, a0);
      }
    }

//        std::cerr << "End Load Matrix\n";

    result.clear();
    result.resize(numeqns);

    matrix->Finalize();

//        std::cerr << "Begin Solve Matrix\n";
    bool solveok = itermethod.Solve(*matrix, *preconditioner, result, rhs);
    if (!solveok)
    {
      break;
    }
//        std::cerr << "End Solve Matrix\n";

    {
      GlobalData::DeviceList_t::const_iterator dit  = dlist.begin();
      GlobalData::DeviceList_t::const_iterator dend = dlist.end();
      for ( ; dit != dend; ++dit)
      {
        std::string name = (dit->first);
        Device *dev =      (dit->second);
        dev->Update(result);
      }
    }

    if (nk.HaveNodes())
    {
      nk.UpdateSolution("dcop", result);
      nk.TriggerCallbacksOnNodes();
    }


    os << "Iteration: " << iter << "\n";
    {
      converged = true;
      GlobalData::DeviceList_t::const_iterator dit  = dlist.begin();
      GlobalData::DeviceList_t::const_iterator dend = dlist.end();
      for ( ; dit != dend; ++dit)
      {
        const std::string &name = dit->first;
        const Device &device = *(dit->second);
        const double devrerr = device.GetRelError();
        const double devaerr = device.GetAbsError();
        os << "  Device: \"" << name << "\""
            << std::scientific << std::setprecision(5) <<
                     "\tRelError: " << devrerr <<
                     "\tAbsError: " << devaerr << "\n";

        const Device::RegionList_t regions = device.GetRegionList();
        for (Device::RegionList_t::const_iterator rit = regions.begin(); rit != regions.end(); ++rit)
        {
          const Region &region = *(rit->second);

          if (region.GetNumberEquations() == 0)
          {
            continue;
          }

          os << "    Region: \"" << region.GetName() << "\""
              << std::scientific << std::setprecision(5) <<
                       "\tRelError: " << region.GetRelError() <<
                       "\tAbsError: " << region.GetAbsError() << "\n";

          const EquationPtrMap_t &equations = region.GetEquationPtrList();
          for (EquationPtrMap_t::const_iterator eit = equations.begin(); eit != equations.end(); ++eit)
          {
            const Equation &equation = *(eit->second);
            os << "      Equation: \"" << equation.GetName() << "\""
                << std::scientific << std::setprecision(5) <<
                         "\tRelError: " << equation.GetRelError() <<
                         "\tAbsError: " << equation.GetAbsError() << "\n";


//            dsErrors::EquationMathErrorInfo(equation, OutputStream::INFO);
          }
        }

        /* if are our is higher than our convergence criteria and it is running away*/
        bool diverged = ((devrerr > relLimit) && (devrerr > last_rel_err))
           || ((devaerr > absLimit) && (devaerr > last_abs_err));
        if (diverged)
        {
          divergence_count += 1;
        }
        else
        {
          divergence_count = 0;
        }
        last_rel_err = devrerr;
        last_abs_err = devaerr;

        converged = converged && (devrerr < relLimit) && (devaerr < absLimit);
      }

      if (nk.HaveNodes())
      {
        const double cirrerr = nk.GetRelError("dcop");
        const double ciraerr = nk.GetAbsError("dcop");
        os << "  Circuit: "
            << std::scientific << std::setprecision(5) <<
                     "\tRelError: " << cirrerr <<
                     "\tAbsError: " << ciraerr << "\n";
        /// TODO: the circuit as well as individual equations in regions need specific convergence criteria
        converged = converged && (cirrerr < relLimit) && (ciraerr < absLimit);
      }
    }

    matrix->ClearMatrix();
    OutputStream::WriteOut(OutputStream::INFO, os.str());
  }

  std::vector<double> newQ;
  if (timemethod != TimeMethods::DCONLY)
  {
    //// New charge based on new assemble
    newQ.resize(numeqns);
    LoadMatrixAndRHS(*matrix, newQ, permvec, dsMathEnum::RHS, dsMathEnum::TIME, 1.0);

    //// Check to see if your projection was correct
    if ((timemethod == TimeMethods::INTEGRATEBDF1) || (timemethod == TimeMethods::INTEGRATETR) || (timemethod == TimeMethods::INTEGRATEBDF2))
    {
      /// need to make sure projection makes sense
      std::vector<double> projectQ(numeqns);
      
      TimeData &tinst = TimeData::GetInstance();

      tinst.AssembleI(TimeData::TM0, tdelta, projectQ);
      tinst.AssembleQ(TimeData::TM0, 1.0,    projectQ);

      double qrel = 0.0;
      for (size_t i = 0; i < numeqns; ++i)
      {
        const double qproj = projectQ[i];
        const double qnew  = newQ[i];
        if (qnew != 0.0)
        {
          const double qr = fabs(qnew - qproj)/(1.0e-20 + fabs(qnew) + fabs(qproj));
          if (qr > qrel)
          {
            qrel = qr;
          }
        }
      }
      std::ostringstream os;
      os << "Charge Relative Error " << std::scientific << std::setprecision(5) << qrel << "\n";
      OutputStream::WriteOut(OutputStream::INFO, os.str());

      if (qrel > qrelLimit)
      {
        converged = false;
      }
    }
  }

  if (!converged)
  {
    RestoreSolutions();
  }
  else
  {

    GlobalData::DeviceList_t::const_iterator dit  = dlist.begin();
    GlobalData::DeviceList_t::const_iterator dend = dlist.end();
    for ( ; dit != dend; ++dit)
    {
      std::string name = (dit->first);
      Device *dev =      (dit->second);
      dev->UpdateContacts();
    }

    if (nk.HaveNodes())
    {
      nk.PrintSolution("dcop");
    }

    if (timemethod != TimeMethods::DCONLY)
    {
      rhs.clear();
      rhs.resize(numeqns);
      TimeData &tinst = TimeData::GetInstance();
      if (timemethod == TimeMethods::INTEGRATEDC)
      {
        //// There is no displacement current for a dc solution
        tinst.SetI(TimeData::TM0, rhs);
        tinst.SetQ(TimeData::TM0, newQ);

        tinst.ClearI(TimeData::TM1);
        tinst.ClearI(TimeData::TM2);
        tinst.ClearQ(TimeData::TM1);
        tinst.ClearQ(TimeData::TM2);
      }
      else
      {
        ////// Charge

        if (tinst.ExistsQ(TimeData::TM1))
        {
          tinst.CopyQ(TimeData::TM1, TimeData::TM2);
        }
        if (tinst.ExistsQ(TimeData::TM0))
        {
          tinst.CopyQ(TimeData::TM0, TimeData::TM1);
        }

        tinst.SetQ(TimeData::TM0, newQ);


        ////// Current
        if (tinst.ExistsI(TimeData::TM1))
        {
          tinst.CopyI(TimeData::TM1, TimeData::TM2);
        }
        if (tinst.ExistsI(TimeData::TM0))
        {
          tinst.CopyI(TimeData::TM0, TimeData::TM1);
        }

        ////// Update Current
        rhs.clear();
        rhs.resize(numeqns);

        if (a0 != 0.0)
        {
          tinst.AssembleQ(TimeData::TM0, a0, rhs);
        }
        if (a1 != 0.0)
        {
          tinst.AssembleQ(TimeData::TM1, a1, rhs);
        }
        if (a2 != 0.0)
        {
          tinst.AssembleQ(TimeData::TM2, a2, rhs);
        }

        if (b1 != 0.0)
        {
          tinst.AssembleI(TimeData::TM1, b1, rhs);
        }
        if (b2 != 0.0)
        {
          tinst.AssembleI(TimeData::TM2, b2, rhs);
        }

        tinst.SetI(TimeData::TM0, rhs);
      }
    }
  }

  return converged;
}

/// Need to be able to solve circuits, even if device doesn't exist
bool Newton::ACSolve(LinearSolver &itermethod, double frequency)
{
  NodeKeeper &nk = NodeKeeper::instance();
  GlobalData &gdata = GlobalData::GetInstance();
  const GlobalData::DeviceList_t      &dlist = gdata.GetDeviceList();

  const size_t numeqns = NumberEquationsAndSetDimension();

  if (nk.HaveNodes())
  {
    nk.InitializeSolution("ssac_real");
    nk.InitializeSolution("ssac_imag");
    nk.InitializeSolution("dcop");
  }


  std::auto_ptr<Matrix> matrix(new CompressedMatrix(numeqns, CompressedMatrix::COMPLEX, CompressedMatrix::CCM));
  std::auto_ptr<Preconditioner> preconditioner(new SuperLUPreconditioner(numeqns, Preconditioner::NOTRANS, Preconditioner::FULL));

  ComplexDoubleVec_t rhs(numeqns);

  /// This is to initialize other copies
  permvec_t permvec_temp(numeqns);

  for (size_t i = 0; i < permvec_temp.size(); ++i)
  {
    permvec_temp[i] = i;
  }

  ComplexDoubleVec_t result(numeqns);

  bool converged = false;

  do
  {
    permvec_t permvec(permvec_temp);

  ///// TODO: would be nice to only have to change the frequency dep entries
    LoadMatrixAndRHSAC(*matrix, rhs, permvec, frequency);
    LoadCircuitRHSAC(rhs);

    matrix->Finalize();

    bool solveok = itermethod.ACSolve(*matrix, *preconditioner, result, rhs);
    converged = solveok;
    if (!solveok)
    {
      break;
    }

    GlobalData::DeviceList_t::const_iterator dit  = dlist.begin();
    GlobalData::DeviceList_t::const_iterator dend = dlist.end();
    for ( ; dit != dend; ++dit)
    {
      std::string name = (dit->first);
      Device *dev =      (dit->second);
      dev->ACUpdate(result);
    }

    if (nk.HaveNodes())
    {
      nk.ACUpdateSolution("ssac_real", "ssac_imag", result);
    }


    {
      std::ostringstream os;
      os << "AC Iteration:\n";
      os << "number of equations " << numeqns << "\n";
      //// TODO: more meaningful reporting
#if 0
        GlobalData::DeviceList_t::const_iterator dit  = dlist.begin();
        GlobalData::DeviceList_t::const_iterator dend = dlist.end();
        for ( ; dit != dend; ++dit)
        {
            std::string name = (dit->first);
            Device *dev =      (dit->second);
            const double devrerr = dev->GetRelError();
            const double devaerr = dev->GetAbsError();
            os << "\tDevice: \"" << name << "\""
                << std::scientific << std::setprecision(5) <<
                         "\tRelError: " << devrerr <<
                         "\tAbsError: " << devaerr << "\n";

        }

        if (nk.HaveNodes())
        {
            const double cirrerr = nk.GetRelError("ssac_real");
            const double ciraerr = nk.GetAbsError("ssac_real");
            os << "  Circuit: "
                << std::scientific << std::setprecision(5) <<
                         "\tRelError: " << cirrerr <<
                         "\tAbsError: " << ciraerr << "\n";
        }
#endif
      OutputStream::WriteOut(OutputStream::INFO, os.str());
    }


    //// ultimately we need to make this a getter
    if (nk.HaveNodes())
    {
      nk.ACPrintSolution("ssac_real", "ssac_imag");
    }
  } while (0);

  //// TODO: have some kind of accuracy check for iterative solvers
  return converged;
}

/// Need to be able to solve circuits, even if device doesn't exist
//TODO: This is a blatant ripoff of the ACSolve.  Should refactor common code.
//TODO: Another possiblity is to create class that handles the element assembly.
bool Newton::NoiseSolve(const std::string &output_name, LinearSolver &itermethod, double frequency)
{

  NodeKeeper &nk = NodeKeeper::instance();
  const size_t numeqns = NumberEquationsAndSetDimension();

  size_t outputeqnnum = size_t(-1);

  if (!nk.HaveNodes())
  {
    std::ostringstream os;
    os << "A circuit is required for a noise solve.\n";
    OutputStream::WriteOut(OutputStream::ERROR, os.str());
    return false;
    //// Should probably abort here
  }
  else
  {
    outputeqnnum = nk.GetEquationNumber(output_name);
  }

  if (outputeqnnum == size_t(-1))
  {
    std::ostringstream os;
    os << "Circuit output " << output_name << " does not exist.\n";
    OutputStream::WriteOut(OutputStream::ERROR, os.str());
    return false;
    //// Should probably abort here
  }
  else
  {
    std::ostringstream os;
    os << "Circuit output " << output_name << " has equation " << outputeqnnum << ".\n";
    OutputStream::WriteOut(OutputStream::INFO, os.str());
  }

  GlobalData &gdata = GlobalData::GetInstance();
  const GlobalData::DeviceList_t      &dlist = gdata.GetDeviceList();

  const std::string circuit_real_name = std::string("noise_") + output_name + "_real";
  const std::string circuit_imag_name = std::string("noise_") + output_name + "_imag";

  nk.InitializeSolution(circuit_real_name);
  nk.InitializeSolution(circuit_imag_name);
  nk.InitializeSolution("dcop");


  std::auto_ptr<Matrix> matrix(new CompressedMatrix(numeqns, CompressedMatrix::COMPLEX, CompressedMatrix::CCM));
  std::auto_ptr<Preconditioner> preconditioner(new SuperLUPreconditioner(numeqns, Preconditioner::TRANS, SuperLUPreconditioner::FULL));

  ComplexDoubleVec_t rhs(numeqns);

  /// This is to initialize other copies
  permvec_t permvec_temp(numeqns);

  for (size_t i = 0; i < permvec_temp.size(); ++i)
  {
      permvec_temp[i] = i;
  }

  ComplexDoubleVec_t result(numeqns);

  bool converged = false;

  do
  {
    permvec_t permvec(permvec_temp);

  ///// TODO: would be nice to only have to change the frequency dep entries
    LoadMatrixAndRHSAC(*matrix, rhs, permvec, frequency);

    /// Since the circuit nodes are not permutated, we don't need to permutate the rhs
    //// TODO: PUBLISH, we can't update contact nodes, since they are solving a different equation!!!!
    rhs[outputeqnnum] = 1.0;

    matrix->Finalize();

    bool solveok = itermethod.NoiseSolve(*matrix, *preconditioner, result, rhs);
    converged = solveok;
    if (!solveok)
    {
      break;
    }

    GlobalData::DeviceList_t::const_iterator dit  = dlist.begin();
    GlobalData::DeviceList_t::const_iterator dend = dlist.end();
    for ( ; dit != dend; ++dit)
    {
      std::string name = (dit->first);
      Device *dev =      (dit->second);
      //// TODO: need to use permutation vec to ensure that permutated out equations are neglected
      // the need to write updaters for interface and contact equations!!!
      dev->NoiseUpdate(output_name, permvec, result);
    }

    nk.ACUpdateSolution(circuit_real_name, circuit_imag_name, result);


    {
      std::ostringstream os;
      os << "Noise Iteration:\n";
      os << "number of equations " << numeqns << "\n";
      //// TODO: more meaningful reporting

      OutputStream::WriteOut(OutputStream::INFO, os.str());
    }
  } while (0);

  //// TODO: have some kind of accuracy check for iterative solvers
  return converged;
}

//#include <iostream>
void Newton::AssembleTclEquations(RealRowColValueVec &mat, RHSEntryVec &rhs, dsMathEnum::WhatToLoad w, dsMathEnum::TimeMode t)
{
  GlobalData &gdata = GlobalData::GetInstance();

  const GlobalData::TclEquationList_t &tlist = gdata.GetTclEquationList();

  std::vector<std::string> arguments;
  arguments.reserve(2);
  arguments.push_back(dsMathEnum::WhatToLoadString[w]);
  arguments.push_back(dsMathEnum::TimeModeString[t]);

  Interpreter MyInterp;
  std::string     result;
  for (GlobalData::TclEquationList_t::const_iterator it = tlist.begin(); it != tlist.end(); ++it)
  {
    const std::string &name = it->first;
    const std::string &proc = it->second;


    bool ok = MyInterp.RunCommand(proc, arguments);


    if (!ok)
    {
      std::ostringstream os; 
      os << "Error when evaluating custom_equation \"" << name << "\" evaluating Tcl Procedure \"" << proc << "\" with result \"" << MyInterp.GetErrorString() << "\"\n";
      OutputStream::WriteOut(OutputStream::FATAL, os.str().c_str());
    }
    else
    {
      std::vector<ObjectHolder> objects;
      ObjectHolder result = MyInterp.GetResult();
      ok = result.GetListOfObjects(objects);


      if ((!ok) || (objects.size() != 2))
      {
        std::ostringstream os; 
        os << "Error when evaluating custom_equation \"" << name << "\" evaluating Tcl Procedure \"" << proc << "\" cannot extract list of length 2 containing matrix and rhs entries\n";
        OutputStream::WriteOut(OutputStream::FATAL, os.str().c_str());
      }

      ObjectHolder rhs_objects = objects[1];
      ObjectHolder matrix_objects = objects[0];

      //// Read in the entries here
      if (w != dsMathEnum::MATRIXONLY)
      {
        ok = rhs_objects.GetListOfObjects(objects);
        size_t len = objects.size();
  
        if ((!ok) || ((len % 2) != 0))
        {
          std::ostringstream os; 
          os << "Error when evaluating custom_equation \"" << name << "\" evaluating Tcl Procedure \"" << proc << "\" rhs entry list of length \"" << len << "\" is not divisible by 2\n";
          OutputStream::WriteOut(OutputStream::FATAL, os.str().c_str());
        }
        else
        {
          for (size_t i = 0; i < len; i += 2)
          {
            ObjectHolder::IntegerEntry_t row = objects[i].GetInteger();
            ObjectHolder::DoubleEntry_t val = objects[i + 1].GetDouble();

            if (row.first && val.first)
            {
              rhs.push_back(std::make_pair(row.second, val.second));
            }
            else
            {
              std::ostringstream os; 
              os << "Error when evaluating custom_equation \"" << name << "\" evaluating Tcl Procedure \"" << proc << "\" rhs val entry " << objects[i].GetString() << " " << objects[i+1].GetString() << "\n";
              OutputStream::WriteOut(OutputStream::FATAL, os.str().c_str());
            }

          }
        }
      }

      if (w != dsMathEnum::RHS)
      {
        ok = matrix_objects.GetListOfObjects(objects);
        size_t len = objects.size();

        if ((!ok) || ((len % 3) != 0))
        {
          std::ostringstream os; 
          os << "Error when evaluating custom_equation \"" << name << "\" evaluating Tcl Procedure \"" << proc << "\" matrix entry list of length \"" << len << "\" is not divisible by 3\n";
          OutputStream::WriteOut(OutputStream::FATAL, os.str().c_str());
        }
        else
        {

          for (size_t i = 0; i < len; i += 3)
          {
            ObjectHolder::IntegerEntry_t row = objects[i].GetInteger();
            ObjectHolder::IntegerEntry_t col = objects[i + 1].GetInteger();
            ObjectHolder::DoubleEntry_t  val = objects[i + 2].GetDouble();

            if (row.first && col.first && val.first)
            {
              mat.push_back(RealRowColVal(row.second, col.second, val.second));
            }
            else
            {
              std::ostringstream os; 
              os << "Error when evaluating custom_equation \"" << name << "\" evaluating Tcl Procedure \"" << proc << "\" matrix entry " <<
                objects[i].GetString() << " " << objects[i+1].GetString() << " " << objects[i+2].GetString() << "\n";
              OutputStream::WriteOut(OutputStream::FATAL, os.str().c_str());
            }
          }
        }
      }
    }
  }
}

}/// end namespace dsMath

