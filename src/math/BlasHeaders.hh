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
