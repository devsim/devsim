
IF (WIN32)
# python needs to be available in the environment used to run the tests
SET (RUNDIFFTEST python ${PROJECT_SOURCE_DIR}/testing/rundifftest.py)
SET (RUNDIR      ${PROJECT_SOURCE_DIR}/testing)
#SET (MODELCOMP   ${PROJECT_BINARY_DIR}/src/adiff/Release/modelcomp)
ELSE (WIN32)
SET (RUNDIFFTEST ${PROJECT_SOURCE_DIR}/testing/rundifftest.py)
SET (RUNDIR      ${PROJECT_SOURCE_DIR}/testing)
#SET (MODELCOMP   ${PROJECT_BINARY_DIR}/src/adiff/modelcomp)
ENDIF (WIN32)

#IF (DEVSIM_PY2_TEST_EXE)
#SET (DEVSIM_PY2   ${DEVSIM_PY2_TEST_EXE})
#ENDIF (DEVSIM_PY2_TEST_EXE)

IF (DEVSIM_PY3_TEST_EXE)
SET (DEVSIM_PY3   ${DEVSIM_PY3_TEST_EXE})
ENDIF (DEVSIM_PY3_TEST_EXE)

IF (DEVSIM_TEST_GOLDENDIR)
SET (GOLDENDIR  ${DEVSIM_TEST_GOLDENDIR})
ENDIF (DEVSIM_TEST_GOLDENDIR)

ENABLE_TESTING()

#SET (NEWPYTESTS
#)
#
#FOREACH(I ${NEWPYTESTS})
#    ADD_TEST("testing/${I}" ${RUNDIFFTEST} "${DEVSIM_PY2} ${I}.py" ${GOLDENDIR}/testing --output ${I}.out ${RUNDIR} ${OUTPUTDIR})
#ENDFOREACH(I)

SET (NEWPY3TESTS
  info
  cap2
  ssac_cap
  ssac_cap_2d_edge
  ssac_cap_2d_element
  ssac_cap_3d_edge
  ssac_cap_3d_element
  equation1
  ptest1
  ptest2
  sqlite2
  sqlite3
  testfunc
  testfunc_extended
  utf8_2
  laux1
  pythonmesh1d
  Fermi1
  Fermi1_float128
  GaussFermi
  GaussFermi_float128
  kahan
  kahan_float128
  fpetest1
  fpetest2
  res1 res2 res3 ssac_res noise_res
  symdiff1
  erf1 erf2
  mesh1 mesh2 mesh3 mesh4
  mesh2d
  transient_circ
  transient_circ2
  transient_circ3
  transient_rc
circ1
circ2
circ3
circ4
ssac_circ
trimesh1
trimesh2
noise_res_2d
dio2_element_2d
mos_2d 
mos_2d_restart
mos_2d_restart2
)
FOREACH(I ${NEWPY3TESTS})
    ADD_TEST("testing/${I}" ${RUNDIFFTEST} --testexe ${DEVSIM_PY3} --args ${I}.py --golden ${GOLDENDIR}/testing --output ${I}.out --working ${RUNDIR})
ENDFOREACH(I)

set_tests_properties("testing/mos_2d_restart" PROPERTIES DEPENDS testing/mos_2d)
set_tests_properties("testing/mos_2d_restart2" PROPERTIES DEPENDS testing/mos_2d)
set_tests_properties("testing/sqlite3" PROPERTIES DEPENDS testing/sqlite2)
set_tests_properties("testing/mesh4" PROPERTIES DEPENDS testing/mesh3)
set_tests_properties("testing/trimesh2" PROPERTIES DEPENDS testing/trimesh1)

ADD_TEST("testing/pythonmesh1d_comp" ${RUNDIFFTEST} --output ${RUNDIR}/pythonmesh1d.msh --golden ${GOLDENDIR}/testing)
set_tests_properties("testing/pythonmesh1d_comp" PROPERTIES DEPENDS "testing/pythonmesh1d")
ADD_TEST("testing/mesh3_comp" ${RUNDIFFTEST} --output ${RUNDIR}/mesh3.msh --golden ${GOLDENDIR}/testing)
set_tests_properties("testing/mesh3_comp" PROPERTIES DEPENDS testing/mesh3)
ADD_TEST("testing/mesh4_comp" ${RUNDIFFTEST} --output ${RUNDIR}/mesh4.msh --golden ${GOLDENDIR}/testing)
set_tests_properties("testing/mesh4_comp" PROPERTIES DEPENDS testing/mesh4)
ADD_TEST("testing/trimesh2_msh_comp" ${RUNDIFFTEST} --output ${RUNDIR}/trimesh2.msh --golden ${GOLDENDIR}/testing)
set_tests_properties("testing/trimesh2_msh_comp" PROPERTIES DEPENDS testing/trimesh1)
ADD_TEST("testing/trimesh3_msh_comp" ${RUNDIFFTEST} --output ${RUNDIR}/trimesh3.msh --golden ${GOLDENDIR}/testing)
set_tests_properties("testing/trimesh3_msh_comp" PROPERTIES DEPENDS testing/trimesh2)
ADD_TEST("testing/mesh2d_flps_comp" ${RUNDIFFTEST} --output ${RUNDIR}/mesh2d.flps --golden ${GOLDENDIR}/testing)
set_tests_properties("testing/mesh2d_flps_comp" PROPERTIES DEPENDS testing/mesh2d)
ADD_TEST("testing/mesh2d_msh_comp" ${RUNDIFFTEST} --output ${RUNDIR}/mesh2d.msh --golden ${GOLDENDIR}/testing)
set_tests_properties("testing/mesh2d_msh_comp" PROPERTIES DEPENDS testing/mesh2d)
ADD_TEST("testing/mesh2d_tecplot_comp" ${RUNDIFFTEST} --output ${RUNDIR}/mesh2d.dat --golden ${GOLDENDIR}/testing)
set_tests_properties("testing/mesh2d_tecplot_comp" PROPERTIES DEPENDS testing/mesh2d)

ADD_TEST("testing/noise_res_2d_flps_comp" ${RUNDIFFTEST} --output ${RUNDIR}/noise_res_2d.flps --golden ${GOLDENDIR}/testing)
set_tests_properties("testing/noise_res_2d_flps_comp" PROPERTIES DEPENDS testing/noise_res_2d)
ADD_TEST("testing/mos_2d_dd_comp" ${RUNDIFFTEST} --output ${RUNDIR}/mos_2d_dd.msh --golden ${GOLDENDIR}/testing)
set_tests_properties("testing/mos_2d_dd_comp" PROPERTIES DEPENDS testing/mos_2d)
ADD_TEST("testing/mos_2d_restart2_comp" ${RUNDIFFTEST} --output ${RUNDIR}/mos_2d_restart2.msh --golden ${GOLDENDIR}/testing)
set_tests_properties("testing/mos_2d_restart2_comp" PROPERTIES DEPENDS testing/mos_2d_restart2)

#### Disable these tests
IF (0)
ADD_TEST("testing/mctest1" ${RUNDIFFTEST} "${MODELCOMP} < ${RUNDIR}/mctest.mc" --golden ${GOLDENDIR}/testing --output mctest.out --working ${RUNDIR})
ADD_TEST("testing/mctest2" ${RUNDIFFTEST} --output ${RUNDIR}/diode.cc --golden ${GOLDENDIR}/testing)
set_tests_properties("testing/mctest2" PROPERTIES DEPENDS testing/mctest1)
ADD_TEST("testing/mctest3" ${RUNDIFFTEST} --output ${RUNDIR}/diode.hh --golden ${GOLDENDIR}/testing)
set_tests_properties("testing/mctest3" PROPERTIES DEPENDS testing/mctest1)

ADD_TEST("testing/mctest_user1" ${RUNDIFFTEST} "${MODELCOMP} < ${RUNDIR}/mctest_user.mc" --golden ${GOLDENDIR}/testing --output mctest_user.out --working ${RUNDIR})
ADD_TEST("testing/mctest_user2" ${RUNDIFFTEST} --output ${RUNDIR}/mctest_user.cc --golden ${GOLDENDIR}/testing)
set_tests_properties("testing/mctest_user2" PROPERTIES DEPENDS testing/mctest_user1)
ADD_TEST("testing/mctest_user3" ${RUNDIFFTEST} --output ${RUNDIR}/mctest_user.hh --golden ${GOLDENDIR}/testing)
set_tests_properties("testing/mctest_user3" PROPERTIES DEPENDS testing/mctest_user1)
ENDIF (0)

SET (CAPACITANCE_DIR  examples/capacitance)
SET (CAPACITANCE_PATH ${PROJECT_SOURCE_DIR}/${CAPACITANCE_DIR})
ADD_TEST("${CAPACITANCE_DIR}/cap1d" ${RUNDIFFTEST} --testexe ${DEVSIM_PY3} --args cap1d.py --golden ${GOLDENDIR}/${CAPACITANCE_DIR} --output cap1d.out --working ${CAPACITANCE_PATH})
ADD_TEST("${CAPACITANCE_DIR}/cap2d" ${RUNDIFFTEST} --testexe ${DEVSIM_PY3} --args cap2d.py --golden ${GOLDENDIR}/${CAPACITANCE_DIR} --output cap2d.out --working ${CAPACITANCE_PATH})
ADD_TEST("${CAPACITANCE_DIR}/cap2d_comp" ${RUNDIFFTEST} --output ${CAPACITANCE_PATH}/cap2d.dat --golden ${GOLDENDIR}/${CAPACITANCE_DIR})
set_tests_properties("${CAPACITANCE_DIR}/cap2d_comp" PROPERTIES DEPENDS "${CAPACITANCE_DIR}/cap2d")

SET (BIOAPP1_DIR  examples/bioapp1)
SET (BIOAPP1_PATH ${PROJECT_SOURCE_DIR}/${BIOAPP1_DIR})
ADD_TEST("${BIOAPP1_DIR}/bioapp1_2d" ${RUNDIFFTEST} --testexe ${DEVSIM_PY3} --args bioapp1_2d.py 7 --golden ${GOLDENDIR}/${BIOAPP1_DIR} --output bioapp1_2d.out --working ${BIOAPP1_PATH})
ADD_TEST("${BIOAPP1_DIR}/bioapp1_2d_comp" ${RUNDIFFTEST} --output ${BIOAPP1_PATH}/bioapp1_2d_7.0.dat --golden ${GOLDENDIR}/${BIOAPP1_DIR})
set_tests_properties("${BIOAPP1_DIR}/bioapp1_2d_comp" PROPERTIES DEPENDS "${BIOAPP1_DIR}/bioapp1_2d")
ADD_TEST("${BIOAPP1_DIR}/bioapp1_3d" ${RUNDIFFTEST} --testexe ${DEVSIM_PY3} --args bioapp1_3d.py 7 --golden ${GOLDENDIR}/${BIOAPP1_DIR} --output bioapp1_3d.out --working ${BIOAPP1_PATH})
ADD_TEST("${BIOAPP1_DIR}/bioapp1_3d_comp" ${RUNDIFFTEST} --output ${BIOAPP1_PATH}/bioapp1_3d_7.0.dat --golden ${GOLDENDIR}/${BIOAPP1_DIR})
set_tests_properties("${BIOAPP1_DIR}/bioapp1_3d_comp" PROPERTIES DEPENDS "${BIOAPP1_DIR}/bioapp1_3d")

SET (DIODE_DIR  examples/diode)
SET (DIODE_PATH ${PROJECT_SOURCE_DIR}/${DIODE_DIR})
SET (DIODE_TESTS diode_1d diode_1d_custom diode_2d gmsh_diode2d gmsh_diode3d gmsh_diode3d_float128 ssac_diode laux2d laux3d pythonmesh3d)
FOREACH(I ${DIODE_TESTS})
    ADD_TEST("${DIODE_DIR}/${I}" ${RUNDIFFTEST} --testexe ${DEVSIM_PY3} --args ${I}.py --golden ${GOLDENDIR}/${DIODE_DIR} --output ${I}.out --working ${DIODE_PATH})
ENDFOREACH(I)
set_tests_properties("${DIODE_DIR}/laux2d" PROPERTIES DEPENDS "${DIODE_DIR}/gmsh_diode2d")
set_tests_properties("${DIODE_DIR}/laux3d" PROPERTIES DEPENDS "${DIODE_DIR}/gmsh_diode3d")

SET (MOBILITY_DIR  examples/mobility)
SET (MOBILITY_PATH ${PROJECT_SOURCE_DIR}/${MOBILITY_DIR})
SET (MOBILITY_TESTS gmsh_mos2d gmsh_mos2d_kla pythonmesh2d)
FOREACH(I ${MOBILITY_TESTS})
    ADD_TEST("${MOBILITY_DIR}/${I}" ${RUNDIFFTEST} --testexe ${DEVSIM_PY3} --args ${I}.py --golden ${GOLDENDIR}/${MOBILITY_DIR} --output ${I}.out --working ${MOBILITY_PATH})
ENDFOREACH(I)

SET (MAGNETIC_DIR  examples/vectorpotential)
SET (MAGNETIC_PATH ${PROJECT_SOURCE_DIR}/${MAGNETIC_DIR})
SET (MAGNETIC_TESTS twowire)
FOREACH(I ${MAGNETIC_TESTS})
    ADD_TEST("${MAGNETIC_DIR}/${I}" ${RUNDIFFTEST} --testexe ${DEVSIM_PY3} --args ${I}.py --golden ${GOLDENDIR}/${MAGNETIC_DIR} --output ${I}.out --working ${MAGNETIC_PATH})
ENDFOREACH(I)

set_tests_properties("testing/laux1" PROPERTIES DEPENDS testing/trimesh2)


