/***
DEVSIM
Copyright 2013 DEVSIM LLC

SPDX-License-Identifier: Apache-2.0
***/

#ifndef SOLVER_UTIL_HH
#define SOLVER_UTIL_HH
#include <cstddef>

namespace dsMath
{
namespace PEnum {
enum class TransposeType_t;
enum class LUType_t;
}

template <typename T> class LinearSolver;
template <typename T> class Preconditioner;
template <typename T> class CompressedMatrix;

template <typename T>
Preconditioner<T> *CreateDirectPreconditioner(size_t numeqns);

template <typename T>
Preconditioner<T> *CreatePreconditioner(LinearSolver<T> &itermethod, size_t numeqns);

template <typename T>
Preconditioner<T> *CreateACPreconditioner(PEnum::TransposeType_t trans_type, size_t numeqns);

template <typename T>
CompressedMatrix<T> *CreateMatrix(Preconditioner<T> *preconditioner, bool is_complex=false);

template <typename T>
CompressedMatrix<T> *CreateACMatrix(Preconditioner<T> *preconditioner);
} 

#endif

