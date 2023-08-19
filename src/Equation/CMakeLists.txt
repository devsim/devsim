SET (CXX_SRCS
    EquationHolder.cc
    Equation.cc
    ContactEquation.cc
    ContactEquationHolder.cc
    InterfaceEquation.cc
    InterfaceEquationHolder.cc
)

INCLUDE_DIRECTORIES (
    ../utility
    ../Geometry
    ../models
    ../math
    ../Circuit/data
    ../errorSystem
    ../GeomModels
    ../common_api
)

ADD_LIBRARY (Equation STATIC ${CXX_SRCS})
