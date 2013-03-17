PROJECT (DEVSIM)
SET (SUBDIRS
    Geometry
    models
    math
    adiff
    AutoEquation
    Equation
    GeomModels
    meshing
    Data
    Circuit
    MathEval
    tclapi
    errorSystem
    myThread
    utility
    commands
    pythonapi
    main
)


ENABLE_TESTING()
FOREACH (SUB ${SUBDIRS})
    ADD_SUBDIRECTORY(${SUB})
ENDFOREACH (SUB ${SUBDIRS})

