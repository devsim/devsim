SET (CXX_SRCS
   GetArgs.cc
   PythonCommands.cc
   CommandHandler.cc
   ObjectHolder.cc
   OutputStream.cc
   Interpreter.cc
   ControlGIL.cc
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
target_compile_definitions(pythonapi_interpreter_py3 PRIVATE -DDEVSIM_MODULE_NAME=devsim_py3 -DPy_LIMITED_API=0x03070000)
ENDIF (PYTHON3)

