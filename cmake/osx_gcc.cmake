ADD_COMPILE_OPTIONS(-Wall -fvisibility=hidden)

SET (EXTERNAL_LIB ${PROJECT_SOURCE_DIR}/external)

SET (FLEX /usr/bin/flex)
SET (BISON /usr/bin/bison)

SET (BOOST_INCLUDE ${EXTERNAL_LIB}/boostorg/math/include ${EXTERNAL_LIB}/boostorg/multiprecision/include)
ADD_DEFINITIONS(-DBOOST_MP_STANDALONE -DBOOST_MP_MATH_AVAILABLE)

SET (QUADMATH_ARCHIVE "-lquadmath")

#SET (MKL_PARDISO_INCLUDE ${CONDA_PREFIX}/include)
#SET (MKL_LIB_DIR         ${CONDA_PREFIX}/lib)
#http://software.intel.com/en-us/articles/intel-mkl-link-line-advisor
#
SET (BLAS_ARCHIVE -L${MKL_LIB_DIR} -lmkl_rt -lm -ldl)

SET (EXTENDED_BLAS_ARCHIVE  ${EXTERNAL_LIB}/getrf/build/libgetrf.a)


SET (SUPERLULOCATE  ${EXTERNAL_LIB}/superlu)
SET (SUPERLU_INCLUDE ${SUPERLULOCATE}/SRC)
SET (SUPERLU_ARCHIVE ${SUPERLULOCATE}/build/SRC/libsuperlu.a)

#SET (SQLITE3_INCLUDE ${CONDA_PREFIX}/include)
#SET (SQLITE3_ARCHIVE -lsqlite3)
#SET (ZLIB_INCLUDE ${CONDA_PREFIX}/include)
#SET (ZLIB_ARCHIVE ${CONDA_PREFIX}/lib/libz.a)

SET (SQLITE3_ARCHIVE -lsqlite3)
SET (ZLIB_ARCHIVE -lz)


SET (SYMDIFF_INCLUDE ${EXTERNAL_LIB}/symdiff/include)
SET (SYMDIFF_ARCHIVE ${EXTERNAL_LIB}/symdiff/lib/libsymdiff_static.a)


