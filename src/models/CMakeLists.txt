SET (CXX_SRCS
    ModelDataHolder.cc
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
    TriangleEdgePairFromEdgeModel.cc
    TriangleEdgeFromEdgeModelDerivative.cc
    TriangleEdgePairFromEdgeModelDerivative.cc
    TetrahedronEdgeScalarData.cc
    TetrahedronEdgeModel.cc
    TetrahedronEdgeSubModel.cc
    TetrahedronEdgeFromEdgeModel.cc
    TetrahedronEdgeFromEdgeModelDerivative.cc
    TetrahedronEdgePairFromEdgeModel.cc
    TetrahedronEdgePairFromEdgeModelDerivative.cc
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
    ../common_api
)

ADD_LIBRARY (models STATIC ${CXX_SRCS})
