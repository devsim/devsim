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

#include "BlasHeaders.hh"

#ifdef USE_EXPLICIT_MATH_LOAD
#include "dsAssert.hh"
#include "OutputStream.hh"
#include <string_view>
#include <sstream>
#include <cstdlib>
#include <vector>
#ifdef _WIN32
#include <windows.h>
#else
#include <dlfcn.h>
#endif
#endif


typedef std::complex<double> doublecomplex;

#if defined(__linux__) || defined(__APPLE__) || defined(_WIN32)
#define external_dgetrf dgetrf_
#define external_dgetrs dgetrs_
#define external_drotg  drotg_
#define external_zgetrf zgetrf_
#define external_zgetrs zgetrs_
#define external_zrotg  zrotg_
// added for SuperLU
#define external_dasum  dasum_
#define external_daxpy  daxpy_
#define external_dcopy  dcopy_
#define external_dgemm  dgemm_
#define external_dgemv  dgemv_
#define external_dnrm2  dnrm2_
#define external_dswap  dswap_
#define external_dtrsm  dtrsm_
#define external_dtrsv  dtrsv_
#define external_dzasum  dzasum_
#define external_dznrm2  dznrm2_
#define external_idamax  idamax_
#define external_izamax  izamax_
#define external_zaxpy  zaxpy_
#define external_zcopy  zcopy_
#define external_zgemm  zgemm_
#define external_zgemv  zgemv_
#define external_zswap  zswap_
#define external_ztrsm  ztrsm_
#define external_ztrsv  ztrsv_

#elif 0
#error "NEW PLATFORM"
#define external_zrotg  ZROTG
#endif

extern "C"
{
// forward declaration required when !defined(USE_EXPLICIT_MATH_LOAD)
void external_dgetrf( int *m, int *n, double *a, int *lda, int *ipiv, int *info );
#ifdef _WIN32
void external_dgetrs( char *trans, int *n, int *nrhs, double *a, int *lda, int *ipiv, double *b, int *ldb, int *info, int trans_len);
//void external_zgetrs( char *trans, int *n, int *nrhs, doublecomplex *a, int *lda, int *ipiv, doublecomplex *b, int *ldb, int *info, int trans_len);
#else
void external_dgetrs( char *trans, int *n, int *nrhs, double *a, int *lda, int *ipiv, double *b, int *ldb, int *info);
//void external_zgetrs( char *trans, int *n, int *nrhs, doublecomplex *a, int *lda, int *ipiv, doublecomplex *b, int *ldb, int *info);
#endif
void external_drotg(double *a, double *b, double *c, double *d);
void external_zrotg(std::complex<double> *a, std::complex<double> *b, std::complex<double> *c, std::complex<double> *d);
}
#if defined(USE_EXPLICIT_MATH_LOAD)
extern "C"
{
using PARDISO_signature = void ( void *,    const int *, const int *, const int *,
                   const int *, const int *, const void *,    const int *,
                   const int *, int *, const int *, int *,
                   const int *, void *,    void *,          int * );
using mkl_get_version_string_signature = void (char *, int);


typedef void (dgetrf_signature)( int *m, int *n, double *a, int *lda, int *ipiv, int *info );
typedef void (zgetrf_signature)( int *m, int *n, doublecomplex *a, int *lda, int *ipiv, int *info );
#ifdef _WIN32
typedef void (dgetrs_signature)( char *trans, int *n, int *nrhs, double *a, int *lda, int *ipiv, double *b, int *ldb, int *info, int trans_len);
typedef void (zgetrs_signature)( char *trans, int *n, int *nrhs, doublecomplex *a, int *lda, int *ipiv, doublecomplex *b, int *ldb, int *info, int trans_len);
#else
typedef void (dgetrs_signature)( char *trans, int *n, int *nrhs, double *a, int *lda, int *ipiv, double *b, int *ldb, int *info);
typedef void (zgetrs_signature)( char *trans, int *n, int *nrhs, doublecomplex *a, int *lda, int *ipiv, doublecomplex *b, int *ldb, int *info);
#endif
typedef void (drotg_signature)(double *, double *, double *, double *);
typedef void (zrotg_signature)(std::complex<double> *, std::complex<double> *, std::complex<double> *, std::complex<double> *);

/////  STUFF for SuperLU BLAS
//https://stanford.edu/~boyd/l1_tf/C/blas.h
typedef double (dasum_signature)(int *n, double *dx, int *incx);
typedef void   (daxpy_signature)(int *n, double *alpha, double *dx, int *incx, double *dy, int *incy);
typedef void   (dcopy_signature)(int *n, double *dx, int *incx, double *dy, int *incy);
typedef void   (dgemm_signature)(char *transa, char *transb, int *m, int *n, int *k, double *alpha, double *a, int *lda, double *b, int *ldb, double *beta, double *c, int *ldc);
typedef void   (dgemv_signature)(char *trans, int *m, int *n, double *alpha, double *a, int *lda, double *x, int *incx, double *beta, double *y, int *incy);
typedef double (dnrm2_signature)(int *n, double *dx, int *incx);
typedef void   (dtrsm_signature)(char *side, char *uplo, char *transa, char *diag, int *m, int *n, double *alpha, double *a, int *lda, double *b, int *ldb);
typedef void   (dtrsv_signature)(char *uplo, char *trans, char *diag, int *n, double *a, int *lda, double *x, int *incx);
typedef void   (dswap_signature)(int *n, double *dx, int *incx, double *dy, int *incy);
typedef int    (idamax_signature)(int *n, double *dx, int *incx);
typedef double (dzasum_signature)(int *n, doublecomplex *dx, int *incx);
typedef double (dznrm2_signature)(int *n, doublecomplex *dx, int *incx);
typedef int    (izamax_signature)(int *n, doublecomplex *dx, int *incx);
typedef void   (zaxpy_signature)(int *n, doublecomplex *alpha, doublecomplex *dx, int *incx, doublecomplex *dy, int *incy);
typedef void   (zcopy_signature)(int *n, doublecomplex *dx, int *incx, doublecomplex *dy, int *incy);
typedef void   (zgemm_signature)(char *transa, char *transb, int *m, int *n, int *k, doublecomplex *alpha, doublecomplex *a, int *lda, doublecomplex *b, int *ldb, doublecomplex *beta, doublecomplex *c, int *ldc);
typedef void   (zgemv_signature)(char *trans, int *m, int *n, doublecomplex *alpha, doublecomplex *a, int *lda, doublecomplex *x, int *incx, doublecomplex *beta, doublecomplex *y, int *incy);
typedef void   (zswap_signature)(int *n, doublecomplex *dx, int *incx, doublecomplex *dy, int *incy);
typedef void   (ztrsm_signature)(char *side, char *uplo, char *transa, char *diag, int *m, int *n, doublecomplex *alpha, doublecomplex *a, int *lda, doublecomplex *b, int *ldb);
typedef void   (ztrsv_signature)(char *uplo, char *trans, char *diag, int *n, doublecomplex *a, int *lda, doublecomplex *x, int *incx);


// initialized through dynamic loading
namespace {
struct blas_table {
  inline static void *dll_handle;
  inline static dgetrf_signature *dgetrf;
  inline static zgetrf_signature *zgetrf;
  inline static dgetrs_signature *dgetrs;
  inline static zgetrs_signature *zgetrs;
  inline static drotg_signature *drotg;
  inline static zrotg_signature *zrotg;
// BLAS

  inline static dasum_signature *dasum;
  inline static daxpy_signature *daxpy;
  inline static dcopy_signature *dcopy;
  inline static dgemm_signature *dgemm;
  inline static dgemv_signature *dgemv;
  inline static dnrm2_signature *dnrm2;
  inline static dswap_signature *dswap;
  inline static dtrsm_signature *dtrsm;

  inline static dtrsv_signature *dtrsv;
  inline static dzasum_signature *dzasum;
  inline static dznrm2_signature *dznrm2;
  inline static idamax_signature *idamax;
  inline static izamax_signature *izamax;
  inline static zaxpy_signature *zaxpy;
  inline static zcopy_signature *zcopy;
  inline static zgemm_signature *zgemm;
  inline static zgemv_signature *zgemv;
  inline static zswap_signature *zswap;
  inline static ztrsm_signature *ztrsm;
  inline static ztrsv_signature *ztrsv;


// PARDISO
  inline static PARDISO_signature *PARDISO;
  inline static mkl_get_version_string_signature *mkl_get_version_string;
};
}

void PARDISO( void *a, const int *b, const int *c, const int *d,
                   const int *e, const int *f, const void *g, const int *h,
                   const int *i, int *j, const int *k, int *l,
                   const int *m, void *n,    void *o, int *p)
{
  dsAssert(MathLoader::IsMKLLoaded(), "MKL Pardiso not available. Please switch solvers or restart program with MKL configured.");
  blas_table::PARDISO(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p);
}

void mkl_get_version_string(char *s, int l)
{
  dsAssert(blas_table::mkl_get_version_string, "mkl_get_version_string");
  blas_table::mkl_get_version_string(s, l);
}

void external_dgetrf( int *m, int *n, double *a, int *lda, int *ipiv, int *info )
{
  blas_table::dgetrf( m, n, a, lda, ipiv, info );
}

void external_zgetrf( int *m, int *n, doublecomplex *a, int *lda, int *ipiv, int *info )
{
  blas_table::zgetrf( m, n, a, lda, ipiv, info );
}

#ifdef _WIN32
void external_dgetrs( char *trans, int *n, int *nrhs, double *a, int *lda, int *ipiv, double *b, int *ldb, int *info, int trans_len)
{
  blas_table::dgetrs( trans, n, nrhs, a, lda, ipiv, b, ldb, info, trans_len);
}
void external_zgetrs( char *trans, int *n, int *nrhs, doublecomplex *a, int *lda, int *ipiv, doublecomplex *b, int *ldb, int *info, int trans_len)
{
  blas_table::zgetrs( trans, n, nrhs, a, lda, ipiv, b, ldb, info, trans_len);
}
#else
void external_dgetrs( char *trans, int *n, int *nrhs, double *a, int *lda, int *ipiv, double *b, int *ldb, int *info)
{
  blas_table::dgetrs( trans, n, nrhs, a, lda, ipiv, b, ldb, info);
}
void external_zgetrs( char *trans, int *n, int *nrhs, doublecomplex *a, int *lda, int *ipiv, doublecomplex *b, int *ldb, int *info)
{
  blas_table::zgetrs( trans, n, nrhs, a, lda, ipiv, b, ldb, info);
}
#endif
void external_drotg(double *a, double *b, double *c, double *d)
{
  blas_table::drotg(a, b, c, d);
}
void external_zrotg(std::complex<double> *a, std::complex<double> *b, std::complex<double> *c, std::complex<double> *d)
{
  blas_table::zrotg(a, b, c, d);
}


void external_dtrsv(char *a, char *b, char *c, int *d, double *e, int *f, double *g, int *h)
{
  blas_table::dtrsv(a, b, c, d, e, f, g, h);
}

double external_dasum(int *n, double *dx, int *incx)
{
  return blas_table::dasum(n, dx, incx);
}
void   external_daxpy(int *n, double *alpha, double *dx, int *incx, double *dy, int *incy)
{
  blas_table::daxpy(n, alpha, dx, incx, dy, incy);
}
void   external_dcopy(int *n, double *dx, int *incx, double *dy, int *incy)
{
  blas_table::dcopy(n, dx, incx, dy, incy);
}
void   external_dgemm(char *transa, char *transb, int *m, int *n, int *k, double *alpha, double *a, int *lda, double *b, int *ldb, double *beta, double *c, int *ldc)
{
  blas_table::dgemm(transa, transb, m, n, k, alpha, a, lda, b, ldb, beta, c, ldc);
}

void external_dgemv(char *trans, int *m, int *n,
                              double *alpha, double *a,
                              int *lda, double *x, int *incx,
                              double *beta, double *y, int *incy)
{
  blas_table::dgemv(trans, m, n, alpha, a, lda, x, incx, beta, y, incy);
}

void external_dtrsm(char *side, char *uplo, char *transa, char *diag, int *m, int *n, double *alpha, double *a, int *lda, double *b, int *ldb)
{
  blas_table::dtrsm(side, uplo, transa, diag, m, n, alpha, a, lda, b, ldb);
}

double external_dnrm2(int *n, double *dx, int *incx)
{
  return blas_table::dnrm2(n, dx, incx);
}
void   external_dswap(int *n, double *dx, int *incx, double *dy, int *incy)
{
  blas_table::dswap(n, dx, incx, dy, incy);
}
int external_idamax(int *n, double *dx, int *incx)
{
  return blas_table::idamax(n, dx, incx);
}

double external_dzasum(int *n, doublecomplex *dx, int *incx)
{
  return blas_table::dzasum(n, dx, incx);
}


double external_dznrm2(int *n, doublecomplex *dx, int *incx)
{
  return blas_table::dznrm2(n, dx, incx);
}


int external_izamax(int *n, doublecomplex *dx, int *incx)
{
  return blas_table::izamax(n, dx, incx);
}


void   external_zaxpy(int *n, doublecomplex *alpha, doublecomplex *dx, int *incx, doublecomplex *dy, int *incy)
{
  blas_table::zaxpy(n, alpha, dx, incx, dy, incy);
}


void   external_zcopy(int *n, doublecomplex *dx, int *incx, doublecomplex *dy, int *incy)
{
  blas_table::zcopy(n, dx, incx, dy, incy);
}


void   external_zgemm(char *transa, char *transb, int *m, int *n, int *k, doublecomplex *alpha, doublecomplex *a, int *lda, doublecomplex *b, int *ldb, doublecomplex *beta, doublecomplex *c, int *ldc)
{
  blas_table::zgemm(transa, transb, m, n, k, alpha, a, lda, b, ldb, beta, c, ldc);
}


void   external_zgemv(char *trans, int *m, int *n, doublecomplex *alpha, doublecomplex *a, int *lda, doublecomplex *x, int *incx, doublecomplex *beta, doublecomplex *y, int *incy)
{
  blas_table::zgemv(trans, m, n, alpha, a, lda, x, incx, beta, y, incy);
}


void   external_zswap(int *n, doublecomplex *dx, int *incx, doublecomplex *dy, int *incy)
{
  blas_table::zswap(n, dx, incx, dy, incy);
}


void   external_ztrsm(char *side, char *uplo, char *transa, char *diag, int *m, int *n, doublecomplex *alpha, doublecomplex *a, int *lda, doublecomplex *b, int *ldb)
{
  blas_table::ztrsm(side, uplo, transa, diag, m, n, alpha, a, lda, b, ldb);
}

void   external_ztrsv(char *uplo, char *trans, char *diag, int *n, doublecomplex *a, int *lda, doublecomplex *x, int *incx)
{
  blas_table::ztrsv(uplo, trans, diag, n, a, lda, x, incx);
}
} // end extern "C"
#endif //USE_EXPLICIT_MATH_LOAD

void getrf( int *m, int *n, double *a, int *lda, int *ipiv, int *info )
{
  external_dgetrf( m, n, a, lda, ipiv, info );
}

void getrs( char *trans, int *n, int *nrhs, double *a, int *lda, int *ipiv, double *b, int *ldb, int *info)
{
#ifdef _WIN32
  external_dgetrs( trans, n, nrhs, a, lda, ipiv, b, ldb, info, 1);
#else
  external_dgetrs( trans, n, nrhs, a, lda, ipiv, b, ldb, info);
#endif
}

void drotg(double *a, double *b, double *c, double *d)
{
  external_drotg(a, b, c, d);
}

void zrotg(std::complex<double> *a, std::complex<double> *b, std::complex<double> *c, std::complex<double> *d)
{
  external_zrotg(a, b, c, d);
}

#ifdef USE_EXPLICIT_MATH_LOAD
namespace {
#if 0
// These are available in dsAssert
#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)
#endif

struct symtable {
  const char *symbol_name;
  void ** const function_pointer;
};

static symtable math_function_table[] = {
  // PARDISO
  {"PARDISO",  reinterpret_cast<void **>(&blas_table::PARDISO)},
  {"mkl_get_version_string",   reinterpret_cast<void **>(&blas_table::mkl_get_version_string)},
  {TOSTRING(external_dgetrf), reinterpret_cast<void **>(&blas_table::dgetrf)},
  {TOSTRING(external_zgetrf), reinterpret_cast<void **>(&blas_table::zgetrf)},
  {TOSTRING(external_dgetrs), reinterpret_cast<void **>(&blas_table::dgetrs)},
  {TOSTRING(external_zgetrs), reinterpret_cast<void **>(&blas_table::zgetrs)},
  {TOSTRING(external_drotg),  reinterpret_cast<void **>(&blas_table::drotg)},
  {TOSTRING(external_zrotg),  reinterpret_cast<void **>(&blas_table::zrotg)},
  // SUPERLU BLAS
  {TOSTRING(external_dasum), reinterpret_cast<void **>(&blas_table::dasum)},
  {TOSTRING(external_daxpy), reinterpret_cast<void **>(&blas_table::daxpy)},
  {TOSTRING(external_dcopy), reinterpret_cast<void **>(&blas_table::dcopy)},
  {TOSTRING(external_dgemm), reinterpret_cast<void **>(&blas_table::dgemm)},
  {TOSTRING(external_dgemv), reinterpret_cast<void **>(&blas_table::dgemv)},
  {TOSTRING(external_dnrm2), reinterpret_cast<void **>(&blas_table::dnrm2)},
  {TOSTRING(external_dswap), reinterpret_cast<void **>(&blas_table::dswap)},
  {TOSTRING(external_dtrsm), reinterpret_cast<void **>(&blas_table::dtrsm)},
  {TOSTRING(external_dtrsv), reinterpret_cast<void **>(&blas_table::dtrsv)},
  {TOSTRING(external_dzasum), reinterpret_cast<void **>(&blas_table::dzasum)},
  {TOSTRING(external_dznrm2), reinterpret_cast<void **>(&blas_table::dznrm2)},
  {TOSTRING(external_idamax), reinterpret_cast<void **>(&blas_table::idamax)},
  {TOSTRING(external_izamax), reinterpret_cast<void **>(&blas_table::izamax)},
  {TOSTRING(external_zaxpy), reinterpret_cast<void **>(&blas_table::zaxpy)},
  {TOSTRING(external_zcopy), reinterpret_cast<void **>(&blas_table::zcopy)},
  {TOSTRING(external_zgemm), reinterpret_cast<void **>(&blas_table::zgemm)},
  {TOSTRING(external_zgemv), reinterpret_cast<void **>(&blas_table::zgemv)},
  {TOSTRING(external_zswap), reinterpret_cast<void **>(&blas_table::zswap)},
  {TOSTRING(external_ztrsm), reinterpret_cast<void **>(&blas_table::ztrsm)},
  {TOSTRING(external_ztrsv), reinterpret_cast<void **>(&blas_table::ztrsv)},
};

void clear_table()
{
  // TODO: dlclose reference count too?
  for (auto &entry : math_function_table)
  {
    *entry.function_pointer = nullptr;
  }
}
}

namespace MathLoader {
LoaderMessages_t LoadBlasDLL(std::string dllname, std::string &errors, bool replace)
{
  // consider if the dll is already open, by using RTLD_NOLOAD first
  //https://stackoverflow.com/questions/34073051/when-we-are-supposed-to-use-rtld-deepbind
#if defined(_WIN32)
  HINSTANCE__ *dll_p = LoadLibrary(dllname.c_str());
  if (!dll_p)
  {
    std::ostringstream os;
    os << "Windows returned error " << GetLastError() << " while trying to load \"" << dllname << "\"\n";
    errors += os.str();
    return LoaderMessages_t::MISSING_DLL;
  }
#else
#if defined(__APPLE__)
  auto dllflags = RTLD_LOCAL | RTLD_NOW;
#elif defined(__linux__)
  auto dllflags = RTLD_LOCAL | RTLD_NOW | RTLD_DEEPBIND;
#endif
  void *dll_p = dlopen(dllname.c_str(), RTLD_NOLOAD | dllflags);
  if (!dll_p)
  {
    // Reference count is zero until this is called
    dll_p = dlopen(dllname.c_str(), dllflags);
  }
  else // reference count is now 2, but we need to make sure someone else does not close
  {
  }
  if (!dll_p)
  {
    errors = dlerror();
    errors += "\n";
    return LoaderMessages_t::MISSING_DLL;
  }
#endif

  blas_table::dll_handle = dll_p;


  for (auto &entry : math_function_table)
  {
    if ((!replace) && *entry.function_pointer)
    {
      continue;
    }


#ifdef _WIN32
    void *h = (void *) GetProcAddress(dll_p, entry.symbol_name);
#else
    void *h = dlsym(dll_p, entry.symbol_name);
#endif
    if (h)
    {
      *entry.function_pointer = h;
//      std::cerr << "DEBUG " << entry.symbol_name << " " << h << std::endl;
    }
#if 0
    else
    {
//handle errors
      *entry.function_pointer = nullptr;
#ifdef _WIN32
      std::ostringstream os;
      os << "Missing Symbol: \"" << entry.symbol_name << "\"\n";
      errors += os.str();
#else
      errors += dlerror();
      errors += "\n";
#endif
      ret = false;
    }
#endif
  }

  return GetMathStatus();
}

LoaderMessages_t LoadFromEnvironment(std::string &errors)
{
  ClearBlasFunctions();
  std::string search_path;
  if (const char *env = std::getenv("DEVSIM_MATH_LIBS"))
  {
    search_path = std::string(env);
    std::stringstream os;
    os << "Searching DEVSIM_MATH_LIBS=\"" << search_path << "\"\n";
    OutputStream::WriteOut(OutputStream::OutputType::INFO, os.str());
  }
  else
  {
    return LoaderMessages_t::NO_ENVIRONMENT;
  }

  std::string_view sv = search_path;
  std::vector<std::string> dll_names;
  while (!sv.empty())
  {
    auto pos = sv.find(':');
    if (pos != std::string_view::npos)
    {
      if (pos != 0)
      {
        dll_names.push_back(std::string(sv.substr(0, pos)));
      }
      sv = sv.substr(pos + 1);
    }
    else
    {
      dll_names.push_back(std::string(sv));
      break;
    }
  }

  LoaderMessages_t status = LoaderMessages_t::MISSING_DLL;
  bool done = false;
  for (const auto &dll_name : dll_names)
  {
    if (done)
    {
      OutputStream::WriteOut(OutputStream::OutputType::INFO, std::string("Skipping ") + dll_name + "\n");
      continue;
    }

    std::stringstream os;
    os << "Loading \"" << dll_name << "\": ";
    status = LoadBlasDLL(dll_name, errors, false);
    if (status == LoaderMessages_t::MATH_LOADED)
    {
      os << "ALL BLAS/LAPACK LOADED";
      done = true;
    }
    else if (status == LoaderMessages_t::MKL_LOADED)
    {
      os << "Intel MKL with PARDISO LOADED";
      done = true;
    }
    else if (status == LoaderMessages_t::MISSING_DLL)
    {
      os << "MISSING DLL";
    }
    else if (status == LoaderMessages_t::MISSING_SYMBOLS)
    {
      os << "MISSING SYMBOLS";
    }
    os << "\n";
    OutputStream::WriteOut(OutputStream::OutputType::INFO, os.str());

#if 0
    if (!loaded)
    {
      std::ostringstream os;
      os << "Could not find Intel MKL. The maximum tested version is \"" << tested_dll_name << "\"\n";
      loaded = LoadBlasDLL(dll_name, errors);
    }
#endif
  }
  return GetMathStatus();
}

LoaderMessages_t LoadIntelMKL(std::string &errors)
{
  LoaderMessages_t ret = LoaderMessages_t::MISSING_DLL;
  ClearBlasFunctions();
  std::vector<std::string> search_paths = {std::string("")};
#if defined(__APPLE__)
// This should always be available through symlink
  const std::string default_name = "libmkl_rt.dylib";
  const std::string prefix_name = "libmkl_rt.";
  const std::string suffix_name = ".dylib";
#elif defined(__linux__)
// This should always be available through symlink
  const std::string default_name = "libmkl_rt.so";
  const std::string prefix_name = "libmkl_rt.so.";
  const std::string suffix_name = "";
#elif defined(_WIN32)
  const std::string default_name = "mkl_rt.dll";
  const std::string prefix_name = "mkl_rt.";
  const std::string suffix_name = ".dll";
#else
#error "MKL LIBRARY RT DLL NOT DEFINED FOR PLATFORM"
#endif

#if defined(__APPLE__) || defined(__linux__)
  if (const char *env = std::getenv("CONDA_PREFIX"))
  {
#ifdef __APPLE__
    search_paths.emplace_back("@rpath/lib/");
#else
    search_paths.emplace_back(std::string(env) + "/lib/");
#endif
  }
#endif

  errors.clear();
  for (auto &path : search_paths)
  {
    ret = LoadBlasDLL(path + default_name, errors, true);
    if (ret == LoaderMessages_t::MKL_LOADED)
    {
      return ret;
    }
  }

  const size_t max_tested_version = 2;
  const size_t min_version_to_test_for = 1;
  const size_t max_version_to_test_for = 5;
  size_t actual_version = size_t(-1);
  std::string dll_name;
  for (size_t i = min_version_to_test_for; i <= max_version_to_test_for; ++i)
  {
    dll_name = prefix_name + std::to_string(i) + suffix_name;
    for (auto &a : search_paths)
    {
      errors.clear();
      ret = LoadBlasDLL(a + dll_name, errors, true);
      if (ret == LoaderMessages_t::MKL_LOADED)
      {
        actual_version = i;
        goto done;
      }
    }
  }
done:
  if (actual_version == size_t(-1))
  {
    std::string tested_dll_name = prefix_name + std::to_string(max_tested_version) + suffix_name;
    std::ostringstream os;
    os << "Could not find Intel MKL. The maximum tested version is \"" << tested_dll_name << "\"\n";
#if __linux__
    os << "This may be fixed by setting LD_LIBRARY_PATH before starting the application.\n";
#endif
    //OutputStream::WriteOut(OutputStream::OutputType::INFO, os.str().c_str());
    errors = os.str();
  }
  else
//  else if (actual_version > max_tested_version)
  {
    std::string tested_dll_name = prefix_name + std::to_string(max_tested_version) + suffix_name;
    std::ostringstream os;
    os << "Found Intel MKL as \"" << dll_name << "\".  The maximum tested version is \"" << tested_dll_name << "\"\n";
    //OutputStream::WriteOut(OutputStream::OutputType::INFO, os.str().c_str());
    errors.clear();
  }

  return ret;
}

bool IsMathLoaded()
{
  bool ret = true;
  for (auto &entry : math_function_table)
  {
    if (*entry.function_pointer)
    {
      continue;
    }
    else if ((entry.function_pointer == reinterpret_cast<void **>(&blas_table::PARDISO)) || (entry.function_pointer == reinterpret_cast<void **>(&blas_table::mkl_get_version_string)))
    {
      continue;
    }
    else
    {
      ret = false;
    }
  }
  return ret;
}

bool IsMKLLoaded()
{
  return blas_table::PARDISO && blas_table::mkl_get_version_string;
}

LoaderMessages_t GetMathStatus()
{
  auto ret = LoaderMessages_t::MISSING_SYMBOLS;
  if (IsMathLoaded())
  {
    if (blas_table::PARDISO && blas_table::mkl_get_version_string)
    {
      ret = LoaderMessages_t::MKL_LOADED;
    }
    else
    {
      ret = LoaderMessages_t::MATH_LOADED;
    }
  }
  return ret;
}

std::string GetMKLVersion()
{
  if (!blas_table::mkl_get_version_string)
  {
    return "Intel MKL Not Loaded";
  }

  const size_t maxlen = 128;
  std::string buf(maxlen, '\0');
  blas_table::mkl_get_version_string(buf.data(), buf.capacity());

  // clear last real null
  auto pos = buf.find('\0');
  if (pos != std::string::npos)
  {
    buf.resize(pos);
  }
  pos = buf.find_last_not_of(" ");
  if (pos != std::string::npos)
  {
    buf.resize(pos+1);
  }
  return buf;
}

LoaderMessages_t LoadMathLibraries(std::string &errors)
{
  LoaderMessages_t ret = LoadFromEnvironment(errors);
#if 0
  if ((ret == LoaderMessages_t::MKL_LOADED) || (ret == LoaderMessages_t::MATH_LOADED))
  {
      return ret;
  }
#endif
  if (ret == LoaderMessages_t::NO_ENVIRONMENT)
  {
    ret = LoadIntelMKL(errors);
  }
  else
  {
    errors += "Not attempting to load Intel MKL since DEVSIM_MATH_LIBS is specified.\n";
  }
  return ret;
}

void ClearBlasFunctions()
{
  clear_table();
}
}

#if 0
void stuff()
{
  std::string errors;
//  bool ret = LoadBlasDLL("./libmkl_rt.dylib", errors);
//  bool ret = LoadBlasDLL("@rpath/libmkl_rt.dylib", errors);
  // don't need loader path
  bool ret = LoadBlasDLL("@executable_path/../lib/libmkl_rt.dylib", errors);
  if (!errors.empty())
  {
    std::cerr << errors;
  }
  std::cerr << "dgetrf " << reinterpret_cast<void *>(blas_table::dgetrf) << std::endl;
  std::cerr << "PARDISO " << blas_table::PARDISO << std::endl;
}
#endif
#endif /*USE_EXPLICIT_MATH_LOAD*/


