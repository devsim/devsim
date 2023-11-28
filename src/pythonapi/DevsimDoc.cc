
static const char add_circuit_node_doc[] =
R"(    devsim.add_circuit_node (name, value, variable_update)

    Adds a circuit node for use in circuit or multi-device simulation

    Parameters
    ----------
    name : str
       Name of the circuit node being created
    value : Float, optional
       initial value (default 0.0)
    variable_update : {'default', 'log_damp', 'positive'}
       update type for circuit variable
)";

static const char circuit_alter_doc[] =
R"(    devsim.circuit_alter (name, param, value)

    Alter the value of a circuit element parameter

    Parameters
    ----------
    name : str
       Name of the circuit node being created
    param : str, optional
       parameter being modified (default 'value')
    value : Float
       value for the parameter
)";

static const char circuit_element_doc[] =
R"(    devsim.circuit_element (name, value, n1, n2, acreal, acimag)

    Adds a circuit element external to the devices

    Parameters
    ----------
    name : str
       Name of the circuit element being created.  A prefix of 'V' is for voltage source, 'I' for current source, 'R' for resistor, 'L' for inductor, and 'C' for capacitor.
    value : Float, optional
       value for the default parameter of the circuit element (default 0.0)
    n1 : str
       circuit node
    n2 : str
       circuit node
    acreal : Float, optional
       real part of AC source for voltage (default 0.0)
    acimag : Float, optional
       imag part of AC source for voltage (default 0.0)
)";

static const char circuit_node_alias_doc[] =
R"(    devsim.circuit_node_alias (node, alias)

    Create an alias for a circuit node

    Parameters
    ----------
    node : str
       circuit node being aliased
    alias : str
       alias for the circuit node
)";

static const char delete_circuit_doc[] =
R"(    devsim.delete_circuit ()

    Deletes any present circuit and its solutions.
)";

static const char get_circuit_equation_number_doc[] =
R"(    devsim.get_circuit_equation_number (node)

    Returns the row number correspond to circuit node in a region.  Values are only valid when during the course of a solve.

    Parameters
    ----------
    node : str
       circuit node
)";

static const char get_circuit_node_list_doc[] =
R"(    devsim.get_circuit_node_list ()

    Gets the list of the nodes in the circuit.
)";

static const char get_circuit_node_value_doc[] =
R"(    devsim.get_circuit_node_value (solution, node)

    Gets the value of a circuit node for a given solution type.

    Parameters
    ----------
    solution : str, optional
       name of the solution. 'dcop' is the name for the DC solution (default 'dcop')
    node : str
       circuit node of interest
)";

static const char get_circuit_solution_list_doc[] =
R"(    devsim.get_circuit_solution_list ()

    Gets the list of available circuit solutions.
)";

static const char set_circuit_node_value_doc[] =
R"(    devsim.set_circuit_node_value (solution, node, value)

    Sets the value of a circuit node for a given solution type.

    Parameters
    ----------
    solution : str, optional
       name of the solution. 'dcop' is the name for the DC solution (default 'dcop')
    node : str
       circuit node of interest
    value : Float, optional
       new value (default 0.0)
)";

static const char contact_equation_doc[] =
R"(    devsim.contact_equation (device, contact, name, circuit_node, edge_charge_model, edge_current_model, edge_model, edge_volume_model, element_charge_model, element_current_model, element_model, volume_node0_model, volume_node1_model, node_charge_model, node_current_model, node_model)

    Create a contact equation on a device

    Parameters
    ----------
    device : str
       The selected device
    contact : str
       Contact on which to apply this command
    name : str
       Name of the contact equation being created
    circuit_node : str, optional
       Name of the circuit we integrate the flux into
    edge_charge_model : str, optional
       Name of the edge model used to determine the charge at this contact
    edge_current_model : str, optional
       Name of the edge model used to determine the current flowing out of this contact
    edge_model : str, optional
       Name of the edge model being integrated at each edge at this contact
    edge_volume_model : str, optional
       Name of the edge model being integrated over the volume of each edge on the contact
    element_charge_model : str, optional
       Name of the element edge model used to determine the charge at this contact
    element_current_model : str, optional
       Name of the element edge model used to determine the current flowing out of this contact
    element_model : str, optional
       Name of the element edge model being integrated at each edge at this contact
    volume_node0_model : str, optional
       Name of the element model being integrated over the volume of node 0 of each edge on the contact
    volume_node1_model : str, optional
       Name of the element model being integrated over the volume of node 1 of each edge on the contact
    node_charge_model : str, optional
       Name of the node model used to determine the charge at this contact
    node_current_model : str, optional
       Name of the node model used to determine the current flowing out of this contact
    node_model : str, optional
       Name of the node model being integrated at each node at this contact
)";

static const char custom_equation_doc[] =
R"(    devsim.custom_equation (name, procedure)

    Custom equation assembly.  See :ref:`models__customequation` for a description of how the function should be structured.

    Parameters
    ----------
    name : str
       Name of the custom equation being created
    procedure : str
       The procedure to be called
)";

static const char delete_contact_equation_doc[] =
R"(    devsim.delete_contact_equation (device, contact, name)

    This command deletes an equation from a contact.

    Parameters
    ----------
    device : str
       The selected device
    contact : str
       Contact on which to apply this command
    name : str
       Name of the contact equation being deleted
)";

static const char delete_equation_doc[] =
R"(    devsim.delete_equation (device, region, name)

    This command deletes an equation from a region.

    Parameters
    ----------
    device : str
       The selected device
    region : str
       The selected region
    name : str
       Name of the equation being deleted
)";

static const char delete_interface_equation_doc[] =
R"(    devsim.delete_interface_equation (device, interface, name)

    This command deletes an equation from an interface.

    Parameters
    ----------
    device : str
       The selected device
    interface : str
       Interface on which to apply this command
    name : str
       Name of the interface equation being deleted
)";

static const char equation_doc[] =
R"(    devsim.equation (device, region, name, variable_name, node_model, edge_model, edge_volume_model, time_node_model, element_model, volume_node0_model, volume_node1_model, variable_update)

    Specify an equation to solve on a device

    Parameters
    ----------
    device : str
       The selected device
    region : str
       The selected region
    name : str
       Name of the equation being created
    variable_name : str
       Name of the node solution being solved
    node_model : str, optional
       Name of the node model being integrated at each node in the device volume
    edge_model : str, optional
       Name of the edge model being integrated over each edge in the device volume
    edge_volume_model : str, optional
       Name of the edge model being integrated over the volume of each edge in the device volume
    time_node_model : str, optional
       Name of the time dependent node_model being integrated at each node in the device volume
    element_model : str, optional
       Name of the element model being integrated over each edge in the device volume
    volume_node0_model : str, optional
       Name of the element model being integrated over the volume of node 0 of each edge on the contact
    volume_node1_model : str, optional
       Name of the element model being integrated over the volume of node 1 of each edge on the contact
    variable_update : {'default', 'log_damp', 'positive'}
       update type for circuit variable

    Notes
    -----

    The integration variables can be changed in 2D for cylindrical coordinate systems by setting the appropriate parameters as described in :ref:`sec__cylindrical`.

    In order to set the node volumes for integration of the ``edge_volume_model``, it is possible to do something like this:

    ..

      devsim.edge_model(device="device", region="region", name="EdgeNodeVolume", equation="0.5*SurfaceArea*EdgeLength")
      devsim.set_parameter(name="edge_node0_volume_model", value="EdgeNodeVolume")
      devsim.set_parameter(name="edge_node1_volume_model", value="EdgeNodeVolume")

)";

static const char get_contact_equation_command_doc[] =
R"(    devsim.get_contact_equation_command (device, contact, name)

    This command gets the options used when creating this contact equation.

    Parameters
    ----------
    device : str
       The selected device
    contact : str
       Contact on which to apply this command
    name : str
       Name of the contact equation being command options returned
)";

static const char get_contact_equation_list_doc[] =
R"(    devsim.get_contact_equation_list (device, contact)

    This command gets a list of equations on the specified contact.

    Parameters
    ----------
    device : str
       The selected device
    contact : str
       Contact on which to apply this command
)";

static const char get_equation_command_doc[] =
R"(    devsim.get_equation_command (device, region, name)

    This command gets the options used when creating this equation.

    Parameters
    ----------
    device : str
       The selected device
    region : str
       The selected region
    name : str
       Name of the equation being command options returned
)";

static const char get_equation_list_doc[] =
R"(    devsim.get_equation_list (device, region)

    This command gets a list of equations on the specified region.

    Parameters
    ----------
    device : str
       The selected device
    region : str
       The selected region
)";

static const char get_equation_numbers_doc[] =
R"(    devsim.get_equation_numbers (device, region, equation, variable)

    Returns a list of the equation numbers corresponding to each node in a region.  Values are only valid when during the course of a solve.

    Parameters
    ----------
    device : str
       The selected device
    region : str
       The selected region
    equation : str, optional
       Name of the equation
    variable : str, optional
       Name of the variable
)";

static const char get_interface_equation_command_doc[] =
R"(    devsim.get_interface_equation_command (device, interface, name)

    This command gets the options used when creating this interface equation.

    Parameters
    ----------
    device : str
       The selected device
    interface : str
       Interface on which to apply this command
    name : str
       Name of the interface equation being command options returned
)";

static const char get_interface_equation_list_doc[] =
R"(    devsim.get_interface_equation_list (device, interface)

    This command gets a list of equations on the specified interface.

    Parameters
    ----------
    device : str
       The selected device
    interface : str
       Interface on which to apply this command
)";

static const char interface_equation_doc[] =
R"(    devsim.interface_equation (device, interface, name, name0, name1, interface_model, type)

    Command to specify an equation at an interface

    Parameters
    ----------
    device : str
       The selected device
    interface : str
       Interface on which to apply this command
    name : str
       Name of the interface equation being created
    name0 : str, optional
       Name of the equation coupling in region 0 being created (default 'name')
    name1 : str, optional
       Name of the equation coupling in region 1 being created (default 'name')
    interface_model : str
       When specified, the bulk equations on both sides of the interface are integrated together.  This model is then used to specify how nodal quantities on both sides of the interface are balanced
    type : {'continuous', 'fluxterm', 'hybrid'} required
       Specifies the type of boundary condition
)";

static const char get_contact_list_doc[] =
R"(    devsim.get_contact_list (device)

    Gets a list of contacts on a device.

    Parameters
    ----------
    device : str
       The selected device
)";

static const char get_device_list_doc[] =
R"(    devsim.get_device_list ()

    Gets a list of devices on the simulation.
)";

static const char get_element_node_list_doc[] =
R"(    devsim.get_element_node_list (device, region, contact, interface, reorder)

    Gets a list of nodes for each element on a device, region, contact, or interface.

    Parameters
    ----------
    device : str
       The selected device
    region : str
       The selected region
    contact : str, optional
       If specified, gets the element nodes for the contact on the specified region
    interface : str, optional
       If specified, gets the element nodes for the interface on the specified region
    reorder : bool, optional
       If specified, reorders the element nodes in a manner compatible in meshing software (default False)
)";

static const char get_interface_list_doc[] =
R"(    devsim.get_interface_list (device)

    Gets a list of interfaces on a device.

    Parameters
    ----------
    device : str
       The selected device
)";

static const char get_region_list_doc[] =
R"(    devsim.get_region_list (device, contact, interface)

    Gets a list of regions on a device, contact, or interface.

    Parameters
    ----------
    device : str
       The selected device
    contact : str, optional
       If specified, gets the name of the region belonging to this contact on the device
    interface : str, optional
       If specified, gets the name of the regions belonging to this interface on the device
)";

static const char reset_devsim_doc[] =
R"(    devsim.reset_devsim ()

    Resets all data for clean restart.
)";

static const char add_db_entry_doc[] =
R"(    devsim.add_db_entry (material, parameter, value, unit, description)

    Adds an entry to the database

    Parameters
    ----------
    material : str
       Material name requested. ``global`` refers to all regions whose material does not have the parameter name specified
    parameter : str
       Parameter name
    value : str
       Value assigned for the parameter
    unit : str
       String describing the units for this parameter name
    description : str
       Description of the parameter for this material type.

    Notes
    -----

    The :meth:`devsim.save_db` command is used to commit these added entries permanently to the database.
)";

static const char close_db_doc[] =
R"(    devsim.close_db ()

    Closes the database so that its entries are no longer available
)";

static const char create_db_doc[] =
R"(    devsim.create_db (filename)

    Create a database to store material properties

    Parameters
    ----------
    filename : str
       filename to create for the db
)";

static const char get_db_entry_doc[] =
R"(    devsim.get_db_entry (material, parameter)

    This command returns a list containing the value, unit, and description for the requested material db entry

    Parameters
    ----------
    material : str
       Material name
    parameter : str
       Parameter name
)";

static const char get_dimension_doc[] =
R"(    devsim.get_dimension (device)

    Get the dimension of the device

    Parameters
    ----------
    device : str, optional
       The selected device
)";

static const char get_material_doc[] =
R"(    devsim.get_material (device, region, contact)

    Returns the material for the specified region

    Parameters
    ----------
    device : str, optional
       The selected device
    region : str, optional
       The selected region
    contact : str, optional
       Contact on which to apply this command
)";

static const char get_parameter_doc[] =
R"(    devsim.get_parameter (device, region, name)

    Get a parameter on a region, device, or globally.

    Parameters
    ----------
    device : str, optional
       The selected device
    region : str, optional
       The selected region
    name : str
       Name of the parameter name being retrieved

    Notes
    -----

    Note that the ``device`` and ``region`` options are optional.  If the region is not specified, the parameter is retrieved for the entire device.  If the device is not specified, the parameter is retrieved for all devices.  If the parameter is not found on the region, it is retrieved on the device.  If it is not found on the device, it is retrieved over all devices.
)";

static const char get_parameter_list_doc[] =
R"(    devsim.get_parameter_list (device, region)

    Get list of parameter names on region, device, or globally

    Parameters
    ----------
    device : str, optional
       The selected device
    region : str, optional
       The selected region

    Notes
    -----

    Note that the ``device`` and ``region`` options are optional.  If the region is not specified, the parameter is retrieved for the entire device.  If the device is not specified, the parameter is retrieved for all devices.  Unlike the :meth:`devsim.getParameter`, parameter names on the the device are not retrieved if they do not exist on the region.  Similarly, the parameter names over all devices are not retrieved if they do not exist on the device.
)";

static const char open_db_doc[] =
R"(    devsim.open_db (filename, permissions)

    Open a database storing material properties

    Parameters
    ----------
    filename : str
       filename to create for the db
    permissions : {'readonly', 'readwrite'}
       permissions on the db
)";

static const char save_db_doc[] =
R"(    devsim.save_db ()

    Saves any new or modified db entries to the database file
)";

static const char set_material_doc[] =
R"(    devsim.set_material (device, region, contact, material)

    Sets the new material for a region

    Parameters
    ----------
    device : str, optional
       The selected device
    region : str, optional
       The selected region
    contact : str, optional
       Contact on which to apply this command
    material : str
       New material name
)";

static const char set_parameter_doc[] =
R"(    devsim.set_parameter (device, region, name, value)

    Set a parameter on region, device, or globally

    Parameters
    ----------
    device : str, optional
       The selected device
    region : str, optional
       The selected region
    name : str
       Name of the parameter name being retrieved
    value : any
       value to set for the parameter

    Notes
    -----

    Note that the device and region options are optional.  If the region is not specified, the parameter is set for the entire device.  If the device is not specified, the parameter is set for all devices.
)";

static const char add_1d_contact_doc[] =
R"(    devsim.add_1d_contact (material, mesh, name, tag)

    Add a contact to a 1D mesh

    Parameters
    ----------
    material : str
       material for the contact being created
    mesh : str
       Mesh to add the contact to
    name : str
       Name for the contact being created
    tag : str
       Text label for the position to add the contact
)";

static const char add_1d_interface_doc[] =
R"(    devsim.add_1d_interface (mesh, tag, name)

    Add an interface to a 1D mesh

    Parameters
    ----------
    mesh : str
       Mesh to add the interface to
    tag : str
       Text label for the position to add the interface
    name : str
       Name for the interface being created
)";

static const char add_1d_mesh_line_doc[] =
R"(    devsim.add_1d_mesh_line (mesh, tag, pos, ns, ps)

    Add a mesh line to a 1D mesh

    Parameters
    ----------
    mesh : str
       Mesh to add the line to
    tag : str, optional
       Text label for the position
    pos : str
       Position for the mesh point
    ns : Float, optional
       Spacing from this point in the negative direction (default ps value)
    ps : Float
       Spacing from this point in the positive direction
)";

static const char add_1d_region_doc[] =
R"(    devsim.add_1d_region (mesh, tag1, tag2, region, material)

    Add a region to a 1D mesh

    Parameters
    ----------
    mesh : str
       Mesh to add the line to
    tag1 : str
       Text label for the position bounding the region being added
    tag2 : str
       Text label for the position bounding the region being added
    region : str
       Name for the region being created
    material : str
       Material for the region being created
)";

static const char add_2d_contact_doc[] =
R"(    devsim.add_2d_contact (name, material, mesh, region, xl, xh, yl, yh, bloat)

    Add an interface to a 2D mesh

    Parameters
    ----------
    name : str
       Name for the contact being created
    material : str
       material for the contact being created
    mesh : str
       Mesh to add the contact to
    region : str
       Name of the region included in the contact
    xl : Float, optional
       x position for corner of bounding box (default -MAXDOUBLE)
    xh : Float, optional
       x position for corner of bounding box (default +MAXDOUBLE)
    yl : Float, optional
       y position for corner of bounding box (default -MAXDOUBLE)
    yh : Float, optional
       y position for corner of bounding box (default +MAXDOUBLE)
    bloat : Float, optional
       Extend bounding box by this amount when search for mesh to include in region (default 1e-10)
)";

static const char add_2d_interface_doc[] =
R"(    devsim.add_2d_interface (mesh, name, region0, region1, xl, xh, yl, yh, bloat)

    Add an interface to a 2D mesh

    Parameters
    ----------
    mesh : str
       Mesh to add the interface to
    name : str
       Name for the interface being created
    region0 : str
       Name of the region included in the interface
    region1 : str
       Name of the region included in the interface
    xl : Float, optional
       x position for corner of bounding box (default -MAXDOUBLE)
    xh : Float, optional
       x position for corner of bounding box (default +MAXDOUBLE)
    yl : Float, optional
       y position for corner of bounding box (default -MAXDOUBLE)
    yh : Float, optional
       y position for corner of bounding box (default +MAXDOUBLE)
    bloat : Float, optional
       Extend bounding box by this amount when search for mesh to include in region (default 1e-10)
)";

static const char add_2d_mesh_line_doc[] =
R"(    devsim.add_2d_mesh_line (mesh, pos, ns, ps)

    Add a mesh line to a 2D mesh

    Parameters
    ----------
    mesh : str
       Mesh to add the line to
    pos : str
       Position for the mesh point
    ns : Float
       Spacing from this point in the negative direction
    ps : Float
       Spacing from this point in the positive direction
)";

static const char add_2d_region_doc[] =
R"(    devsim.add_2d_region (mesh, region, material, xl, xh, yl, yh, bloat)

    Add a region to a 2D mesh

    Parameters
    ----------
    mesh : str
       Mesh to add the region to
    region : str
       Name for the region being created
    material : str
       Material for the region being created
    xl : Float, optional
       x position for corner of bounding box (default -MAXDOUBLE)
    xh : Float, optional
       x position for corner of bounding box (default +MAXDOUBLE)
    yl : Float, optional
       y position for corner of bounding box (default -MAXDOUBLE)
    yh : Float, optional
       y position for corner of bounding box (default +MAXDOUBLE)
    bloat : Float, optional
       Extend bounding box by this amount when search for mesh to include in region (default 1e-10)
)";

static const char add_gmsh_contact_doc[] =
R"(    devsim.add_gmsh_contact (gmsh_name, material, mesh, name, region)

    Create a mesh to import a Gmsh mesh

    Parameters
    ----------
    gmsh_name : str
       physical group name in the Gmsh file
    material : str
       material for the contact being created
    mesh : str
       name of the mesh being generated
    name : str
       name of the contact begin created
    region : str
       region that the contact is attached to
)";

static const char add_gmsh_interface_doc[] =
R"(    devsim.add_gmsh_interface (gmsh_name, mesh, name, region0, region1)

    Create an interface for an imported Gmsh mesh

    Parameters
    ----------
    gmsh_name : str
       physical group name in the Gmsh file
    mesh : str
       name of the mesh being generated
    name : str
       name of the interface begin created
    region0 : str
       first region that the interface is attached to
    region1 : str
       second region that the interface is attached to
)";

static const char add_gmsh_region_doc[] =
R"(    devsim.add_gmsh_region (gmsh_name, mesh, region, material)

    Create a region for an imported Gmsh mesh

    Parameters
    ----------
    gmsh_name : str
       physical group name in the Gmsh file
    mesh : str
       name of the mesh being generated
    region : str
       name of the region begin created
    material : str
       material for the region being created
)";

static const char create_1d_mesh_doc[] =
R"(    devsim.create_1d_mesh (mesh)

    Create a mesh to create a 1D device

    Parameters
    ----------
    mesh : str
       name of the 1D mesh being created
)";

static const char create_2d_mesh_doc[] =
R"(    devsim.create_2d_mesh (mesh)

    Create a mesh to create a 2D device

    Parameters
    ----------
    mesh : str
       name of the 2D mesh being created
)";

static const char create_contact_from_interface_doc[] =
R"(    devsim.create_contact_from_interface (device, region, interface, material, name)

    Creates a contact on a device from an existing interface

    Parameters
    ----------
    device : str
       The selected device
    region : str
       The selected region
    interface : str
       Interface on which to apply this command
    material : str
       material for the contact being created
    name : str
       name of the contact begin created
)";

static const char create_device_doc[] =
R"(    devsim.create_device (mesh, device)

    Create a device from a mesh

    Parameters
    ----------
    mesh : str
       name of the mesh being used to create a device
    device : str
       name of the device being created
)";

static const char create_gmsh_mesh_doc[] =
R"(    devsim.create_gmsh_mesh (mesh, file, coordinates, elements, physical_names)

    Create a mesh to import a Gmsh mesh

    Parameters
    ----------
    mesh : str
       name of the mesh being generated
    file : str, optional
       name of the Gmsh mesh file being read into DEVSIM
    coordinates : list, optional
       List of coordinate positions on mesh.
    elements : list, optional
       List of elements on the mesh.
    physical_names : list, optional
       List of names for each contact, interface, and region on mesh.

    Notes
    -----

    This file will import a Gmsh format mesh from a file.  Alternatively, the mesh structure may be passed in as as arguments:

    ``coordinates`` is a float list of positions in the mesh.  Each coordinate adds an x, y, and z position so that the coordinate list length is 3 times the number of coordinates.

    ``physical_names`` is a list of contact, interface, and region names.  It is referenced by index by the ``elements`` list.

    ``elements`` is a list of elements.  Each element adds

    * Element Type (float)

      - 0 node
      - 1 edge
      - 2 triangle
      - 3 tetrahedron

    * Physical Index

      - This indexes into the ``physical_names`` list.

    * Nodes

      - Each node of the element indexes into the coordinates list.

)";

static const char create_interface_from_nodes_doc[] =
R"(    devsim.create_interface_from_nodes (device, name, region0, region1, nodes0, nodes1)

    Creates an interface from lists of nodes

    Parameters
    ----------
    device : str
       The selected device
    name : str
       name of the interface begin created
    region0 : str
       first region that the interface is attached to
    region1 : str
       second region that the interface is attached to
    nodes0 : str
       list of nodes for the interface in the first region
    nodes1 : str
       list of nodes for the interface in the second region
)";

static const char delete_device_doc[] =
R"(    devsim.delete_device (device)

    Delete a device and its parameters

    Parameters
    ----------
    device : str
       name of the device being deleted
)";

static const char delete_mesh_doc[] =
R"(    devsim.delete_mesh (mesh)

    Delete a mesh so devices can no longer be instantiated from it.

    Parameters
    ----------
    mesh : str
       Mesh to delete
)";

static const char finalize_mesh_doc[] =
R"(    devsim.finalize_mesh (mesh)

    Finalize a mesh so no additional mesh specifications can be added and devices can be created.

    Parameters
    ----------
    mesh : str
       Mesh to finalize
)";

static const char get_mesh_list_doc[] =
R"(    devsim.get_mesh_list ()

    Get list of meshes
)";

static const char load_devices_doc[] =
R"(    devsim.load_devices (file)

    Load devices from a DEVSIM file

    Parameters
    ----------
    file : str
       name of the file to load the meshes from
)";

static const char write_devices_doc[] =
R"(    devsim.write_devices (file, device, type)

    Write a device to a file for visualization or restart

    Parameters
    ----------
    file : str
       name of the file to write the meshes to
    device : str, optional
       name of the device to write
    type : {'devsim', 'devsim_data', 'floops', 'tecplot', 'vtk'}
       format to use
)";

static const char contact_edge_model_doc[] =
R"(    devsim.contact_edge_model (device, contact, name, equation, display_type)

    Create an edge model evaluated at a contact

    Parameters
    ----------
    device : str
       The selected device
    contact : str
       Contact on which to apply this command
    name : str
       Name of the contact edge model being created
    equation : str
       Equation used to describe the contact edge model being created
    display_type : {'vector', 'nodisplay', 'scalar'}
       Option for output display in graphical viewer
)";

static const char contact_node_model_doc[] =
R"(    devsim.contact_node_model (device, contact, name, equation, display_type)

    Create an node model evaluated at a contact

    Parameters
    ----------
    device : str
       The selected device
    contact : str
       Contact on which to apply this command
    name : str
       Name of the contact node model being created
    equation : str
       Equation used to describe the contact node model being created
    display_type : {'scalar', 'nodisplay'}
       Option for output display in graphical viewer
)";

static const char cylindrical_edge_couple_doc[] =
R"(    devsim.cylindrical_edge_couple (device, region)

    This command creates the ``EdgeCouple`` model for 2D cylindrical simulation

    Parameters
    ----------
    device : str
       The selected device
    region : str
       The selected region

    Notes
    -----

    This model is only available in 2D.  The created variables are

    - ``ElementCylindricalEdgeCouple`` (Element Edge Model)
    - ``CylindricalEdgeCouple`` (Edge Model)

    The :meth:`devsim.set_parameter` must be used to set

    - ``raxis_variable``, the variable (``x`` or ``y``) which is the radial axis variable in the cylindrical coordinate system
    - ``raxis_zero``, the location of the z axis for the radial axis variable
)";

static const char cylindrical_node_volume_doc[] =
R"(    devsim.cylindrical_node_volume (device, region)

    This command creates the ``NodeVolume`` model for 2D cylindrical simulation

    Parameters
    ----------
    device : str
       The selected device
    region : str
       The selected region

    Notes
    -----

    This model is only available in 2D.  The created variables are

    - ``ElementCylindricalNodeVolume@en0`` (Element Edge Model)
    - ``ElementCylindricalNodeVolume@en1`` (Element Edge Model)
    - ``CylindricalEdgeNodeVolume@n0`` (Edge Model)
    - ``CylindricalEdgeNodeVolume@n1`` (Edge Model)
    - ``CylindricalNodeVolume`` (Node Model)

    The ``ElementCylindricalNodeVolume@en0`` and ``ElementCylindricalNodeVolume@en1`` represent the node volume at each end of the element edge.

    The :meth:`devsim.set_parameter` must be used to set

    - ``raxis_variable``, the variable (``x`` or ``y``) which is the radial axis variable in the cylindrical coordinate system
    - ``raxis_zero``, the location of the z axis for the radial axis variable

)";

static const char cylindrical_surface_area_doc[] =
R"(    devsim.cylindrical_surface_area (device, region)

    This command creates the ``SurfaceArea`` model for 2D cylindrical simulation

    Parameters
    ----------
    device : str
       The selected device
    region : str
       The selected region

    Notes
    -----

    This model is only available in 2D.  The created variables are

    - ``CylindricalSurfaceArea`` (Node Model)

    and is the cylindrical surface area along each contact and interface node in the device region.

    The :meth:`devsim.set_parameter` must be used to set

    - ``raxis_variable``, the variable (``x`` or ``y``) which is the radial axis variable in the cylindrical coordinate system
    - ``raxis_zero``, the location of the z axis for the radial axis variable

)";

static const char debug_triangle_models_doc[] =
R"(    devsim.debug_triangle_models (device, region)

    Debugging command used in the development of DEVSIM and used in regressions.

    Parameters
    ----------
    device : str
       The selected device
    region : str
       The selected region
)";

static const char delete_edge_model_doc[] =
R"(    devsim.delete_edge_model (device, region, name)

    Deletes an edge model from a region

    Parameters
    ----------
    device : str
       The selected device
    region : str
       The selected region
    name : str
       Name of the edge model being deleted
)";

static const char delete_element_model_doc[] =
R"(    devsim.delete_element_model (device, region, name)

    Deletes a element model from a region

    Parameters
    ----------
    device : str
       The selected device
    region : str
       The selected region
    name : str
       Name of the node model being deleted
)";

static const char delete_interface_model_doc[] =
R"(    devsim.delete_interface_model (device, interface, name)

    Deletes an interface model from an interface

    Parameters
    ----------
    device : str
       The selected device
    interface : str
       Interface on which to apply this command
    name : str
       Name of the interface model being deleted
)";

static const char delete_node_model_doc[] =
R"(    devsim.delete_node_model (device, region, name)

    Deletes a node model from a region

    Parameters
    ----------
    device : str
       The selected device
    region : str
       The selected region
    name : str
       Name of the node model being deleted
)";

static const char edge_average_model_doc[] =
R"(    devsim.edge_average_model (device, region, node_model, edge_model, derivative, average_type)

    Creates an edge model based on the node model values

    Parameters
    ----------
    device : str
       The selected device
    region : str
       The selected region
    node_model : str
       The node model from which we are creating the edge model.  If ``derivative`` is specified, the edge model is created from ``nodeModel:derivativeModel``
    edge_model : str
       The edge model name being created.  If ``derivative`` is specified, the edge models created are ``edgeModel:derivativeModel@n0`` ``edgeModel:derivativeModel@n1``, which are the derivatives with respect to the derivative model on each side of the edge
    derivative : str, optional
       The node model of the variable for which the derivative is being taken.  The node model ``nodeModel:derivativeModel`` is used to create the resulting edge models.
    average_type : {'arithmetic', 'geometric', 'gradient', 'negative_gradient'}
       The node models on both sides of the edge are averaged together to create one of the following types of averages.

    Notes
    -----

    For a node model, creates 2 edge models referring to the node model value at both ends of the edge.  For example, to calculate electric field:

    ..

      devsim.edge_average_model(device=device, region=region, node_model="Potential", edge_model="ElecticField", average_type="negative_gradient")

    and the derivatives ``ElectricField:Potential@n0`` and ``ElectricField:Potential@n1`` are then created from

    ..

      devsim.edge_average_model(device=device, region=region, node_model="Potential", edge_model="ElecticField", average_type="negative_gradient", derivative="Potential")
)";

static const char edge_from_node_model_doc[] =
R"(    devsim.edge_from_node_model (device, region, node_model)

    For a node model, creates an 2 edge models referring to the node model value at both ends of the edge.

    Parameters
    ----------
    device : str
       The selected device
    region : str
       The selected region
    node_model : str
       The node model from which we are creating the edge model

    Notes
    -----

    For example, to calculate electric field:

    ..

      devsim.edge_from_node_model(device=device, region=region, node_model="Potential")

)";

static const char edge_model_doc[] =
R"(    devsim.edge_model (device, region, name, equation, display_type)

    Creates an edge model based on an equation

    Parameters
    ----------
    device : str
       The selected device
    region : str
       The selected region
    name : str
       Name of the edge model being created
    equation : str
       Equation used to describe the edge model being created
    display_type : {'scalar', 'nodisplay', 'vector'}
       Option for output display in graphical viewer

    Notes
    -----

    The ``vector`` option uses an averaging scheme for the edge values projected in the direction of each edge.  For a given model, ``model``, the generated components in the visualization files is:

    - ``model_x_onNode``
    - ``model_y_onNode``
    - ``model_z_onNode`` (3D)

    This averaging scheme does not produce accurate results, and it is recommended to use the :meth:`devsim.element_from_edge_model` to create components better suited for visualization.  See :ref:`ch__visualization` for more information about creating data files for external visualization programs.
)";

static const char edge_solution_doc[] =
R"(    devsim.edge_solution (device, region, name)

    Create node model whose values are set.

    Parameters
    ----------
    device : str
       The selected device
    region : str
       The selected region
    name : str
       Name of the solution being created
)";

static const char element_from_edge_model_doc[] =
R"(    devsim.element_from_edge_model (device, region, edge_model, derivative)

    Creates element edge models from an edge model

    Parameters
    ----------
    device : str
       The selected device
    region : str
       The selected region
    edge_model : str
       The edge model from which we are creating the element model
    derivative : str, optional
       The variable we are taking with respect to edge_model

    Notes
    -----

    For an edge model ``emodel``, creates an element models referring to the directional components on each edge of the element:

    - ``emodel_x``
    - ``emodel_y``

    If the ``derivative`` ``variable`` option is specified, the ``emodel@n0`` and ``emodel@n1`` are used to create:

    - ``emodel_x:variable@en0``
    - ``emodel_y:variable@en0``
    - ``emodel_x:variable@en1``
    - ``emodel_y:variable@en1``
    - ``emodel_x:variable@en2``
    - ``emodel_y:variable@en2``

    in 2D for each node on a triangular element. and

    - ``emodel_x:variable@en0``
    - ``emodel_y:variable@en0``
    - ``emodel_z:variable@en0``
    - ``emodel_x:variable@en1``
    - ``emodel_y:variable@en1``
    - ``emodel_z:variable@en1``
    - ``emodel_x:variable@en2``
    - ``emodel_y:variable@en2``
    - ``emodel_z:variable@en2``
    - ``emodel_x:variable@en3``
    - ``emodel_y:variable@en3``
    - ``emodel_z:variable@en3``

    in 3D for each node on a tetrahedral element.

    The suffix ``en0`` refers to the first node on the edge of the element and ``en1`` refers to the second node.  ``en2`` and ``en3`` specifies the derivatives with respect the variable at the nodes opposite the edges on the element being considered.
)";

static const char element_from_node_model_doc[] =
R"(    devsim.element_from_node_model (device, region, node_model)

    Creates element edge models from a node model

    Parameters
    ----------
    device : str
       The selected device
    region : str
       The selected region
    node_model : str
       The node model from which we are creating the edge model

    Notes
    -----

    This command creates an element edge model from a node model so that each corner of the element is represented.  A node model, ``nmodel``, would be be accessible as

    - ``nmodel@en0``
    - ``nmodel@en1``
    - ``nmodel@en2``
    - ``nmodel@en3`` (3D)

    where ``en0``, and ``en1`` refers to the nodes on the element's edge.  In 2D, ``en2`` refers to the node on the triangle node opposite the edge.  In 3D, ``en2`` and ``en3`` refers to the nodes on the nodes off the element edge on the tetrahedral element.
)";

static const char element_model_doc[] =
R"(    devsim.element_model (device, region, name, equation, display_type)

    Create a model evaluated on element edges.

    Parameters
    ----------
    device : str
       The selected device
    region : str
       The selected region
    name : str
       Name of the element edge model being created
    equation : str
       Equation used to describe the element edge model being created
    display_type : {'scalar', 'nodisplay'}
       Option for output display in graphical viewer
)";

static const char element_pair_from_edge_model_doc[] =
R"(    devsim.element_pair_from_edge_model (device, region, edge_model, derivative)

    Creates element edge models from an edge model

    Parameters
    ----------
    device : str
       The selected device
    region : str
       The selected region
    edge_model : str
       The edge model from which we are creating the element model
    derivative : str, optional
       The variable we are taking with respect to edge_model

    Notes
    -----

    For an edge model ``emodel``, creates an element models referring to the directional components on each edge of the element:

    - ``emodel_node0_x``
    - ``emodel_node0_y``
    - ``emodel_node1_x``
    - ``emodel_node1_y``

    If the ``derivative`` ``variable`` option is specified, the ``emodel@n0`` and ``emodel@n1`` are used to create:

    - ``emodel_node0_x:variable@en0``
    - ``emodel_node0_y:variable@en0``
    - ``emodel_node0_x:variable@en1``
    - ``emodel_node0_y:variable@en1``
    - ``emodel_node0_x:variable@en2``
    - ``emodel_node0_y:variable@en2``
    - ``emodel_node1_x:variable@en0``
    - ``emodel_node1_y:variable@en0``
    - ``emodel_node1_x:variable@en1``
    - ``emodel_node1_y:variable@en1``
    - ``emodel_node1_x:variable@en2``
    - ``emodel_node1_y:variable@en2``

    in 2D for each node on a triangular element. and

    - ``emodel_node0_x:variable@en0``
    - ``emodel_node0_y:variable@en0``
    - ``emodel_node0_z:variable@en0``
    - ``emodel_node0_x:variable@en1``
    - ``emodel_node0_y:variable@en1``
    - ``emodel_node0_z:variable@en1``
    - ``emodel_node0_x:variable@en2``
    - ``emodel_node0_y:variable@en2``
    - ``emodel_node0_z:variable@en2``
    - ``emodel_node0_x:variable@en3``
    - ``emodel_node0_y:variable@en3``
    - ``emodel_node0_z:variable@en3``
    - ``emodel_node1_x:variable@en0``
    - ``emodel_node1_y:variable@en0``
    - ``emodel_node1_z:variable@en0``
    - ``emodel_node1_x:variable@en1``
    - ``emodel_node1_y:variable@en1``
    - ``emodel_node1_z:variable@en1``
    - ``emodel_node1_x:variable@en2``
    - ``emodel_node1_y:variable@en2``
    - ``emodel_node1_z:variable@en2``
    - ``emodel_node1_x:variable@en3``
    - ``emodel_node1_y:variable@en3``
    - ``emodel_node1_z:variable@en3``

    in 3D for each node on a tetrahedral element.

    The label ``node0`` and ``node1`` refer to the node on the edge for which the element field average was performed.  For example, ``node0`` signifies that all edges connected to ``node0`` where used to calculate the element field.

    The suffix ``en0`` refers to the first node on the edge of the element and ``en1`` refers to the second node.  ``en2`` and ``en3`` specifies the derivatives with respect the variable at the nodes opposite the edges on the element being considered.
)";

static const char element_solution_doc[] =
R"(    devsim.element_solution (device, region, name)

    Create node model whose values are set.

    Parameters
    ----------
    device : str
       The selected device
    region : str
       The selected region
    name : str
       Name of the solution being created
)";

static const char get_edge_model_list_doc[] =
R"(    devsim.get_edge_model_list (device, region)

    Returns a list of the edge models on the device region

    Parameters
    ----------
    device : str
       The selected device
    region : str
       The selected region
)";

static const char get_edge_model_values_doc[] =
R"(    devsim.get_edge_model_values (device, region, name)

    Get the edge model values calculated at each edge.

    Parameters
    ----------
    device : str
       The selected device
    region : str
       The selected region
    name : str
       Name of the edge model values being returned as a list
)";

static const char get_element_model_list_doc[] =
R"(    devsim.get_element_model_list (device, region)

    Returns a list of the element edge models on the device region

    Parameters
    ----------
    device : str
       The selected device
    region : str
       The selected region
)";

static const char get_element_model_values_doc[] =
R"(    devsim.get_element_model_values (device, region, name)

    Get element model values at each element edge

    Parameters
    ----------
    device : str
       The selected device
    region : str
       The selected region
    name : str
       Name of the element edge model values being returned as a list
)";

static const char get_interface_model_list_doc[] =
R"(    devsim.get_interface_model_list (device, interface)

    Returns a list of the interface models on the interface

    Parameters
    ----------
    device : str
       The selected device
    interface : str
       Interface on which to apply this command
)";

static const char get_interface_model_values_doc[] =
R"(    devsim.get_interface_model_values (device, interface, name)

    Gets interface model values evaluated at each interface node.

    Parameters
    ----------
    device : str
       The selected device
    interface : str
       Interface on which to apply this command
    name : str
       Name of the interface model values being returned as a list
)";

static const char get_node_model_list_doc[] =
R"(    devsim.get_node_model_list (device, region)

    Returns a list of the node models on the device region

    Parameters
    ----------
    device : str
       The selected device
    region : str
       The selected region
)";

static const char get_node_model_values_doc[] =
R"(    devsim.get_node_model_values (device, region, name)

    Get node model values evaluated at each node in a region.

    Parameters
    ----------
    device : str
       The selected device
    region : str
       The selected region
    name : str
       Name of the node model values being returned as a list
)";

static const char interface_model_doc[] =
R"(    devsim.interface_model (device, interface, equation)

    Create an interface model from an equation.

    Parameters
    ----------
    device : str
       The selected device
    interface : str
       Interface on which to apply this command
    equation : str
       Equation used to describe the interface node model being created
)";

static const char interface_normal_model_doc[] =
R"(    devsim.interface_normal_model (device, region, interface)

    Creates edge models whose components are based on direction and distance to an interface

    Parameters
    ----------
    device : str
       The selected device
    region : str
       The selected region
    interface : str
       Interface on which to apply this command

    Notes
    -----

    This model creates the following edge models:

    - ``iname_distance``
    - ``iname_normal_x`` (2D and 3D)
    - ``iname_normal_y`` (2D and 3D)
    - ``iname_normal_z`` (3D only)

    where ``iname`` is the name of the interface.  The normals are of the closest node on the interface.  The sign is toward the interface.
)";

static const char node_model_doc[] =
R"(    devsim.node_model (device, region, name, equation, display_type)

    Create a node model from an equation.

    Parameters
    ----------
    device : str
       The selected device
    region : str
       The selected region
    name : str
       Name of the node model being created
    equation : str
       Equation used to describe the node model being created
    display_type : {'scalar', 'nodisplay'}
       Option for output display in graphical viewer
)";

static const char node_solution_doc[] =
R"(    devsim.node_solution (device, region, name)

    Create node model whose values are set.

    Parameters
    ----------
    device : str
       The selected device
    region : str
       The selected region
    name : str
       Name of the solution being created
)";

static const char print_edge_values_doc[] =
R"(    devsim.print_edge_values (device, region, name)

    Print edge values for debugging.

    Parameters
    ----------
    device : str
       The selected device
    region : str
       The selected region
    name : str
       Name of the edge model values being printed to the screen
)";

static const char print_element_values_doc[] =
R"(    devsim.print_element_values (device, region, name)

    Print element values for debugging.

    Parameters
    ----------
    device : str
       The selected device
    region : str
       The selected region
    name : str
       Name of the element edge model values being printed to the screen
)";

static const char print_node_values_doc[] =
R"(    devsim.print_node_values (device, region, name)

    Print node values for debugging.

    Parameters
    ----------
    device : str
       The selected device
    region : str
       The selected region
    name : str
       Name of the node model values being printed to the screen
)";

static const char register_function_doc[] =
R"(    devsim.register_function (name, nargs, procedure)

    This command is used to register a new Python procedure for evaluation by SYMDIFF.

    Parameters
    ----------
    name : str
       Name of the function
    nargs : str
       Number of arguments to the function
    procedure : str
       The procedure to be called
)";

static const char set_edge_values_doc[] =
R"(    devsim.set_edge_values (device, region, name, init_from, values)

    Set edge model values from another edge model, or a list of values.

    Parameters
    ----------
    device : str
       The selected device
    region : str
       The selected region
    name : str
       Name of the edge model being initialized
    init_from : str, optional
       Node model we are using to initialize the edge solution
    values : list, optional
       List of values for each edge in the region.
)";

static const char set_element_values_doc[] =
R"(    devsim.set_element_values (device, region, name, init_from, values)

    Set element model values from another element model, or a list of values.

    Parameters
    ----------
    device : str
       The selected device
    region : str
       The selected region
    name : str
       Name of the element model being initialized
    init_from : str, optional
       Node model we are using to initialize the element solution
    values : list, optional
       List of values for each element in the region.
)";

static const char set_node_value_doc[] =
R"(    devsim.set_node_value (device, region, name, index, value)

    A uniform value is used if index is not specified.  Note that equation based node models will lose this value if their equation is recalculated.

    Parameters
    ----------
    device : str
       The selected device
    region : str
       The selected region
    name : str
       Name of the node model being whose value is being set
    index : int
       Index of node being set
    value : Float
       Value of node being set
)";

static const char set_node_values_doc[] =
R"(    devsim.set_node_values (device, region, name, init_from, values)

    Set node model values from another node model, or a list of values.

    Parameters
    ----------
    device : str
       The selected device
    region : str
       The selected region
    name : str
       Name of the node model being initialized
    init_from : str, optional
       Node model we are using to initialize the node solution
    values : list, optional
       List of values for each node in the region.
)";

static const char symdiff_doc[] =
R"(    devsim.symdiff (expr)

    This command returns an expression.  All strings are treated as independent variables.  It is primarily used for defining new functions to the parser.

    Parameters
    ----------
    expr : str
       Expression to send to SYMDIFF
)";

static const char vector_element_model_doc[] =
R"(    devsim.vector_element_model (device, region, element_model)

    Create vector components from an element edge model

    Parameters
    ----------
    device : str
       The selected device
    region : str
       The selected region
    element_model : str
       The element model for which we are calculating the vector components

    Notes
    -----

    This command creates element edge models from an element model which represent the vector components on the element edge.  An element model, ``emodel``, would then have

    - ``emodel_x``
    - ``emodel_y``
    - ``emodel_z`` (3D only)

    The primary use of these components are for visualization.
)";

static const char vector_gradient_doc[] =
R"(    devsim.vector_gradient (device, region, node_model, calc_type)

    Creates the vector gradient for noise analysis

    Parameters
    ----------
    device : str
       The selected device
    region : str
       The selected region
    node_model : str
       The node model from which we are creating the edge model
    calc_type : {'default', 'avoidzero'}
       The node model from which we are creating the edge model

    Notes
    -----

    Used for noise analysis.  The ``avoidzero`` option is important for noise analysis, since a node model value of zero is not physical for some contact and interface boundary conditions.  For a given node model, ``model``, a node model is created in each direction:

    - ``model_gradx`` (1D)
    - ``model_grady`` (2D and 3D)
    - ``model_gradz`` (3D)

    It is important not to use these models for simulation, since DEVSIM, does not have a way of evaluating the derivatives of these models.  The models can be used for integrating the impedance field, and other postprocessing.  The :meth:`devsim.element_from_edge_model` command can be used to create gradients for use in a simulation.
)";

static const char get_contact_charge_doc[] =
R"(    devsim.get_contact_charge (device, contact, equation)

    Get charge at the contact

    Parameters
    ----------
    device : str
       The selected device
    contact : str
       Contact on which to apply this command
    equation : str
       Name of the contact equation from which we are retrieving the charge
)";

static const char get_contact_current_doc[] =
R"(    devsim.get_contact_current (device, contact, equation)

    Get current at the contact

    Parameters
    ----------
    device : str
       The selected device
    contact : str
       Contact on which to apply this command
    equation : str
       Name of the contact equation from which we are retrieving the current
)";

static const char get_matrix_and_rhs_doc[] =
R"(    devsim.get_matrix_and_rhs (format)

    Returns matrices and rhs vectors.

    Parameters
    ----------
    format : {'csc', 'csr'} required
       Option for returned matrix format.
)";

static const char set_initial_condition_doc[] =
R"(    devsim.set_initial_condition (static_rhs, dynamic_rhs)

    Sets the initial condition for subsequent transient solver steps.

    Parameters
    ----------
    static_rhs : list, optional
       List of double values for non time-displacement terms in right hand side.
    dynamic_rhs : list, optional
       List of double values for time-displacement terms in right hand side.
)";

static const char solve_doc[] =
R"(    devsim.solve (type, solver_type, absolute_error, relative_error, maximum_error, charge_error, gamma, tdelta, maximum_iterations, maximum_divergence, frequency, output_node, info, symbolic_iteration_limit)

    Call the solver.  A small-signal AC source is set with the circuit voltage source.

    Parameters
    ----------
    type : {'dc', 'ac', 'noise', 'transient_dc', 'transient_bdf1', 'transient_bdf2', 'transient_tr'} required
       type of solve being performed
    solver_type : {'direct', 'iterative'} required
       Linear solver type
    absolute_error : Float, optional
       Required update norm in the solve (default 0.0)
    relative_error : Float, optional
       Required relative update in the solve (default 0.0)
    maximum_error : Float, optional
       Maximum absolute error before solve stops (default MAXDOUBLE)
    charge_error : Float, optional
       Relative error between projected and solved charge during transient simulation (default 0.0)
    gamma : Float, optional
       Scaling factor for transient time step (default 1.0)
    tdelta : Float, optional
       time step (default 0.0)
    maximum_iterations : int, optional
       Maximum number of iterations in the DC solve (default 20)
    maximum_divergence : int, optional
       Maximum number of diverging iterations during solve (default 20)
    frequency : Float, optional
       Frequency for small-signal AC simulation (default 0.0)
    output_node : str, optional
       Output circuit node for noise simulation
    info : bool, optional
       Solve command return convergence information (default False)
    symbolic_iteration_limit : int, optional
       Reuse symbolic matrix factorization after this number of iterations (default 1)
)";
