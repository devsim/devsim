# Copyright 2013 Devsim LLC
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

#### circ1.tcl
####
#### two devices which connected to one another

## basically just a resistor
proc printCurrents {device cname} {
set ecurr [get_contact_current -contact $cname -equation ElectronContinuityEquation -device $device]
#set hcurr [get_contact_current -contact $cname -equation HoleContinuityEquation -device $device]
set hcurr 0
set tcurr [expr {$ecurr+$hcurr}]                                        
puts [format "Contact: %s\nElectron %s\nHole %s\nTotal %s" $cname $ecurr $hcurr $tcurr]
}

set devices {MyDevice1 MyDevice2}
set region MyRegion

####
#### Meshing
####

create_1d_mesh -mesh dog
add_1d_mesh_line -mesh dog -pos 0 -ps 0.1 -tag top
add_1d_mesh_line -mesh dog -pos 1 -ps 0.1 -tag bot
add_1d_contact   -mesh dog -name top -tag top -material metal
add_1d_contact   -mesh dog -name bot -tag bot -material metal
add_1d_region    -mesh dog -material Si -region $region -tag1 top -tag2 bot
finalize_mesh -mesh dog

foreach device $devices {
    puts $device;
    create_device -mesh dog -device $device
}

foreach device $devices {
    puts $device;
####
#### Constants
####
set_parameter -device $device -name "Permittivity"     -value [expr 11.1*8.85e-14]
set_parameter -device $device -name "ElectronCharge"   -value 1.6e-19
set_parameter -device $device -name "IntrinsicDensity" -value 1.0e10
set_parameter -device $device -name "ThermalVoltage"   -value 0.0259

set_parameter -device $device -name "mu_n" -value 400
set_parameter -device $device -name "mu_p" -value 200

####
#### Potential
####

node_solution -device $device -region $region -name Potential
edge_from_node_model -device $device -region $region -node_model Potential

####
#### NetDoping
####

node_model -device $device -region $region -name NetDoping -equation "1.0e16;"

####
#### IntrinsicElectrons
####
node_model -device $device -region $region -name "IntrinsicElectrons"           -equation "NetDoping;"
#node_model -device $device -region $region -name "IntrinsicElectrons:Potential" -equation  "diff(IntrinsicDensity*exp(Potential/ThermalVoltage), Potential);"

####
#### IntrinsicCharge
####
node_model -device $device -region $region -name "IntrinsicCharge"           -equation "-IntrinsicElectrons + NetDoping;"
node_model -device $device -region $region -name "IntrinsicCharge:Potential" -equation "-IntrinsicElectrons:Potential;"


####
#### ElectricField
####

edge_model -device $device -region $region -name ElectricField -equation "(Potential@n0 - Potential@n1)*EdgeInverseLength;"
edge_model -device $device -region $region -name "ElectricField:Potential@n0" -equation "EdgeInverseLength;"
edge_model -device $device -region $region -name "ElectricField:Potential@n1" -equation "-EdgeInverseLength;"

####
#### PotentialEdgeFlux
####

edge_model -device $device -region $region -name PotentialEdgeFlux -equation "Permittivity*ElectricField;"
edge_model -device $device -region $region -name PotentialEdgeFlux:Potential@n0 -equation "diff(Permittivity*ElectricField, Potential@n0);"
edge_model -device $device -region $region -name PotentialEdgeFlux:Potential@n1 -equation "-PotentialEdgeFlux:Potential@n0;"

####
#### PotentialEquation
####

equation -device $device -region $region -name PotentialEquation -variable_name Potential -node_model "" \
    -edge_model "PotentialEdgeFlux" -time_node_model "" -variable_update log_damp 


}
set_parameter -name topbias -value 0.0

add_circuit_node -name cnode1 -variable_update log_damp
circuit_element -name R1 -n1 cnode1 -n2 0 -value 1e15
####
#### Potential contact equations
####

set conteq "Permittivity*ElectricField;"

node_model -device MyDevice1 -region $region -name "topnode_model"           -equation "Potential - topbias;"
node_model -device MyDevice1 -region $region -name "topnode_model:Potential" -equation "1;"
edge_model -device MyDevice1 -region $region -name "contactcharge_edge_top"  -equation $conteq
#
node_model -device MyDevice1 -region $region -name "bottomnode_model"           -equation "Potential - cnode1;"
node_model -device MyDevice1 -region $region -name "bottomnode_model:Potential" -equation "1;"
node_model -device MyDevice1 -region $region -name "bottomnode_model:cnode1"    -equation "-1;"
edge_model -device MyDevice1 -region $region -name "contactcharge_edge_bottom"  -equation $conteq

contact_equation -device MyDevice1 -contact "top" -name "PotentialEquation" -variable_name Potential \
			-node_model topnode_model -edge_model "" \
			-node_charge_model "" -edge_charge_model "contactcharge_edge_top" \
			-node_current_model ""   -edge_current_model ""

contact_equation -device MyDevice1 -contact "bot" -name "PotentialEquation" -variable_name Potential \
			-node_model bottomnode_model -edge_model "" \
			-node_charge_model "" -edge_charge_model "contactcharge_edge_bottom" \
			-node_current_model ""   -edge_current_model "" -circuit_node cnode1

node_model -device MyDevice2 -region $region -name "topnode_model"           -equation "Potential - cnode1;"
node_model -device MyDevice2 -region $region -name "topnode_model:Potential" -equation "1;"
node_model -device MyDevice2 -region $region -name "topnode_model:cnode1" -equation "-1;"
edge_model -device MyDevice2 -region $region -name "contactcharge_edge_top"  -equation $conteq
#
node_model -device MyDevice2 -region $region -name "bottomnode_model"           -equation "Potential;"
node_model -device MyDevice2 -region $region -name "bottomnode_model:Potential" -equation "1;"
edge_model -device MyDevice2 -region $region -name "contactcharge_edge_bottom"  -equation $conteq

contact_equation -device MyDevice2 -contact "top" -name "PotentialEquation" -variable_name Potential \
			-node_model topnode_model -edge_model "" \
			-node_charge_model "" -edge_charge_model "contactcharge_edge_top" \
			-node_current_model ""   -edge_current_model "" -circuit_node cnode1

contact_equation -device MyDevice2 -contact "bot" -name "PotentialEquation" -variable_name Potential \
			-node_model bottomnode_model -edge_model "" \
			-node_charge_model "" -edge_charge_model "contactcharge_edge_bottom" \
			-node_current_model ""   -edge_current_model ""


####
#### Initial DC Solution
####
#catch {solve -type dc} foo
#puts $foo
solve -type dc -absolute_error 1.0 -relative_error 1e-14 -maximum_iterations 30

foreach device $devices {
    puts $device;
print_node_values -device $device -region $region -name Potential
print_node_values -device $device -region $region -name IntrinsicElectrons
}


foreach device $devices {
    puts $device;
####
#### Electrons
####
node_solution        -device $device -region $region -name Electrons
edge_from_node_model -device $device -region $region -node_model Electrons
set_node_values -device $device -region $region -name Electrons -init_from IntrinsicElectrons


####
#### PotentialNodeCharge
####
node_model -device $device -region $region -name "PotentialNodeCharge"           -equation "-ElectronCharge*(-Electrons + NetDoping);"
node_model -device $device -region $region -name "PotentialNodeCharge:Electrons" -equation "+ElectronCharge;"

####
#### PotentialEquation modified for carriers present
####
equation -device $device -region $region -name PotentialEquation -variable_name Potential -node_model "PotentialNodeCharge" \
    -edge_model "PotentialEdgeFlux" -time_node_model "" -variable_update default 


####
#### vdiff, Bern01, Bern10
####
edge_model -device $device -region $region -name "vdiff"              -equation "(Potential@n0 - Potential@n1)/ThermalVoltage;"
edge_model -device $device -region $region -name "vdiff:Potential@n0"  -equation "ThermalVoltage^(-1);"
edge_model -device $device -region $region -name "vdiff:Potential@n1"  -equation "-ThermalVoltage^(-1);"
edge_model -device $device -region $region -name "Bern01"             -equation "B(vdiff);"
edge_model -device $device -region $region -name "Bern01:Potential@n0" -equation "diff(B(vdiff), Potential@n0);"
edge_model -device $device -region $region -name "Bern01:Potential@n1" -equation "diff(B(vdiff), Potential@n1);"
#edge_model -device $device -region $region -name "Bern01:Potential@n0" -equation "dBdx(vdiff)*vdiff:Potential@n0;"
#edge_model -device $device -region $region -name "Bern01:Potential@n1" -equation "dBdx(vdiff)*vdiff:Potential@n1;"
edge_model -device $device -region $region -name "Bern10"             -equation "B(-vdiff);"
edge_model -device $device -region $region -name "Bern10:Potential@n0" -equation "diff(B(-vdiff), Potential@n0);"
edge_model -device $device -region $region -name "Bern10:Potential@n1" -equation "diff(B(-vdiff), Potential@n1);"
#edge_model -device $device -region $region -name "Bern10:Potential@n0" -equation "-dBdx(-vdiff)*vdiff:Potential@n0;"
#edge_model -device $device -region $region -name "Bern10:Potential@n1" -equation "-dBdx(-vdiff)*vdiff:Potential@n1;"

####
#### Electron Current
####
set Jn       "ElectronCharge*mu_n*EdgeInverseLength*ThermalVoltage*(Electrons@n1*Bern10 - Electrons@n0*Bern01)";
set dJndn0   "simplify(diff( $Jn, Electrons@n0));";
set dJndn1   "simplify(diff( $Jn, Electrons@n1));";
set dJndpot0 "simplify(diff( $Jn, Potential@n0));";
set dJndpot1 "simplify(diff( $Jn, Potential@n1));";
edge_model -device $device -region $region -name "ElectronCurrent"             -equation "$Jn;"
edge_model -device $device -region $region -name "ElectronCurrent:Electrons@n0" -equation $dJndn0
edge_model -device $device -region $region -name "ElectronCurrent:Electrons@n1" -equation $dJndn1
edge_model -device $device -region $region -name "ElectronCurrent:Potential@n0" -equation $dJndpot0
edge_model -device $device -region $region -name "ElectronCurrent:Potential@n1" -equation $dJndpot1

####
#### Electron Continuity Equation
####
equation -device $device -region $region -name ElectronContinuityEquation -variable_name Electrons \
	 -edge_model "ElectronCurrent" -variable_update "positive" 

####
#### Electron Continuity Contact Equation
####
node_model -device $device -region $region -name "celec" -equation "0.5*(NetDoping+(NetDoping^2 + 4 * IntrinsicDensity^2)^(0.5));"
node_model -device $device -region $region -name "topnodeelectrons"           -equation "Electrons - celec;"
node_model -device $device -region $region -name "topnodeelectrons:Electrons" -equation "1.0;"
edge_model -device $device -region $region -name "topnodeelectroncurrent"     -equation "ElectronCurrent;"
edge_model -device $device -region $region -name "topnodeelectroncurrent:Electrons@n0" -equation "ElectronCurrent:Electrons@n0;"
edge_model -device $device -region $region -name "topnodeelectroncurrent:Electrons@n1" -equation "ElectronCurrent:Electrons@n1;"
edge_model -device $device -region $region -name "topnodeelectroncurrent:Potential@n0" -equation "ElectronCurrent:Potential@n0;"
edge_model -device $device -region $region -name "topnodeelectroncurrent:Potential@n1" -equation "ElectronCurrent:Potential@n1;"
}

contact_equation -device MyDevice1 -contact "top" -name "ElectronContinuityEquation" -variable_name Electrons \
			-node_model "topnodeelectrons" \
			-edge_current_model "topnodeelectroncurrent"

contact_equation -device MyDevice1 -contact "bot" -name "ElectronContinuityEquation" -variable_name Electrons \
			-node_model "topnodeelectrons" \
			-edge_current_model "topnodeelectroncurrent" \
			-circuit_node cnode1

contact_equation -device MyDevice2 -contact "top" -name "ElectronContinuityEquation" -variable_name Electrons \
			-node_model "topnodeelectrons" \
			-edge_current_model "topnodeelectroncurrent" \
			-circuit_node cnode1
contact_equation -device MyDevice2 -contact "bot" -name "ElectronContinuityEquation" -variable_name Electrons \
			-node_model "topnodeelectrons" \
			-edge_current_model "topnodeelectroncurrent"





#foreach {v} {0.0 0.01 0.02 0.03 0.04 0.05 0.06 0.07 .2}  \
# small steps hurt convergence for some reason
#foreach {v} {0.0 0.01 0.02 0.03 0.04 0.05 0.06 0.07 .2}  \

foreach {v} {0.0 0.01 0.02 0.03 0.04 0.05 0.06 0.07 0.08 0.09 0.10} \
{
puts "topbias $v";
set_parameter  -name "topbias" -value $v
#set_parameter -device MyDevice2 -name "topbias" -value [expr $v*0.5]

solve -type dc -absolute_error 1 -relative_error 1e-9 -maximum_iterations 30
#catch {solve -type dc} foo
#puts $foo

foreach device $devices {
    puts $device;
printCurrents $device "top"
printCurrents $device "bot"
}
}


foreach device $devices {
puts "device $device $region\n";
#print_node_values -device $device -region $region -name Electrons
#print_node_values -device $device -region $region -name Potential
#print_edge_values -device $device -region $region -name ElectricField
#print_edge_values -device $device -region $region -name ElectronCurrent
print_edge_values -device $device -region $region -name topnodeelectroncurrent:Electrons@n0
print_edge_values -device $device -region $region -name ElectronCurrent:Electrons@n0
print_edge_values -device $device -region $region -name topnodeelectroncurrent:Electrons@n1
print_edge_values -device $device -region $region -name ElectronCurrent:Electrons@n1
}



set l 1
set q 1.6e-19
set n 1.0e16
set u 400
set R [expr $l/($q*$n*$u)]
puts [expr 0.1/$R]
##
