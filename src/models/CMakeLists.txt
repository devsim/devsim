SET (CXX_SRCS
    NodeModel.cc
    EdgeModel.cc
    InterfaceNodeScalarData.cc
    NodeScalarData.cc
    EdgeScalarData.cc
    TriangleEdgeScalarData.cc
    NodeSolution.cc
    EdgeSubModel.cc
    EdgeFromNodeModel.cc
    InterfaceNodeModel.cc
    TriangleEdgeModel.cc
    TriangleEdgeSubModel.cc
    TriangleEdgeFromEdgeModel.cc
    TriangleEdgeFromEdgeModelDerivative.cc
    TetrahedronEdgeScalarData.cc
    TetrahedronEdgeModel.cc
    TetrahedronEdgeSubModel.cc
    TetrahedronEdgeFromEdgeModel.cc
    TetrahedronEdgeFromEdgeModelDerivative.cc
    TriangleEdgeFromNodeModel.cc
    TetrahedronEdgeFromNodeModel.cc
    VectorTriangleEdgeModel.cc
    VectorTetrahedronEdgeModel.cc
    ParallelOpEqual.cc
)

INCLUDE_DIRECTORIES (
    ../utility
    ../Geometry
    ../math
    ../errorSystem
    ../MathEval
    ../myThread
    ../common_api
)

ADD_LIBRARY (models ${CXX_SRCS})
