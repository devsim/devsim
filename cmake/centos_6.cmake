SET (EXTERNAL_LIB ${PROJECT_SOURCE_DIR}/external)
SET (SUPERLULOCATE  ${EXTERNAL_LIB}/SuperLU_4.3)
SET (SUPERLU_INCLUDE ${SUPERLULOCATE}/SRC)
SET (SUPERLU_ARCHIVE ${SUPERLULOCATE}/lib/libsuperlu_4.3.a)
SET (PTHREAD_LIB -lpthread)
SET (SQLITE3_INCLUDE $ENV{HOME}/anaconda/include)

SET (CGNS_ARCHIVE -L${EXTERNAL_LIB}/cgnslib/lib -lcgns)
SET (CGNS_INCLUDE ${EXTERNAL_LIB}/cgnslib/include)

SET (SYMDIFF_ARCHIVE ${EXTERNAL_LIB}/symdiff/lib/libsymdiff_dynamic.a)
SET (SYMDIFF_INCLUDE ${EXTERNAL_LIB}/symdiff/include)
ADD_DEFINITIONS(-DTCL_THREADS -pthread -fvisibility=hidden)
SET (FLEX /usr/bin/flex)
SET (BISON /usr/bin/bison)
SET (ZLIB_INCLUDE $ENV{HOME}/anaconda/include)

SET (BOOST_INCLUDE ${EXTERNAL_LIB}/boost_1_66_0)
SET (QUADMATH_ARCHIVE "-lquadmath")

SET (MKL_PARDISO_INCLUDE $ENV{HOME}/anaconda/include)
SET (MKLROOT $ENV{HOME}/anaconda)
SET (BLAS_ARCHIVE
 ${EXTERNAL_LIB}/getrf/build/libgetrf.a
#http://software.intel.com/en-us/articles/intel-mkl-link-line-advisor
#requires duplicate library for 1-pass linker
 -L$ENV{HOME}/anaconda/lib -Wl,--no-as-needed -lmkl_rt -lpthread -lm -ldl
)
SET (ZLIB_ARCHIVE $ENV{HOME}/anaconda/lib/libz.a)
SET (SQLITE3_ARCHIVE $ENV{HOME}/anaconda/lib/libsqlite3.a)
SET (TCL_INCLUDE "$ENV{HOME}/anaconda/include")
#SET (TCL_STUB /usr/lib64/libtclstub8.6.a)
SET (TCL_ARCHIVE -L$ENV{HOME}/anaconda/lib -ltcl8.6 -ldl)

SET (PYTHON_INCLUDE $ENV{HOME}/anaconda/include/python2.7)
SET (PYTHON_ARCHIVE -L$ENV{HOME}/anaconda/lib -lpython2.7)
SET (PYTHON3_INCLUDE $ENV{HOME}/anaconda/envs/python36/include/python3.6m)
SET (PYTHON3_ARCHIVE -L$ENV{HOME}/anaconda/envs/python36/lib -lpython3.6m)
SET (PYTHON37_ARCHIVE -L$ENV{HOME}/anaconda/envs/python37/lib -lpython3.7m)

SET (CMAKE_POSITION_INDEPENDENT_CODE ON)
