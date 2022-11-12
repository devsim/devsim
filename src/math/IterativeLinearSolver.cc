/***
DEVSIM
Copyright 2013 DEVSIM LLC

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

#include "IterativeLinearSolver.hh"
#include "Preconditioner.hh"

#include "OutputStream.hh"
#include "gmres.hh"

#ifdef DEVSIM_EXTENDED_PRECISION
#include "Float128.hh"
#endif

#include <sstream>


//#include <iostream>
namespace dsMath {
template <typename DoubleType>
IterativeLinearSolver<DoubleType>::IterativeLinearSolver() : restart_(50), linear_iterations_(100), relative_tolerance_(1e-20)
{}

template <>
bool IterativeLinearSolver<double>::SolveImpl(Matrix<double> &mat, Preconditioner<double> &pre, std::vector<double> &sol, std::vector<double> &rhs)
{

  bool ret = false;
//std::cerr << "Begin LUFactor Matrix\n";
  ret = pre.LUFactor(&mat);
//std::cerr << "End LUFactor Matrix\n";
//std::cerr << "Begin LUSolve Matrix\n";
  if (ret)
  {
    int m = restart_;
    int iter = linear_iterations_;
    double tol = relative_tolerance_;
    int ret = GMRES(mat, sol, rhs, pre, m, iter, tol);
    std::ostringstream os;
    os
      << "GMRES back vectors " << m
      << "/" << restart_
      << " linear iterations " << iter
      << "/" << linear_iterations_
      << " relative tolerance " << tol
      << "/" << relative_tolerance_
      << " linear convergence " << ret
      << "\n";
      OutputStream::WriteOut(OutputStream::OutputType::INFO, os.str());
  }
  else
  {
    std::ostringstream os;
    os << "Matrix factorization failed\n";
    OutputStream::WriteOut(OutputStream::OutputType::ERROR, os.str());
  }
//std::cerr << "End LUFactor Matrix\n";

  return ret;
}

#ifdef DEVSIM_EXTENDED_PRECISION
template <>
bool IterativeLinearSolver<float128>::SolveImpl(Matrix<float128> &mat, Preconditioner<float128> &pre, std::vector<float128> &sol, std::vector<float128> &rhs)
{
  bool ret = false;
  std::ostringstream os;
  os << "GMRES not implemented for extended precision\n";
  OutputStream::WriteOut(OutputStream::OutputType::ERROR, os.str());
  return ret;
}
#endif

template <typename DoubleType>
bool IterativeLinearSolver<DoubleType>::ACSolveImpl(Matrix<DoubleType> &mat, Preconditioner<DoubleType> &pre, std::vector<std::complex<DoubleType>> &sol, std::vector<std::complex<DoubleType>> &rhs)
{
  bool ret = false;
  {
    std::ostringstream os;
    os << "AC iterative solve not implemented\n";
    OutputStream::WriteOut(OutputStream::OutputType::ERROR, os.str());
  }
  return ret;
}

template <typename DoubleType>
bool IterativeLinearSolver<DoubleType>::NoiseSolveImpl(Matrix<DoubleType> &mat, Preconditioner<DoubleType> &pre, std::vector<std::complex<DoubleType>> &sol, std::vector<std::complex<DoubleType>> &rhs)
{
  bool ret = false;
  {
    std::ostringstream os;
    os << "Noise iterative solve not implemented\n";
    /// requires working transpose
    OutputStream::WriteOut(OutputStream::OutputType::ERROR, os.str());
  }
  return ret;
}
}

template class dsMath::IterativeLinearSolver<double>;
#ifdef DEVSIM_EXTENDED_PRECISION
#include "Float128.hh"
template class dsMath::IterativeLinearSolver<float128>;
#endif

