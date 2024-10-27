SET (EXTERNAL_LIB ${PROJECT_SOURCE_DIR}/external)

ADD_COMPILE_OPTIONS(-Wall)
SET (FLEX flex)
SET (BISON bison)

SET (BOOST_INCLUDE ${EXTERNAL_LIB}/boostorg/config/include ${EXTERNAL_LIB}/boostorg/math/include ${EXTERNAL_LIB}/boostorg/multiprecision/include)
ADD_DEFINITIONS(-DBOOST_MP_STANDALONE -DBOOST_MATH_STANDALONE)
ADD_DEFINITIONS(-DBOOST_NO_CXX14_CONSTEXPR)
#ADD_DEFINITIONS(-DSTATIC_BUILD -D_USE_MATH_DEFINES)

#SET (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /fp:strict /EHsc ${WARNINGS_IGNORE}")
#SET (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /fp:strict ${WARNINGS_IGNORE}")

#SET (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SAFESEH:NO")
#SET (CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} /SAFESEH:NO /NODEFAULTLIB:python3")
#SET (CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} /SAFESEH:NO")

# windows build

SET (EXTENDED_BLAS_ARCHIVE ${CMAKE_SOURCE_DIR}/external/getrf/msys/libgetrf.a)

SET (SYMDIFF_INCLUDE ${CMAKE_SOURCE_DIR}/external/symdiff/include)
SET (SYMDIFF_ARCHIVE ${CMAKE_SOURCE_DIR}/external/symdiff/msys_x86_64_release/src/engine/libsymdiff_dynamic.a)

SET (QUADMATH_ARCHIVE "-lquadmath")

