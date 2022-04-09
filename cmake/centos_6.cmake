ADD_COMPILE_OPTIONS(-Wall -fvisibility=hidden)

SET (EXTERNAL_LIB ${PROJECT_SOURCE_DIR}/external)

SET (FLEX /usr/bin/flex)
SET (BISON /usr/bin/bison)

SET (BOOST_INCLUDE "${CONDA_PREFIX}/include")

SET (QUADMATH_ARCHIVE "-lquadmath")

# to prevent linking against bad libstdc++
SET (TMPLIBPATH ${CMAKE_BINARY_DIR}/linklibs)

SET (MKL_PARDISO_INCLUDE ${CONDA_PREFIX}/include)
SET (MKL_LIB_DIR         ${TMPLIBPATH})
#http://software.intel.com/en-us/articles/intel-mkl-link-line-advisor
#requires duplicate library for 1-pass linker
SET (BLAS_ARCHIVE -L${MKL_LIB_DIR} -Wl,--no-as-needed -lmkl_rt -lpthread -lm -ldl)
SET (EXTENDED_BLAS_ARCHIVE ${EXTERNAL_LIB}/getrf/build/libgetrf.a)


SET (SUPERLULOCATE  ${EXTERNAL_LIB}/SuperLU_4.3)
SET (SUPERLU_INCLUDE ${SUPERLULOCATE}/SRC)
SET (SUPERLU_ARCHIVE ${SUPERLULOCATE}/lib/libsuperlu_4.3.a)

SET (SQLITE3_INCLUDE ${CONDA_PREFIX}/include)
SET (SQLITE3_ARCHIVE -L${TMPLIBPATH} -lsqlite3)

SET (ZLIB_INCLUDE ${CONDA_PREFIX}/include)
SET (ZLIB_ARCHIVE ${TMPLIBPATH}/libz.a)

SET (SYMDIFF_INCLUDE ${EXTERNAL_LIB}/symdiff/include)
SET (SYMDIFF_ARCHIVE ${EXTERNAL_LIB}/symdiff/lib/libsymdiff_dynamic.a)

#SET (PYTHON3_INCLUDE   ${CONDA_PREFIX}/include/python3.8)
#SET (PYTHON3_ARCHIVE -L${CONDA_PREFIX}/lib   -lpython3.8m)

# important flag for dynamic linking of static archives on linux
SET (CMAKE_POSITION_INDEPENDENT_CODE ON)

