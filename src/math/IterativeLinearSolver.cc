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

#include "IterativeLinearSolver.hh"
#include "Preconditioner.hh"

#include "OutputStream.hh"
#include "DenseMatrix.hh"
#include "gmres.hh"

#include <sstream>

//#include <iostream>
namespace dsMath {
IterativeLinearSolver::IterativeLinearSolver() : restart_(50), linear_iterations_(100), relative_tolerance_(1e-20)
{}

bool IterativeLinearSolver::SolveImpl(Matrix &mat, Preconditioner &pre, DoubleVec_t &sol, DoubleVec_t &rhs)
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
      OutputStream::WriteOut(OutputStream::INFO, os.str());
  }
  else
  {
    std::ostringstream os;
    os << "Matrix factorization failed\n";
    OutputStream::WriteOut(OutputStream::ERROR, os.str());
  }
//std::cerr << "End LUFactor Matrix\n";

  return ret;
}

bool IterativeLinearSolver::ACSolveImpl(Matrix &mat, Preconditioner &pre, ComplexDoubleVec_t &sol, ComplexDoubleVec_t &rhs)
{
  bool ret = false;
  {
    std::ostringstream os;
    os << "AC iterative solve not implemented\n";
    OutputStream::WriteOut(OutputStream::ERROR, os.str());
  }
  return ret;
}

bool IterativeLinearSolver::NoiseSolveImpl(Matrix &mat, Preconditioner &pre, ComplexDoubleVec_t &sol, ComplexDoubleVec_t &rhs)
{
  bool ret = false;
  {
    std::ostringstream os;
    os << "Noise iterative solve not implemented\n";
    /// requires working transpose
    OutputStream::WriteOut(OutputStream::ERROR, os.str());
  }
  return ret;
}
}

