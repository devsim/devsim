/***
DEVSIM
Copyright 2013 DEVSIM LLC

SPDX-License-Identifier: Apache-2.0
***/

#include "BlasHeaders.hh"

#include "dsAssert.hh"
#include "OutputStream.hh"
#include <string_view>
#include <sstream>
#include <cstdlib>
#include <vector>
#include <algorithm>
#ifdef _WIN32
#include <windows.h>
#else
#include <dlfcn.h>
#endif


typedef std::complex<double> doublecomplex;

#if defined(__linux__) || defined(__APPLE__) || defined(_WIN32)
#define external_dgetrf dgetrf_
#define external_dgetrs dgetrs_
#define external_drotg  drotg_
#define external_zgetrf zgetrf_
#define external_zgetrs zgetrs_
#define external_zrotg  zrotg_

#elif 0
#error "NEW PLATFORM"
#define external_zrotg  ZROTG
#endif

extern "C"
{
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

// initialized through dynamic loading
namespace {
struct blas_table {
// LAPACK
  inline static std::vector<std::pair<std::string, void *>> dll_handles;
  inline static dgetrf_signature *dgetrf;
  inline static zgetrf_signature *zgetrf;
  inline static dgetrs_signature *dgetrs;
  inline static zgetrs_signature *zgetrs;
  inline static drotg_signature *drotg;
  inline static zrotg_signature *zrotg;

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
} // end extern "C"

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
#if defined(USE_LAPACK)
  {TOSTRING(external_dgetrf), reinterpret_cast<void **>(&blas_table::dgetrf)},
  {TOSTRING(external_zgetrf), reinterpret_cast<void **>(&blas_table::zgetrf)},
  {TOSTRING(external_dgetrs), reinterpret_cast<void **>(&blas_table::dgetrs)},
  {TOSTRING(external_zgetrs), reinterpret_cast<void **>(&blas_table::zgetrs)},
#endif
  {TOSTRING(external_drotg),  reinterpret_cast<void **>(&blas_table::drotg)},
  {TOSTRING(external_zrotg),  reinterpret_cast<void **>(&blas_table::zrotg)},
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
  const auto dllflags = RTLD_LOCAL | RTLD_NOW;
#elif defined(__linux__)
  const auto dllflags = RTLD_LOCAL | RTLD_NOW | RTLD_DEEPBIND;
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

  blas_table::dll_handles.push_back({dllname, dll_p});


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
    }
  }

  return GetMathStatus();
}

LoaderMessages_t LoadFromEnvironment(const std::string &environment_var, std::string &errors)
{
  ClearBlasFunctions();
  std::string search_path;
  if (!environment_var.empty())
  {
    search_path = environment_var;
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
#if defined(_WIN32)
    auto pos = sv.find(';');
#elif defined(__APPLE__) || defined(__linux__)
    auto pos = sv.find(':');
#else
#error "SET SEPARATOR FOR NEW PLATFORM"
#endif
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
      blas_table::dll_handles.push_back({dll_name, nullptr});
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

  errors.clear();
  ret = LoadBlasDLL(default_name, errors, true);

  const size_t max_tested_version = 2;
  const size_t min_version_to_test_for = 1;
  const size_t max_version_to_test_for = 5;
  size_t actual_version = size_t(-1);
  std::string dll_name;
  for (size_t i = min_version_to_test_for; i <= max_version_to_test_for; ++i)
  {
    dll_name = prefix_name + std::to_string(i) + suffix_name;
    errors.clear();
    ret = LoadBlasDLL(dll_name, errors, true);
    if (ret == LoaderMessages_t::MKL_LOADED)
    {
      actual_version = i;
      break;
    }
  }

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

std::vector<std::string> GetLoadedMathDLLs()
{
  std::vector<std::string> dll_names(blas_table::dll_handles.size());
  std::transform(blas_table::dll_handles.begin(), blas_table::dll_handles.end(), dll_names.begin(), [](auto &a){return a.first;});
  return dll_names;
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
  LoaderMessages_t ret = LoaderMessages_t::NO_ENVIRONMENT;
  const char *env = std::getenv("DEVSIM_MATH_LIBS");

  if (env)
  {
    ret = LoadFromEnvironment(env, errors);
  }
  else
  {
    ret = LoadIntelMKL(errors);
  }

  if ((ret == LoaderMessages_t::MKL_LOADED) || (ret == LoaderMessages_t::MATH_LOADED))
  {
      return ret;
  }

#if defined(_WIN32)
    const char *teststring = "libopenblas.dll;liblapack.dll;libblas.dll";
#elif defined(__APPLE__)
    const char *teststring = "libopenblas.dylib:liblapack.dylib:libblas.dylib";
#elif defined(__linux__)
    const char *teststring = "libopenblas.so:liblapack.so:libblas.so";
#else
#error "SET MATH LIBRARIES TEST FOR NEW PLATFORM"
#endif

  if (!env)
  {
    ret = LoadFromEnvironment(teststring, errors);
  }

  return ret;
}

void ClearBlasFunctions()
{
  clear_table();
}
}

