SET (CXX_SRCS
    Device.cc
    Region.cc
    Edge.cc
    Node.cc
    Coordinate.cc
    Triangle.cc
    Contact.cc
    Permutation.cc
    Interface.cc
    GradientField.cc
    TriangleElementField.cc
    Tetrahedron.cc
    TetrahedronElementField.cc
    GeometryStream.cc
)

INCLUDE_DIRECTORIES (
    ../utility
    ../Equation
    ../models
     ../Geometry
     ../math
    ../errorSystem
     ../common_api
     ../Data
)

ADD_LIBRARY (Geometry STATIC ${CXX_SRCS})
