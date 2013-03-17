package provide dsSimpleDD 1.0

package require dsBase

proc createBernoulli {device region} {
  ensureEdgeFromNodeModelExists $device $region Potential

# #### test for requisite models here
 set vdiffstr "(Potential@n0 - Potential@n1)/V_t"
 createEdgeModel $device $region vdiff $vdiffstr
# #assuming that Potential is based on a solution, and not a model
 createEdgeModel $device $region vdiff:Potential@n0 "V_t^(-1)"
 createEdgeModel $device $region vdiff:Potential@n1 "-vdiff:Potential@n0"
#
 createEdgeModel $device $region "Bern01"              "B(vdiff)"
 createEdgeModel $device $region "Bern01:Potential@n0" "dBdx(vdiff) * vdiff:Potential@n0"
 createEdgeModel $device $region "Bern01:Potential@n1" "-Bern01:Potential@n0"
# #identity of Bernoulli functions
 createEdgeModel $device $region "Bern10"              "Bern01 + vdiff"
 createEdgeModel $device $region "Bern10:Potential@n0" "Bern01:Potential@n0 + vdiff:Potential@n0"
 createEdgeModel $device $region "Bern10:Potential@n1" "Bern01:Potential@n1 + vdiff:Potential@n1"
}

proc createElectronCurrent {device region} {
  ensureEdgeFromNodeModelExists $device $region Potential
  ensureEdgeFromNodeModelExists $device $region Electrons

  # Make sure the bernoulli functions exist
  if {![inEdgeModelList $device $region Bern01]} {
    createBernoulli $device $region
  }

#### test for requisite models here
 set Jn       "ElectronCharge*mu_n*EdgeInverseLength*V_t*(Electrons@n1*Bern10 - Electrons@n0*Bern01)"

 createEdgeModel           $device $region "ElectronCurrent"  $Jn

 foreach {i} {Electrons Potential} {
   createEdgeModelDerivatives $device $region "ElectronCurrent"  $Jn $i
 }
}

proc createHoleCurrent {device region} {
  ensureEdgeFromNodeModelExists $device $region Potential
  ensureEdgeFromNodeModelExists $device $region Holes

  if {![inEdgeModelList $device $region Bern01]} {
    createBernoulli $device $region
  }

### test for requisite models here
 set Jp       "-ElectronCharge*mu_p*EdgeInverseLength*V_t*(Holes@n1*Bern01 - Holes@n0*Bern10)"

 createEdgeModel           $device $region "HoleCurrent"  $Jp

 foreach {i} {Holes Potential} {
   createEdgeModelDerivatives $device $region "HoleCurrent"  $Jp $i
 }
}

