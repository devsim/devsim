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
#include "EquationHolder.hh"
#include "OutputStream.hh"
#include "dsAssert.hh"

#include "BlockPreconditioner.hh"
#include "IterativeLinearSolver.hh"
#include "TimeData.hh"

#include "ObjectHolder.hh"
#include "Interpreter.hh"
#include "dsTimer.hh"

//#undef USE_MKL_PARDISO

#ifdef USE_MKL_PARDISO
#include "MKLPardisoPreconditioner.hh"
#endif

#ifdef DEVSIM_EXTENDED_PRECISION
#include "Float128.hh"
#endif

#include <sstream>
#include <iomanip>
#include <cmath>
using std::abs;

namespace dsMath {

template <>
const double Newton<double>::rhssign = -1.0;

template <>
const size_t Newton<double>::DefaultMaxIter = 20;

template <>
const double Newton<double>::DefaultAbsError  = 0.0;

template <>
const double Newton<double>::DefaultRelError  = 0.0;

template <>
const double Newton<double>::DefaultQRelError = 0.0;

#ifdef DEVSIM_EXTENDED_PRECISION
template <>
const float128 Newton<float128>::rhssign = -1.0;

template <>
const size_t Newton<float128>::DefaultMaxIter = 20;

template <>
const float128 Newton<float128>::DefaultAbsError  = 0.0;

template <>
const float128 Newton<float128>::DefaultRelError  = 0.0;

template <>
const float128 Newton<float128>::DefaultQRelError = 0.0;
#endif

template <typename DoubleType>
Newton<DoubleType>::Newton() : maxiter(DefaultMaxIter), absLimit(DefaultAbsError), relLimit(DefaultRelError), qrelLimit(DefaultQRelError), dimension(0) {}

template <typename DoubleType>
Newton<DoubleType>::~Newton() {};

template <typename DoubleType>
size_t Newton<DoubleType>::NumberEquationsAndSetDimension()
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
      OutputStream::WriteOut(OutputStream::OutputType::INFO, os.str());

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
      OutputStream::WriteOut(OutputStream::OutputType::INFO, os.str());
#endif
      eqnnum = maxnum + 1;
    }
  }

  return eqnnum;
}

template <typename DoubleType>
void Newton<DoubleType>::AssembleContactsAndInterfaces(RealRowColValueVec<DoubleType> &mat, RHSEntryVec<DoubleType> &rhs, permvec_t &permvec, Device &dev, dsMathEnum::WhatToLoad w, dsMathEnum::TimeMode t)
{
  PermutationMap         p;

  dev.ContactAssemble(mat, rhs, p, w, t);
  dev.InterfaceAssemble(mat, rhs, p, w, t);

  PermutationMap::iterator pit = p.begin();
  const PermutationMap::iterator pend = p.end();
  for (; pit != pend; ++pit)
  {
    permvec.at(pit->first) = (pit->second);
  }
}

template <typename DoubleType>
void Newton<DoubleType>::AssembleBulk(RealRowColValueVec<DoubleType> &mat, RHSEntryVec<DoubleType> &rhs, Device &dev, dsMathEnum::WhatToLoad w, dsMathEnum::TimeMode t)
{
  dev.RegionAssemble(mat, rhs, dsMathEnum::WhatToLoad::MATRIXANDRHS, t);
}

template <typename DoubleType>
void Newton<DoubleType>::LoadMatrixAndRHSOnCircuit(RealRowColValueVec<DoubleType> &mat, RHSEntryVec<DoubleType> &rhs, dsMathEnum::WhatToLoad w, dsMathEnum::TimeMode t)
{
  InstanceKeeper &ik = InstanceKeeper::instance();
  NodeKeeper     &nk = NodeKeeper::instance();

  RHSEntryVec<double> drhs;
  RealRowColValueVec<double> dmat;

  if (t == dsMathEnum::TimeMode::DC)
  {
    std::vector<double> *sol = nk.GetSolution("dcop");
    dsAssert(sol != 0, "UNEXPECTED");
    if (w == dsMathEnum::WhatToLoad::MATRIXANDRHS)
    {
      ik.AssembleDCMatrix(dmat, *sol, drhs);
    }
    else if (w == dsMathEnum::WhatToLoad::MATRIXONLY)
    {
      RHSEntryVec<double> r;
      ik.AssembleDCMatrix(dmat, *sol, r);
    }
    else if (w == dsMathEnum::WhatToLoad::RHS)
    {
      //// Make more efficient later
      RealRowColValueVec<double> m;
      ik.AssembleDCMatrix(m, *sol, drhs);
    }
    else
    {
      dsAssert(0, "UNEXPECTED");
    }
  }
  else if (t == dsMathEnum::TimeMode::TIME)
  {
    std::vector<double> *sol = nk.GetSolution("dcop");
    dsAssert(sol != 0, "UNEXPECTED");
    if (w == dsMathEnum::WhatToLoad::MATRIXANDRHS)
    {
      ik.AssembleTRMatrix(&dmat, *sol, drhs, 1.0);
    }
    else if (w == dsMathEnum::WhatToLoad::MATRIXONLY)
    {
      RHSEntryVec<double> r;
      ik.AssembleTRMatrix(&dmat, *sol, r, 1.0);
    }
    else if (w == dsMathEnum::WhatToLoad::RHS)
    {
      RealRowColValueVec<double> *m = nullptr;
      ik.AssembleTRMatrix(m, *sol, drhs, 1.0);
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

  for (size_t i = 0; i < dmat.size(); ++i)
  {
    auto &entry = dmat[i];
    mat.push_back(RowColVal<DoubleType>(entry.row, entry.col, static_cast<DoubleType>(entry.val)));
  }

  for (size_t i = 0; i < drhs.size(); ++i)
  {
    auto &entry = drhs[i];
    rhs.push_back(std::make_pair(entry.first, static_cast<DoubleType>(entry.second)));
  }
}

template <typename DoubleType>
template <typename T>
void Newton<DoubleType>::LoadIntoMatrix(const RealRowColValueVec<DoubleType> &rcv, Matrix<DoubleType> &mat, T scl, size_t offset)
{
  for (typename RealRowColValueVec<DoubleType>::const_iterator it = rcv.begin(); it != rcv.end(); ++it)
  {
    const size_t row = it->row + offset;
    const size_t col = it->col + offset;
    const T val = scl * it->val;
//    std::cerr << "RCV: " << row << "\t" << col << "\t" << val << "\n";
    mat.AddEntry(row, col, val);
  }
}

template <typename DoubleType>
template <typename T>
void Newton<DoubleType>::LoadIntoMatrixPermutated(const RealRowColValueVec<DoubleType> &rcv, Matrix<DoubleType> &mat, const permvec_t &permvec, T scl, size_t offset)
{
  for (typename RealRowColValueVec<DoubleType>::const_iterator it = rcv.begin(); it != rcv.end(); ++it)
  {
    auto original_row = it->row;
    const auto &Entry = permvec[original_row];
    auto row = Entry.GetRow();

    if (row == size_t(-1))
    {
      continue;
    }

    const auto col = it->col + offset;
    const T val = scl * it->val;
    row += offset;
    mat.AddEntry(row, col, val);

    if (Entry.KeepCopy())
    {
      original_row += offset;
      mat.AddEntry(original_row, col, val);
    }
  }
}

template <typename DoubleType>
template <typename T>
void Newton<DoubleType>::LoadIntoRHS(const RHSEntryVec<DoubleType> &r, std::vector<T> &rhs, T scl, size_t offset)
{
  for (typename RHSEntryVec<DoubleType>::const_iterator it = r.begin(); it != r.end(); ++it)
  {
    const size_t row = it->first + offset;
    const T val = scl * rhssign * it->second;
//    std::cerr << "RHS " << row << "\t" << val << "\n";
    rhs[row] += val;
  }
}

template <typename DoubleType>
template <typename T>
void Newton<DoubleType>::LoadIntoRHSPermutated(const RHSEntryVec<DoubleType> &r, std::vector<T> &rhs, const permvec_t &permvec, T scl, size_t offset)
{
  for (typename RHSEntryVec<DoubleType>::const_iterator it = r.begin(); it != r.end(); ++it)
  {
    auto original_row = it->first;
    const auto &Entry = permvec[original_row];
    auto row = permvec[it->first].GetRow();
    if (row == size_t(-1))
    {
      continue;
    }

    row += offset;
    const T val = scl * rhssign * it->second;
    rhs[row] += val;

    if (Entry.KeepCopy())
    {
      original_row += offset;
      rhs[original_row] += val;
    }
  }
}

template <typename DoubleType>
template <typename T>
void Newton<DoubleType>::LoadMatrixAndRHS(Matrix<DoubleType> &matrix, std::vector<T> &rhs, permvec_t &permvec, dsMathEnum::WhatToLoad w, dsMathEnum::TimeMode t, T scl)
{
  dsTimer timer("LoadMatrixAndRHS");

  RHSEntryVec<DoubleType>    v;
  RealRowColValueVec<DoubleType> m;

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

    if (w != dsMathEnum::WhatToLoad::PERMUTATIONSONLY)
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

  if (w != dsMathEnum::WhatToLoad::PERMUTATIONSONLY)
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
template <typename DoubleType>
void Newton<DoubleType>::LoadCircuitRHSAC(std::vector<std::complex<DoubleType>> &rhs)
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

    for (typename ComplexRHSEntryVec_t::iterator it = cv.begin(); it != cv.end(); ++it)
    {
      rhs[it->first + offset] = std::complex<DoubleType>(static_cast<DoubleType>(it->second.real()), static_cast<DoubleType>(it->second.imag()));
    }
  }
  OutputStream::WriteOut(OutputStream::OutputType::INFO, os.str());
}

template <typename DoubleType>
void Newton<DoubleType>::LoadMatrixAndRHSAC(Matrix<DoubleType> &matrix, std::vector<std::complex<DoubleType>> &rhs, permvec_t &permvec, DoubleType frequency)
{
#ifdef DEVSIM_EXTENDED_PRECISION
  static const DoubleType two_pi = boost::math::constants::two_pi<DoubleType>();
#else
  static const DoubleType two_pi = 2.0*M_PI;
#endif
  const std::complex<DoubleType> jOmega  = two_pi * std::complex<DoubleType>(0,1.0) * frequency;

  std::vector<DoubleType>                   r(rhs.size());
  std::vector<std::complex<DoubleType>> c(rhs.size());
  LoadMatrixAndRHS(matrix, r, permvec, dsMathEnum::WhatToLoad::PERMUTATIONSONLY, dsMathEnum::TimeMode::DC,   static_cast<DoubleType>(1.0));
  LoadMatrixAndRHS(matrix, r, permvec, dsMathEnum::WhatToLoad::MATRIXONLY, dsMathEnum::TimeMode::DC,   static_cast<DoubleType>(1.0));
  LoadMatrixAndRHS(matrix, c, permvec, dsMathEnum::WhatToLoad::MATRIXONLY, dsMathEnum::TimeMode::TIME, jOmega);
}

template <typename DoubleType>
void Newton<DoubleType>::RestoreSolutions()
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

template <typename DoubleType>
void Newton<DoubleType>::BackupSolutions()
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

namespace {
Preconditioner<double> *CreatePreconditioner(LinearSolver<double> &itermethod, size_t numeqns)
{
  Preconditioner<double> *preconditioner;
  if (dynamic_cast<IterativeLinearSolver<double> *>(&itermethod))
  {
    preconditioner = new BlockPreconditioner<double>(numeqns, PEnum::TransposeType_t::NOTRANS);
  }
  else
  {
#ifdef USE_MKL_PARDISO
    preconditioner = new MKLPardisoPreconditioner<double>(numeqns, PEnum::TransposeType_t::NOTRANS);
#else
    preconditioner = new SuperLUPreconditioner<double>(numeqns, PEnum::TransposeType_t::NOTRANS, PEnum::LUType_t::FULL);
#endif
  }
  return preconditioner;
}

#ifdef DEVSIM_EXTENDED_PRECISION
Preconditioner<float128> *CreatePreconditioner(LinearSolver<float128> &itermethod, size_t numeqns)
{
  Preconditioner<float128> *preconditioner;
  if (dynamic_cast<IterativeLinearSolver<float128> *>(&itermethod))
  {
    preconditioner = new BlockPreconditioner<float128>(numeqns, PEnum::TransposeType_t::NOTRANS);
  }
  else
  {
#ifdef USE_MKL_PARDISO
    preconditioner = new MKLPardisoPreconditioner<float128>(numeqns, PEnum::TransposeType_t::NOTRANS);
#else
    preconditioner = new SuperLUPreconditioner<float128>(numeqns, PEnum::TransposeType_t::NOTRANS, PEnum::LUType_t::FULL);
#endif
  }
  return preconditioner;
}
#endif

template <typename DoubleType>
Preconditioner<DoubleType> *CreateACPreconditioner(PEnum::TransposeType_t trans_type, size_t numeqns)
{
#ifdef USE_MKL_PARDISO
  return new MKLPardisoPreconditioner<DoubleType>(numeqns, trans_type);
#else
  return new SuperLUPreconditioner<DoubleType>(numeqns, trans_type, PEnum::LUType_t::FULL);
#endif
}

template <typename DoubleType>
CompressedMatrix<DoubleType> *CreateMatrix(Preconditioner<DoubleType> *preconditioner, bool is_complex=false)
{
  const auto numeqns = preconditioner->size();

  auto matrix_type = MatrixType::REAL;
  if (is_complex)
  {
    matrix_type = MatrixType::COMPLEX;
  }

  auto compression_type = CompressionType::CCM;

  if (dynamic_cast<SuperLUPreconditioner<DoubleType> *>(preconditioner))
  {
    compression_type = CompressionType::CCM;
  }
  else if (dynamic_cast<BlockPreconditioner<DoubleType> *>(preconditioner))
  {
    compression_type = CompressionType::CCM;
  }
#ifdef USE_MKL_PARDISO
  else if (dynamic_cast<MKLPardisoPreconditioner<DoubleType> *>(preconditioner))
  {
    compression_type = CompressionType::CCM;
  }
#endif
  else
  {
    dsAssert(0, "UNEXPECTED");
  }

  return new CompressedMatrix<DoubleType>(numeqns, matrix_type, compression_type);

}

template <typename DoubleType>
CompressedMatrix<DoubleType> *CreateACMatrix(Preconditioner<DoubleType> *preconditioner)
{
  return CreateMatrix(preconditioner, true);
}

}

namespace {
void CallUpdateSolution(NodeKeeper &nk, const std::string &name, std::vector<double> &result)
{
  nk.UpdateSolution(name, result);
}

#ifdef DEVSIM_EXTENDED_PRECISION
void CallUpdateSolution(NodeKeeper &nk, const std::string &name, std::vector<float128> &result)
{
  std::vector<double> tmp(result.size());
  for (size_t i = 0; i < result.size(); ++i)
  {
    tmp[i] = static_cast<double>(result[i]);
  }
  nk.UpdateSolution(name, tmp);
}
#endif

void CallACUpdateSolution(NodeKeeper &nk, const std::string &rname, const std::string &iname, std::vector<std::complex<double>> &result)
{
  nk.ACUpdateSolution(rname, iname, result);
}

#ifdef DEVSIM_EXTENDED_PRECISION
void CallACUpdateSolution(NodeKeeper &nk, const std::string &rname, const std::string &iname, std::vector<std::complex<float128>> &result)
{
  std::vector<std::complex<double>> tmp(result.size());
  for (size_t i = 0; i < result.size(); ++i)
  {
    tmp[i] = std::complex<double>(static_cast<double>(result[i].real()), static_cast<double>(result[i].imag()));
  }
  nk.ACUpdateSolution(rname, iname, tmp);
}
#endif

}


template <typename DoubleType>
bool Newton<DoubleType>::Solve(LinearSolver<DoubleType> &itermethod, const TimeMethods::TimeParams<DoubleType> &timeinfo, ObjectHolderMap_t *ohm)
{
  NodeKeeper &nk = NodeKeeper::instance();
  GlobalData &gdata = GlobalData::GetInstance();
  const GlobalData::DeviceList_t      &dlist = gdata.GetDeviceList();

  const size_t numeqns = NumberEquationsAndSetDimension();

  if (nk.HaveNodes())
  {
    nk.InitializeSolution("dcop");
  }

  PrintNumberEquations(numeqns, ohm);

  std::unique_ptr<Preconditioner<DoubleType>> preconditioner;

  std::unique_ptr<Matrix<DoubleType>> matrix;

  preconditioner = std::unique_ptr<Preconditioner<DoubleType>>(CreatePreconditioner(itermethod, numeqns));

  matrix = std::unique_ptr<Matrix<DoubleType>>(CreateMatrix(preconditioner.get()));

  std::vector<DoubleType> rhs(numeqns);

  bool converged = false;

  BackupSolutions();

  /////
  ///// Permutation vector
  /////
  permvec_t permvec(numeqns);
  for (size_t i = 0; i < permvec.size(); ++i)
  {
    permvec[i] = PermutationEntry(i, false);
  }

  std::vector<DoubleType> result(numeqns);

  std::vector<DoubleType> rhs_constant(numeqns);
  if (!timeinfo.IsDCMethod())
  {
    InitializeTransientAssemble(timeinfo, numeqns, rhs_constant);
  }
  

  LoadMatrixAndRHS(*matrix, rhs, permvec, dsMathEnum::WhatToLoad::PERMUTATIONSONLY, dsMathEnum::TimeMode::DC, static_cast<DoubleType>(1.0));

  size_t divergence_count = 0;
  DoubleType last_rel_err = 0.0;
  DoubleType last_abs_err = 0.0;

  ObjectHolderList_t iteration_list;

  for (size_t iter = 0; (iter < maxiter) && (!converged) && (divergence_count < 5); ++iter)
  {
    ObjectHolderMap_t iteration_map;
    ObjectHolderMap_t *p_iteration_map = nullptr;
    if (ohm)
    {
      p_iteration_map = &iteration_map;
    }

    rhs = rhs_constant;

//        std::cerr << "Begin Load Matrix\n";
    /// This is the resistive portion (always assembled
    if (timeinfo.IsDCOnly())
    {
      LoadMatrixAndRHS(*matrix, rhs, permvec, dsMathEnum::WhatToLoad::MATRIXANDRHS, dsMathEnum::TimeMode::DC, static_cast<DoubleType>(1.0));
    }
    else
    {
      LoadMatrixAndRHS(*matrix, rhs, permvec, dsMathEnum::WhatToLoad::MATRIXANDRHS, dsMathEnum::TimeMode::DC, static_cast<DoubleType>(1.0));

      /// This assembles the time derivative current 
      if (timeinfo.a0 != 0.0)
      {
        LoadMatrixAndRHS(*matrix, rhs, permvec, dsMathEnum::WhatToLoad::MATRIXANDRHS, dsMathEnum::TimeMode::TIME, timeinfo.a0);
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
      CallUpdateSolution(nk, "dcop", result);
      nk.TriggerCallbacksOnNodes();
    }

    PrintIteration(iter, p_iteration_map);
    {
      converged = true;
      GlobalData::DeviceList_t::const_iterator dit  = dlist.begin();
      GlobalData::DeviceList_t::const_iterator dend = dlist.end();
      ObjectHolderList_t dobjlist;
      for ( ; dit != dend; ++dit)
      {
        const std::string &name = dit->first;
        const Device &device = *(dit->second);
        const DoubleType devrerr = device.GetRelError<DoubleType>();
        const DoubleType devaerr = device.GetAbsError<DoubleType>();

        if (ohm)
        {
          ObjectHolderMap_t dmap;
          PrintDeviceErrors(device, &dmap);
          dobjlist.push_back(ObjectHolder(dmap));
        }
        else
        {
          PrintDeviceErrors(device, ohm);
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
      if (p_iteration_map)
      {
        (*p_iteration_map)["devices"] = ObjectHolder(dobjlist);
      }

      if (nk.HaveNodes())
      {
        const DoubleType cirrerr = nk.GetRelError("dcop");
        const DoubleType ciraerr = nk.GetAbsError("dcop");
        PrintCircuitErrors(p_iteration_map);
        converged = converged && (cirrerr < relLimit) && (ciraerr < absLimit);
      }
    }

    matrix->ClearMatrix();
    if (p_iteration_map)
    {
      iteration_list.push_back(ObjectHolder(iteration_map));
    }
  }

  std::vector<DoubleType> newQ;
  if (timeinfo.IsTransient())
  {
    //// New charge based on new assemble
    newQ.resize(numeqns);
    LoadMatrixAndRHS(*matrix, newQ, permvec, dsMathEnum::WhatToLoad::RHS, dsMathEnum::TimeMode::TIME, static_cast<DoubleType>(1.0));

    //// Check to see if your projection was correct
    if (timeinfo.IsIntegration())
    {
      converged = CheckTransientProjection(timeinfo, newQ);
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

    if (timeinfo.IsTransient())
    {
      UpdateTransientCurrent(timeinfo, numeqns, newQ, rhs);
    }

  }

  if (ohm)
  {
    (*ohm)["iterations"] = ObjectHolder(iteration_list);
  }

  return converged;
}

template <typename DoubleType>
void Newton<DoubleType>::PrintNumberEquations(size_t numeqns, ObjectHolderMap_t *ohm)
{
  std::ostringstream os; 
  os << "number of equations " << numeqns << "\n";
  OutputStream::WriteOut(OutputStream::OutputType::INFO, os.str());
  if (ohm)
  {
    (*ohm)["number_of_equations"] = ObjectHolder(static_cast<int>(numeqns));
  }
}

template <typename DoubleType>
void Newton<DoubleType>::PrintIteration(size_t iter, ObjectHolderMap_t *ohm)
{
  std::ostringstream os; 
  os << "Iteration: " << iter << "\n";
  OutputStream::WriteOut(OutputStream::OutputType::INFO, os.str());
  if (ohm)
  {
    (*ohm)["iteration"] = ObjectHolder(static_cast<int>(iter));
  }
}

template <typename DoubleType>
void Newton<DoubleType>::PrintCircuitErrors(ObjectHolderMap_t *ohm)
{
  NodeKeeper &nk = NodeKeeper::instance();
  const double cirrerr = nk.GetRelError("dcop");
  const double ciraerr = nk.GetAbsError("dcop");
  std::ostringstream os;
  os << "  Circuit: "
      << std::scientific << std::setprecision(5) <<
               "\tRelError: " << cirrerr <<
               "\tAbsError: " << ciraerr << "\n";
  OutputStream::WriteOut(OutputStream::OutputType::INFO, os.str());
  if (ohm)
  {
    ObjectHolderMap_t cir;
    cir["relative_error"] = ObjectHolder(cirrerr);
    cir["absolute_error"] = ObjectHolder(ciraerr);
    (*ohm)["circuit"] = ObjectHolder(cir);
  }
}

template <typename DoubleType>
void Newton<DoubleType>::PrintDeviceErrors(const Device &device, ObjectHolderMap_t *ohm)
{
  const std::string &name = device.GetName();
  const DoubleType devrerr = device.GetRelError<DoubleType>();
  const DoubleType devaerr = device.GetAbsError<DoubleType>();

  std::unique_ptr<ObjectHolderMap_t> dmap;
  std::unique_ptr<ObjectHolderList_t> rlist;
  std::unique_ptr<ObjectHolderList_t> elist;

  std::ostringstream os;

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
                 "\tRelError: " << region.GetRelError<DoubleType>() <<
                 "\tAbsError: " << region.GetAbsError<DoubleType>() << "\n";

    const EquationPtrMap_t &equations = region.GetEquationPtrList();
    for (EquationPtrMap_t::const_iterator eit = equations.begin(); eit != equations.end(); ++eit)
    {
      const EquationHolder &equation = (eit->second);
      os << "      Equation: \"" << equation.GetName() << "\""
          << std::scientific << std::setprecision(5) <<
                   "\tRelError: " << equation.GetRelError<DoubleType>() <<
                   "\tAbsError: " << equation.GetAbsError<DoubleType>() << "\n";

    }
  }
  OutputStream::WriteOut(OutputStream::OutputType::INFO, os.str());

  if (ohm)
  {
    ObjectHolderMap_t &dmap = *ohm;
    dmap["name"] = ObjectHolder(name);
    dmap["relative_error"] = ObjectHolder(static_cast<double>(devrerr));
    dmap["absolute_error"] = ObjectHolder(static_cast<double>(devaerr));

    ObjectHolderList_t rlist;
    for (Device::RegionList_t::const_iterator rit = regions.begin(); rit != regions.end(); ++rit)
    {
      const Region &region = *(rit->second);
      ObjectHolderMap_t rmap;
      rmap["name"] = ObjectHolder(region.GetName());
      rmap["relative_error"] = ObjectHolder(region.GetRelError<double>());
      rmap["absolute_error"] = ObjectHolder(region.GetAbsError<double>());

      ObjectHolderList_t elist;
      const EquationPtrMap_t &equations = region.GetEquationPtrList();
      for (EquationPtrMap_t::const_iterator eit = equations.begin(); eit != equations.end(); ++eit)
      {
        const EquationHolder &equation = (eit->second);
        ObjectHolderMap_t emap;
        emap["name"] = ObjectHolder(equation.GetName());
        emap["relative_error"] = ObjectHolder(equation.GetRelError<double>());
        emap["absolute_error"] = ObjectHolder(equation.GetAbsError<double>());
        elist.push_back(ObjectHolder(emap));
      }
      rmap["equations"] = ObjectHolder(elist);
      rlist.push_back(ObjectHolder(rmap));
    }
    dmap["regions"] = ObjectHolder(rlist);
  }
}

template <typename DoubleType>
void Newton<DoubleType>::InitializeTransientAssemble(const TimeMethods::TimeParams<DoubleType> &timeinfo, size_t numeqns, std::vector<DoubleType> &rhs_constant)
{
  TimeData<DoubleType> &tinst = TimeData<DoubleType>::GetInstance();
  if (timeinfo.a1 != 0.0)
  {
    tinst.AssembleQ(TimePoint_t::TM0, timeinfo.a1, rhs_constant);
  }
  if (timeinfo.a2 != 0.0)
  {
    tinst.AssembleQ(TimePoint_t::TM1, timeinfo.a2, rhs_constant);
  }

  if (timeinfo.b1 != 0.0)
  {
    tinst.AssembleI(TimePoint_t::TM0, timeinfo.b1, rhs_constant);
  }
  if (timeinfo.b2 != 0.0)
  {
    tinst.AssembleI(TimePoint_t::TM1, timeinfo.b2, rhs_constant);
  }
}

template <typename DoubleType>
bool Newton<DoubleType>::CheckTransientProjection(const TimeMethods::TimeParams<DoubleType> &timeinfo, const std::vector<DoubleType> &newQ)
{
  const size_t numeqns = newQ.size();
  bool converged = true;
  /// need to make sure projection makes sense
  std::vector<DoubleType> projectQ(numeqns);
  
  TimeData<DoubleType> &tinst = TimeData<DoubleType>::GetInstance();

  tinst.AssembleI(TimePoint_t::TM0, timeinfo.tdelta, projectQ);
  tinst.AssembleQ(TimePoint_t::TM0, 1.0,    projectQ);

  DoubleType qrel = 0.0;
  for (size_t i = 0; i < numeqns; ++i)
  {
    const DoubleType qproj = projectQ[i];
    const DoubleType qnew  = newQ[i];
    if (qnew != 0.0)
    {
      const DoubleType qr = abs(qnew - qproj)/(1.0e-20 + abs(qnew) + abs(qproj));
      if (qr > qrel)
      {
        qrel = qr;
      }
    }
  }
  std::ostringstream os;
  os << "Charge Relative Error " << std::scientific << std::setprecision(5) << qrel << "\n";
  OutputStream::WriteOut(OutputStream::OutputType::INFO, os.str());
  if (qrel > qrelLimit)
  {
    converged = false;
  }
  return converged;
}

template <typename DoubleType>
void Newton<DoubleType>::UpdateTransientCurrent(const TimeMethods::TimeParams<DoubleType> &timeinfo, size_t numeqns, const std::vector<DoubleType> &newQ, std::vector<DoubleType> &newI)
{
  newI.clear();
  newI.resize(numeqns);
  TimeData<DoubleType> &tinst = TimeData<DoubleType>::GetInstance();
  if (timeinfo.IsDCMethod())
  {
    //// There is no displacement current for a dc solution
    tinst.SetI(TimePoint_t::TM0, newI);
    tinst.SetQ(TimePoint_t::TM0, newQ);

    tinst.ClearI(TimePoint_t::TM1);
    tinst.ClearI(TimePoint_t::TM2);
    tinst.ClearQ(TimePoint_t::TM1);
    tinst.ClearQ(TimePoint_t::TM2);
  }
  else
  {
    ////// Charge

    if (tinst.ExistsQ(TimePoint_t::TM1))
    {
      tinst.CopyQ(TimePoint_t::TM1, TimePoint_t::TM2);
    }
    if (tinst.ExistsQ(TimePoint_t::TM0))
    {
      tinst.CopyQ(TimePoint_t::TM0, TimePoint_t::TM1);
    }

    tinst.SetQ(TimePoint_t::TM0, newQ);


    ////// Current
    if (tinst.ExistsI(TimePoint_t::TM1))
    {
      tinst.CopyI(TimePoint_t::TM1, TimePoint_t::TM2);
    }
    if (tinst.ExistsI(TimePoint_t::TM0))
    {
      tinst.CopyI(TimePoint_t::TM0, TimePoint_t::TM1);
    }

    ////// Update Current
    newI.clear();
    newI.resize(numeqns);

    if (timeinfo.a0 != 0.0)
    {
      tinst.AssembleQ(TimePoint_t::TM0, timeinfo.a0, newI);
    }
    if (timeinfo.a1 != 0.0)
    {
      tinst.AssembleQ(TimePoint_t::TM1, timeinfo.a1, newI);
    }
    if (timeinfo.a2 != 0.0)
    {
      tinst.AssembleQ(TimePoint_t::TM2, timeinfo.a2, newI);
    }

    if (timeinfo.b1 != 0.0)
    {
      tinst.AssembleI(TimePoint_t::TM1, timeinfo.b1, newI);
    }
    if (timeinfo.b2 != 0.0)
    {
      tinst.AssembleI(TimePoint_t::TM2, timeinfo.b2, newI);
    }

    tinst.SetI(TimePoint_t::TM0, newI);
  }
}

template <typename DoubleType>
bool Newton<DoubleType>::ACSolve(LinearSolver<DoubleType> &itermethod, DoubleType frequency)
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


  std::unique_ptr<Preconditioner<DoubleType>> preconditioner(CreateACPreconditioner<DoubleType>(PEnum::TransposeType_t::NOTRANS, numeqns));

  std::unique_ptr<Matrix<DoubleType>> matrix(CreateACMatrix<DoubleType>(preconditioner.get()));

  std::vector<std::complex<DoubleType>> rhs(numeqns);

  /// This is to initialize other copies
  permvec_t permvec_temp(numeqns);

  for (size_t i = 0; i < permvec_temp.size(); ++i)
  {
    permvec_temp[i] = PermutationEntry(i, false);
  }

  std::vector<std::complex<DoubleType>> result(numeqns);

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
      CallACUpdateSolution(nk, "ssac_real", "ssac_imag", result);
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
            const DoubleType devrerr = dev->GetRelError();
            const DoubleType devaerr = dev->GetAbsError();
            os << "\tDevice: \"" << name << "\""
                << std::scientific << std::setprecision(5) <<
                         "\tRelError: " << devrerr <<
                         "\tAbsError: " << devaerr << "\n";

        }

        if (nk.HaveNodes())
        {
            const DoubleType cirrerr = nk.GetRelError("ssac_real");
            const DoubleType ciraerr = nk.GetAbsError("ssac_real");
            os << "  Circuit: "
                << std::scientific << std::setprecision(5) <<
                         "\tRelError: " << cirrerr <<
                         "\tAbsError: " << ciraerr << "\n";
        }
#endif
      OutputStream::WriteOut(OutputStream::OutputType::INFO, os.str());
    }


    //// ultimately we need to make this a getter
    if (nk.HaveNodes())
    {
      nk.ACPrintSolution("ssac_real", "ssac_imag");
    }
  } while (0);

  return converged;
}

template <typename DoubleType>
bool Newton<DoubleType>::NoiseSolve(const std::string &output_name, LinearSolver<DoubleType> &itermethod, DoubleType frequency)
{

  NodeKeeper &nk = NodeKeeper::instance();
  const size_t numeqns = NumberEquationsAndSetDimension();

  size_t outputeqnnum = size_t(-1);

  if (!nk.HaveNodes())
  {
    std::ostringstream os;
    os << "A circuit is required for a noise solve.\n";
    OutputStream::WriteOut(OutputStream::OutputType::ERROR, os.str());
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
    OutputStream::WriteOut(OutputStream::OutputType::ERROR, os.str());
    return false;
    //// Should probably abort here
  }
  else
  {
    std::ostringstream os;
    os << "Circuit output " << output_name << " has equation " << outputeqnnum << ".\n";
    OutputStream::WriteOut(OutputStream::OutputType::INFO, os.str());
  }

  GlobalData &gdata = GlobalData::GetInstance();
  const GlobalData::DeviceList_t      &dlist = gdata.GetDeviceList();

  const std::string circuit_real_name = std::string("noise_") + output_name + "_real";
  const std::string circuit_imag_name = std::string("noise_") + output_name + "_imag";

  nk.InitializeSolution(circuit_real_name);
  nk.InitializeSolution(circuit_imag_name);
  nk.InitializeSolution("dcop");

  std::unique_ptr<Preconditioner<DoubleType>> preconditioner(CreateACPreconditioner<DoubleType>(PEnum::TransposeType_t::TRANS, numeqns));

  std::unique_ptr<Matrix<DoubleType>> matrix(CreateACMatrix<DoubleType>(preconditioner.get()));

  std::vector<std::complex<DoubleType>> rhs(numeqns);

  /// This is to initialize other copies
  permvec_t permvec_temp(numeqns);

  for (size_t i = 0; i < permvec_temp.size(); ++i)
  {
      permvec_temp[i] = PermutationEntry(i, false);
  }

  std::vector<std::complex<DoubleType>> result(numeqns);

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

    CallACUpdateSolution(nk, circuit_real_name, circuit_imag_name, result);


    {
      std::ostringstream os;
      os << "Noise Iteration:\n";
      os << "number of equations " << numeqns << "\n";
      //// TODO: more meaningful reporting

      OutputStream::WriteOut(OutputStream::OutputType::INFO, os.str());
    }
  } while (0);

  //// TODO: have some kind of accuracy check for iterative solvers
  return converged;
}

template <typename DoubleType>
void Newton<DoubleType>::AssembleTclEquations(RealRowColValueVec<DoubleType> &mat, RHSEntryVec<DoubleType> &rhs, dsMathEnum::WhatToLoad w, dsMathEnum::TimeMode t)
{
  GlobalData &gdata = GlobalData::GetInstance();

  const GlobalData::TclEquationList_t &tlist = gdata.GetTclEquationList();

  std::vector<std::string> arguments;
  arguments.reserve(2);
  arguments.push_back(dsMathEnum::WhatToLoadString[static_cast<size_t>(w)]);
  arguments.push_back(dsMathEnum::TimeModeString[static_cast<size_t>(t)]);

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
      OutputStream::WriteOut(OutputStream::OutputType::FATAL, os.str().c_str());
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
        OutputStream::WriteOut(OutputStream::OutputType::FATAL, os.str().c_str());
      }

      ObjectHolder rhs_objects = objects[1];
      ObjectHolder matrix_objects = objects[0];

      //// Read in the entries here
      if (w != dsMathEnum::WhatToLoad::MATRIXONLY)
      {
        ok = rhs_objects.GetListOfObjects(objects);
        size_t len = objects.size();
  
        if ((!ok) || ((len % 2) != 0))
        {
          std::ostringstream os; 
          os << "Error when evaluating custom_equation \"" << name << "\" evaluating Tcl Procedure \"" << proc << "\" rhs entry list of length \"" << len << "\" is not divisible by 2\n";
          OutputStream::WriteOut(OutputStream::OutputType::FATAL, os.str().c_str());
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
              OutputStream::WriteOut(OutputStream::OutputType::FATAL, os.str().c_str());
            }

          }
        }
      }

      if (w != dsMathEnum::WhatToLoad::RHS)
      {
        ok = matrix_objects.GetListOfObjects(objects);
        size_t len = objects.size();

        if ((!ok) || ((len % 3) != 0))
        {
          std::ostringstream os; 
          os << "Error when evaluating custom_equation \"" << name << "\" evaluating Tcl Procedure \"" << proc << "\" matrix entry list of length \"" << len << "\" is not divisible by 3\n";
          OutputStream::WriteOut(OutputStream::OutputType::FATAL, os.str().c_str());
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
              mat.push_back(RealRowColVal<DoubleType>(row.second, col.second, val.second));
            }
            else
            {
              std::ostringstream os; 
              os << "Error when evaluating custom_equation \"" << name << "\" evaluating Tcl Procedure \"" << proc << "\" matrix entry " <<
                objects[i].GetString() << " " << objects[i+1].GetString() << " " << objects[i+2].GetString() << "\n";
              OutputStream::WriteOut(OutputStream::OutputType::FATAL, os.str().c_str());
            }
          }
        }
      }
    }
  }
}

template class Newton<double>;
#ifdef DEVSIM_EXTENDED_PRECISION
template class Newton<float128>;
#endif
}/// end namespace dsMath

