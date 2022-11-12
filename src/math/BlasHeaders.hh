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

#ifndef BLASHEADERS_HH
#define BLASHEADERS_HH
#include <complex>

#ifdef USE_EXPLICIT_MATH_LOAD
#include <string>
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
}
#endif

void getrf( int *m, int *n, double *a, int *lda, int *ipiv, int *info );

void getrs( char *trans, int *n, int *nrhs, double *a, int *lda, int *ipiv, double *b, int *ldb, int *info);

void drotg(double *a, double *b, double *c, double *d);

void zrotg(std::complex<double> *a, std::complex<double> *b, std::complex<double> *c, std::complex<double> *d);

#ifdef DEVSIM_EXTENDED_PRECISION
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

