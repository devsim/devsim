ADD_DEFINITIONS(-fvisibility=hidden)
ADD_COMPILE_OPTIONS(-Wall)

SET (EXTERNAL_LIB ${PROJECT_SOURCE_DIR}/external)

SET (FLEX /usr/bin/flex)
SET (BISON /usr/bin/bison)

SET (BOOST_INCLUDE "${CONDA_PREFIX}/include")

SET (QUADMATH_ARCHIVE "-lquadmath")

SET (MKL_PARDISO_INCLUDE ${CONDA_PREFIX}/include)
SET (MKL_LIB_DIR         ${CONDA_PREFIX}/lib)
#http://software.intel.com/en-us/articles/intel-mkl-link-line-advisor
#
SET (BLAS_ARCHIVE  ${EXTERNAL_LIB}/getrf/build/libgetrf.a
 -L${MKL_LIB_DIR} -lmkl_rt -lm -ldl
)


SET (SUPERLULOCATE  ${EXTERNAL_LIB}/SuperLU_4.3)
SET (SUPERLU_INCLUDE ${SUPERLULOCATE}/SRC)
SET (SUPERLU_ARCHIVE ${SUPERLULOCATE}/lib/libsuperlu_4.3_x86_64.a)

SET (SQLITE3_INCLUDE ${CONDA_PREFIX}/include)
SET (SQLITE3_ARCHIVE -lsqlite3)

SET (ZLIB_INCLUDE ${CONDA_PREFIX}/include)
SET (ZLIB_ARCHIVE ${CONDA_PREFIX}/lib/libz.a)

SET (SYMDIFF_INCLUDE ${EXTERNAL_LIB}/symdiff/include)
SET (SYMDIFF_ARCHIVE ${EXTERNAL_LIB}/symdiff/lib/libsymdiff_static.a)

