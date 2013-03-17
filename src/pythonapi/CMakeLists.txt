SET (CXX_SRCS1
   GetArgs.cc
   PythonAppInit.cc
   PythonCommands.cc
   CommandHandler.cc
)

SET (CXX_SRCS2
    ObjectHolder.cc
    OutputStream.cc
    Interpreter.cc
    dsTimer.cc
    mycondition.cc
    mymutex.cc
    threadBaseClass.cc
)

INCLUDE_DIRECTORIES (
    ../utility
    ../pythonapi
    ../commands
    ../common_api
    ${PYTHON_INCLUDE}
)


ADD_LIBRARY (pythonapi_interpreter ${CXX_SRCS1})
ADD_LIBRARY (pythonapi_api ${CXX_SRCS2})


