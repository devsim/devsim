ADD_COMPILE_OPTIONS(-Wall -fvisibility=hidden)

SET (EXTERNAL_LIB ${PROJECT_SOURCE_DIR}/external)

SET (FLEX /usr/bin/flex)
SET (BISON /usr/bin/bison)

SET (BOOST_INCLUDE ${EXTERNAL_LIB}/boostorg/math/include ${EXTERNAL_LIB}/boostorg/multiprecision/include)
ADD_DEFINITIONS(-DBOOST_MP_STANDALONE -DBOOST_MP_MATH_AVAILABLE)

SET (BLAS_ARCHIVE  -weak-lblas -weak-llapack)
SET (EXTERNAL_LIB ${PROJECT_SOURCE_DIR}/external)
SET (EIGEN_INCLUDE ${EXTERNAL_LIB}/eigen)
SET (SUPERLULOCATE  ${EXTERNAL_LIB}/superlu)
SET (SUPERLU_INCLUDE ${SUPERLULOCATE}/SRC)
SET (SUPERLU_ARCHIVE ${SUPERLULOCATE}/build/SRC/libsuperlu.a)
SET (SQLITE3_ARCHIVE "-lsqlite3")
SET (SQLITE3_INCLUDE "/usr/include")
SET (ZLIB_INCLUDE "/usr/include")
SET (ZLIB_ARCHIVE "-lz")
SET (SYMDIFF_ARCHIVE ${EXTERNAL_LIB}/symdiff/lib/libsymdiff_static.a)
SET (SYMDIFF_INCLUDE ${EXTERNAL_LIB}/symdiff/include)
SET (ADD_LINKER_FLAGS " -flat_namespace")


