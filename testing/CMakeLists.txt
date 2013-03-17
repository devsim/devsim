INCLUDE_DIRECTORIES (
    ../Geometry
    ../models
    ../Potential
    ../math
    ../adiff
    ../AutoEquation
    ../GeomModels
    ../Equation
    ../tclapi
    ../Data
)

# pass linker so order important
SET (LIBRARIES
    tclapi
    potential
    carrier
    math
    AutoEquation
    adiff
    GeomModels
    models
    geom
    Data
    meshing
    Equation
    GeomModels
    geom
)

IF (WIN32)
SET (DIFF C:/cygwin/bin/diff.exe)
SET (PERL C:/cygwin/bin/perl.exe)
SET (RUNDIFFTEST ${PERL} ${PROJECT_SOURCE_DIR}/testing/rundifftest.pl)
SET (RUNDIR      ${PROJECT_SOURCE_DIR}/testing)
SET (OUTPUTDIR   ${PROJECT_BINARY_DIR}/testing)
SET (MODELCOMP   ${PROJECT_BINARY_DIR}/src//adiff/Release/modelcomp)
SET (DEVSIM      "${PROJECT_BINARY_DIR}/src/main/Release/devsim ${RUNDIR}/devsimwrapper.tcl")
IF (${CMAKE_SIZEOF_VOID_P} MATCHES 4)
SET (GOLDENDIR   ${PROJECT_SOURCE_DIR}/goldenresults/win32)
ELSE (${CMAKE_SIZEOF_VOID_P} MATCHES 4)
SET (GOLDENDIR   ${PROJECT_SOURCE_DIR}/goldenresults/win64)
ENDIF (${CMAKE_SIZEOF_VOID_P} MATCHES 4)
ELSE (WIN32)
SET (DIFF diff)
SET (RUNDIFFTEST ${PROJECT_SOURCE_DIR}/testing/rundifftest.pl)
SET (DEVSIM      ${PROJECT_BINARY_DIR}/src/main/devsim)
SET (DEVSIMPY    ${PROJECT_BINARY_DIR}/src/main/devsim_py)
SET (GOLDENDIR   ${PROJECT_SOURCE_DIR}/goldenresults/${HOSTNAME})
SET (RUNDIR      ${PROJECT_SOURCE_DIR}/testing)
SET (RUNDIRPY    ${PROJECT_SOURCE_DIR}/py_test)
SET (OUTPUTDIR   ${PROJECT_BINARY_DIR}/testing)
SET (OUTPUTDIRPY ${PROJECT_SOURCE_DIR}/py_test)
SET (MODELCOMP   ${PROJECT_BINARY_DIR}/src/adiff/modelcomp)
ENDIF (WIN32)

IF(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
# Linux specific code
IF (${CMAKE_SIZEOF_VOID_P} MATCHES 8)
SET (GOLDENDIR ${PROJECT_SOURCE_DIR}/goldenresults/${DEVSIM_CONFIG}_x86_64)
ELSE (${CMAKE_SIZEOF_VOID_P} MATCHES 8)
SET (GOLDENDIR ${PROJECT_SOURCE_DIR}/goldenresults/${DEVSIM_CONFIG}_i686)
ENDIF (${CMAKE_SIZEOF_VOID_P} MATCHES 8)
ENDIF (${CMAKE_SYSTEM_NAME} MATCHES "Linux")

IF (APPLE)
IF (${CMAKE_SIZEOF_VOID_P} MATCHES 8)
SET (GOLDENDIR ${PROJECT_SOURCE_DIR}/goldenresults/${DEVSIM_CONFIG}_x86_64)
ELSE (${CMAKE_SIZEOF_VOID_P} MATCHES 8)
SET (GOLDENDIR ${PROJECT_SOURCE_DIR}/goldenresults/${DEVSIM_CONFIG}_i686)
ENDIF (${CMAKE_SIZEOF_VOID_P} MATCHES 8)
ENDIF (APPLE)

ENABLE_TESTING()
#### todo create platform based golden results and remove arch from filename
SET (NEWPYTESTS
  cap2
  equation1
  ptest1
  ptest2
  sqlite2
  sqlite3
  testfunc
  utf8_2
)
FOREACH(I ${NEWPYTESTS})
    ADD_TEST("${I}" ${RUNDIFFTEST} "${DEVSIMPY} ${I}.py" ${GOLDENDIR} ${I}.out ${RUNDIR} ${OUTPUTDIR})
ENDFOREACH(I)

SET (NEWTESTS res1 res2 res3 dio1_utf8 dio2 circ1 circ2 circ3 circ4
ssac_circ ssac_cap ssac_res ssac_diode utf8_1 mesh1 mesh2 mesh3 mesh4 trimesh1 trimesh2 floops erf1 Fermi1
mesh2d dio2_2d mos_2d mos_2d_restart mos_2d_restart2 noise_res noise_res_2d mos_2d_temtest dio2_element_2d
gmsh_resistor2d gmsh_resistor3d
transient_circ transient_circ2 transient_circ3 mos_2d_temtest2
package_cap1
sqlite1
rename1
symdiff1
thread_1
fpetest1
fpetest2
kahan
)
FOREACH(I ${NEWTESTS})
    ADD_TEST("${I}" ${RUNDIFFTEST} "${DEVSIM} ${I}.tcl" ${GOLDENDIR} ${I}.out ${RUNDIR} ${OUTPUTDIR})
ENDFOREACH(I)

set_tests_properties("mos_2d_temtest" PROPERTIES DEPENDS mos_2d)
set_tests_properties("mos_2d_temtest2" PROPERTIES DEPENDS mos_2d)
set_tests_properties("mos_2d_restart" PROPERTIES DEPENDS mos_2d)
set_tests_properties("mos_2d_restart2" PROPERTIES DEPENDS mos_2d)
set_tests_properties("sqlite2" PROPERTIES DEPENDS sqlite1)
set_tests_properties("sqlite3" PROPERTIES DEPENDS sqlite2)
set_tests_properties("mesh4" PROPERTIES DEPENDS mesh3)
set_tests_properties("trimesh2" PROPERTIES DEPENDS trimesh1)

ADD_TEST("mesh3_comp" ${DIFF} --brief ${RUNDIR}/mesh3.msh ${GOLDENDIR}/mesh3.msh)
set_tests_properties("mesh3_comp" PROPERTIES DEPENDS mesh3)
ADD_TEST("mesh4_comp" ${DIFF} --brief ${RUNDIR}/mesh4.msh ${GOLDENDIR}/mesh4.msh)
set_tests_properties("mesh4_comp" PROPERTIES DEPENDS mesh4)
ADD_TEST("trimesh2_msh_comp" ${DIFF} --brief ${RUNDIR}/trimesh2.msh ${GOLDENDIR}/trimesh2.msh)
set_tests_properties("trimesh2_msh_comp" PROPERTIES DEPENDS trimesh1)
ADD_TEST("trimesh3_msh_comp" ${DIFF} --brief ${RUNDIR}/trimesh3.msh ${GOLDENDIR}/trimesh3.msh)
set_tests_properties("trimesh3_msh_comp" PROPERTIES DEPENDS trimesh2)
ADD_TEST("floops_comp" ${DIFF} --brief ${RUNDIR}/floops.flps ${GOLDENDIR}/floops.flps)
set_tests_properties("floops_comp" PROPERTIES DEPENDS floops)
ADD_TEST("mesh2d_flps_comp" ${DIFF} --brief ${RUNDIR}/mesh2d.flps ${GOLDENDIR}/mesh2d.flps)
set_tests_properties("mesh2d_flps_comp" PROPERTIES DEPENDS mesh2d)
ADD_TEST("mesh2d_msh_comp" ${DIFF} --brief ${RUNDIR}/mesh2d.msh ${GOLDENDIR}/mesh2d.msh)
set_tests_properties("mesh2d_msh_comp" PROPERTIES DEPENDS mesh2d)
ADD_TEST("mesh2d_tecplot_comp" ${DIFF} --brief ${RUNDIR}/mesh2d.dat ${GOLDENDIR}/mesh2d.dat)
set_tests_properties("mesh2d_tecplot_comp" PROPERTIES DEPENDS mesh2d)

ADD_TEST("noise_res_2d_flps_comp" ${DIFF} --brief ${RUNDIR}/noise_res_2d.flps ${GOLDENDIR}/noise_res_2d.flps)
set_tests_properties("noise_res_2d_flps_comp" PROPERTIES DEPENDS noise_res_2d)
ADD_TEST("mos_2d_dd_comp" ${DIFF} --brief ${RUNDIR}/mos_2d_dd.msh ${GOLDENDIR}/mos_2d_dd.msh)
set_tests_properties("mos_2d_dd_comp" PROPERTIES DEPENDS mos2d)
ADD_TEST("mos_2d_restart2_comp" ${DIFF} --brief ${RUNDIR}/mos_2d_restart2.msh ${GOLDENDIR}/mos_2d_restart2.msh)
set_tests_properties("mos_2d_restart2_comp" PROPERTIES DEPENDS mos_2d_restart2)
ADD_TEST("gmsh_resistor3d_comp" ${DIFF} --brief ${RUNDIR}/gmsh_resistor3d.dat ${GOLDENDIR}/gmsh_resistor3d.dat)
set_tests_properties("gmsh_resistor3d_comp" PROPERTIES DEPENDS gmsh_resistor3d)

ADD_TEST("mctest1" ${RUNDIFFTEST} "${MODELCOMP} < ${RUNDIR}/mctest.mc" ${GOLDENDIR} mctest.out ${RUNDIR} ${OUTPUTDIR})
ADD_TEST("mctest2" ${DIFF} --brief ${RUNDIR}/diode.cc ${GOLDENDIR}/diode.cc)
set_tests_properties("mctest2" PROPERTIES DEPENDS mctest1)
ADD_TEST("mctest3" ${DIFF} --brief ${RUNDIR}/diode.hh ${GOLDENDIR}/diode.hh)
set_tests_properties("mctest3" PROPERTIES DEPENDS mctest1)

ADD_TEST("mctest_user1" ${RUNDIFFTEST} "${MODELCOMP} < ${RUNDIR}/mctest_user.mc" ${GOLDENDIR} mctest_user.out ${RUNDIR} ${OUTPUTDIR})
ADD_TEST("mctest_user2" ${DIFF} --brief ${RUNDIR}/mctest_user.cc ${GOLDENDIR}/mctest_user.cc)
set_tests_properties("mctest_user2" PROPERTIES DEPENDS mctest_user1)
ADD_TEST("mctest_user3" ${DIFF} --brief ${RUNDIR}/mctest_user.hh ${GOLDENDIR}/mctest_user.hh)
set_tests_properties("mctest_user3" PROPERTIES DEPENDS mctest_user1)

SET (CAPACITANCE_DIR  examples/capacitance)
SET (CAPACITANCE_PATH ${PROJECT_SOURCE_DIR}/${CAPACITANCE_DIR})
ADD_TEST("${CAPACITANCE_DIR}/cap1d" ${RUNDIFFTEST} "${DEVSIMPY} cap1d.py" ${GOLDENDIR} cap1d_example.out ${CAPACITANCE_PATH} ${CAPACITANCE_PATH})
ADD_TEST("${CAPACITANCE_DIR}/cap2d" ${RUNDIFFTEST} "${DEVSIMPY} cap2d.py" ${GOLDENDIR} cap2d_example.out ${CAPACITANCE_PATH} ${CAPACITANCE_PATH})
ADD_TEST("${CAPACITANCE_DIR}/cap2d_comp" ${DIFF} --brief ${CAPACITANCE_PATH}/cap2d.dat ${GOLDENDIR}/cap2d_example.dat)
set_tests_properties("${CAPACITANCE_DIR}/cap2d_comp" PROPERTIES DEPENDS "${CAPACITANCE_DIR}/cap2d")

SET (BIOAPP1_DIR  examples/bioapp1)
SET (BIOAPP1_PATH ${PROJECT_SOURCE_DIR}/${BIOAPP1_DIR})
ADD_TEST("${BIOAPP1_DIR}/bioapp1_2d" ${RUNDIFFTEST} "${DEVSIMPY} bioapp1_2d.py 7" ${GOLDENDIR} bioapp1_2d.out ${BIOAPP1_PATH} ${BIOAPP1_PATH})
ADD_TEST("${BIOAPP1_DIR}/bioapp1_2d_comp" ${DIFF} --brief ${BIOAPP1_PATH}/bioapp1_2d_7.0.dat ${GOLDENDIR}/bioapp1_2d_7.0.dat)
set_tests_properties("${BIOAPP1_DIR}/bioapp1_2d_comp" PROPERTIES DEPENDS "${BIOAPP1_DIR}/bioapp1_2d")
ADD_TEST("${BIOAPP1_DIR}/bioapp1_3d" ${RUNDIFFTEST} "${DEVSIMPY} bioapp1_3d.py 7" ${GOLDENDIR} bioapp1_3d.out ${BIOAPP1_PATH} ${BIOAPP1_PATH})
ADD_TEST("${BIOAPP1_DIR}/bioapp1_3d_comp" ${DIFF} --brief ${BIOAPP1_PATH}/bioapp1_3d_7.0.dat ${GOLDENDIR}/bioapp1_3d_7.0.dat)
set_tests_properties("${BIOAPP1_DIR}/bioapp1_3d_comp" PROPERTIES DEPENDS "${BIOAPP1_DIR}/bioapp1_3d")

SET (DIODE_DIR  examples/diode)
SET (DIODE_PATH ${PROJECT_SOURCE_DIR}/${DIODE_DIR})
SET (DIODE_TESTS diode_1d diode_2d gmsh_diode2d gmsh_diode3d ssac_diode)
FOREACH(I ${DIODE_TESTS})
    ADD_TEST("${DIODE_DIR}/${I}" ${RUNDIFFTEST} "${DEVSIMPY} ${I}.py" ${GOLDENDIR} ${I}_example.out ${DIODE_PATH} ${DIODE_PATH})
ENDFOREACH(I)

SET (MOBILITY_DIR  examples/mobility)
SET (MOBILITY_PATH ${PROJECT_SOURCE_DIR}/${MOBILITY_DIR})
SET (MOBILITY_TESTS gmsh_mos2d gmsh_mos2d_kla)
FOREACH(I ${MOBILITY_TESTS})
    ADD_TEST("${MOBILITY_DIR}/${I}" ${RUNDIFFTEST} "${DEVSIMPY} ${I}.py" ${GOLDENDIR} ${I}_example.out ${MOBILITY_PATH} ${MOBILITY_PATH})
ENDFOREACH(I)

SET (MAGNETIC_DIR  examples/vectorpotential)
SET (MAGNETIC_PATH ${PROJECT_SOURCE_DIR}/${MAGNETIC_DIR})
SET (MAGNETIC_TESTS twowire)
FOREACH(I ${MAGNETIC_TESTS})
    ADD_TEST("${MAGNETIC_DIR}/${I}" ${RUNDIFFTEST} "${DEVSIMPY} ${I}.py" ${GOLDENDIR} ${I}_example.out ${MAGNETIC_PATH} ${MAGNETIC_PATH})
ENDFOREACH(I)


