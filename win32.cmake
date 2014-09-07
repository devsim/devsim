## This version strips the newline at the end
##EXEC_PROGRAM(uname  ARGS -m OUTPUT_VARIABLE ARCH)
## There should be a strip option in later versions of cmake (e.g. 2.4.4)
##EXECUTE_PROCESS(COMMAND uname -m OUTPUT_VARIABLE ARCH)
##MESSAGE("ARCH ${ARCH}")
#
#SET (CMAKE_CXX_WARNING_LEVEL 4)
SET (FLEX "C:/cygwin/bin/flex.exe")
SET (BISON "C:/cygwin/bin/bison.exe")
#SET (Boost_INCLUDE_DIRS "C:/Users/jsanchez/WindowsLibs/externallib_win32/boost_1_39_0") 
#INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIRS})
ADD_DEFINITIONS(-DSTATIC_BUILD -D_USE_MATH_DEFINES -DTCL_THREADS)
#Math stuff
SET (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /fp:strict")
SET (CMAKE_C_FLAGS "${CMAKE_CXX_FLAGS} /fp:strict")
SET (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SAFESEH:NO")
SET (CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} /SAFESEH:NO")
SET (CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} /SAFESEH:NO")
#ADD_DEFINITIONS(-DSTATIC_BUILD /fp:strict)
#SET (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /NODEFAULTLIB:LIBCMT")
#SET (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /NODEFAULTLIB:LIBCMT /NODEFAULTLIB:OLDNAMES")
#
#
# windows build

#IF (${CMAKE_SIZEOF_VOID_P} MATCHES 4)
SET (THIRDPARTY c:/build/devsim_external)
SET (BLAS_ARCHIVE 
${THIRDPARTY}/CLAPACK/lib/lapack.lib
${THIRDPARTY}/CLAPACK/lib/blas.lib
${THIRDPARTY}/CLAPACK/lib/libf2c.lib
)

SET (CGNS_ARCHIVE ${THIRDPARTY}/cgnslib/lib/cgns.lib)
SET (CGNS_INCLUDE ${THIRDPARTY}/cgnslib/include)
SET (PYTHON_ARCHIVE C:/Python27/libs/python27.lib)
SET (PYTHON_INCLUDE C:/Python27/include)
SET (SQLITE3_ARCHIVE "${THIRDPARTY}/sqlite-amalgamation-3080600/build/Release/sqlite3.lib")
SET (SQLITE3_INCLUDE "${THIRDPARTY}/sqlite-amalgamation-3080600")
SET (SUPERLULOCATE  ${THIRDPARTY}/SuperLU_4.3)
SET (SUPERLU_ARCHIVE ${SUPERLULOCATE}/build/Release/superlu.lib)
SET (SUPERLU_INCLUDE ${SUPERLULOCATE}/SRC)
SET (SYMDIFF_INCLUDE c:/build/symdiff_github/include)
SET (SYMDIFF_ARCHIVE c:/build/symdiff_github/lib/symdiff_static.lib)
SET (TCL_ARCHIVE "c:/Tcl/lib/tcl85.lib")
SET (TCL_INCLUDE "c:/Tcl/include")
SET (TCL_STUB    "c:/Tcl/lib/tclstub85.lib")
SET (ZLIB_ARCHIVE "${THIRDPARTY}/zlib/lib/zlibstatic.lib")
SET (ZLIB_INCLUDE "${THIRDPARTY}/zlib/include")

#ELSE (${CMAKE_SIZEOF_VOID_P} MATCHES 4)
#SET(TCL_INCLUDE "c:/Tcl/include")
#SET(TCL_STUB "c:/Tcl/lib/tclstub85.lib")
#SET (SUPERLU_ARCHIVE ${SUPERLULOCATE}/lib/superlu_64.lib)
#SET (BLAS_ARCHIVE  "${THIRDPARTY}/AMD/acml4.4.0/ifort64/lib/libacml_dll.lib")
#SET (TCL_ARCHIVE "${THIRDPARTY}/tcl8.5.9/win/Release_AMD64_VC9/tcl85tsx.lib")
#SET (ZLIB_ARCHIVE "${THIRDPARTY}/zlib-1.2.3/zlib_64.lib")
#ENDIF (${CMAKE_SIZEOF_VOID_P} MATCHES 4)




