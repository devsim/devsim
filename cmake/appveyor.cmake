## This version strips the newline at the end
##EXEC_PROGRAM(uname  ARGS -m OUTPUT_VARIABLE ARCH)
## There should be a strip option in later versions of cmake (e.g. 2.4.4)
##EXECUTE_PROCESS(COMMAND uname -m OUTPUT_VARIABLE ARCH)
##MESSAGE("ARCH ${ARCH}")
#
#SET (CMAKE_CXX_WARNING_LEVEL 4)
SET (FLEX  "C:/msys64/usr/bin/flex.exe")
SET (BISON "C:/msys64/usr/bin/bison.exe")
SET (EXTERNAL_LIB ${PROJECT_SOURCE_DIR}/external)
SET (BOOST_INCLUDE ${EXTERNAL_LIB}/boostorg/config/include ${EXTERNAL_LIB}/boostorg/math/include ${EXTERNAL_LIB}/boostorg/multiprecision/include)
ADD_DEFINITIONS(-DBOOST_MP_STANDALONE -DBOOST_MATH_STANDALONE)

ADD_DEFINITIONS(-DSTATIC_BUILD -D_USE_MATH_DEFINES)
#Math stuff
# warning C4005: concerning macro redefines in stdint.h
# warning C4244: conversion from 'size_t' to 'const double'
# warning C4267: possible loss of data
# warning C4503: Decorated name length truncated
# warning C4800: casting from point to bool performance warning
# warning C4996: 'isatty': The POSIX name for this item is deprecated
# warning C4661: no suitable definition provided for explicit template instantiation request
# dll linkage warning from python, define HAVE_ROUND
SET (WARNINGS_IGNORE " /wd4005 /wd4244 /wd4267 /wd4503 /wd4800 /wd4996 /wd4661 /DHAVE_ROUND")
SET (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /fp:strict /EHsc ${WARNINGS_IGNORE}")
SET (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /fp:strict ${WARNINGS_IGNORE}")

SET (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SAFESEH:NO")
SET (CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} /SAFESEH:NO /NODEFAULTLIB:python3")
SET (CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} /SAFESEH:NO")

#SET (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /NODEFAULTLIB:LIBCMT")
#SET (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /NODEFAULTLIB:LIBCMT /NODEFAULTLIB:OLDNAMES")

# windows build

SET (EIGEN_INCLUDE ${CMAKE_SOURCE_DIR}/external/eigen)

SET (BLAS_ARCHIVE     ${CONDA_PREFIX}/Library/lib/mkl_rt.lib)
SET (MKL_PARDISO_INCLUDE ${CONDA_PREFIX}/Library/include)

SET (SQLITE3_INCLUDE  ${CONDA_PREFIX}/Library/include)
SET (SQLITE3_ARCHIVE  ${CONDA_PREFIX}/Library/lib/sqlite3.lib)

SET (ZLIB_INCLUDE ${CONDA_PREFIX}/Library/include)
SET (ZLIB_ARCHIVE ${CONDA_PREFIX}/Library/lib/zlib.lib)

SET (SYMDIFF_INCLUDE ${CMAKE_SOURCE_DIR}/external/symdiff/include)
SET (SYMDIFF_ARCHIVE ${CMAKE_SOURCE_DIR}/external/symdiff/${BUILDDIR}/src/engine/Release/symdiff_dynamic.lib)

