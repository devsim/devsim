
IF (WIN32)
SET (DIFF C:/cygwin/bin/diff.exe)
SET (DIFF_ARGS --brief -w)
SET (PERL C:/cygwin/bin/perl.exe)
SET (RUNDIFFTEST ${PERL} ${PROJECT_SOURCE_DIR}/testing/rundifftest.pl)
SET (RUNDIR      ${PROJECT_SOURCE_DIR}/testing)
SET (OUTPUTDIR   ${RUNDIR})
SET (MODELCOMP   ${PROJECT_BINARY_DIR}/src/adiff/Release/modelcomp)
ELSE (WIN32)
SET (DIFF diff)
SET (DIFF_ARGS --brief)
SET (RUNDIFFTEST ${PROJECT_SOURCE_DIR}/testing/rundifftest.pl)
SET (RUNDIR      ${PROJECT_SOURCE_DIR}/testing)
SET (OUTPUTDIR   ${RUNDIR})
SET (MODELCOMP   ${PROJECT_BINARY_DIR}/src/adiff/modelcomp)
ENDIF (WIN32)

IF (DEVSIM_PY2_TEST_EXE)
SET (DEVSIM_PY2   ${DEVSIM_PY2_TEST_EXE})
ENDIF (DEVSIM_PY2_TEST_EXE)

IF (DEVSIM_PY3_TEST_EXE)
SET (DEVSIM_PY3   ${DEVSIM_PY3_TEST_EXE})
ENDIF (DEVSIM_PY3_TEST_EXE)

IF (DEVSIM_TCL_TEST_EXE)
SET (DEVSIM_TCL  ${DEVSIM_TCL_TEST_EXE})
ENDIF (DEVSIM_TCL_TEST_EXE)

IF (DEVSIM_TEST_GOLDENDIR)
SET (GOLDENDIR  ${DEVSIM_TEST_GOLDENDIR})
ENDIF (DEVSIM_TEST_GOLDENDIR)

ENABLE_TESTING()

SET (NEWPYTESTS
  cap2
  equation1
  ptest1
  ptest2
  sqlite2
  sqlite3
  testfunc
  utf8_2
  laux1
  pythonmesh1d
)

FOREACH(I ${NEWPYTESTS})
    ADD_TEST("testing/${I}" ${RUNDIFFTEST} "${DEVSIM_PY2} ${I}.py" ${GOLDENDIR}/testing ${I}.out ${RUNDIR} ${OUTPUTDIR})
ENDFOREACH(I)

ADD_TEST("testing/pythonmesh1d_comp" ${DIFF} ${DIFF_ARGS} ${RUNDIR}/pythonmesh1d.msh ${GOLDENDIR}/testing/pythonmesh1d.msh)
set_tests_properties("testing/pythonmesh1d_comp" PROPERTIES DEPENDS "testing/pythonmesh1d")

SET (NEWPY3TESTS
  cap3
)
FOREACH(I ${NEWPY3TESTS})
    ADD_TEST("testing/${I}_python3" ${RUNDIFFTEST} "${DEVSIM_PY3} ${I}.py" ${GOLDENDIR}/testing ${I}.out ${RUNDIR} ${OUTPUTDIR})
ENDFOREACH(I)

SET (NEWTESTS res1 res2 res3 dio1_utf8 dio2 circ1 circ2 circ3 circ4
ssac_circ ssac_cap ssac_res ssac_diode utf8_1 mesh1 mesh2 mesh3 mesh4 trimesh1 trimesh2 floops erf1 Fermi1
mesh2d dio2_2d mos_2d mos_2d_restart mos_2d_restart2 noise_res noise_res_2d dio2_element_2d
gmsh_resistor2d gmsh_resistor3d
transient_circ transient_circ2 transient_circ3
#mos_2d_temtest
#mos_2d_temtest2
package_cap1
#sqlite1
rename1
symdiff1
#thread_1
fpetest1
#fpetest2
kahan
)
FOREACH(I ${NEWTESTS})
    ADD_TEST("testing/${I}" ${RUNDIFFTEST} "${DEVSIM_TCL} ${I}.tcl" ${GOLDENDIR}/testing ${I}.out ${RUNDIR} ${OUTPUTDIR})
ENDFOREACH(I)


# on redhat 6.4 this test fails on the Tcl platform
#set_tests_properties("testing/fpetest2" PROPERTIES TIMEOUT 5)
#set_tests_properties("testing/mos_2d_temtest" PROPERTIES DEPENDS testing/mos_2d)
#set_tests_properties("testing/mos_2d_temtest2" PROPERTIES DEPENDS testing/mos_2d)
set_tests_properties("testing/mos_2d_restart" PROPERTIES DEPENDS testing/mos_2d)
set_tests_properties("testing/mos_2d_restart2" PROPERTIES DEPENDS testing/mos_2d)
set_tests_properties("testing/sqlite2" PROPERTIES DEPENDS testing/sqlite1)
set_tests_properties("testing/sqlite3" PROPERTIES DEPENDS testing/sqlite2)
set_tests_properties("testing/mesh4" PROPERTIES DEPENDS testing/mesh3)
set_tests_properties("testing/trimesh2" PROPERTIES DEPENDS testing/trimesh1)

ADD_TEST("testing/mesh3_comp" ${DIFF} ${DIFF_ARGS} ${RUNDIR}/mesh3.msh ${GOLDENDIR}/testing/mesh3.msh)
set_tests_properties("testing/mesh3_comp" PROPERTIES DEPENDS testing/mesh3)
ADD_TEST("testing/mesh4_comp" ${DIFF} ${DIFF_ARGS} ${RUNDIR}/mesh4.msh ${GOLDENDIR}/testing/mesh4.msh)
set_tests_properties("testing/mesh4_comp" PROPERTIES DEPENDS testing/mesh4)
ADD_TEST("testing/trimesh2_msh_comp" ${DIFF} ${DIFF_ARGS} ${RUNDIR}/trimesh2.msh ${GOLDENDIR}/testing/trimesh2.msh)
set_tests_properties("testing/trimesh2_msh_comp" PROPERTIES DEPENDS testing/trimesh1)
ADD_TEST("testing/trimesh3_msh_comp" ${DIFF} ${DIFF_ARGS} ${RUNDIR}/trimesh3.msh ${GOLDENDIR}/testing/trimesh3.msh)
set_tests_properties("testing/trimesh3_msh_comp" PROPERTIES DEPENDS testing/trimesh2)
ADD_TEST("testing/floops_comp" ${DIFF} ${DIFF_ARGS} ${RUNDIR}/floops.flps ${GOLDENDIR}/testing/floops.flps)
set_tests_properties("testing/floops_comp" PROPERTIES DEPENDS testing/floops)
ADD_TEST("testing/mesh2d_flps_comp" ${DIFF} ${DIFF_ARGS} ${RUNDIR}/mesh2d.flps ${GOLDENDIR}/testing/mesh2d.flps)
set_tests_properties("testing/mesh2d_flps_comp" PROPERTIES DEPENDS testing/mesh2d)
ADD_TEST("testing/mesh2d_msh_comp" ${DIFF} ${DIFF_ARGS} ${RUNDIR}/mesh2d.msh ${GOLDENDIR}/testing/mesh2d.msh)
set_tests_properties("testing/mesh2d_msh_comp" PROPERTIES DEPENDS testing/mesh2d)
ADD_TEST("testing/mesh2d_tecplot_comp" ${DIFF} ${DIFF_ARGS} ${RUNDIR}/mesh2d.dat ${GOLDENDIR}/testing/mesh2d.dat)
set_tests_properties("testing/mesh2d_tecplot_comp" PROPERTIES DEPENDS testing/mesh2d)

ADD_TEST("testing/noise_res_2d_flps_comp" ${DIFF} ${DIFF_ARGS} ${RUNDIR}/noise_res_2d.flps ${GOLDENDIR}/testing/noise_res_2d.flps)
set_tests_properties("testing/noise_res_2d_flps_comp" PROPERTIES DEPENDS testing/noise_res_2d)
ADD_TEST("testing/mos_2d_dd_comp" ${DIFF} ${DIFF_ARGS} ${RUNDIR}/mos_2d_dd.msh ${GOLDENDIR}/testing/mos_2d_dd.msh)
set_tests_properties("testing/mos_2d_dd_comp" PROPERTIES DEPENDS testing/mos_2d)
ADD_TEST("testing/mos_2d_restart2_comp" ${DIFF} ${DIFF_ARGS} ${RUNDIR}/mos_2d_restart2.msh ${GOLDENDIR}/testing/mos_2d_restart2.msh)
set_tests_properties("testing/mos_2d_restart2_comp" PROPERTIES DEPENDS testing/mos_2d_restart2)
ADD_TEST("testing/gmsh_resistor3d_comp" ${DIFF} ${DIFF_ARGS} ${RUNDIR}/gmsh_resistor3d.dat ${GOLDENDIR}/testing/gmsh_resistor3d.dat)
set_tests_properties("testing/gmsh_resistor3d_comp" PROPERTIES DEPENDS testing/gmsh_resistor3d)

#### Disable these tests
IF (0)
ADD_TEST("testing/mctest1" ${RUNDIFFTEST} "${MODELCOMP} < ${RUNDIR}/mctest.mc" ${GOLDENDIR}/testing mctest.out ${RUNDIR} ${OUTPUTDIR})
ADD_TEST("testing/mctest2" ${DIFF} ${DIFF_ARGS} ${RUNDIR}/diode.cc ${GOLDENDIR}/testing/diode.cc)
set_tests_properties("testing/mctest2" PROPERTIES DEPENDS testing/mctest1)
ADD_TEST("testing/mctest3" ${DIFF} ${DIFF_ARGS} ${RUNDIR}/diode.hh ${GOLDENDIR}/testing/diode.hh)
set_tests_properties("testing/mctest3" PROPERTIES DEPENDS testing/mctest1)

ADD_TEST("testing/mctest_user1" ${RUNDIFFTEST} "${MODELCOMP} < ${RUNDIR}/mctest_user.mc" ${GOLDENDIR}/testing mctest_user.out ${RUNDIR} ${OUTPUTDIR})
ADD_TEST("testing/mctest_user2" ${DIFF} ${DIFF_ARGS} ${RUNDIR}/mctest_user.cc ${GOLDENDIR}/testing/mctest_user.cc)
set_tests_properties("testing/mctest_user2" PROPERTIES DEPENDS testing/mctest_user1)
ADD_TEST("testing/mctest_user3" ${DIFF} ${DIFF_ARGS} ${RUNDIR}/mctest_user.hh ${GOLDENDIR}/testing/mctest_user.hh)
set_tests_properties("testing/mctest_user3" PROPERTIES DEPENDS testing/mctest_user1)
ENDIF (0)

SET (CAPACITANCE_DIR  examples/capacitance)
SET (CAPACITANCE_PATH ${PROJECT_SOURCE_DIR}/${CAPACITANCE_DIR})
ADD_TEST("${CAPACITANCE_DIR}/cap1d" ${RUNDIFFTEST} "${DEVSIM_PY3} cap1d.py" ${GOLDENDIR}/${CAPACITANCE_DIR} cap1d.out ${CAPACITANCE_PATH} ${CAPACITANCE_PATH})
ADD_TEST("${CAPACITANCE_DIR}/cap2d" ${RUNDIFFTEST} "${DEVSIM_PY3} cap2d.py" ${GOLDENDIR}/${CAPACITANCE_DIR} cap2d.out ${CAPACITANCE_PATH} ${CAPACITANCE_PATH})
ADD_TEST("${CAPACITANCE_DIR}/cap2d_comp" ${DIFF} ${DIFF_ARGS} ${CAPACITANCE_PATH}/cap2d.dat ${GOLDENDIR}/${CAPACITANCE_DIR}/cap2d.dat)
set_tests_properties("${CAPACITANCE_DIR}/cap2d_comp" PROPERTIES DEPENDS "${CAPACITANCE_DIR}/cap2d")

SET (BIOAPP1_DIR  examples/bioapp1)
SET (BIOAPP1_PATH ${PROJECT_SOURCE_DIR}/${BIOAPP1_DIR})
ADD_TEST("${BIOAPP1_DIR}/bioapp1_2d" ${RUNDIFFTEST} "${DEVSIM_PY3} bioapp1_2d.py 7" ${GOLDENDIR}/${BIOAPP1_DIR} bioapp1_2d.out ${BIOAPP1_PATH} ${BIOAPP1_PATH})
ADD_TEST("${BIOAPP1_DIR}/bioapp1_2d_comp" ${DIFF} ${DIFF_ARGS} ${BIOAPP1_PATH}/bioapp1_2d_7.0.dat ${GOLDENDIR}/${BIOAPP1_DIR}/bioapp1_2d_7.0.dat)
set_tests_properties("${BIOAPP1_DIR}/bioapp1_2d_comp" PROPERTIES DEPENDS "${BIOAPP1_DIR}/bioapp1_2d")
ADD_TEST("${BIOAPP1_DIR}/bioapp1_3d" ${RUNDIFFTEST} "${DEVSIM_PY3} bioapp1_3d.py 7" ${GOLDENDIR}/${BIOAPP1_DIR} bioapp1_3d.out ${BIOAPP1_PATH} ${BIOAPP1_PATH})
ADD_TEST("${BIOAPP1_DIR}/bioapp1_3d_comp" ${DIFF} ${DIFF_ARGS} ${BIOAPP1_PATH}/bioapp1_3d_7.0.dat ${GOLDENDIR}/${BIOAPP1_DIR}/bioapp1_3d_7.0.dat)
set_tests_properties("${BIOAPP1_DIR}/bioapp1_3d_comp" PROPERTIES DEPENDS "${BIOAPP1_DIR}/bioapp1_3d")

SET (DIODE_DIR  examples/diode)
SET (DIODE_PATH ${PROJECT_SOURCE_DIR}/${DIODE_DIR})
SET (DIODE_TESTS diode_1d diode_2d gmsh_diode2d gmsh_diode3d ssac_diode laux2d laux3d pythonmesh3d)
FOREACH(I ${DIODE_TESTS})
    ADD_TEST("${DIODE_DIR}/${I}" ${RUNDIFFTEST} "${DEVSIM_PY3} ${I}.py" ${GOLDENDIR}/${DIODE_DIR} ${I}.out ${DIODE_PATH} ${DIODE_PATH})
ENDFOREACH(I)
set_tests_properties("${DIODE_DIR}/laux2d" PROPERTIES DEPENDS "${DIODE_DIR}/gmsh_diode2d")
set_tests_properties("${DIODE_DIR}/laux3d" PROPERTIES DEPENDS "${DIODE_DIR}/gmsh_diode3d")

SET (MOBILITY_DIR  examples/mobility)
SET (MOBILITY_PATH ${PROJECT_SOURCE_DIR}/${MOBILITY_DIR})
SET (MOBILITY_TESTS gmsh_mos2d gmsh_mos2d_kla pythonmesh2d)
FOREACH(I ${MOBILITY_TESTS})
    ADD_TEST("${MOBILITY_DIR}/${I}" ${RUNDIFFTEST} "${DEVSIM_PY3} ${I}.py" ${GOLDENDIR}/${MOBILITY_DIR} ${I}.out ${MOBILITY_PATH} ${MOBILITY_PATH})
ENDFOREACH(I)

SET (MAGNETIC_DIR  examples/vectorpotential)
SET (MAGNETIC_PATH ${PROJECT_SOURCE_DIR}/${MAGNETIC_DIR})
SET (MAGNETIC_TESTS twowire)
FOREACH(I ${MAGNETIC_TESTS})
    ADD_TEST("${MAGNETIC_DIR}/${I}" ${RUNDIFFTEST} "${DEVSIM_PY3} ${I}.py" ${GOLDENDIR}/${MAGNETIC_DIR} ${I}.out ${MAGNETIC_PATH} ${MAGNETIC_PATH})
ENDFOREACH(I)

SET (GENIUS_DIR  examples/genius)
SET (GENIUS_PATH ${PROJECT_SOURCE_DIR}/${GENIUS_DIR})
SET (GENIUS_TESTS read_nmos read_pn2d genius_diode2d)
FOREACH(I ${GENIUS_TESTS})
    ADD_TEST("${GENIUS_DIR}/${I}" ${RUNDIFFTEST} "${DEVSIM_PY3} ${I}.py" ${GOLDENDIR}/${GENIUS_DIR} ${I}.out ${GENIUS_PATH} ${GENIUS_PATH})
ENDFOREACH(I)
ADD_TEST("${GENIUS_DIR}/pn2d_comp" ${DIFF} ${DIFF_ARGS} ${GENIUS_PATH}/pn2d.msh ${GOLDENDIR}/${GENIUS_DIR}/pn2d.msh)
set_tests_properties("${GENIUS_DIR}/pn2d_comp" PROPERTIES DEPENDS "${GENIUS_DIR}/read_pn2d")
set_tests_properties("${GENIUS_DIR}/genius_diode2d" PROPERTIES DEPENDS "${GENIUS_DIR}/pn2d_comp")



#ADD_TEST("testing/laux1" ${RUNDIFFTEST} "${DEVSIM_PY2} laux1.py" ${GOLDENDIR}/testing laux1.out ${RUNDIR} ${OUTPUTDIR})
# this is weird since it is dependent on one of our tcl tests
set_tests_properties("testing/laux1" PROPERTIES DEPENDS testing/trimesh2)




