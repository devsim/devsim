## This version strips the newline at the end
##EXEC_PROGRAM(uname  ARGS -m OUTPUT_VARIABLE ARCH)
## There should be a strip option in later versions of cmake (e.g. 2.4.4)
##EXECUTE_PROCESS(COMMAND uname -m OUTPUT_VARIABLE ARCH)
##MESSAGE("ARCH ${ARCH}")
#
#SET (CMAKE_CXX_WARNING_LEVEL 4)
SET (FLEX "C:/cygwin/bin/flex.exe")
SET (BISON "C:/cygwin/bin/bison.exe")

ADD_DEFINITIONS(-DSTATIC_BUILD -D_USE_MATH_DEFINES -DTCL_THREADS)
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

IF (${CMAKE_SIZEOF_VOID_P} MATCHES 4)
SET (MINICONDA27 c:/Miniconda)
SET (MINICONDA36 c:/Miniconda36)
SET (MINICONDA37 c:/Miniconda37)
SET (BUILDDIR    win32)
ELSE (${CMAKE_SIZEOF_VOID_P} MATCHES 4)
SET (MINICONDA27 c:/Miniconda-x64)
SET (MINICONDA36 c:/Miniconda36-x64)
SET (MINICONDA37 c:/Miniconda37-x64)
SET (BUILDDIR    win64)
# 64 bit
#SET(MKL_DIR
#"C:/Program Files (x86)/IntelSWTools/compilers_and_libraries_2016/windows/mkl/lib/intel64_win"
#)
#SET(INTEL_MKL_LINK
#${MKL_DIR}/mkl_intel_lp64.lib
#${MKL_DIR}/mkl_core.lib
#${MKL_DIR}/mkl_sequential.lib
#)
#SET (BLAS_ARCHIVE
#${INTEL_MKL_LINK}
#)

#SET (CGNS_ARCHIVE ${THIRDPARTY}/cgnslib64/lib/cgns.lib)
#SET (CGNS_INCLUDE ${THIRDPARTY}/cgnslib64/include)
ENDIF (${CMAKE_SIZEOF_VOID_P} MATCHES 4)

SET (THIRDPARTY ${CMAKE_SOURCE_DIR}/external/devsim_win32_deps)
SET (SUPERLULOCATE  ${THIRDPARTY}/SuperLU_4.3)
SET (SUPERLU_ARCHIVE ${SUPERLULOCATE}/${BUILDDIR}/superlu.lib)
SET (SUPERLU_INCLUDE ${SUPERLULOCATE}/SRC)

SET (BLAS_ARCHIVE     ${MINICONDA27}/Library/lib/mkl_rt.lib)
SET (PYTHON27_ARCHIVE ${MINICONDA27}/libs/python27.lib)
SET (PYTHON27_INCLUDE ${MINICONDA27}/include)
SET (PYTHON36_ARCHIVE ${MINICONDA36}/libs/python36.lib)
SET (PYTHON36_INCLUDE ${MINICONDA36}/include)
SET (PYTHON37_ARCHIVE ${MINICONDA37}/libs/python37.lib)
SET (PYTHON37_INCLUDE ${MINICONDA37}/include)

SET (SQLITE3_INCLUDE  ${MINICONDA27}/Library/include)
SET (SQLITE3_ARCHIVE  ${MINICONDA27}/Library/lib/sqlite3.lib)

SET (ZLIB_INCLUDE ${MINICONDA27}/Library/include)
SET (ZLIB_ARCHIVE ${MINICONDA27}/Library/lib/zlib.lib)

SET(TCL_INCLUDE "${MINICONDA27}/Library/include")
SET(TCL_ARCHIVE "${MINICONDA27}/Library/lib/tcl86t.lib")
SET(TCL_STUB    "${MINICONDA27}/Library/lib/tclstub86.lib")

SET (SYMDIFF_INCLUDE ${CMAKE_SOURCE_DIR}/external/symdiff/include)
SET (SYMDIFF_ARCHIVE ${CMAKE_SOURCE_DIR}/external/symdiff/${BUILDDIR}/src/engine/Release/symdiff_static.lib)

