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

#include "SuperLUData.hh"
#include "dsMathTypes.hh"
#include "CompressedMatrix.hh"
#include "dsAssert.hh"
#include "OutputStream.hh"

#include "slu_ddefs.h"

#ifdef DEVSIM_EXTENDED_PRECISION
#include "Float128.hh"
#endif

namespace dsMath {

#if 0
int check_perm(const int n, const int * const perm)
{
  int ret = -1;
  std::vector<int> marker(n);

  for (int i = 0; i < n; ++i)
  {
    if ( marker[perm[i]] == 1 || perm[i] >= n ) {
      ret = i;
      break;
    }
    else
    {
      marker[perm[i]] = 1;
    }
  }

  return ret;
}
#endif


SuperLUData::SuperLUData(size_t numeqns, bool tran, PEnum::LUType_t lutype) : numeqns_(numeqns), transpose_(tran), lutype_(lutype), perm_r_(nullptr), perm_c_(nullptr), etree_(nullptr), L_(nullptr), U_(nullptr), info_(0)
{
}

SuperLUData::~SuperLUData()
{
  DeleteStorage();
}

template <>
bool SuperLUData::LUFactorMatrix(CompressedMatrix<double> *cm)
{
  bool ret = false;
  if (cm->GetMatrixType() == MatrixType::REAL)
  {
    ret = LUFactorRealMatrix(cm, cm->GetReal());
  }
  else if (cm->GetMatrixType() == MatrixType::COMPLEX)
  {
    ret = LUFactorComplexMatrix(cm, cm->GetComplex());
  }
  return ret;
}

#ifdef DEVSIM_EXTENDED_PRECISION
template <>
bool SuperLUData::LUFactorMatrix(CompressedMatrix<float128> *cm)
{
  bool ret = false;
  if (cm->GetMatrixType() == MatrixType::REAL)
  {
    const DoubleVec_t<float128> &fvals = cm->GetReal();
    DoubleVec_t<double> dvals(fvals.size());
    for (size_t i = 0; i < fvals.size(); ++i)
    {
      dvals[i] = static_cast<double>(fvals[i]);
    }
    ret = LUFactorRealMatrix(cm, dvals);
  }
  else if (cm->GetMatrixType() == MatrixType::COMPLEX)
  {
    const ComplexDoubleVec_t<float128> &fvals = cm->GetComplex();
    ComplexDoubleVec_t<double> dvals(fvals.size());
    for (size_t i = 0; i < fvals.size(); ++i)
    {
      dvals[i] = ComplexDouble_t<double>(static_cast<double>(fvals[i].real()), static_cast<double>(fvals[i].imag()));
    }
    ret = LUFactorComplexMatrix(cm, dvals);
  }
  return ret;
}
#endif

template <typename DoubleType>
bool SuperLUData::LUFactorRealMatrix(CompressedMatrix<DoubleType> *cm, const DoubleVec_t<double> &Vals)
{
  SymbolicStatus_t sstatus= cm->GetSymbolicStatus();

  const int n = numeqns_;

  int *perm_c = perm_c_; /* column permutation vector */
  int *etree  = etree_;  /* column elimination tree */

  if (perm_c_ && (sstatus == SymbolicStatus_t::SAME_SYMBOLIC))
  {
    //// This is so it doesn't get deleted by DeleteStorage
    perm_c_ = nullptr;
    etree_  = nullptr;
  }
  else
  {
    perm_c = intMalloc(n+1);
    etree  = intMalloc(n+1);
  }

  DeleteStorage();

  const IntVec_t    &Cols = cm->GetCols();
  const IntVec_t    &Rows = cm->GetRows();

  const int nnz = Rows.size();

  SuperMatrix A, AC;
  SuperMatrix *L, *U;
  int *perm_r; /* row permutations from partial pivoting */

  int      panel_size, permc_spec, relax;

  superlu_options_t options;

  SuperLUStat_t stat;

  /* Set the default input options. */
  if (lutype_ == PEnum::LUType_t::FULL)
  {
    set_default_options(&options);
  }
  else if (lutype_ == PEnum::LUType_t::INCOMPLETE)
  {
    ilu_set_default_options(&options);
  }

//  options.ColPerm = NATURAL;
  options.ColPerm = MMD_ATA;
//  options.ColPerm = COLAMD;
  if (transpose_)
  {
    options.Trans = TRANS;
  }
  else
  {
    options.Trans = NOTRANS;
  }


  /* Initialize the statistics variables. */
  StatInit(&stat);

  int * const rows = const_cast<int *>(&Rows[0]);
  int * const cols = const_cast<int *>(&Cols[0]);
  double * const vals = const_cast<double *>(&Vals[0]);

  dCreate_CompCol_Matrix(&A, n, n, nnz, vals, rows, cols,
                         SLU_NC, SLU_D, SLU_GE);
  L = (SuperMatrix *) SUPERLU_MALLOC( sizeof(SuperMatrix) );
  U = (SuperMatrix *) SUPERLU_MALLOC( sizeof(SuperMatrix) );
  perm_r = intMalloc(n+1);

  /*
   * Get column permutation vector perm_c[], according to permc_spec:
   *   permc_spec = 0: natural ordering
   *   permc_spec = 1: minimum degree on structure of A'*A
   *   permc_spec = 2: minimum degree on structure of A'+A
   *   permc_spec = 3: approximate minimum degree for unsymmetric matrices
   */
  if (sstatus == SymbolicStatus_t::NEW_SYMBOLIC)
  {
    permc_spec = options.ColPerm;
    get_perm_c(permc_spec, &A, perm_c);
  }
  else
  {
    options.Fact = SamePattern;
  }
  sp_preorder(&options, &A, perm_c, etree, &AC);

#if 0
  {
    int check = check_perm(n, perm_c);
    if (check != -1)
    {
      //// need to do memory cleanup
      std::ostringstream os;
      os << "Matrix Permutation failed!! << perm[" << check << "] = " << perm_c[check] << "\n";
      OutputStream::WriteOut(OutputStream::OutputType::FATAL, os.str());
    }
  }
#endif

  panel_size = sp_ienv(1);
  relax = sp_ienv(2);

#if SUPERLU_MAJOR_VERSION == 5
  GlobalLU_t Glu;
#endif

  if (lutype_ == PEnum::LUType_t::FULL)
  {
  #if SUPERLU_MAJOR_VERSION == 5
    dgstrf(&options, &AC, relax, panel_size,
           etree, nullptr, 0, perm_c, perm_r, L, U, &Glu, &stat, &info_);
  #else
    dgstrf(&options, &AC, relax, panel_size,
           etree, nullptr, 0, perm_c, perm_r, L, U, &stat, &info_);
  #endif
  }
  else if (lutype_ == PEnum::LUType_t::INCOMPLETE)
  {
  #if SUPERLU_MAJOR_VERSION == 5
    dgsitrf(&options, &AC, relax, panel_size,
           etree, nullptr, 0, perm_c, perm_r, L, U, &Glu, &stat, &info_);
  #else
    dgsitrf(&options, &AC, relax, panel_size,
           etree, nullptr, 0, perm_c, perm_r, L, U, &stat, &info_);
  #endif
  }


//  dsAssert(info == 0, "MATRIX FACTORIZATION FAILED");

#if 0
  if ( *info == 0 ) {
      Lstore = (SCformat *) L->Store;
      Ustore = (NCformat *) U->Store;
      printf("No of nonzeros in factor L = %d\n", Lstore->nnz);
      printf("No of nonzeros in factor U = %d\n", Ustore->nnz);
      printf("No of nonzeros in L+U = %d\n", Lstore->nnz + Ustore->nnz);
      dQuerySpace(L, U, &mem_usage);
      printf("L\\U MB %.3f\ttotal MB needed %.3f\texpansions %d\n",
             mem_usage.for_lu/1e6, mem_usage.total_needed/1e6,
             mem_usage.expansions);
  } else {
      printf("dgstrf() error returns INFO= %d\n", *info);
      if ( *info <= *n ) { /* factorization completes */
          dQuerySpace(L, U, &mem_usage);
          printf("L\\U MB %.3f\ttotal MB needed %.3f\texpansions %d\n",
                 mem_usage.for_lu/1e6, mem_usage.total_needed/1e6,
                 mem_usage.expansions);
      }
  }
#endif

  /* Save the LU factors in the factors handle */
  L_ = L;
  U_ = U;
  perm_c_ = perm_c;
  etree_ =  etree;
  perm_r_ = perm_r;

  /* Free un-wanted storage */
//  SUPERLU_FREE(etree);
  Destroy_SuperMatrix_Store(&A);
  Destroy_CompCol_Permuted(&AC);
  StatFree(&stat);

  return (info_ == 0);
}

template <>
void SuperLUData::LUSolve(DoubleVec_t<double> &x, const DoubleVec_t<double> &b)
{
  if (info_ == 0)
  {
    x = b;
  }
  else
  {
    x.clear();
    x.resize(numeqns_);
    return;
  }

/*
  SuperMatrix A, AC, B;
*/
  SuperMatrix B;
  SuperMatrix *L, *U;
  int *perm_r; /* row permutations from partial pivoting */
  int *perm_c; /* column permutation vector */

  trans_t  trans;

  const int nrhs = 1;
  const int n = numeqns_;
  dsAssert(static_cast<size_t>(n) == x.size(), "UNEXPECTED");

  SuperLUStat_t stat;

  if (transpose_)
  {
    trans = TRANS;
  }
  else
  {
    trans = NOTRANS;
  }

  /* Initialize the statistics variables. */
  StatInit(&stat);

  /* Extract the LU factors in the factors handle */
  L = L_;
  U = U_;
  perm_c = perm_c_;
  perm_r = perm_r_;

  dCreate_Dense_Matrix(&B, n, nrhs, &x[0], n, SLU_DN, SLU_D, SLU_GE);

  /* Solve the system A*X=B, overwriting B with X. */
  dgstrs (trans, L, U, perm_c, perm_r, &B, &stat, &info_);

  Destroy_SuperMatrix_Store(&B);
  StatFree(&stat);
}

#ifdef DEVSIM_EXTENDED_PRECISION
template <>
void SuperLUData::LUSolve(DoubleVec_t<float128> &x, const DoubleVec_t<float128> &b)
{
  DoubleVec_t<double> b64(b.size());
  DoubleVec_t<double> x64;
  for (size_t i = 0; i < b.size(); ++i)
  {
    b64[i] = static_cast<double>(b[i]);
  }
  this->LUSolve(x64, b64);

  x.resize(x64.size());
  for (size_t i = 0; i < x64.size(); ++i)
  {
    x[i] = static_cast<float128>(x64[i]);
  }
}
#endif

void SuperLUData::DeleteStorage()
{
  if (perm_r_)
  {
    SUPERLU_FREE (perm_r_);
    perm_r_ = nullptr;
  }

  if (perm_c_)
  {
    SUPERLU_FREE (perm_c_);
    perm_c_ = nullptr;
  }

  if (etree_)
  {
    SUPERLU_FREE (etree_);
    etree_ = nullptr;
  }

  if (L_)
  {
    Destroy_SuperNode_Matrix(L_);
    SUPERLU_FREE (L_);
    L_ = nullptr;
  }

  if (U_)
  {
    Destroy_CompCol_Matrix(U_);
    SUPERLU_FREE (U_);
    U_ = nullptr;
  }
}
}

#if 0
// Specialized above
template bool dsMath::SuperLUData::LUFactorMatrix(CompressedMatrix<double> *cm);
#ifdef DEVSIM_EXTENDED_PRECISION
template bool dsMath::SuperLUData::LUFactorMatrix(CompressedMatrix<float128> *cm);
#endif
#endif

#ifdef SUPERLU_MAJOR_VERSION
extern "C" {
extern const char * const devsim_superluversion = TOSTRING(SUPERLU_MAJOR_VERSION) "." TOSTRING(SUPERLU_MINOR_VERSION) "."  TOSTRING(SUPERLU_PATCH_VERSION);
}
#endif


