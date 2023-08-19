SET (CXX_SRCS
    TimeData.cc
    DenseMatrix.cc
    LinearSolver.cc
    DirectLinearSolver.cc
    Matrix.cc
    CompressedMatrix.cc
    Newton.cc
    Preconditioner.cc
    BlockPreconditioner.cc
    MathEnum.cc
    ExternalPreconditioner.cc
    SolverUtil.cc
)

IF (SUPERLU_PRECONDITIONER)
SET (CXX_SRCS ${CXX_SRCS}
    SuperLUPreconditioner.cc
    SuperLUData.cc
    SuperLUDataZ.cc
)
ENDIF ()
IF (DEVSIM_LOAD_MATHLIBS)
SET (CXX_SRCS ${CXX_SRCS} BlasHeaders.cc gmres.cc IterativeLinearSolver.cc)
INCLUDE_DIRECTORIES(${SUPERLU_INCLUDE})
ENDIF()

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

ADD_LIBRARY (math STATIC ${CXX_SRCS})

