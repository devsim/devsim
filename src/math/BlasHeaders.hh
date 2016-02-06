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

#ifdef __linux__
extern "C"
{

#define dgetrf dgetrf_
#define dgetrs dgetrs_
#define drotg  drotg_
#define zgetrf zgetrf_
#define zgetrs zgetrs_
#define zrotg  zrotg_

void dgetrf( int *m, int *n, double *a, int *lda, int *ipiv, int *info );
void dgetrs( char *trans, int *n, int *nrhs, double *a, int *lda, int *ipiv, double *b, int *ldb, int *info);
void zgetrf( int *m, int *n, doublecomplex *a, int *lda, int *ipiv, int *info );
void zgetrs( char *trans, int *n, int *nrhs, doublecomplex *a, int *lda, int *ipiv, doublecomplex *b, int *ldb, int *info);

void drotg(double *, double *, double *, double *);
/// rely on memory layout being the same between FORTRAN complex and std::complex<double>
void zrotg(std::complex<double> *, std::complex<double> *, std::complex<double> *, std::complex<double> *);
}
#endif

#ifdef __APPLE__
extern "C"
{

#define dgetrf dgetrf_
#define dgetrs dgetrs_
#define drotg  drotg_
#define zgetrf zgetrf_
#define zgetrs zgetrs_
#define zrotg  zrotg_

void dgetrf( int *m, int *n, double *a, int *lda, int *ipiv, int *info );
void dgetrs( char *trans, int *n, int *nrhs, double *a, int *lda, int *ipiv, double *b, int *ldb, int *info);
void zgetrf( int *m, int *n, doublecomplex *a, int *lda, int *ipiv, int *info );
void zgetrs( char *trans, int *n, int *nrhs, doublecomplex *a, int *lda, int *ipiv, doublecomplex *b, int *ldb, int *info);

void drotg(double *, double *, double *, double *);
/// rely on memory layout being the same between FORTRAN complex and std::complex<double>
void zrotg(std::complex<double> *, std::complex<double> *, std::complex<double> *, std::complex<double> *);
}
#endif

/// More specific for ACML on linux
#ifdef _WIN32
extern "C"
{
//// Using the ACML convention
#if 0
#define dgetrf DGETRF
#define dgetrs DGETRS
#define drotg  DROTG
#define zgetrf ZGETRF
#define zgetrs ZGETRS
#define zrotg  ZROTG
#else
#define dgetrf dgetrf_
#define dgetrs dgetrs_
#define drotg  drotg_
#define zgetrf zgetrf_
#define zgetrs zgetrs_
#define zrotg  zrotg_
#endif

void dgetrf( int *m, int *n, double *a, int *lda, int *ipiv, int *info );
void dgetrs( char *trans, int *n, int *nrhs, double *a, int *lda, int *ipiv, double *b, int *ldb, int *info, int trans_len);
void zgetrf( int *m, int *n, doublecomplex *a, int *lda, int *ipiv, int *info );
void zgetrs( char *trans, int *n, int *nrhs, doublecomplex *a, int *lda, int *ipiv, doublecomplex *b, int *ldb, int *info, int trans_len);

void drotg(double *, double *, double *, double *);

/// rely on memory layout being the same between FORTRAN complex and std::complex<double>
void zrotg(std::complex<double> *, std::complex<double> *, std::complex<double> *, std::complex<double> *);
}
#endif

#if 0
extern "C" {
void cblas_drotg(double *, double *, double *, double *);

/// rely on memory layout being the same between FORTRAN complex and std::complex<double>

void cblas_zrotg(std::complex<double> *, std::complex<double> *, std::complex<double> *, std::complex<double> *);
}
#endif
#endif
