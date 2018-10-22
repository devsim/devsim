#IF (APPLE)
#SET (CMAKE_EXE_LINKER_FLAGS "-framework CoreFoundation")
#SET (CMAKE_EXE_LINKER_FLAGS "-framework CoreFoundation -flat_namespace")
ADD_DEFINITIONS(-DTCL_THREADS -fvisibility=hidden)
#ENDIF (APPLE)
#IF (${DEVSIM_CONFIG} STREQUAL "osx_10.9")
SET (FLEX /usr/bin/flex)
SET (BISON /usr/bin/bison)
SET (PYTHON_INCLUDE $ENV{HOME}/anaconda/include/python2.7)
SET (PYTHON_ARCHIVE -L$ENV{HOME}/anaconda/lib -lpython2.7)
SET (PYTHON3_INCLUDE $ENV{HOME}/anaconda/envs/python36/include/python3.6m)
SET (PYTHON3_ARCHIVE -L$ENV{HOME}/anaconda/envs/python36/lib -lpython3.6m)
SET (PYTHON37_ARCHIVE -L$ENV{HOME}/anaconda/envs/python37/lib -lpython3.7m)
SET (BOOST_INCLUDE "/usr/local/include")
SET (BOOST_ARCHIVE )

SET (QUADMATH_ARCHIVE /usr/local/Cellar/gcc/8.1.0/lib/gcc/8/libquadmath.a)
#SET (QUADMATH_ARCHIVE "-lquadmath")
SET (EXTERNAL_LIB ${PROJECT_SOURCE_DIR}/external)
#SET (BLAS_ARCHIVE  -lblas -llapack)
#SET (BLAS_ARCHIVE  -lblas -llapack ${EXTERNAL_LIB}/getrf/build/libgetrf.a)
#SET (BLAS_ARCHIVE  -weak-lblas -weak-llapack)

SET (MKL_PARDISO_INCLUDE $ENV{HOME}/anaconda/include)
SET (MKLROOT $ENV{HOME}/anaconda)
SET (BLAS_ARCHIVE  ${EXTERNAL_LIB}/getrf/build/libgetrf.a
 -L${MKLROOT}/lib -lmkl_rt -lm -ldl
# -L${MKLROOT}/lib -lmkl_rt -lpthread -lm -ldl
#-L${MKLROOT}/lib -Wl,-rpath,${MKLROOT}/lib -lmkl_rt -lpthread -lm -ldl
)


SET (SUPERLULOCATE  ${EXTERNAL_LIB}/SuperLU_4.3)
SET (SUPERLU_INCLUDE ${SUPERLULOCATE}/SRC)
SET (SUPERLU_ARCHIVE ${SUPERLULOCATE}/lib/libsuperlu_4.3_x86_64.a)
SET (TCL_INCLUDE "$ENV{HOME}/anaconda/include")
SET (TCL_ARCHIVE -L$ENV{HOME}/anaconda/lib -ltcl8.6 -ldl)
#SET (TCL_STUB /usr/lib/libtclstub8.5.a)
SET (SQLITE3_ARCHIVE "-lsqlite3")
SET (SQLITE3_INCLUDE "/usr/include")
SET (CGNS_ARCHIVE ${EXTERNAL_LIB}/cgnslib/lib/libcgns.a)
SET (CGNS_INCLUDE ${EXTERNAL_LIB}/cgnslib/include)
SET (ZLIB_INCLUDE "/usr/include")
SET (ZLIB_ARCHIVE "-lz")
SET (SYMDIFF_ARCHIVE ${EXTERNAL_LIB}/symdiff/lib/libsymdiff_static.a)
SET (SYMDIFF_INCLUDE ${EXTERNAL_LIB}/symdiff/include)
# FLAT NAMESPACE CAUSES AN ABORT WHEN EXCEPTIONS ARE THROWN
# For Example: https://github.com/elemental/Elemental/issues/203
#SET (ADD_LINKER_FLAGS " -flat_namespace")
#SET (ADD_RPATH "@executable_path/../lib")
#ENDIF (${DEVSIM_CONFIG} STREQUAL "osx_10.9")

