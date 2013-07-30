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

#####
# dio1
#
# Make doping a step function
# print dat to text file for viewing in grace
# verify currents analytically
# in dio2 add recombination
#
## basically just a resistor
proc printCurrents {device cname bias} {
set ecurr [get_contact_current -contact $cname -equation ECE -device $device]
set hcurr [get_contact_current -contact $cname -equation HCE -device $device]
set tcurr [expr {$ecurr+$hcurr}]                                        
puts [format "%s\t%s\t%s\t%s\t%s" $cname $bias $ecurr $hcurr $tcurr]
}

set device MyDevice
set region MyRegion

####
#### Meshing
####

create_1d_mesh -mesh dog
add_1d_mesh_line -mesh dog -pos 0 -ps 1e-6 -tag top
add_1d_mesh_line -mesh dog -pos 0.5e-5 -ps 1e-7 -tag mid
add_1d_mesh_line -mesh dog -pos 1e-5 -ps 1e-6 -tag bot
add_1d_contact   -mesh dog -name top -tag top -material metal
add_1d_contact   -mesh dog -name bot -tag bot -material metal
add_1d_region    -mesh dog -material Si -region $region -tag1 top -tag2 bot
finalize_mesh -mesh dog
create_device -mesh dog -device $device

####
#### Constants
####

set_parameter -device $device -region $region -name "ϵ"     -value [expr 11.1*8.85e-14]
set_parameter -device $device -region $region -name "q"   -value 1.6e-19
set_parameter -device $device -region $region -name "n_i" -value 1.0e10
set_parameter -device $device -region $region -name "V_T"   -value 0.0259

set_parameter -device $device -region $region -name "μ_n" -value 400
set_parameter -device $device -region $region -name "μ_p" -value 200

####
#### ψ
####

node_solution -device $device -region $region -name ψ
edge_from_node_model -device $device -region $region -node_model ψ


####
#### N⁺
####

node_model -device $device -region $region -name N_A -equation "1.0e17*step(0.5e-5-x);"
node_model -device $device -region $region -name N_D -equation "1.0e18*step(x-0.5e-5);"
node_model -device $device -region $region -name N⁺ -equation "N_D-N_A;"
print_node_values -device $device -region $region -name N⁺


# Net doping is set in executable until we use analytic step function
node_model -device $device -region $region -name "n_0"    -equation "n_i*exp(ψ/V_T);"
node_model -device $device -region $region -name "n_0:ψ"  -equation  "diff(n_i*exp(ψ/V_T), ψ);"
node_model -device $device -region $region -name "p_0"    -equation                "n_i^2/n_0;"
node_model -device $device -region $region -name "p_0:ψ"  -equation      "diff(n_i^2/n_0, ψ);"
node_model -device $device -region $region -name "IntrinsicCharge"                -equation               "p_0-n_0 + N⁺;"
node_model -device $device -region $region -name "IntrinsicCharge:ψ"              -equation     "diff(p_0-n_0, ψ);"
node_model -device $device -region $region -name "PotentialIntrinsicNodeCharge"   -equation           "-q*IntrinsicCharge;"
node_model -device $device -region $region -name "PotentialIntrinsicNodeCharge:ψ" -equation "diff(-q*IntrinsicCharge, ψ);"

edge_model -device $device -region $region -name "EField"          -equation "(ψ@n0-ψ@n1)*EdgeInverseLength;"
edge_model -device $device -region $region -name "EField:ψ@n0"     -equation "EdgeInverseLength;"
edge_model -device $device -region $region -name "EField:ψ@n1"     -equation "-EField:ψ@n0;"
edge_model -device $device -region $region -name "PotentialEdgeFlux"      -equation "ϵ*EField;"
edge_model -device $device -region $region -name "PotentialEdgeFlux:ψ@n0" -equation "diff(ϵ*EField,ψ@n0);"
edge_model -device $device -region $region -name "PotentialEdgeFlux:ψ@n1" -equation "-PotentialEdgeFlux:ψ@n0;"

equation -device $device -region $region -name PE -variable_name ψ \
	 -node_model "PotentialIntrinsicNodeCharge" -edge_model "PotentialEdgeFlux" -variable_update log_damp 

set_parameter -device $device -region $region -name "V_top" -value 0.0
#added safe value so log doesn't blow up
#node_model -device $device -region $region -name "celec_top" -equation "0.5*(N⁺+(N⁺^2 + 4 * n_i^2)^(0.5))+1e-10;"
#node_model -device $device -region $region -name "chole_top" -equation "0.5*(-N⁺+(N⁺^2 + 4 * n_i^2)^(0.5))+1e-10;"
contact_node_model -device $device -contact top -name "celec_top" -equation "0.5*(N⁺+(N⁺^2 + 4 * n_i^2)^(0.5))+1e-10;"
contact_node_model -device $device -contact top -name "chole_top" -equation "0.5*(-N⁺+(N⁺^2 + 4 * n_i^2)^(0.5))+1e-10;"
#node_model -device $device -region $region -name "topnodemodel" -equation "ψ-V_top-V_T*log(celec_top/n_i);"
#node_model -device $device -region $region -name "topnodemodel:ψ" -equation "1;"
contact_node_model -device $device -contact top -name "topnodemodel" -equation "ψ-V_top-V_T*log(celec_top/n_i);"
contact_node_model -device $device -contact top -name "topnodemodel:ψ" -equation "1;"
#print_node_values -device $device -region $region -name topnodemodel
#print_node_values -device $device -region $region -name topnodemodel:ψ
#contact_node_model -device $device -contact bot -name "celec_bot" -equation "0.5*(N⁺+(N⁺^2 + 4 * n_i^2)^(0.5))+1e-10;"
#contact_node_model -device $device -contact bot -name "chole_bot" -equation "0.5*(-N⁺+(N⁺^2 + 4 * n_i^2)^(0.5))+1e-10;"
#contact_node_model -device $device -contact bot -name "botnodemodel"           -equation "ψ +V_T*log(chole_bot/n_i);"
#contact_node_model -device $device -contact bot -name "botnodemodel:ψ" -equation "1;"
contact_node_model -device $device -contact bot -name "celec_bot" -equation "0.5*(N⁺+(N⁺^2 + 4 * n_i^2)^(0.5))+1e-10;"
contact_node_model -device $device -contact bot  -name "chole_bot" -equation "0.5*(-N⁺+(N⁺^2 + 4 * n_i^2)^(0.5))+1e-10;"
contact_node_model -device $device -contact bot  -name "botnodemodel"           -equation "ψ +V_T*log(chole_bot/n_i);"
contact_node_model -device $device -contact bot  -name "botnodemodel:ψ" -equation "1;"
# Still need to debug the proper sign
# The Intrinsic Charge may be constrained to zero from celec and chole above
node_model -device $device -region $region -name "contactcharge_node" -equation "q*IntrinsicCharge;"
edge_model -device $device -region $region -name "contactcharge_edge" -equation "q*EField;"


contact_equation -device $device -contact "top" -name "PE" -variable_name ψ \
			-node_model "topnodemodel" -edge_model "" \
			-node_charge_model "contactcharge_node" -edge_charge_model "contactcharge_edge" \
			-node_current_model ""   -edge_current_model ""

contact_equation -device $device -contact "bot" -name "PE" -variable_name ψ \
			-node_model "botnodemodel" -edge_model "" \
			-node_charge_model "contactcharge_node" -edge_charge_model "contactcharge_edge" \
			-node_current_model ""   -edge_current_model ""

#### tod debug the log of zero issue
#print_node_values -device $device -region $region -name ψ
#print_node_values -device $device -region $region -name chole
#print_node_values -device $device -region $region -name topnodemodel
#print_node_values -device $device -region $region -name botnodemodel
solve -type dc -absolute_error 1.0 -relative_error 1e-14 -maximum_iterations 30

#need to initialize these models somehow
node_solution        -device $device -region $region -name n
edge_from_node_model -device $device -region $region -node_model n
node_solution        -device $device -region $region -name p
edge_from_node_model -device $device -region $region -node_model p

set_node_values -device $device -region $region -name n -init_from n_0
set_node_values -device $device -region $region -name p     -init_from p_0

node_model -device $device -region $region -name "PotentialNodeCharge"           -equation "-q*(p -n + N⁺);"
node_model -device $device -region $region -name "PotentialNodeCharge:n" -equation "+q;"
node_model -device $device -region $region -name "PotentialNodeCharge:p"     -equation "-q;"

equation -device $device -region $region -name PE -variable_name ψ -node_model "PotentialNodeCharge" \
    -edge_model "PotentialEdgeFlux" -time_node_model "" -variable_update log_damp 

edge_model -device $device -region $region -name "vdiff"       -equation "(ψ@n0 - ψ@n1)/V_T;"
edge_model -device $device -region $region -name "vdiff:ψ@n0"  -equation "V_T^(-1);"
edge_model -device $device -region $region -name "vdiff:ψ@n1"  -equation "-V_T^(-1);"
edge_model -device $device -region $region -name "Bern01"      -equation "B(vdiff);"
edge_model -device $device -region $region -name "Bern01:ψ@n0" -equation "dBdx(vdiff)*vdiff:ψ@n0;"
edge_model -device $device -region $region -name "Bern01:ψ@n1" -equation "dBdx(vdiff)*vdiff:ψ@n1;"
edge_model -device $device -region $region -name "Bern10"      -equation "B(-vdiff);"
edge_model -device $device -region $region -name "Bern10:ψ@n0" -equation "-dBdx(-vdiff)*vdiff:ψ@n0;"
edge_model -device $device -region $region -name "Bern10:ψ@n1" -equation "-dBdx(-vdiff)*vdiff:ψ@n1;"

set_parameter -device $device -region $region -name "μ_n" -value 400
set_parameter -device $device -region $region -name "μ_p" -value 200

set Jn       "q*μ_n*EdgeInverseLength*V_T*(n@n1*Bern10 - n@n0*Bern01)";
set dJndn0   "simplify(diff( $Jn, n@n0));";
set dJndn1   "simplify(diff( $Jn, n@n1));";
set dJndpot0 "simplify(diff( $Jn, ψ@n0));";
set dJndpot1 "simplify(diff( $Jn, ψ@n1));";
edge_model -device $device -region $region -name "J_n"             -equation "$Jn;"
edge_model -device $device -region $region -name "J_n:n@n0" -equation $dJndn0
edge_model -device $device -region $region -name "J_n:n@n1" -equation $dJndn1
edge_model -device $device -region $region -name "J_n:ψ@n0" -equation $dJndpot0
edge_model -device $device -region $region -name "J_n:ψ@n1" -equation $dJndpot1

set Jp       "-q*μ_p*EdgeInverseLength*V_T*(p@n1*Bern01 - p@n0*Bern10)";
set dJpdp0   "simplify(diff($Jp, p@n0));"
set dJpdp1   "simplify(diff($Jp, p@n1));"
set dJpdpot0 "simplify(diff($Jp, ψ@n0));"
set dJpdpot1 "simplify(diff($Jp, ψ@n1));"

edge_model -device $device -region $region -name "J_p"             -equation "$Jp;"
edge_model -device $device -region $region -name "J_p:p@n0"     -equation $dJpdp0
edge_model -device $device -region $region -name "J_p:p@n1"     -equation $dJpdp1
edge_model -device $device -region $region -name "J_p:ψ@n0" -equation $dJpdpot0
edge_model -device $device -region $region -name "J_p:ψ@n1" -equation $dJpdpot1

equation -device $device -region $region -name ECE -variable_name n \
	 -edge_model "J_n" -variable_update "positive" 

equation -device $device -region $region -name HCE -variable_name p \
	 -edge_model "J_p" -variable_update "positive" 


contact_node_model -device $device -contact top -name "topnodeelectrons"           -equation "n - celec_top;"
contact_node_model -device $device -contact top -name "topnodeholes"               -equation "p - chole_top;"
contact_node_model -device $device -contact top -name "topnodeelectrons:n" -equation "1.0;"
contact_node_model -device $device -contact top -name "topnodeholes:p"         -equation "1.0;"

contact_node_model -device $device -contact bot -name "botnodeelectrons"           -equation "n - celec_bot;"
contact_node_model -device $device -contact bot -name "botnodeholes"               -equation "p - chole_bot;"
contact_node_model -device $device -contact bot -name "botnodeelectrons:n" -equation "1.0;"
contact_node_model -device $device -contact bot -name "botnodeholes:p"         -equation "1.0;"

edge_model -device $device -region $region -name "topnodeelectroncurrent"     -equation "q*J_n;"
contact_edge_model -device $device -contact top -name "topnodeholecurrent"         -equation "q*J_p;"
contact_edge_model -device $device -contact bot -name "botnodeholecurrent"         -equation "q*J_p;"

contact_equation -device $device -contact "top" -name "ECE" -variable_name n \
			-node_model "topnodeelectrons" \
			-node_charge_model "contactcharge_node" \
			-edge_current_model "topnodeelectroncurrent"

contact_equation -device $device -contact "bot" -name "ECE" -variable_name n \
			-node_model "botnodeelectrons" \
			-node_charge_model "contactcharge_node" \
			-edge_current_model "topnodeelectroncurrent"

contact_equation -device $device -contact "top" -name "HCE" -variable_name p \
			-node_model "topnodeholes" \
			-node_charge_model "contactcharge_node" \
			-edge_current_model "topnodeholecurrent"

contact_equation -device $device -contact "bot" -name "HCE" -variable_name p \
			-node_model "botnodeholes" \
			-node_charge_model "contactcharge_node" \
			-edge_current_model "botnodeholecurrent"

set_parameter -device $device -region $region -name "V_top" -value 0
print_node_values -device $device -region $region -name ψ
print_node_values -device $device -region $region -name n
print_node_values -device $device -region $region -name p

for {set v 0.1} {$v < 0.91} {set v [expr $v + 0.1]} {
set_parameter -device $device -region $region -name "V_top" -value $v
solve -type dc -absolute_error 1e10 -relative_error 1e-12 -maximum_iterations 30

printCurrents $device "top" $v
printCurrents $device "bot" 0.0
}

print_node_values -device $device -region $region -name n
print_node_values -device $device -region $region -name p
print_node_values -device $device -region $region -name AtContactNode
print_node_values -device $device -region $region -name botnodemodel
print_node_values -device $device -region $region -name botnodemodel:ψ
print_node_values -device $device -region $region -name topnodemodel
print_node_values -device $device -region $region -name topnodemodel:ψ
#print_node_values -device $device -region $region -name chole_top
#print_node_values -device $device -region $region -name chole_bot
#print_node_values -device $device -region $region -name botnodeholes
#print_edge_values -device $device -region $region -name topnodeholecurrent
#print_edge_values -device $device -region $region -name botnodeholecurrent
