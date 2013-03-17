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

#include "SuperLUData.hh"
#include "dsMathTypes.hh"
#include "CompressedMatrix.hh"
#include "dsAssert.hh"
#include "OutputStream.hh"

#include "slu_zdefs.h"

namespace dsMath {
bool SuperLUData::LUFactorComplexMatrix(CompressedMatrix *cm)
{
  CompressedMatrix::SymbolicStatus_t sstatus= cm->GetSymbolicStatus();

  const int n = numeqns_;

  int *perm_c = perm_c_; /* column permutation vector */
  int *etree  = etree_;  /* column elimination tree */

  if (perm_c_ && (sstatus == CompressedMatrix::SAME_SYMBOLIC))
  {
    //// This is so it doesn't get deleted by DeleteStorage
    perm_c_ = NULL;
    etree_  = NULL;
  }
  else
  {
    perm_c = intMalloc(n+1);
    etree  = intMalloc(n+1);
  }

  DeleteStorage();

  const IntVec_t    &Cols = cm->GetCols();
  const IntVec_t    &Rows = cm->GetRows();
  const ComplexDoubleVec_t &Vals = cm->GetComplex();

  const int nnz = Rows.size();

  SuperMatrix A, AC;
  SuperMatrix *L, *U;
  int *perm_r; /* row permutations from partial pivoting */

  int      panel_size, permc_spec, relax;

  superlu_options_t options;

  SuperLUStat_t stat;

  /* Set the default input options. */
  if (lutype_ == Preconditioner::FULL)
  {
    set_default_options(&options);
  }
  else if (lutype_ == Preconditioner::INCOMPLETE)
  {
    ilu_set_default_options(&options);
  }

//  options.ColPerm = NATURAL;
//  options.ColPerm = MMD_ATA;
  options.ColPerm = COLAMD;
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
  doublecomplex * const vals = const_cast<doublecomplex *>(reinterpret_cast<const doublecomplex *>(&Vals[0]));

  zCreate_CompCol_Matrix(&A, n, n, nnz, vals, rows, cols,
                         SLU_NC, SLU_Z, SLU_GE);
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
  
  if (sstatus == CompressedMatrix::NEW_SYMBOLIC)
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
    //// need to do memory cleanup
    int check = check_perm(n, perm_c);
    if (check != -1)
    {
      std::ostringstream os;
      os << "Matrix Permutation failed!! << perm[" << check << "] = " << perm_c[check] << "\n";
      OutputStream::WriteOut(OutputStream::FATAL, os.str());
    }
  }
#endif

  panel_size = sp_ienv(1);
  relax = sp_ienv(2);

  if (lutype_ == Preconditioner::FULL)
  {
    zgstrf(&options, &AC, relax, panel_size, 
           etree, NULL, 0, perm_c, perm_r, L, U, &stat, &info_);
  }
  else if (lutype_ ==  Preconditioner::INCOMPLETE)
  {
    zgsitrf(&options, &AC, relax, panel_size,
           etree, NULL, 0, perm_c, perm_r, L, U, &stat, &info_);
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

void SuperLUData::LUSolve(ComplexDoubleVec_t &x, const ComplexDoubleVec_t &b)
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

  dsAssert(static_cast<size_t>(n) == b.size(), "UNEXPECTED");

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

  zCreate_Dense_Matrix(&B, n, nrhs, reinterpret_cast<doublecomplex *>(&x[0]), n, SLU_DN, SLU_Z, SLU_GE);

  /* Solve the system A*X=B, overwriting B with X. */
  zgstrs (trans, L, U, perm_c, perm_r, &B, &stat, &info_);

  Destroy_SuperMatrix_Store(&B);
  StatFree(&stat);
}

}

