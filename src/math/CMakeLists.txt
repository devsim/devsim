SET (CXX_SRCS
    TimeData.cc
    DenseMatrix.cc
    SuperLUPreconditioner.cc
    LinearSolver.cc
    DirectLinearSolver.cc
    IterativeLinearSolver.cc
    Matrix.cc
    CompressedMatrix.cc
    Newton.cc
    Preconditioner.cc
    SuperLUData.cc
    SuperLUDataZ.cc
    BlockPreconditioner.cc
    gmres.cc
    MathEnum.cc
)

IF (MKL_PARDISO)
SET (CXX_SRCS ${CXX_SRCS} MKLPardisoPreconditioner.cc)
ADD_DEFINITIONS(-DUSE_MKL_PARDISO)
ENDIF (MKL_PARDISO)

INCLUDE_DIRECTORIES (
    ../utility
    ../Geometry
    ../Data
    ../Circuit/data
    ../Equation
    ../math
    ../errorSystem
    ../MathEval
    ../common_api
    ${SUPERLU_INCLUDE}
    ${MKL_PARDISO_INCLUDE}
)

ADD_LIBRARY (math ${CXX_SRCS})

