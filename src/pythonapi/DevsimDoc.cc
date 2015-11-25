
static const char add_circuit_node_doc[] =
"ds.add_circuit_node (name=STRING, value=FLOAT, variable_update=OPTION)\n"
"\n"
"Adds a circuit node for use in circuit or multi-device simulation\n"
"\n"
"name      = Name of the circuit node being created (required) \n"
"value     = initial value (optional) (0.0 default)\n"
"variable_update= update type for circuit variable (optional) (\"default\" default)\n"
"  default         Variable can be positive or negative\n"
"  log_damp        Variable update is damped\n"
"  positive        Solution update results in positive quantity\n"
;

static const char circuit_alter_doc[] =
"ds.circuit_alter (name=STRING, param=STRING, value=FLOAT)\n"
"\n"
"Alter the value of a circuit element parameter\n"
"\n"
"name      = Name of the circuit node being created (required) \n"
"param     = parameter being modified (optional) (\"value\" default)\n"
"value     = value for the parameter (required) \n"
;

static const char circuit_element_doc[] =
"ds.circuit_element (name=STRING, value=FLOAT, n1=STRING, n2=STRING, \n"
"    acreal=FLOAT, acimag=FLOAT)\n"
"\n"
"Adds a circuit element external to the devices\n"
"\n"
"name      = Name of the circuit element being created.  A prefix of 'V' is for voltage source, 'I' for current source, 'R' for resistor, 'L' for inductor, and 'C' for capacitor. (required) \n"
"value     = value for the default parameter of the circuit element (optional) (0.0 default)\n"
"n1        = circuit node (required) \n"
"n2        = circuit node (required) \n"
"acreal    = real part of AC source for voltage (optional) (0.0 default)\n"
"acimag    = imag part of AC source for voltage (optional) (0.0 default)\n"
;

static const char circuit_node_alias_doc[] =
"ds.circuit_node_alias (node=STRING, alias=STRING)\n"
"\n"
"Create an alias for a circuit node\n"
"\n"
"node      = circuit node being aliased (required) \n"
"alias     = alias for the circuit node (required) \n"
;

static const char get_circuit_equation_number_doc[] =
"ds.get_circuit_equation_number (node=STRING)\n"
"\n"
"Returns the row number correspond to circuit node in a region.  Values are only valid when during the course of a solve.\n"
"\n"
"node      = circuit node (required) \n"
;

static const char get_circuit_node_list_doc[] =
"ds.get_circuit_node_list ()\n"
"\n"
"Gets the list of the nodes in the circuit.\n"
"\n"
;

static const char get_circuit_node_value_doc[] =
"ds.get_circuit_node_value (solution=STRING, node=STRING)\n"
"\n"
"Gets the value of a circuit node for a given solution type.\n"
"\n"
"solution  = name of the solution. 'dcop' is the name for the DC solution (optional) (\"dcop\" default)\n"
"node      = circuit node of interest (required) \n"
;

static const char get_circuit_solution_list_doc[] =
"ds.get_circuit_solution_list ()\n"
"\n"
"Gets the list of available circuit solutions.\n"
"\n"
;

static const char set_circuit_node_value_doc[] =
"ds.set_circuit_node_value (solution=STRING, node=STRING, value=FLOAT)\n"
"\n"
"Sets the value of a circuit node for a given solution type.\n"
"\n"
"solution  = name of the solution. 'dcop' is the name for the DC solution (optional) \n"
"node      = circuit node of interest (required) \n"
"value     = new value (optional) (0.0 default)\n"
;

static const char contact_equation_doc[] =
"ds.contact_equation (device=STRING, contact=STRING, name=STRING, \n"
"    variable_name=STRING, circuit_node=STRING, \n"
"    edge_charge_model=STRING, edge_current_model=STRING, \n"
"    edge_model=STRING, element_charge_model=STRING, \n"
"    element_current_model=STRING, element_model=STRING, \n"
"    node_charge_model=STRING, node_current_model=STRING, \n"
"    node_model=STRING)\n"
"\n"
"Create a contact equation on a device\n"
"\n"
"device    = The selected device (required) \n"
"contact   = Contact on which to apply this command (required) \n"
"name      = Name of the contact equation being created (required) \n"
"variable_name= The variable name is used to determine the bulk equation we are replacing at this contact (optional) \n"
"circuit_node= Name of the circuit we integrate the flux into (optional) \n"
"edge_charge_model= Name of the edge model used to determine the charge at this contact (optional) \n"
"edge_current_model= Name of the edge model used to determine the current flowing out of this contact (optional) \n"
"edge_model= Name of the edge model being integrated at each edge at this contact (optional) \n"
"element_charge_model= Name of the element edge model used to determine the charge at this contact (optional) \n"
"element_current_model= Name of the element edge model used to determine the current flowing out of this contact (optional) \n"
"element_model= Name of the element edge model being integrated at each edge at this contact (optional) \n"
"node_charge_model= Name of the node model used to determine the charge at this contact (optional) \n"
"node_current_model= Name of the node model used to determine the current flowing out of this contact (optional) \n"
"node_model= Name of the node_model being integrated at each node at this contact (optional) \n"
;

static const char custom_equation_doc[] =
"ds.custom_equation (name=STRING, procedure=STRING)\n"
"\n"
"Custom equation assembly.  See sectiononpageref{models:customequation} for a description of how the function should be structured.\n"
"\n"
"name      = Name of the custom equation being created (required) \n"
"procedure = The procedure to be called (required) \n"
;

static const char delete_contact_equation_doc[] =
"ds.delete_contact_equation (device=STRING, contact=STRING, name=STRING)\n"
"\n"
"This command deletes an equation from a contact.\n"
"\n"
"device    = The selected device (required) \n"
"contact   = Contact on which to apply this command (required) \n"
"name      = Name of the contact equation being deleted (required) \n"
;

static const char delete_equation_doc[] =
"ds.delete_equation (device=STRING, region=STRING, name=STRING)\n"
"\n"
"This command deletes an equation from a region.\n"
"\n"
"device    = The selected device (required) \n"
"region    = The selected region (required) \n"
"name      = Name of the equation being deleted (required) \n"
;

static const char delete_interface_equation_doc[] =
"ds.delete_interface_equation (device=STRING, interface=STRING, \n"
"    name=STRING)\n"
"\n"
"This command deletes an equation from an interface.\n"
"\n"
"device    = The selected device (required) \n"
"interface = Interface on which to apply this command (required) \n"
"name      = Name of the interface equation being deleted (required) \n"
;

static const char equation_doc[] =
"ds.equation (device=STRING, region=STRING, name=STRING, \n"
"    variable_name=STRING, node_model=STRING, edge_model=STRING, \n"
"    edge_volume_model=STRING, time_node_model=STRING, \n"
"    element_model=STRING, volume_model=STRING, variable_update=STRING)\n"
"\n"
"Specify an equation to solve on a device\n"
"\n"
"device    = The selected device (required) \n"
"region    = The selected region (required) \n"
"name      = Name of the equation being created (required) \n"
"variable_name= Name of the node_solution being solved (required) \n"
"node_model= Name of the node_model being integrated at each node in the device volume (optional) \n"
"edge_model= Name of the edge model being integrated over each edge in the device volume (optional) \n"
"edge_volume_model= Name of the edge model being integrated over the volume of each edge in the device volume (optional) \n"
"time_node_model= Name of the time dependent node_model being integrated at each node in the device volume (optional) \n"
"element_model= Name of the element_model being integrated over each edge in the device volume (optional) \n"
"volume_model= Name of the element_model being integrated over the volume of each edge in the device volume (optional) \n"
"variable_update= update type for circuit variable (optional) (\"default\" default)\n"
"  default         Variable can be positive or negative\n"
"  log_damp        Variable update is damped\n"
"  positive        Solution update results in positive quantity\n"
;

static const char get_contact_equation_command_doc[] =
"ds.get_contact_equation_command (device=STRING, contact=STRING, \n"
"    name=STRING)\n"
"\n"
"This command gets the options used when creating this contact equation.\n"
"\n"
"device    = The selected device (required) \n"
"contact   = Contact on which to apply this command (required) \n"
"name      = Name of the contact equation being command options returned (required) \n"
;

static const char get_contact_equation_list_doc[] =
"ds.get_contact_equation_list (device=STRING, contact=STRING)\n"
"\n"
"This command gets a list of equations on the specified contact.\n"
"\n"
"device    = The selected device (required) \n"
"contact   = Contact on which to apply this command (required) \n"
;

static const char get_equation_command_doc[] =
"ds.get_equation_command (device=STRING, region=STRING, name=STRING)\n"
"\n"
"This command gets the options used when creating this equation.\n"
"\n"
"device    = The selected device (required) \n"
"region    = The selected region (required) \n"
"name      = Name of the equation being command options returned (required) \n"
;

static const char get_equation_list_doc[] =
"ds.get_equation_list (device=STRING, region=STRING)\n"
"\n"
"This command gets a list of equations on the specified region.\n"
"\n"
"device    = The selected device (required) \n"
"region    = The selected region (required) \n"
;

static const char get_equation_numbers_doc[] =
"ds.get_equation_numbers (device=STRING, region=STRING, equation=STRING, \n"
"    variable=STRING)\n"
"\n"
"Returns a list of the equation numbers corresponding to each node in a region.  Values are only valid when during the course of a solve.\n"
"\n"
"device    = The selected device (required) \n"
"region    = The selected region (required) \n"
"equation  = Name of the equation (optional) \n"
"variable  = Name of the variable (optional) \n"
;

static const char get_interface_equation_command_doc[] =
"ds.get_interface_equation_command (device=STRING, interface=STRING, \n"
"    name=STRING)\n"
"\n"
"This command gets the options used when creating this interface equation.\n"
"\n"
"device    = The selected device (required) \n"
"interface = Interface on which to apply this command (required) \n"
"name      = Name of the interface equation being command options returned (required) \n"
;

static const char get_interface_equation_list_doc[] =
"ds.get_interface_equation_list (device=STRING, interface=STRING)\n"
"\n"
"This command gets a list of equations on the specified interface.\n"
"\n"
"device    = The selected device (required) \n"
"interface = Interface on which to apply this command (required) \n"
;

static const char interface_equation_doc[] =
"ds.interface_equation (device=STRING, interface=STRING, name=STRING, \n"
"    variable_name=STRING, interface_model=STRING, type=STRING)\n"
"\n"
"Command to specify an equation at an interface\n"
"\n"
"device    = The selected device (required) \n"
"interface = Interface on which to apply this command (required) \n"
"name      = Name of the interface equation being created (required) \n"
"variable_name= The variable name is used to determine the bulk equation we are coupling this interface to (required) \n"
"interface_model= When specified, the bulk equations on both sides of the interface are integrated together.  This model is then used to specify how nodal quantities on both sides of the interface are balanced (required) \n"
"type      = Specifies the type of boundary condition (required) \n"
"  continuous      Equations of the same name in the two regions are added.  The texttt{interface_model} is an additional equation is created to specify how quantities across the interface are solved\n"
"  fluxterm        The texttt{interface_model} is added to the bulk equation in the first region, and subtracted from the second\n"
;

static const char get_contact_list_doc[] =
"ds.get_contact_list (device=STRING)\n"
"\n"
"Gets a list of contacts on a device.\n"
"\n"
"device    = The selected device (required) \n"
;

static const char get_device_list_doc[] =
"ds.get_device_list ()\n"
"\n"
"Gets a list of devices on the simulation.\n"
"\n"
;

static const char get_interface_list_doc[] =
"ds.get_interface_list (device=STRING)\n"
"\n"
"Gets a list of interfaces on a device.\n"
"\n"
"device    = The selected device (required) \n"
;

static const char get_region_list_doc[] =
"ds.get_region_list (device=STRING, contact=STRING, interface=STRING)\n"
"\n"
"Gets a list of regions on a device, contact, or interface.\n"
"\n"
"device    = The selected device (required) \n"
"contact   = If specified, gets the name of the region belonging to this contact on the device (optional) \n"
"interface = If specified, gets the name of the regions belonging to this interface on the device (optional) \n"
;

static const char add_db_entry_doc[] =
"ds.add_db_entry (material=STRING, parameter=STRING, value=STRING, \n"
"    unit=STRING, description=STRING)\n"
"\n"
"Adds an entry to the database\n"
"\n"
"material  = Material name requested. modelName{global} refers to all regions whose material does not have the parameter name specified (required) \n"
"parameter = Parameter name (required) \n"
"value     = Value assigned for the parameter (required) \n"
"unit      = String describing the units for this parameter name (required) \n"
"description= Description of the parameter for this material type. (required) \n"
;

static const char close_db_doc[] =
"ds.close_db ()\n"
"\n"
"Closes the database so that its entries are no longer available\n"
"\n"
;

static const char create_db_doc[] =
"ds.create_db (filename=STRING)\n"
"\n"
"Create a database to store material properties\n"
"\n"
"filename  = filename to create for the db (required) \n"
;

static const char get_db_entry_doc[] =
"ds.get_db_entry (material=STRING, parameter=STRING)\n"
"\n"
"This command returns a list containing the value, unit, and description for the requested material db entry\n"
"\n"
"material  = Material name (required) \n"
"parameter = Parameter name (required) \n"
;

static const char get_dimension_doc[] =
"ds.get_dimension (device=STRING)\n"
"\n"
"Get the dimension of the device\n"
"\n"
"device    = The selected device (optional) \n"
;

static const char get_material_doc[] =
"ds.get_material (device=STRING, region=STRING)\n"
"\n"
"Returns the material for the specified region\n"
"\n"
"device    = The selected device (optional) \n"
"region    = The selected region (optional) \n"
;

static const char get_parameter_doc[] =
"ds.get_parameter (device=STRING, region=STRING, name=STRING)\n"
"\n"
"Get a parameter on a region, device, or globally.\n"
"\n"
"device    = The selected device (optional) \n"
"region    = The selected region (optional) \n"
"name      = Name of the parameter name being retrieved (required) \n"
;

static const char get_parameter_list_doc[] =
"ds.get_parameter_list (device=STRING, region=STRING)\n"
"\n"
"Get list of parameter names on region, device, or globally\n"
"\n"
"device    = The selected device (optional) \n"
"region    = The selected region (optional) \n"
;

static const char open_db_doc[] =
"ds.open_db (filename=STRING, permissions=STRING)\n"
"\n"
"Open a database storing material properties\n"
"\n"
"filename  = filename to create for the db (required) \n"
"permissions= permissions on the db (required) (\"readonly\" default)\n"
"  readwrite       Open file for reading and writing\n"
"  readonly        Open file for read only (default)\n"
;

static const char save_db_doc[] =
"ds.save_db ()\n"
"\n"
"Saves any new or modified db entries to the database file\n"
"\n"
;

static const char set_material_doc[] =
"ds.set_material (device=STRING, region=STRING, material=STRING)\n"
"\n"
"Sets the new material for a region\n"
"\n"
"device    = The selected device (optional) \n"
"region    = The selected region (optional) \n"
"material  = New material name (required) \n"
;

static const char set_parameter_doc[] =
"ds.set_parameter (device=STRING, region=STRING, name=STRING, value=ANY)\n"
"\n"
"Set a parameter on region, device, or globally\n"
"\n"
"device    = The selected device (optional) \n"
"region    = The selected region (optional) \n"
"name      = Name of the parameter name being retrieved (required) \n"
"value     = value to set for the parameter (required) \n"
;

static const char add_1d_contact_doc[] =
"ds.add_1d_contact (material=STRING, mesh=STRING, name=STRING, \n"
"    tag=STRING)\n"
"\n"
"Add a contact to a 1D mesh\n"
"\n"
"material  = material for the contact being created (required) \n"
"mesh      = Mesh to add the contact to (required) \n"
"name      = Name for the contact being created (required) \n"
"tag       = Text label for the position to add the contact (required) \n"
;

static const char add_1d_interface_doc[] =
"ds.add_1d_interface (mesh=STRING, tag=STRING, name=STRING)\n"
"\n"
"Add an interface to a 1D mesh\n"
"\n"
"mesh      = Mesh to add the interface to (required) \n"
"tag       = Text label for the position to add the interface (required) \n"
"name      = Name for the interface being created (required) \n"
;

static const char add_1d_mesh_line_doc[] =
"ds.add_1d_mesh_line (mesh=STRING, tag=STRING, pos=STRING, ns=FLOAT, \n"
"    ps=FLOAT)\n"
"\n"
"Add a mesh line to a 1D mesh\n"
"\n"
"mesh      = Mesh to add the line to (required) \n"
"tag       = Text label for the position (optional) \n"
"pos       = Position for the mesh point (required) \n"
"ns        = Spacing from this point in the negative direction (optional) (ps value default)\n"
"ps        = Spacing from this point in the positive direction (required) \n"
;

static const char add_1d_region_doc[] =
"ds.add_1d_region (mesh=STRING, tag1=STRING, tag2=STRING, region=STRING, \n"
"    material=STRING)\n"
"\n"
"Add a region to a 1D mesh\n"
"\n"
"mesh      = Mesh to add the line to (required) \n"
"tag1      = Text label for the position bounding the region being added (required) \n"
"tag2      = Text label for the position bounding the region being added (required) \n"
"region    = Name for the region being created (required) \n"
"material  = Material for the region being created (required) \n"
;

static const char add_2d_contact_doc[] =
"ds.add_2d_contact (name=STRING, material=STRING, mesh=STRING, \n"
"    region=STRING, xl=FLOAT, xh=FLOAT, yl=FLOAT, yh=FLOAT, bloat=FLOAT)\n"
"\n"
"Add an interface to a 2D mesh\n"
"\n"
"name      = Name for the contact being created (required) \n"
"material  = material for the contact being created (required) \n"
"mesh      = Mesh to add the contact to (required) \n"
"region    = Name of the region included in the contact (required) \n"
"xl        = x position for corner of bounding box (optional) (-MAXDOUBLE default)\n"
"xh        = x position for corner of bounding box (optional) (+MAXDOUBLE default)\n"
"yl        = y position for corner of bounding box (optional) (-MAXDOUBLE default)\n"
"yh        = y position for corner of bounding box (optional) (+MAXDOUBLE default)\n"
"bloat     = Extend bounding box by this amount when search for mesh to include in region (optional) (1e-10 default)\n"
;

static const char add_2d_interface_doc[] =
"ds.add_2d_interface (mesh=STRING, name=STRING, region0=STRING, \n"
"    region1=STRING, xl=FLOAT, xh=FLOAT, yl=FLOAT, yh=FLOAT, bloat=FLOAT)\n"
"\n"
"Add an interface to a 2D mesh\n"
"\n"
"mesh      = Mesh to add the interface to (required) \n"
"name      = Name for the interface being created (required) \n"
"region0   = Name of the region included in the interface (required) \n"
"region1   = Name of the region included in the interface (required) \n"
"xl        = x position for corner of bounding box (optional) (-MAXDOUBLE default)\n"
"xh        = x position for corner of bounding box (optional) (+MAXDOUBLE default)\n"
"yl        = y position for corner of bounding box (optional) (-MAXDOUBLE default)\n"
"yh        = y position for corner of bounding box (optional) (+MAXDOUBLE default)\n"
"bloat     = Extend bounding box by this amount when search for mesh to include in region (optional) (1e-10 default)\n"
;

static const char add_2d_mesh_line_doc[] =
"ds.add_2d_mesh_line (mesh=STRING, pos=STRING, ns=FLOAT, ps=FLOAT)\n"
"\n"
"Add a mesh line to a 2D mesh\n"
"\n"
"mesh      = Mesh to add the line to (required) \n"
"pos       = Position for the mesh point (required) \n"
"ns        = Spacing from this point in the negative direction (defaults to ps value) (required) \n"
"ps        = Spacing from this point in the positive direction (required) \n"
;

static const char add_2d_region_doc[] =
"ds.add_2d_region (mesh=STRING, region=STRING, material=STRING, \n"
"    xl=FLOAT, xh=FLOAT, yl=FLOAT, yh=FLOAT, bloat=FLOAT)\n"
"\n"
"Add a region to a 2D mesh\n"
"\n"
"mesh      = Mesh to add the region to (required) \n"
"region    = Name for the region being created (required) \n"
"material  = Material for the region being created (required) \n"
"xl        = x position for corner of bounding box (optional) (-MAXDOUBLE default)\n"
"xh        = x position for corner of bounding box (optional) (+MAXDOUBLE default)\n"
"yl        = y position for corner of bounding box (optional) (-MAXDOUBLE default)\n"
"yh        = y position for corner of bounding box (optional) (+MAXDOUBLE default)\n"
"bloat     = Extend bounding box by this amount when search for mesh to include in region (optional) (1e-10 default)\n"
;

static const char add_genius_contact_doc[] =
"ds.add_genius_contact (genius_name=STRING, material=STRING, \n"
"    mesh=STRING, name=STRING, region=STRING)\n"
"\n"
"Create a contact for an imported genius  mesh\n"
"\n"
"genius_name= boundary condition name in the genius cgns file (required) \n"
"material  = material for the contact being created (required) \n"
"mesh      = name of the mesh being generated (required) \n"
"name      = name of the contact begin created (required) \n"
"region    = region that the contact is attached to (required) \n"
;

static const char add_genius_interface_doc[] =
"ds.add_genius_interface (genius_name=STRING, mesh=STRING, name=STRING, \n"
"    region0=STRING, region1=STRING)\n"
"\n"
"Create an interface for an imported genius  mesh\n"
"\n"
"genius_name= boundary condition name in the genius cgns file (required) \n"
"mesh      = name of the mesh being generated (required) \n"
"name      = name of the interface begin created (required) \n"
"region0   = first region that the interface is attached to (required) \n"
"region1   = second region that the interface is attached to (required) \n"
;

static const char add_genius_region_doc[] =
"ds.add_genius_region (genius_name=STRING, mesh=STRING, region=STRING, \n"
"    material=STRING)\n"
"\n"
"Create a region for an imported genius  mesh\n"
"\n"
"genius_name= region name in the genius cgns file (required) \n"
"mesh      = name of the mesh being generated (required) \n"
"region    = name of the region begin created (required) \n"
"material  = material for the region being created (required) \n"
;

static const char add_gmsh_contact_doc[] =
"ds.add_gmsh_contact (gmsh_name=STRING, material=STRING, mesh=STRING, \n"
"    name=STRING, region=STRING)\n"
"\n"
"Create a mesh to import a gmsh mesh\n"
"\n"
"gmsh_name = physical group name in the gmsh file (required) \n"
"material  = material for the contact being created (required) \n"
"mesh      = name of the mesh being generated (required) \n"
"name      = name of the contact begin created (required) \n"
"region    = region that the contact is attached to (required) \n"
;

static const char add_gmsh_interface_doc[] =
"ds.add_gmsh_interface (gmsh_name=STRING, mesh=STRING, name=STRING, \n"
"    region0=STRING, region1=STRING)\n"
"\n"
"Create an interface for an imported gmsh mesh\n"
"\n"
"gmsh_name = physical group name in the gmsh file (required) \n"
"mesh      = name of the mesh being generated (required) \n"
"name      = name of the interface begin created (required) \n"
"region0   = first region that the interface is attached to (required) \n"
"region1   = second region that the interface is attached to (required) \n"
;

static const char add_gmsh_region_doc[] =
"ds.add_gmsh_region (gmsh_name=STRING, mesh=STRING, region=STRING, \n"
"    material=STRING)\n"
"\n"
"Create a region for an imported gmsh mesh\n"
"\n"
"gmsh_name = physical group name in the gmsh file (required) \n"
"mesh      = name of the mesh being generated (required) \n"
"region    = name of the region begin created (required) \n"
"material  = material for the region being created (required) \n"
;

static const char create_1d_mesh_doc[] =
"ds.create_1d_mesh (mesh=STRING)\n"
"\n"
"Create a mesh to create a 1D device\n"
"\n"
"mesh      = name of the 1D mesh being created (required) \n"
;

static const char create_2d_mesh_doc[] =
"ds.create_2d_mesh (mesh=STRING)\n"
"\n"
"Create a mesh to create a 2D device\n"
"\n"
"mesh      = name of the 2D mesh being created (required) \n"
;

static const char create_contact_from_interface_doc[] =
"ds.create_contact_from_interface (device=STRING, region=STRING, \n"
"    interface=STRING, material=STRING, name=STRING)\n"
"\n"
"Creates a contact on a device from an existing interface\n"
"\n"
"device    = The selected device (required) \n"
"region    = The selected region (required) \n"
"interface = Interface on which to apply this command (required) \n"
"material  = material for the contact being created (required) \n"
"name      = name of the contact begin created (required) \n"
;

static const char create_device_doc[] =
"ds.create_device (mesh=STRING, device=STRING)\n"
"\n"
"Create a device from a mesh\n"
"\n"
"mesh      = name of the mesh being used to create a device (required) \n"
"device    = name of the device being created (required) \n"
;

static const char create_genius_mesh_doc[] =
"ds.create_genius_mesh (file=STRING, mesh=STRING)\n"
"\n"
"This command reads in a genius mesh written in the cgns format\n"
"\n"
"file      = name of the genius mesh file being read into devsim (required) \n"
"mesh      = name of the mesh being generated (required) \n"
;

static const char create_gmsh_mesh_doc[] =
"ds.create_gmsh_mesh (file=STRING, mesh=STRING)\n"
"\n"
"Create a mesh to import a gmsh mesh\n"
"\n"
"file      = name of the gmsh mesh file being read into devsim (required) \n"
"mesh      = name of the mesh being generated (required) \n"
;

static const char finalize_mesh_doc[] =
"ds.finalize_mesh (mesh=STRING)\n"
"\n"
"Finalize a mesh so no additional mesh specifications can be added and devices can be created.\n"
"\n"
"mesh      = Mesh to finalize (required) \n"
;

static const char load_devices_doc[] =
"ds.load_devices (file=STRING)\n"
"\n"
"Load devices from a devsim file\n"
"\n"
"file      = name of the file to load the meshes from (required) \n"
;

static const char write_devices_doc[] =
"ds.write_devices (file=STRING, device=STRING, type=OPTION)\n"
"\n"
"Write a device to a file for visualization or restart\n"
"\n"
"file      = name of the file to write the meshes to (required) \n"
"device    = name of the device to write (optional) \n"
"type      = format to use (optional) (\"devsim\" default)\n"
"  devsim          devsim format\n"
"  devsim_data     devsim output format with numerical data for all models\n"
"  floops          Floops format (for visualization in postmini)\n"
"  tecplot         Tecplot format (for visualization in tecplot)\n"
"  vtk             VTK format (for visualization in paraview and visit)\n"
;

static const char contact_edge_model_doc[] =
"ds.contact_edge_model (device=STRING, contact=STRING, name=STRING, \n"
"    equation=STRING, display_type=OPTION)\n"
"\n"
"Create an edge model evaluated at a contact\n"
"\n"
"device    = The selected device (required) \n"
"contact   = Contact on which to apply this command (required) \n"
"name      = Name of the contact edge model being created (required) \n"
"equation  = Equation used to describe the contact edge model being created (required) \n"
"display_type= Option for output display in graphical viewer (optional) (\"vector\" default)\n"
"  nodisplay       Data on edge will not be displayed\n"
"  scalar          Data on edge is a scalar quantity\n"
"  vector          Data on edge is a vector quantity\n"
;

static const char contact_node_model_doc[] =
"ds.contact_node_model (device=STRING, contact=STRING, name=STRING, \n"
"    equation=STRING, display_type=OPTION)\n"
"\n"
"Create an node model evaluated at a contact\n"
"\n"
"device    = The selected device (required) \n"
"contact   = Contact on which to apply this command (required) \n"
"name      = Name of the contact node model being created (required) \n"
"equation  = Equation used to describe the contact node model being created (required) \n"
"display_type= Option for output display in graphical viewer (optional) (\"scalar\" default)\n"
"  nodisplay       Data on edge will not be displayed\n"
"  scalar          Data on edge is a scalar quantity\n"
;

static const char cylindrical_edge_couple_doc[] =
"ds.cylindrical_edge_couple (device=STRING, region=STRING)\n"
"\n"
"This command creates the modelName(EdgeCouple) model for 2D cylindrical simulation\n"
"\n"
"device    = The selected device (required) \n"
"region    = The selected region (required) \n"
;

static const char cylindrical_node_volume_doc[] =
"ds.cylindrical_node_volume (device=STRING, region=STRING)\n"
"\n"
"This command creates the modelName(NodeVolume) model for 2D cylindrical simulation\n"
"\n"
"device    = The selected device (required) \n"
"region    = The selected region (required) \n"
;

static const char cylindrical_surface_area_doc[] =
"ds.cylindrical_surface_area (device=STRING, region=STRING)\n"
"\n"
"This command creates the modelName(SurfaceArea) model for 2D cylindrical simulation\n"
"\n"
"device    = The selected device (required) \n"
"region    = The selected region (required) \n"
;

static const char debug_triangle_models_doc[] =
"ds.debug_triangle_models (device=STRING, region=STRING)\n"
"\n"
"Debugging command used in the development of devsim and used in regressions.\n"
"\n"
"device    = The selected device (required) \n"
"region    = The selected region (required) \n"
;

static const char delete_edge_model_doc[] =
"ds.delete_edge_model (device=STRING, region=STRING, name=STRING)\n"
"\n"
"Deletes an edge model from a region\n"
"\n"
"device    = The selected device (required) \n"
"region    = The selected region (required) \n"
"name      = Name of the edge model being deleted (required) \n"
;

static const char delete_element_model_doc[] =
"ds.delete_element_model (device=STRING, region=STRING, name=STRING)\n"
"\n"
"Deletes a element model from a region\n"
"\n"
"device    = The selected device (required) \n"
"region    = The selected region (required) \n"
"name      = Name of the node model being deleted (required) \n"
;

static const char delete_interface_model_doc[] =
"ds.delete_interface_model (device=STRING, interface=STRING, name=STRING)\n"
"\n"
"Deletes an interface model from an interface\n"
"\n"
"device    = The selected device (required) \n"
"interface = Interface on which to apply this command (required) \n"
"name      = Name of the interface model being deleted (required) \n"
;

static const char delete_node_model_doc[] =
"ds.delete_node_model (device=STRING, region=STRING, name=STRING)\n"
"\n"
"Deletes a node model from a region\n"
"\n"
"device    = The selected device (required) \n"
"region    = The selected region (required) \n"
"name      = Name of the node model being deleted (required) \n"
;

static const char edge_average_model_doc[] =
"ds.edge_average_model (device=STRING, region=STRING, node_model=STRING, \n"
"    edge_model=STRING, derivative=STRING, average_type=STRING)\n"
"\n"
"Creates an edge model based on the node model values\n"
"\n"
"device    = The selected device (required) \n"
"region    = The selected region (required) \n"
"node_model= The node model from which we are creating the edge model.  If commandOptionName{derivative} is specified, the edge model is created from modelName{nodeModel:derivativeModel} (required) \n"
"edge_model= The edge model name being created.  If commandOptionName{derivative} is specified, the edge models created are modelName{edgeModel:derivativeModel@n0} modelName{edgeModel:derivativeModel@n1}, which are the derivatives with respect to the derivative model on each side of the edge (required) \n"
"derivative= The node model of the variable for which the derivative is being taken.  The node model modelName{nodeModel:derivativeModel} is used to create the resulting edge models. (optional) \n"
"average_type= The node models on both sides of the edge are averaged together to create one of the following types of averages. (optional) (\"arithmetic\" default)\n"
"  arithmetic      The edge model is the average of the node model on both sides\n"
"  geometric       The edge model is the square root of the product of the node model evaluated on each side\n"
"  gradient        The edge model is the gradient along the edge with respect to the distance between the two nodes.\n"
"  negative_gradient The edge model is the negative of the gradient along the edge\n"
;

static const char edge_from_node_model_doc[] =
"ds.edge_from_node_model (device=STRING, region=STRING, \n"
"    node_model=STRING)\n"
"\n"
"For a node model, creates an 2 edge models referring to the node model value at both ends of the edge.\n"
"\n"
"device    = The selected device (required) \n"
"region    = The selected region (required) \n"
"node_model= The node model from which we are creating the edge model (required) \n"
;

static const char edge_model_doc[] =
"ds.edge_model (device=STRING, region=STRING, name=STRING, \n"
"    equation=STRING, display_type=STRING)\n"
"\n"
"Creates an edge model based on an equation\n"
"\n"
"device    = The selected device (required) \n"
"region    = The selected region (required) \n"
"name      = Name of the edge model being created (required) \n"
"equation  = Equation used to describe the edge model being created (required) \n"
"display_type= Option for output display in graphical viewer (optional) (\"scalar\" default)\n"
"  nodisplay       Data on edge will not be displayed\n"
"  scalar          Data on edge is a scalar quantity\n"
"  vector          Data on edge is a vector quantity (deprecated)\n"
;

static const char element_from_edge_model_doc[] =
"ds.element_from_edge_model (device=STRING, region=STRING, \n"
"    edge_model=STRING, derivative=STRING)\n"
"\n"
"Creates element edge models from an edge model\n"
"\n"
"device    = The selected device (required) \n"
"region    = The selected region (required) \n"
"edge_model= The edge model from which we are creating the element model (required) \n"
"derivative= The variable we are taking with respect to edge_model (optional) \n"
;

static const char element_from_node_model_doc[] =
"ds.element_from_node_model (device=STRING, region=STRING, \n"
"    node_model=STRING)\n"
"\n"
"Creates element edge models from a node model\n"
"\n"
"device    = The selected device (required) \n"
"region    = The selected region (required) \n"
"node_model= The node model from which we are creating the edge model (required) \n"
;

static const char element_model_doc[] =
"ds.element_model (device=STRING, region=STRING, name=STRING, \n"
"    equation=STRING, display_type=STRING)\n"
"\n"
"Create a model evaluated on element edges.\n"
"\n"
"device    = The selected device (required) \n"
"region    = The selected region (required) \n"
"name      = Name of the element edge model being created (required) \n"
"equation  = Equation used to describe the element edge model being created (required) \n"
"display_type= Option for output display in graphical viewer (optional) (\"scalar\" default)\n"
"  nodisplay       Data on edge will not be displayed\n"
"  scalar          Data on edge is a scalar quantity\n"
;

static const char get_edge_model_list_doc[] =
"ds.get_edge_model_list (device=STRING, region=STRING)\n"
"\n"
"Returns a list of the edge models on the device region\n"
"\n"
"device    = The selected device (required) \n"
"region    = The selected region (required) \n"
;

static const char get_edge_model_values_doc[] =
"ds.get_edge_model_values (device=STRING, region=STRING, name=STRING)\n"
"\n"
"Get the edge model values calculated at each edge.\n"
"\n"
"device    = The selected device (required) \n"
"region    = The selected region (required) \n"
"name      = Name of the edge model values being returned as a list (required) \n"
;

static const char get_element_model_list_doc[] =
"ds.get_element_model_list (device=STRING, region=STRING)\n"
"\n"
"Returns a list of the element edge models on the device region\n"
"\n"
"device    = The selected device (required) \n"
"region    = The selected region (required) \n"
;

static const char get_element_model_values_doc[] =
"ds.get_element_model_values (device=STRING, region=STRING, name=STRING)\n"
"\n"
"Get element model values at each element edge\n"
"\n"
"device    = The selected device (required) \n"
"region    = The selected region (required) \n"
"name      = Name of the element edge model values being returned as a list (required) \n"
;

static const char get_interface_model_list_doc[] =
"ds.get_interface_model_list (device=STRING, interface=STRING)\n"
"\n"
"Returns a list of the interface models on the interface\n"
"\n"
"device    = The selected device (required) \n"
"interface = Interface on which to apply this command (required) \n"
;

static const char get_interface_model_values_doc[] =
"ds.get_interface_model_values (device=STRING, interface=STRING, \n"
"    name=STRING)\n"
"\n"
"Gets interface model values evaluated at each interface node.\n"
"\n"
"device    = The selected device (required) \n"
"interface = Interface on which to apply this command (required) \n"
"name      = Name of the interface model values being returned as a list (required) \n"
;

static const char get_node_model_list_doc[] =
"ds.get_node_model_list (device=STRING, region=STRING)\n"
"\n"
"Returns a list of the node models on the device region\n"
"\n"
"device    = The selected device (required) \n"
"region    = The selected region (required) \n"
;

static const char get_node_model_values_doc[] =
"ds.get_node_model_values (device=STRING, region=STRING, name=STRING)\n"
"\n"
"Get node model values evaluated at each node in a region.\n"
"\n"
"device    = The selected device (required) \n"
"region    = The selected region (required) \n"
"name      = Name of the node model values being returned as a list (required) \n"
;

static const char interface_model_doc[] =
"ds.interface_model (device=STRING, interface=STRING, equation=STRING)\n"
"\n"
"Create an interface model from an equation.\n"
"\n"
"device    = The selected device (required) \n"
"interface = Interface on which to apply this command (required) \n"
"equation  = Equation used to describe the interface node model being created (required) \n"
;

static const char interface_normal_model_doc[] =
"ds.interface_normal_model (device=STRING, region=STRING, \n"
"    interface=STRING)\n"
"\n"
"Creates edge models whose components are based on direction and distance to an interface\n"
"\n"
"device    = The selected device (required) \n"
"region    = The selected region (required) \n"
"interface = Interface on which to apply this command (required) \n"
;

static const char node_model_doc[] =
"ds.node_model (device=STRING, region=STRING, name=STRING, \n"
"    equation=STRING, display_type=STRING)\n"
"\n"
"Create a node model from an equation.\n"
"\n"
"device    = The selected device (required) \n"
"region    = The selected region (required) \n"
"name      = Name of the node model being created (required) \n"
"equation  = Equation used to describe the node model being created (required) \n"
"display_type= Option for output display in graphical viewer (required) \n"
"  nodisplay       Data on node will not be displayed\n"
"  scalar          Data on node is a scalar quantity (default)\n"
;

static const char node_solution_doc[] =
"ds.node_solution (device=STRING, region=STRING, name=STRING)\n"
"\n"
"Create node model whose values are set.\n"
"\n"
"device    = The selected device (required) \n"
"region    = The selected region (required) \n"
"name      = Name of the solution being created (required) \n"
;

static const char print_edge_values_doc[] =
"ds.print_edge_values (device=STRING, region=STRING, name=STRING)\n"
"\n"
"Print edge values for debugging.\n"
"\n"
"device    = The selected device (required) \n"
"region    = The selected region (required) \n"
"name      = Name of the edge model values being printed to the screen (required) \n"
;

static const char print_element_values_doc[] =
"ds.print_element_values (device=STRING, region=STRING, name=STRING)\n"
"\n"
"Print element values for debugging.\n"
"\n"
"device    = The selected device (required) \n"
"region    = The selected region (required) \n"
"name      = Name of the element edge model values being printed to the screen (required) \n"
;

static const char print_node_values_doc[] =
"ds.print_node_values (device=STRING, region=STRING, name=STRING)\n"
"\n"
"Print node values for debugging.\n"
"\n"
"device    = The selected device (required) \n"
"region    = The selected region (required) \n"
"name      = Name of the node model values being printed to the screen (required) \n"
;

static const char register_function_doc[] =
"ds.register_function (name=STRING, nargs=STRING)\n"
"\n"
"This command is used to register a new python procedure for evaluation by symdiff.\n"
"\n"
"name      = Name of the function (required) \n"
"nargs     = Number of arguments to the function (required) \n"
;

static const char set_node_value_doc[] =
"ds.set_node_value (device=STRING, region=STRING, name=STRING, \n"
"    index=INTEGER, value=FLOAT)\n"
"\n"
"A uniform value is used if index is not specified.  Note that equation based node models will lose this value if their equation is recalculated.\n"
"\n"
"device    = The selected device (required) \n"
"region    = The selected region (required) \n"
"name      = Name of the node model being whose value is being set (required) \n"
"index     = Index of node being set (required) \n"
"value     = Value of node being set (required) \n"
;

static const char set_node_values_doc[] =
"ds.set_node_values (device=STRING, region=STRING, name=STRING, \n"
"    init_from=STRING, values=LIST)\n"
"\n"
"Set node model values from another node model, or a list of values.\n"
"\n"
"device    = The selected device (required) \n"
"region    = The selected region (required) \n"
"name      = Name of the node model being initialized (required) \n"
"init_from = Node model we are using to initialize the node solution (optional) \n"
"values    = List of values for each node in the region. (optional) \n"
;

static const char symdiff_doc[] =
"ds.symdiff (expr=STRING)\n"
"\n"
"This command returns an expression.  All strings are treated as independent variables.  It is primarily used for defining new functions to the parser.\n"
"\n"
"expr      = Expression to send to symdiff (required) \n"
;

static const char vector_element_model_doc[] =
"ds.vector_element_model (device=STRING, region=STRING, \n"
"    element_model=STRING)\n"
"\n"
"Create vector components from an element edge model\n"
"\n"
"device    = The selected device (required) \n"
"region    = The selected region (required) \n"
"element_model= The element model for which we are calculating the vector compoenents (required) \n"
;

static const char vector_gradient_doc[] =
"ds.vector_gradient (device=STRING, region=STRING, node_model=STRING, \n"
"    calc_type=STRING)\n"
"\n"
"Creates the vector gradient for noise analysis\n"
"\n"
"device    = The selected device (required) \n"
"region    = The selected region (required) \n"
"node_model= The node model from which we are creating the edge model (required) \n"
"calc_type = The node model from which we are creating the edge model (optional) (\"default\" default)\n"
"  default         Consider all nodes for calculating the gradient field\n"
"  avoidzero       Do not take gradient at nodes where the node_model is zero\n"
;

static const char get_contact_charge_doc[] =
"ds.get_contact_charge (device=STRING, contact=STRING, equation=STRING)\n"
"\n"
"Get charge at the contact\n"
"\n"
"device    = The selected device (required) \n"
"contact   = Contact on which to apply this command (required) \n"
"equation  = Name of the contact equation from which we are retrieving the charge (required) \n"
;

static const char get_contact_current_doc[] =
"ds.get_contact_current (device=STRING, contact=STRING, equation=STRING)\n"
"\n"
"Get current at the contact\n"
"\n"
"device    = The selected device (required) \n"
"contact   = Contact on which to apply this command (required) \n"
"equation  = Name of the contact equation from which we are retrieving the current (required) \n"
;

static const char solve_doc[] =
"ds.solve (type=OPTION, solver_type=OPTION, absolute_error=FLOAT, \n"
"    relative_error=FLOAT, charge_error=FLOAT, gamma=FLOAT, \n"
"    tdelta=FLOAT, maximum_iterations=INTEGER, frequency=FLOAT, \n"
"    output_node=STRING, info=STRING)\n"
"\n"
"Call the solver.  A small-signal AC source is set with the circuit voltage source.\n"
"\n"
"type      = type of solve being performed (required) \n"
"  dc              DC steady state simulation\n"
"  ac              Small-signal AC simulation\n"
"  noise           Small-signal AC simulation\n"
"  transient_dc    Perform DC steady state and keep calculate charge at initial time step\n"
"  transient_bdf1  Perform transient simulation using backward difference integration\n"
"  transient_bdf2  Perform transient simulation using backward difference integration\n"
"  transient_tr    Perform transient simulation using trapezoidal integration\n"
"solver_type= Linear solver type (required) \n"
"  direct          Use LU factorization (default)\n"
"  iterative       Use iterative solver\n"
"absolute_error= Required update norm in the solve (required) \n"
"relative_error= Required relative update in the solve (required) \n"
"charge_error= Relative error between projected and solved charge during transient simulation (required) \n"
"gamma     = Scaling factor for transient time step (default 1.0) (required) \n"
"tdelta    = time step (required) \n"
"maximum_iterations= Maximum number of iterations in the DC solve (required) \n"
"frequency = Frequency for small-signal AC simulation (required) \n"
"output_node= Output circuit node for noise simulation (required) \n"
"info      = Solve command return convergence information (required) \n"
;
