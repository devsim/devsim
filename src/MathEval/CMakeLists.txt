SET (CXX_SRCS
    Bernoulli.cc
    Fermi.cc
    MathEval.cc
    MiscMathFunc.cc
    MathWrapper.cc
    MathPacket.cc
    kahan.cc
)

INCLUDE_DIRECTORIES (
  ../utility
  ../errorSystem
  ../Data
  ../common_api
)


ADD_LIBRARY (MathEval ${CXX_SRCS})

