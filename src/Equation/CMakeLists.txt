SET (CXX_SRCS
    Equation.cc
    ContactEquation.cc
    InterfaceEquation.cc
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

ADD_LIBRARY (Equation ${CXX_SRCS})
