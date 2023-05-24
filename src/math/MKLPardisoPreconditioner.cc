/***
DEVSIM
Copyright 2018 DEVSIM LLC

SPDX-License-Identifier: Apache-2.0
***/


#include "MKLPardisoPreconditioner.hh"
#include "CompressedMatrix.hh"
#include "dsAssert.hh"
#include "OutputStream.hh"
#include <utility>
#include <complex>
#include <vector>

extern "C" {
void PARDISO( void *a, const int *b, const int *c, const int *d,
                   const int *e, const int *f, const void *g, const int *h,
                   const int *i, int *j, const int *k, int *l,
                   const int *m, void *n,    void *o, int *p);
}

#ifdef DEVSIM_EXTENDED_PRECISION
#include "Float128.hh"
#endif

// USING CCM until we can find out what is wrong with CRM
#define USE_CCM

//#define DEBUG_MATRIX
#ifdef DEBUG_MATRIX
#include <fstream>
#endif

namespace dsMath {

class MKLPardisoData {
  public:

  explicit MKLPardisoData(int numeqn);

  ~MKLPardisoData();

  void SetTranspose(bool trans);

  template <typename DoubleType>
  void SetComplex(CompressedMatrix<DoubleType> *cm);

  template <typename DoubleType>
  bool LUFactorMatrix(CompressedMatrix<DoubleType> *);

  template <typename DoubleType>
  void LUSolve(DoubleVec_t<DoubleType> &/*x*/, const DoubleVec_t<DoubleType> &/*b*/);

  template <typename DoubleType>
  void LUSolve(ComplexDoubleVec_t<DoubleType> &/*x*/, const ComplexDoubleVec_t<DoubleType> &/*b*/);


  protected:
    template <typename DoubleType>
    bool LUFactorMatrixImpl(CompressedMatrix<DoubleType> *, const void *);

    void LUSolveImpl(void *x, const void *b);


  int iparm[64];
  void *pt[64];
  double ddum;
  int idum;
  int maxfct;
  int mnum;
  int msglvl;
  int error;
  int phase;
  int mtype;
  int n;
  int nrhs;
  const int *ia;
  const int *ja;
  const void *a;
};

MKLPardisoData::MKLPardisoData(int numeqn) {
  for (size_t i = 0; i < 64; ++i)
  {
    iparm[i] = 0;
  }

  iparm[0] = 1;         /* No solver default */
//  iparm[1] = 0;         /* Fill-in reordering from METIS */
  iparm[1] = 2;         /* Fill-in reordering from METIS */
  iparm[3] = 0;         /* No iterative-direct algorithm */
  iparm[4] = 0;         /* No user fill-in reducing permutation */
  iparm[5] = 0;         /* Write solution into x */
  iparm[6] = 0;         /* Not in use */
  iparm[7] = 0;         /* Max numbers of iterative refinement steps */
  iparm[8] = 0;         /* Not in use */
  iparm[9] = 13;        /* Perturb the pivot elements with 1E-13 */
  iparm[10] = 1;        /* Use nonsymmetric permutation and scaling MPS */
  iparm[11] = 0;        /* Conjugate transposed/transpose solve */
  iparm[12] = 1;        /* Maximum weighted matching algorithm is switched-on (default for non-symmetric) */
  iparm[13] = 0;        /* Output: Number of perturbed pivots */
  iparm[14] = 0;        /* Not in use */
  iparm[15] = 0;        /* Not in use */
  iparm[16] = 0;        /* Not in use */
  iparm[17] = -1;       /* Output: Number of nonzeros in the factor LU */
  iparm[18] = -1;       /* Output: Mflops for LU factorization */
  iparm[19] = 0;        /* Output: Numbers of CG Iterations */
//#warning "set to 0 after done debugging"
  iparm[26] = 0;        /* matrix checker */
  iparm[27] = 0;        /* double precision */
  iparm[34] = 1;        /* 0 based solve */

  mtype = 11;       /* Real unsymmetric matrix */
  maxfct = 1;           /* Maximum number of numerical factorizations. */
  mnum = 1;         /* Which factorization to use. */
  msglvl = 0;           /* Print statistical information  */
  error = 0;            /* Initialize error flag */
  phase = 0;
  ddum = 0.0;
  idum = 0;
  n = numeqn;
  nrhs = 1;

  ia = nullptr;
  ja = nullptr;
  a  = nullptr;

  for (size_t i = 0; i < 64; ++i)
  {
    pt[i] = nullptr;
  }
}


MKLPardisoData::~MKLPardisoData() {
  phase = -1;           /* Release internal memory. */

  PARDISO (pt, &maxfct, &mnum, &mtype, &phase,
           &n, a, ia, ja, &idum, &nrhs,
           &iparm[0], &msglvl, &ddum, &ddum, &error);
}

void MKLPardisoData::SetTranspose(bool trans) {
  // for IFM we use transpose, not conjugate transpose
  if (trans)
  {
#ifdef USE_CCM
    iparm[11] = 0;
#else
    iparm[11] = 2;
#endif
  }
  else
  {
#ifdef USE_CCM
    iparm[11] = 2;
#else
    iparm[11] = 0;
#endif
  }
}

template <typename DoubleType>
void MKLPardisoData::SetComplex(CompressedMatrix<DoubleType> *cm)
{
  if (cm->GetMatrixType() == MatrixType::COMPLEX)
  {
    mtype = 13;
  }
  else if (cm->GetMatrixType() == MatrixType::REAL)
  {
    mtype = 11;
  }
  else
  {
    dsAssert(cm, "UNEXPECTED");
  }
}

template <typename DoubleType>
bool MKLPardisoData::LUFactorMatrixImpl(CompressedMatrix<DoubleType> *cm, const void *a_input)
{
#ifdef DEBUG_MATRIX
  std::ofstream myfile;
#ifdef USE_CCM
  myfile.open("matrixdebug_ccm.out");
#else
  myfile.open("matrixdebug_crm.out");
#endif
  cm->DebugMatrix(myfile);
  myfile.flush();
  throw 1;
#endif

  SetComplex(cm);
  const IntVec_t    &Rows = cm->GetRows();
  const IntVec_t    &Cols = cm->GetCols();

#ifdef USE_CCM
  ja = &Rows[0];
  ia = &Cols[0];
#else
  ia = &Rows[0];
  ja = &Cols[0];
#endif
  a  = a_input;

  if (cm->GetSymbolicStatus() == SymbolicStatus_t::NEW_SYMBOLIC)
  {
    phase = 11;
    PARDISO (pt, &maxfct, &mnum, &mtype, &phase,
             &n, a, ia, ja, &idum, &nrhs, &iparm[0], &msglvl, &ddum, &ddum, &error);
  }
  else if (cm->GetSymbolicStatus() == SymbolicStatus_t::SAME_SYMBOLIC)
  {
    error = 0;
  }
  else
  {
    dsAssert(false, "UNEXPECTED");
    error = 0;
  }

  if (error != 0)
  {
    // print message
    return (error == 0);
  }

  phase = 22;
  PARDISO (pt, &maxfct, &mnum, &mtype, &phase,
          &n, a, ia, ja, &idum, &nrhs, &iparm[0], &msglvl, &ddum, &ddum, &error);

  return (error == 0);
}

template <>
bool MKLPardisoData::LUFactorMatrix(CompressedMatrix<double> *cm)
{
  bool ret = false;
  if (cm->GetMatrixType() == MatrixType::REAL)
  {
    const auto &mat = cm->GetReal();
    ret = LUFactorMatrixImpl(cm, reinterpret_cast<const void *>(&(mat[0])));
  }
  else if (cm->GetMatrixType() == MatrixType::COMPLEX)
  {
    const auto &mat = cm->GetComplex();
    ret = LUFactorMatrixImpl(cm, reinterpret_cast<const void *>(&(mat[0])));
  }
  return ret;
}

#ifdef DEVSIM_EXTENDED_PRECISION
template <>
bool MKLPardisoData::LUFactorMatrix(CompressedMatrix<float128> *cm)
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
    ret = LUFactorMatrixImpl(cm, reinterpret_cast<void *>(&dvals[0]));
  }
  else if (cm->GetMatrixType() == MatrixType::COMPLEX)
  {
    const ComplexDoubleVec_t<float128> &fvals = cm->GetComplex();
    ComplexDoubleVec_t<double> dvals(fvals.size());
    for (size_t i = 0; i < fvals.size(); ++i)
    {
      dvals[i] = ComplexDouble_t<double>(static_cast<double>(fvals[i].real()), static_cast<double>(fvals[i].imag()));
    }
    ret = LUFactorMatrixImpl(cm, reinterpret_cast<void *>(&dvals[0]));
  }
  return ret;
}
#endif

void MKLPardisoData::LUSolveImpl(void *x, const void *b_input)
{
  phase = 33;

  void *b = const_cast<void *>(b_input);

  PARDISO (pt, &maxfct, &mnum, &mtype, &phase,
           &n, a, ia, ja, &idum, &nrhs, &iparm[0], &msglvl, b, x, &error);
}

template <>
void MKLPardisoData::LUSolve(DoubleVec_t<double> &x, const DoubleVec_t<double> &b)
{
  dsAssert(error == 0, "UNEXPECTED");

  x.clear();
  x.resize(n);

  // trans and complex flag should already be set
  LUSolveImpl(&x[0], &b[0]);

}

#ifdef DEVSIM_EXTENDED_PRECISION
template <>
void MKLPardisoData::LUSolve(DoubleVec_t<float128> &x, const DoubleVec_t<float128> &b)
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


template <>
void MKLPardisoData::LUSolve(ComplexDoubleVec_t<double> &x, const ComplexDoubleVec_t<double> &b)
{
  dsAssert(error == 0, "UNEXPECTED");

  x.clear();
  x.resize(n);

  // trans and complex flag should already be set
  LUSolveImpl(&x[0], &b[0]);

}

#ifdef DEVSIM_EXTENDED_PRECISION
template <>
void MKLPardisoData::LUSolve(ComplexDoubleVec_t<float128> &x, const ComplexDoubleVec_t<float128> &b)
{
  ComplexDoubleVec_t<double> b64(b.size());
  ComplexDoubleVec_t<double> x64;
  for (size_t i = 0; i < b.size(); ++i)
  {
    b64[i] = ComplexDouble_t<double>(static_cast<double>(b[i].real()), static_cast<double>(b[i].imag()));
  }
  this->LUSolve(x64, b64);

  x.resize(x64.size());
  for (size_t i = 0; i < x64.size(); ++i)
  {
    x[i] = ComplexDouble_t<float128>(static_cast<float128>(x64[i].real()), static_cast<float128>(x64[i].imag()));
  }
}
#endif


template <typename DoubleType>
MKLPardisoPreconditioner<DoubleType>::MKLPardisoPreconditioner(size_t sz, PEnum::TransposeType_t transpose) : Preconditioner<DoubleType>(sz, transpose), mklpardisodata_(nullptr)
{
  mklpardisodata_ = new MKLPardisoData(sz);

  if (transpose == PEnum::TransposeType_t::TRANS)
  {
    mklpardisodata_->SetTranspose(true);
  }
  else
  {
    mklpardisodata_->SetTranspose(false);
  }
}

template <typename DoubleType>
dsMath::CompressionType MKLPardisoPreconditioner<DoubleType>::GetRealMatrixCompressionType() const
{
  return dsMath::CompressionType::CCM;
}

template <typename DoubleType>
dsMath::CompressionType MKLPardisoPreconditioner<DoubleType>::GetComplexMatrixCompressionType() const
{
  return dsMath::CompressionType::CCM;
}

template <typename DoubleType>
MKLPardisoPreconditioner<DoubleType>::~MKLPardisoPreconditioner()
{
  delete mklpardisodata_;
}

template <typename DoubleType>
bool MKLPardisoPreconditioner<DoubleType>::DerivedLUFactor(Matrix<DoubleType> *m)
{
  CompressedMatrix<DoubleType> *cm = dynamic_cast<CompressedMatrix<DoubleType> *>(m);
  dsAssert(cm, "UNEXPECTED");
  // Still need to debug what the issue is with CRM Matrix
#ifdef USE_CCM
  dsAssert(cm->GetCompressionType() == CompressionType::CCM, "UNEXPECTED");
#else
  dsAssert(cm->GetCompressionType() == CompressionType::CRM, "UNEXPECTED");
#endif

  bool ret = false;

  auto &data = *mklpardisodata_;

  ret=data.LUFactorMatrix(cm);

  return ret;
}

template <typename DoubleType>
void MKLPardisoPreconditioner<DoubleType>::DerivedLUSolve(DoubleVec_t<DoubleType> &x, const DoubleVec_t<DoubleType> &b) const
{
  mklpardisodata_->LUSolve(x, b);
}

template <typename DoubleType>
void MKLPardisoPreconditioner<DoubleType>::DerivedLUSolve(ComplexDoubleVec_t<DoubleType> &x, const ComplexDoubleVec_t<DoubleType> &b) const
{
  mklpardisodata_->LUSolve(x, b);
}
}


template class dsMath::MKLPardisoPreconditioner<double>;
#ifdef DEVSIM_EXTENDED_PRECISION
#include "Float128.hh"
template class dsMath::MKLPardisoPreconditioner<float128>;
#endif

