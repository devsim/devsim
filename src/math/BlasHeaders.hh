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

#ifndef BLASHEADERS_HH
#define BLASHEADERS_HH

typedef std::complex<double> doublecomplex;

extern "C"
{

#if defined(__linux__) || defined(__APPLE__) || defined(_WIN32)
#define external_dgetrf dgetrf_
#define external_dgetrs dgetrs_
#define external_drotg  drotg_
#define external_zgetrf zgetrf_
#define external_zgetrs zgetrs_
#define external_zrotg  zrotg_
#elif 0
#define dgetrf DGETRF
#define dgetrs DGETRS
#define drotg  DROTG
#define zgetrf ZGETRF
#define zgetrs ZGETRS
#define zrotg  ZROTG
#endif

void external_dgetrf( int *m, int *n, double *a, int *lda, int *ipiv, int *info );
void external_zgetrf( int *m, int *n, doublecomplex *a, int *lda, int *ipiv, int *info );
#ifdef _WIN32
void external_dgetrs( char *trans, int *n, int *nrhs, double *a, int *lda, int *ipiv, double *b, int *ldb, int *info, int trans_len);
void external_zgetrs( char *trans, int *n, int *nrhs, doublecomplex *a, int *lda, int *ipiv, doublecomplex *b, int *ldb, int *info, int trans_len);
#else
void external_dgetrs( char *trans, int *n, int *nrhs, double *a, int *lda, int *ipiv, double *b, int *ldb, int *info);
void external_zgetrs( char *trans, int *n, int *nrhs, doublecomplex *a, int *lda, int *ipiv, doublecomplex *b, int *ldb, int *info);
#endif
void external_drotg(double *, double *, double *, double *);
void external_zrotg(std::complex<double> *, std::complex<double> *, std::complex<double> *, std::complex<double> *);
}

inline void getrf( int *m, int *n, double *a, int *lda, int *ipiv, int *info )
{
  external_dgetrf( m, n, a, lda, ipiv, info );
}

inline void getrs( char *trans, int *n, int *nrhs, double *a, int *lda, int *ipiv, double *b, int *ldb, int *info)
{
#ifdef _WIN32
  external_dgetrs( trans, n, nrhs, a, lda, ipiv, b, ldb, info, 1);
#else
  external_dgetrs( trans, n, nrhs, a, lda, ipiv, b, ldb, info);
#endif
}

#if 0
inline void getrf( int *m, int *n, doublecomplex *a, int *lda, int *ipiv, int *info )
{
  external_zgetrf( m, n, a, lda, ipiv, info );
}

inline void getrs( char *trans, int *n, int *nrhs, doublecomplex *a, int *lda, int *ipiv, doublecomplex *b, int *ldb, int *info)
{
  external_zgetrs( trans, n, nrhs, a, lda, ipiv, b, ldb, info);
}
#endif

inline void drotg(double *a, double *b, double *c, double *d)
{
  external_drotg(a, b, c, d);
}

inline void zrotg(std::complex<double> *a, std::complex<double> *b, std::complex<double> *c, std::complex<double> *d)
{
  external_zrotg(a, b, c, d);
}


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

#if 0
inline void getrf( int *m, int *n, float128complex *a, int *lda, int *ipiv, int *info )
{
#ifndef _WIN32
#warning "IMPLEMENT!!!!"
#endif
*info = 1;
}

inline void getrs( char *trans, int *n, int *nrhs, float128complex *a, int *lda, int *ipiv, float128complex *b, int *ldb, int *info)
{
#ifndef _WIN32
#warning "IMPLEMENT!!!!"
#endif
*info = 1;
}
#endif

#if 0
void drotg(float128 *a, double *b, double *c, double *d);

void zrotg(std::complex<float128> *a, std::complex<double> *b, std::complex<double> *c, std::complex<double> *d);
#endif
#endif

#endif

