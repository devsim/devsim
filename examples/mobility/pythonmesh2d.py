from devsim import *
import devsim.python_packages.pythonmesh

def gmsh_reader(mesh, device, filename="", coordinates="", physical_names="", elements=""):
    if (filename):
        create_gmsh_mesh(file=filename, mesh=mesh)
    else:
        create_gmsh_mesh(mesh=mesh, coordinates=coordinates, physical_names=physical_names, elements=elements)
    add_gmsh_region    (mesh=mesh, gmsh_name="bulk",    region="bulk", material="Silicon")
    add_gmsh_region    (mesh=mesh, gmsh_name="oxide",    region="oxide", material="Silicon")
    add_gmsh_region    (mesh=mesh, gmsh_name="gate",    region="gate", material="Silicon")
    add_gmsh_contact   (mesh=mesh, gmsh_name="drain_contact",  region="bulk", name="drain", material="metal")
    add_gmsh_contact   (mesh=mesh, gmsh_name="source_contact", region="bulk", name="source", material="metal")
    add_gmsh_contact   (mesh=mesh, gmsh_name="body_contact",   region="bulk", name="body", material="metal")
    add_gmsh_contact   (mesh=mesh, gmsh_name="gate_contact",   region="gate", name="gate", material="metal")
    add_gmsh_interface (mesh=mesh, gmsh_name="gate_oxide_interface", region0="gate", region1="oxide", name="gate_oxide")
    add_gmsh_interface (mesh=mesh, gmsh_name="bulk_oxide_interface", region0="bulk", region1="oxide", name="bulk_oxide")
    finalize_mesh(mesh=mesh)
    create_device(mesh=mesh, device=device)

data = devsim.python_packages.pythonmesh.read_gmsh_file("gmsh_mos2d.msh")
coordinates=data['coordinates']
elements=data['elements']
physical_names=data['physical_names']

gmsh_reader(mesh="mos2da", device="mos2da", coordinates=coordinates, physical_names=physical_names, elements=elements)
gmsh_reader(mesh="mos2db", device="mos2db", filename="gmsh_mos2d.msh")

write_devices(device="mos2da", file="mos2da.msh")
write_devices(device="mos2db", file="mos2db.msh")

with open('mos2da.msh', 'r') as f:
    file1 = f.read()
with open('mos2db.msh', 'r') as f:
    file2 = f.read()

file2=file2.replace('mos2db', 'mos2da')

if (file1 != file2):
    raise RuntimeError('mos2da.msh and mos2db.msh do not match')



