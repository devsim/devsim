SET (CXX_SRCS1
    GetArgs.cc
    TclAppInit.cc
    TclCommands.cc
    CommandHandler.cc
)

SET (CXX_SRCS2
    ObjectHolder.cc
    OutputStream.cc
    dsTimer.cc
    Interpreter.cc
    mycondition.cc
    mymutex.cc
    threadBaseClass.cc
)

INCLUDE_DIRECTORIES (
    ../utility
    ../tclapi
    ../commands
    ../common_api
    ${TCL_INCLUDE}
)


IF (TCLMAIN)
ADD_LIBRARY (tclapi_interpreter ${CXX_SRCS1})
ADD_LIBRARY (tclapi_api ${CXX_SRCS2})
ENDIF (TCLMAIN)


