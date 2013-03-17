package provide dsBase 1.0
proc createSolution {device region name} {
    node_solution -name $name -device $device -region $region
    edge_from_node_model -node_model $name  -device $device -region $region
}

proc createNodeModel {device region model_name expression} {
set result [node_model -device $device -region $region -name $model_name -equation "$expression;"]
puts "NODEMODEL $device $region $model_name \"$result\"";
}

proc createNodeModelDerivative {device region model_name expression variable_name} {
createNodeModel $device $region "${model_name}:${variable_name}" "simplify(diff($expression, ${variable_name}))"
}


proc createContactNodeModel {device contact model_name expression} {
set result [contact_node_model -device $device -contact $contact -name $model_name -equation "$expression;"]
puts "CONTACTNODEMODEL $device $contact $model_name \"$result\"";
}

proc createContactNodeModelDerivative {device contact model_name expression variable_name} {
createContactNodeModel $device $contact "${model_name}:${variable_name}" "simplify(diff($expression, ${variable_name}))"
}

proc createEdgeModel {device region model_name expression} {
set result [edge_model -device $device -region $region -name $model_name -equation "$expression;"]
puts "EDGEMODEL $device $region $model_name \"$result\"";
}

proc createEdgeModelDerivatives {device region model_name expression variable_name} {
createEdgeModel $device $region "${model_name}:${variable_name}@n0" "simplify(diff($expression, ${variable_name}@n0))"
createEdgeModel $device $region "${model_name}:${variable_name}@n1" "simplify(diff($expression, ${variable_name}@n1))"
}

proc createContactEdgeModel {device contact model_name expression} {
set result [contact_edge_model -device $device -contact $contact -name $model_name -equation "$expression;"]
puts "CONTACTEDGEMODEL $device $contact $model_name \"$result\"";
}

proc createContactEdgeModelDerivative {device contact model_name expression variable_name} {
createContactEdgeModel $device $contact "${model_name}:${variable_name}" "simplify(diff($expression, ${variable_name}))"
}

proc inEdgeModelList {device region edgemodel} {
  return [expr {-1 != [lsearch -exact [get_edge_model_list -device $device -region $region] $edgemodel]}]
}

proc inNodeModelList {device region nodemodel} {
  return [expr {-1 != [lsearch -exact [get_node_model_list -device $device -region $region] $nodemodel]}]
}

#### Make sure that the model exists, as well as it's node model
proc ensureEdgeFromNodeModelExists {device region nodemodel} {
  if {![inNodeModelList $device $region $nodemodel]} {
    error "${nodemodel} must exist"
  }

  set emlist [get_edge_model_list -device $device -region $region]
  if {![inEdgeModelList $device $region "${nodemodel}@n0"]} {
    puts "INFO: Creating ${nodemodel}@n0 and ${nodemodel}@n1"
    edge_from_node_model -device $device -region $region -node_model $nodemodel
  }
}

