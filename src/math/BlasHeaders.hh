/***
DEVSIM
Copyright 2013 DEVSIM LLC

SPDX-License-Identifier: Apache-2.0
***/

#if !defined(LOAD_MATHLIBS)
#error "Do not include if LOAD_MATHLIBS is not set"
#endif

#ifndef BLASHEADERS_HH
#define BLASHEADERS_HH
#include <complex>

#include <string>
#include <vector>
namespace MathLoader {
// MKL_LOADED implies MKL Functions Loaded
enum class LoaderMessages_t {NO_ENVIRONMENT, MISSING_DLL, MISSING_SYMBOLS, MATH_LOADED, MKL_LOADED};
LoaderMessages_t LoadMathLibraries(std::string &errors);
LoaderMessages_t LoadBlasDLL(std::string dllname, std::string &errors, bool replace);
void ClearBlasFunctions();
LoaderMessages_t LoadFromEnvironment(const std::string &envstring, std::string &errors);
LoaderMessages_t LoadIntelMKL(std::string &errors);
bool IsMathLoaded();
bool IsMKLLoaded();
LoaderMessages_t GetMathStatus();
std::string GetMKLVersion();
std::vector<std::string> GetLoadedMathDLLs();
}

void getrf( int *m, int *n, double *a, int *lda, int *ipiv, int *info );

void getrs( char *trans, int *n, int *nrhs, double *a, int *lda, int *ipiv, double *b, int *ldb, int *info);

void drotg(double *a, double *b, double *c, double *d);

void zrotg(std::complex<double> *a, std::complex<double> *b, std::complex<double> *c, std::complex<double> *d);

#ifdef DEVSIM_EXTENDED_PRECISION
#ifndef USE_CPP_BIN_FLOAT
#include "Float128.hh"

extern "C" {
void quad_dgetrf_( int *m, int *n, float128 *a, int *lda, int *ipiv, int *info );
void quad_dgetrs_( char *trans, int *n, int *nrhs, float128 *a, int *lda, int *ipiv, float128 *b, int *ldb, int *info);
}

typedef std::complex<float128> float128complex;
inline void getrf( int *m, int *n, float128 *a, int *lda, int *ipiv, int *info )
{
  quad_dgetrf_(m, n, a, lda, ipiv, info);
}

inline void getrs( char *trans, int *n, int *nrhs, float128 *a, int *lda, int *ipiv, float128 *b, int *ldb, int *info)
{
  quad_dgetrs_(trans, n, nrhs, a, lda, ipiv, b, ldb, info);
}
#endif
#endif
#endif

