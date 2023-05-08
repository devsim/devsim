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

