INCLUDE_DIRECTORIES (
    ../utility
    ../Geometry
    ../models
    ../math
    ../AutoEquation
    ../GeomModels
    ../Equation
    ../tclapi
    ../errorSystem
    ../Circuit/data
    ../Data
    ../meshing
    ../MathEval
    ../common_api
    ../pythonapi
)

SET (LIBRARIES
    tclapi_interpreter
    commands
    Data
    AutoEquation
    meshing
    GeomModels
    Equation
    Geometry
    math
    MathEval
    models
    myThread
    circuitSources
    circuitIdeal
    circuitData
    errorSystem
    tclapi_api
    utility
)

SET (PYLIBRARIES
    commands
    pythonapi_interpreter
    commands
    pythonapi_interpreter
    Data
    AutoEquation
    meshing
    GeomModels
    Equation
    Geometry
    math
    MathEval
    models
    myThread
    circuitSources
    circuitIdeal
    circuitData
    errorSystem
    pythonapi_api
    utility
)

SET (PY3LIBRARIES
    commands
    python3api_interpreter
    commands
    python3api_interpreter
    Data
    AutoEquation
    meshing
    GeomModels
    Equation
    Geometry
    math
    MathEval
    models
    myThread
    circuitSources
    circuitIdeal
    circuitData
    errorSystem
    python3api_api
    utility
)

SET (OPTIONAL_LIBS "")
IF (VTKWRITER)
SET (OPTIONAL_LIBS ${OPTIONAL_LIBS} ${ZLIB_ARCHIVE})
ENDIF (VTKWRITER)
IF (GENIUSREADER)
SET (OPTIONAL_LIBS ${OPTIONAL_LIBS} ${CGNS_ARCHIVE})
ENDIF (GENIUSREADER)
IF (DEVSIM_EXTENDED_PRECISION)
SET (OPTIONAL_LIBS ${OPTIONAL_LIBS} ${QUADMATH_ARCHIVE} ${BOOST_ARCHIVE})
ENDIF (DEVSIM_EXTENDED_PRECISION)


IF (TCLMAIN)
ADD_EXECUTABLE (devsim_tcl devsim_tcl.cc)
TARGET_INCLUDE_DIRECTORIES (devsim_tcl BEFORE PUBLIC ${TCL_INCLUDE})
TARGET_LINK_LIBRARIES(devsim_tcl ${LIBRARIES} ${TCL_ARCHIVE} ${SUPERLU_ARCHIVE} ${BLAS_ARCHIVE} ${SQLITE3_ARCHIVE} ${SYMDIFF_ARCHIVE} ${OPTIONAL_LIBS} ${PTHREAD_LIB} ${DLOPEN_LIB})
ENDIF (TCLMAIN)

ADD_EXECUTABLE (devsim_py devsim_py.cc)
TARGET_INCLUDE_DIRECTORIES (devsim_py BEFORE PUBLIC ${PYTHON_INCLUDE})
TARGET_LINK_LIBRARIES(devsim_py ${PYLIBRARIES} ${PYTHON_ARCHIVE} ${SUPERLU_ARCHIVE} ${BLAS_ARCHIVE} ${SQLITE3_ARCHIVE} ${SYMDIFF_ARCHIVE} ${OPTIONAL_LIBS} ${PTHREAD_LIB} ${DLOPEN_LIB})

IF (PYTHON3)
ADD_EXECUTABLE (devsim_py3 devsim_py.cc)
TARGET_INCLUDE_DIRECTORIES (devsim_py3 BEFORE PUBLIC ${PYTHON3_INCLUDE})
TARGET_LINK_LIBRARIES(devsim_py3 ${PY3LIBRARIES} ${PYTHON3_ARCHIVE} ${SUPERLU_ARCHIVE} ${BLAS_ARCHIVE} ${SQLITE3_ARCHIVE} ${SYMDIFF_ARCHIVE} ${OPTIONAL_LIBS} ${PTHREAD_LIB} ${DLOPEN_LIB})
ENDIF (PYTHON3)

IF (ADD_LINKER_FLAGS)
SET_TARGET_PROPERTIES(devsim_tcl PROPERTIES LINK_FLAGS ${ADD_LINKER_FLAGS})
SET_TARGET_PROPERTIES(devsim_py PROPERTIES LINK_FLAGS ${ADD_LINKER_FLAGS})
SET_TARGET_PROPERTIES(devsim_py3 PROPERTIES LINK_FLAGS ${ADD_LINKER_FLAGS})
ENDIF (ADD_LINKER_FLAGS)
IF (ADD_RPATH)
SET_TARGET_PROPERTIES(devsim_tcl PROPERTIES INSTALL_RPATH ${ADD_RPATH} BUILD_WITH_INSTALL_RPATH ON)
SET_TARGET_PROPERTIES(devsim_py PROPERTIES INSTALL_RPATH ${ADD_RPATH} BUILD_WITH_INSTALL_RPATH ON)
SET_TARGET_PROPERTIES(devsim_py3 PROPERTIES INSTALL_RPATH ${ADD_RPATH} BUILD_WITH_INSTALL_RPATH ON)
ENDIF (ADD_RPATH)

#IF (WIN32)
#SET_TARGET_PROPERTIES(devsim_tcl PROPERTIES
#        LINK_FLAGS " /NODEFAULTLIB:LIBMSVCRT"
#        )
#ENDIF (WIN32)


