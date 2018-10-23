ADD_DEFINITIONS(-DTCL_THREADS -pthread -fvisibility=hidden)

SET (EXTERNAL_LIB ${PROJECT_SOURCE_DIR}/external)

SET (FLEX /usr/bin/flex)
SET (BISON /usr/bin/bison)

SET (BOOST_INCLUDE ${EXTERNAL_LIB}/boost_1_66_0)

SET (QUADMATH_ARCHIVE "-lquadmath")





SET (MKL_PARDISO_INCLUDE $ENV{HOME}/anaconda/envs/python27_devsim_build/include)
SET (MKL_LIB_DIR $ENV{HOME}/anaconda/envs/python27_devsim_build/lib)
#http://software.intel.com/en-us/articles/intel-mkl-link-line-advisor
#requires duplicate library for 1-pass linker
SET (BLAS_ARCHIVE
 ${EXTERNAL_LIB}/getrf/build/libgetrf.a
 -L${MKL_LIB_DIR} -Wl,--no-as-needed -lmkl_rt -lpthread -lm -ldl
)


SET (SUPERLULOCATE  ${EXTERNAL_LIB}/SuperLU_4.3)
SET (SUPERLU_INCLUDE ${SUPERLULOCATE}/SRC)
SET (SUPERLU_ARCHIVE ${SUPERLULOCATE}/lib/libsuperlu_4.3.a)

SET (TCL_INCLUDE "$ENV{HOME}/anaconda/include")
SET (TCL_ARCHIVE -L$ENV{HOME}/anaconda/lib -ltcl8.6 -ldl)
#SET (TCL_STUB /usr/lib64/libtclstub8.6.a)

SET (SQLITE3_INCLUDE $ENV{HOME}/anaconda/include)
SET (SQLITE3_ARCHIVE $ENV{HOME}/anaconda/lib/libsqlite3.a)

SET (CGNS_INCLUDE ${EXTERNAL_LIB}/cgnslib/include)
SET (CGNS_ARCHIVE -L${EXTERNAL_LIB}/cgnslib/lib -lcgns)

SET (ZLIB_INCLUDE $ENV{HOME}/anaconda/include)
SET (ZLIB_ARCHIVE $ENV{HOME}/anaconda/lib/libz.a)

SET (SYMDIFF_INCLUDE ${EXTERNAL_LIB}/symdiff/include)
SET (SYMDIFF_ARCHIVE ${EXTERNAL_LIB}/symdiff/lib/libsymdiff_dynamic.a)

SET (PYTHON27_INCLUDE   $ENV{HOME}/anaconda/envs/python27_devsim_build/include/python2.7)
SET (PYTHON27_ARCHIVE -L$ENV{HOME}/anaconda/envs/python27_devsim_build/lib   -lpython2.7)
SET (PYTHON36_INCLUDE   $ENV{HOME}/anaconda/envs/python36_devsim_build/include/python3.6m)
SET (PYTHON36_ARCHIVE -L$ENV{HOME}/anaconda/envs/python36_devsim_build/lib   -lpython3.6m)
SET (PYTHON37_INCLUDE   $ENV{HOME}/anaconda/envs/python37_devsim_build/include/python3.7m)
SET (PYTHON37_ARCHIVE -L$ENV{HOME}/anaconda/envs/python37_devsim_build/lib   -lpython3.7m)

# important flag for dynamic linking of static archives on linux
SET (CMAKE_POSITION_INDEPENDENT_CODE ON)

