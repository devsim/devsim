
#ifndef FLOAT128_HH
#define FLOAT128_HH
#ifndef DEVSIM_EXTENDED_PRECISION
#error "File included when DEVSIM_EXTENDED_PRECISION not enabled"
#endif
#if defined (USE_CPP_BIN_FLOAT)
#include <boost/multiprecision/cpp_bin_float.hpp>
using float128=boost::multiprecision::cpp_bin_float_quad;
#else
#include <boost/multiprecision/float128.hpp>
using float128=boost::multiprecision::float128;
#endif

#endif

