SET (CXX_SRCS
   GetArgs.cc
   PythonCommands.cc
   CommandHandler.cc
   ObjectHolder.cc
   OutputStream.cc
   Interpreter.cc
   ControlGIL.cc
   ZlibCompress.cc
)

INCLUDE_DIRECTORIES (
    ../utility
    ../pythonapi
    ../commands
    ../common_api
    ../math
)

IF (PYTHON3)
ADD_LIBRARY (pythonapi_interpreter_py3 OBJECT ${CXX_SRCS})
TARGET_INCLUDE_DIRECTORIES (pythonapi_interpreter_py3 BEFORE PRIVATE ${PYTHON3_INCLUDE})
target_compile_definitions(pythonapi_interpreter_py3 PRIVATE -DDEVSIM_MODULE_NAME=devsim_py3 -DPy_LIMITED_API=0x03090000)
ENDIF (PYTHON3)

IF (PYTHON3T)
ADD_LIBRARY (pythonapi_interpreter_py3t OBJECT ${CXX_SRCS})
TARGET_INCLUDE_DIRECTORIES (pythonapi_interpreter_py3t BEFORE PRIVATE ${PYTHON3T_INCLUDE})
TARGET_COMPILE_DEFINITIONS (pythonapi_interpreter_py3t PRIVATE DEVSIM_PYTHON3T Py_TARGET_ABI3T=0x030f0000)
IF (WIN32)
TARGET_COMPILE_DEFINITIONS (pythonapi_interpreter_py3t PRIVATE DEVSIM_MODULE_NAME=devsim_py3 Py_GIL_DISABLED=1)
ELSE ()
TARGET_COMPILE_DEFINITIONS (pythonapi_interpreter_py3t PRIVATE DEVSIM_MODULE_NAME=devsim_py3)
ENDIF (WIN32)
ENDIF (PYTHON3T)
