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

## basically just a resistor
## test fluxterm interface model

proc printCurrents {device cname} {
set ecurr [get_contact_current -contact $cname -equation ElectronContinuityEquation -device $device]
#set hcurr [get_contact_current -contact $cname -equation HoleContinuityEquation -device $device]
set hcurr 0
set tcurr [expr {$ecurr+$hcurr}]                                        
puts [format "Contact: %s\nElectron %s\nHole %s\nTotal %s" $cname $ecurr $hcurr $tcurr]
}

set device MyDevice
set interface MyInt
set regions {MySi1 MySi2}

####
#### Meshing
####
create_1d_mesh -mesh dog
add_1d_mesh_line -mesh dog -pos 0 -ps 0.1 -tag top
add_1d_mesh_line -mesh dog -pos 0.5 -ps 0.1 -tag mid
add_1d_mesh_line -mesh dog -pos 1 -ps 0.1 -tag bot
add_1d_contact   -mesh dog -name top -tag top -material "metal"
add_1d_contact   -mesh dog -name bot -tag bot -material "metal"
add_1d_interface -mesh dog -name MyInt -tag mid
add_1d_region    -mesh dog -material Si -region MySi1 -tag1 top -tag2 mid
add_1d_region    -mesh dog -material Ox -region MySi2 -tag1 mid -tag2 bot
finalize_mesh -mesh dog
create_device -mesh dog -device $device


####
#### Constants
####
foreach {region} $regions {
set_parameter -device $device -region $region -name "Permittivity"     -value [expr 11.1*8.85e-14]
set_parameter -device $device -region $region -name "ElectronCharge"   -value 1.6e-19
set_parameter -device $device -region $region -name "IntrinsicDensity" -value 1.0e10
set_parameter -device $device -region $region -name "ThermalVoltage"   -value 0.0259

set_parameter -device $device -region $region -name "mu_n" -value 400
set_parameter -device $device -region $region -name "mu_p" -value 200

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
node_model -device $device -region $region -name "IntrinsicElectrons:Potential" -equation  "diff(IntrinsicDensity*exp(Potential/ThermalVoltage), Potential);"

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

set_parameter -device $device -region MySi1 -name topbias -value 0.0
set_parameter -device $device -region MySi2 -name bottombias -value 0.0


####
#### Potential contact equations
####

set conteq "Permittivity*ElectricField;"

node_model -device $device -region MySi1 -name "topnode_model"           -equation "Potential - topbias;"
node_model -device $device -region MySi1 -name "topnode_model:Potential" -equation "1;"
edge_model -device $device -region MySi1 -name "contactcharge_edge_top"  -equation $conteq
#
node_model -device $device -region MySi2 -name "bottomnode_model"           -equation "Potential - bottombias;"
node_model -device $device -region MySi2 -name "bottomnode_model:Potential" -equation "1;"
edge_model -device $device -region MySi2 -name "contactcharge_edge_bottom"  -equation $conteq

contact_equation -device $device -contact "top" -name "PotentialEquation" -variable_name Potential \
			-node_model topnode_model -edge_model "" \
			-node_charge_model "" -edge_charge_model "contactcharge_edge_top" \
			-node_current_model ""   -edge_current_model ""

contact_equation -device $device -contact "bot" -name "PotentialEquation" -variable_name Potential \
			-node_model bottomnode_model -edge_model "" \
			-node_charge_model "" -edge_charge_model "contactcharge_edge_bottom" \
			-node_current_model ""   -edge_current_model ""

# type continuous means that regular equations in both regions are swapped into the primary region
set_parameter -device $device -name "one" -value 1.0
#set_parameter -device $device -region MySi1 -name "one" -value 0.0
interface_model -device $device -interface "MyInt" -name continuousPotential -equation "one*(Potential@r0-Potential@r1);"
interface_model -device $device -interface "MyInt" -name continuousPotential:Potential@r0 -equation  "one;"
interface_model -device $device -interface "MyInt" -name continuousPotential:Potential@r1 -equation "-one;"
interface_equation   -device $device -interface "MyInt" -name "PotentialEquation" -variable_name "Potential" -interface_model "continuousPotential" -type "continuous"
####

#### Initial DC Solution
####
#catch {solve -type dc} foo
#puts $foo
solve -type dc -absolute_error 1.0 -relative_error 1e-10 -maximum_iterations 30

print_node_values -device $device -region MySi1 -name Potential
print_node_values -device $device -region MySi1 -name IntrinsicElectrons

print_node_values -device $device -region MySi2 -name Potential
print_node_values -device $device -region MySi2 -name IntrinsicElectrons

foreach {region} $regions {
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
edge_model -device $device -region $region -name "Bern01:Potential@n0" -equation "dBdx(vdiff)*vdiff:Potential@n0;"
edge_model -device $device -region $region -name "Bern01:Potential@n1" -equation "dBdx(vdiff)*vdiff:Potential@n1;"
edge_model -device $device -region $region -name "Bern10"             -equation "B(-vdiff);"
edge_model -device $device -region $region -name "Bern10:Potential@n0" -equation "-dBdx(-vdiff)*vdiff:Potential@n0;"
edge_model -device $device -region $region -name "Bern10:Potential@n1" -equation "-dBdx(-vdiff)*vdiff:Potential@n1;"

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
edge_model -device $device -region $region -name "topnodeelectroncurrent"     -equation "(ElectronCurrent);"
}

contact_equation -device $device -contact "top" -name "ElectronContinuityEquation" -variable_name Electrons \
			-node_model "topnodeelectrons" \
			-edge_current_model "topnodeelectroncurrent"

contact_equation -device $device -contact "bot" -name "ElectronContinuityEquation" -variable_name Electrons \
			-node_model "topnodeelectrons" \
			-edge_current_model "topnodeelectroncurrent"

#set alpha_n 1e-7
set_parameter -device $device -name "alpha_n" -value 1e-7
### this is for testing the derivatives of node models consisting of node models
node_model -device $device -region MySi2 -name electrons -equation "Electrons;"
node_model -device $device -region MySi2 -name electrons:Electrons -equation "1;"
set iexp "(alpha_n@r0)*(Electrons@r0-electrons@r1)"
#set iexp "($alpha_n)*exp(Electrons@r0-Electrons@r1)"
interface_model -device $device -interface "MyInt" -name srvElectrons -equation "$iexp;"
interface_model -device $device -interface "MyInt" -name srvElectrons2 -equation "srvElectrons;"
#### TODO: the differentiator can't handle this
#interface_model -device $device -interface "MyInt" -name srvElectrons:Electrons@r0 -equation "$alpha_n;"
#interface_model -device $device -interface "MyInt" -name srvElectrons:Electrons@r1 -equation "-$alpha_n;"
interface_model -device $device -interface "MyInt" -name srvElectrons:Electrons@r0 -equation "diff($iexp,Electrons@r0);"
interface_model -device $device -interface "MyInt" -name srvElectrons:Electrons@r1 -equation "diff($iexp,Electrons@r1);"
interface_model -device $device -interface "MyInt" -name srvElectrons2:Electrons@r0 -equation "srvElectrons:Electrons@r0;"
interface_model -device $device -interface "MyInt" -name srvElectrons2:Electrons@r1 -equation "srvElectrons:Electrons@r1;"
interface_equation   -device $device -interface "MyInt" -name "ElectronContinuityEquation" -variable_name "Electrons" -interface_model "srvElectrons2" -type fluxterm


#foreach {v} {0.0 0.01} {
foreach {v} {0.0 0.01 0.02 0.03 0.04 0.05 0.06 0.07 0.08 0.09 0.10} {
set_parameter -device $device -region MySi1 -name "topbias" -value $v
solve -type dc -absolute_error 1.0 -relative_error 1e-10 -maximum_iterations 10
#catch {solve -type dc} foo
#puts $foo

printCurrents $device "top"
printCurrents $device "bot"
}


print_node_values -device $device -region MySi1 -name Electrons
print_node_values -device $device -region MySi1 -name Potential
print_edge_values -device $device -region MySi1 -name ElectricField
print_edge_values -device $device -region MySi1 -name ElectronCurrent
print_edge_values -device $device -region MySi1 -name topnodeelectroncurrent
print_node_values -device $device -region MySi2 -name Electrons
print_node_values -device $device -region MySi2 -name Potential
print_edge_values -device $device -region MySi2 -name ElectricField
print_edge_values -device $device -region MySi2 -name ElectronCurrent
print_edge_values -device $device -region MySi2 -name topnodeelectroncurrent


printCurrents $device "top"
printCurrents $device "bot"

#set l 1
#set q 1.6e-19
#set n 1.0e16
#set u 400
#set R [expr $l/($q*$n*$u)]
#puts [expr 0.1/$R]
##

#foreach {interface} [get_interface_list -device $device] {
#  puts "interface $interface"
#
#  foreach {interfaceModel} [get_interface_model_list -device $device -interface $interface] {
#    puts "interface_model $interfaceModel"
#    foreach {values} [get_interface_model_values -device $device -interface $interface -name $interfaceModel] {
#      puts "values $values"
#    }
#  }
#}

#puts "interface models"
#puts [get_interface_model_list -device $device -interface $interface]
#delete_interface_model -device $device -interface $interface -name continuousPotential
#puts [get_interface_model_list -device $device -interface $interface]
#puts "node models"
#puts [get_node_model_list -device $device -region $region]
#delete_node_model -device $device -region $region -name x
#puts [get_node_model_list -device $device -region $region]
##get_node_model_values -device $device -region $region -name x
#puts "edge models"
#puts [get_edge_model_list -device $device -region $region]
#delete_edge_model -device $device -region $region -name Potential@n1
##delete_edge_model -device $device -region $region -name Potential@n0
#puts [get_edge_model_list -device $device -region $region]
#print_edge_values -device $device -region $region -name ElectricField
#write_devices -file res3.flps -type floops
