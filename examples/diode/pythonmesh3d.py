from devsim import *
import devsim.python_packages.pythonmesh


def gmsh_reader(mesh, device, filename="", coordinates="", physical_names="", elements=""):
    if (filename):
        create_gmsh_mesh(mesh=mesh, file=filename)
    else:
        create_gmsh_mesh(mesh=mesh, coordinates=coordinates, physical_names=physical_names, elements=elements)
    add_gmsh_region  (mesh=mesh, gmsh_name="Bulk",    region="bulk", material="Silicon")
    add_gmsh_contact (mesh=mesh, gmsh_name="Base",    region="bulk", material="metal", name="top")
    add_gmsh_contact (mesh=mesh, gmsh_name="Emitter", region="bulk", material="metal", name="bot")
    finalize_mesh    (mesh=mesh)
    create_device    (mesh=mesh, device=device)

data = devsim.python_packages.pythonmesh.read_gmsh_file("gmsh_diode3d.msh")
coordinates=data['coordinates']
elements=data['elements']
physical_names=data['physical_names']

gmsh_reader(mesh="gmsh_diode3da", device="gmsh_diode3da", coordinates=coordinates, physical_names=physical_names, elements=elements)
gmsh_reader(mesh="gmsh_diode3db", device="gmsh_diode3db", filename="gmsh_diode3d.msh")

write_devices(device="gmsh_diode3da", file="gmsh_diode3da.msh")
write_devices(device="gmsh_diode3db", file="gmsh_diode3db.msh")

with open('gmsh_diode3da.msh', 'r') as f:
    file1 = f.read()
with open('gmsh_diode3db.msh', 'r') as f:
    file2 = f.read()

file2=file2.replace('gmsh_diode3db', 'gmsh_diode3da')

if (file1 != file2):
    raise RuntimeError('gmsh_diode3da.msh and gmsh_diode3db.msh do not match')



