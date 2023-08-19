SET (CXX_SRCS
    ModelExprEval.cc
    ModelExprData.cc
    InterfaceNodeExprModel.cc
    NodeExprModel.cc
    EdgeExprModel.cc
    ExprEquation.cc
    ExprContactEquation.cc
    InterfaceModelExprEval.cc
    InterfaceExprEquation.cc
    InterfaceModelExprData.cc
    DefaultDerivatives.cc
    TriangleEdgeExprModel.cc
    TetrahedronEdgeExprModel.cc
    EquationFunctions.cc
)

INCLUDE_DIRECTORIES (
    ../utility
    ../Geometry
    ../math
    ../models
    ../Equation
    ../Data
    ../Circuit/data
    ../errorSystem
    ../MathEval
    ../common_api
    ${SYMDIFF_INCLUDE}
)

ADD_LIBRARY (AutoEquation STATIC ${CXX_SRCS})
