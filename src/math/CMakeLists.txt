SET (CXX_SRCS
    TimeData.cc
    DenseMatrix.cc
    LinearSolver.cc
    DirectLinearSolver.cc
    IterativeLinearSolver.cc
    Matrix.cc
    CompressedMatrix.cc
    Newton.cc
    Preconditioner.cc
    BlockPreconditioner.cc
    gmres.cc
    MathEnum.cc
    BlasHeaders.cc
    ExternalPreconditioner.cc
    SolverUtil.cc
)

IF (MKL_PARDISO)
SET (CXX_SRCS ${CXX_SRCS} MKLPardisoPreconditioner.cc)
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
    ${EIGEN_INCLUDE}
)

ADD_LIBRARY (math ${CXX_SRCS})

