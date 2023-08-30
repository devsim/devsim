SET (CXX_SRCS
    Bernoulli.cc
    Fermi.cc
    MathEval.cc
    MiscMathFunc.cc
    MathWrapper.cc
    MathPacket.cc
    kahan.cc
    GaussFermi.cc
)

INCLUDE_DIRECTORIES (
  ../utility
  ../errorSystem
  ../Data
  ../common_api
)


ADD_LIBRARY (MathEval STATIC ${CXX_SRCS})
ADD_EXECUTABLE (test_fermi Fermi.cc)
TARGET_COMPILE_DEFINITIONS(test_fermi PRIVATE DEVSIM_UNIT_TEST)
TARGET_LINK_LIBRARIES(test_fermi ${QUADMATH_ARCHIVE})

ADD_EXECUTABLE (test_gaussfermi GaussFermi.cc MiscMathFunc.cc)
TARGET_COMPILE_DEFINITIONS(test_gaussfermi PRIVATE DEVSIM_UNIT_TEST)
TARGET_LINK_LIBRARIES(test_gaussfermi ${QUADMATH_ARCHIVE})

