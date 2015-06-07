SET (CXX_SRCS
    NodePosition.cc
    NodeVolume.cc
    EdgeCouple.cc
    EdgeLength.cc
    EdgeIndex.cc
    EdgeInverseLength.cc
    ModelCreate.cc
    AtContactNode.cc
    SurfaceArea.cc
    UnitVec.cc
    VectorGradient.cc
    TriangleEdgeCouple.cc
    TriangleNodeVolume.cc
    TetrahedronEdgeCouple.cc
    TetrahedronNodeVolume.cc
    InterfaceNormal.cc
    TriangleCylindricalEdgeCouple.cc
    TriangleCylindricalNodeVolume.cc
    CylindricalEdgeCouple.cc
    CylindricalNodeVolume.cc
    CylindricalEdgeNodeVolume.cc
    CylindricalSurfaceArea.cc
    AverageEdgeModel.cc
)

INCLUDE_DIRECTORIES (
    ../utility
    ../Geometry
    ../math
    ../models
    ../errorSystem
    ../Data
    ../common_api
)

ADD_LIBRARY (GeomModels ${CXX_SRCS})

