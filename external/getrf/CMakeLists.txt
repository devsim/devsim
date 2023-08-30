PROJECT(GETRF)
ENABLE_LANGUAGE(CXX Fortran)

ADD_LIBRARY(getrf STATIC dgetrf.f dgetrf2.f dgetrs.f dgemm.f dlamch.f dlaswp.f dscal.f dtrsm.f ilaenv.f idamax.f lsame.f ieeeck.f iparam2stage.F iparmq.f xerbla.f)

SET(CMAKE_Fortran_FLAGS "${CMAKE_Fortran_FLAGS} -freal-8-real-16 -ffixed-line-length-0")
#TARGET_COMPILE_OPTIONS(getrf CMAKE_Fortran_FLAGS -freal-8-real-16)

# should be a better way
ADD_EXECUTABLE(getrftest getrftest.cc)
TARGET_LINK_LIBRARIES(getrftest getrf)

