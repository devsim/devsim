
from devsim import *

print("coordinates")
coordinates=[]
for i in range(0,11):
    coordinates.extend([float(i), 0.0, 0.0])
print(coordinates)
print()

print("elements")
elements=[]
for i in range(0,5):
    # line type, physical region 0
    x=[1, 0, i, i+1]
    print(x)
    elements.extend(x)
for i in range(5,10):
    # line type, physical region 1
    x=[1, 1, i, i+1]
    print(x)
    elements.extend([1, 1, i, i+1])

#points for boundary conditions
elements.extend([0, 2, 0])
print(elements[-3:])
elements.extend([0, 3, 10])
print(elements[-3:])
elements.extend([0, 4, 5])
print(elements[-3:])
print()

print("physical_names")
physical_names = [
    "top",
  "bot",
  "top_contact",
  "bot_contact",
  "top_bot_interface"
]
print(physical_names)
print()

create_gmsh_mesh(mesh="toy", coordinates=coordinates, physical_names=physical_names, elements=elements)
add_gmsh_region(mesh="toy", gmsh_name="top", region="top", material="silicon")
add_gmsh_region(mesh="toy", gmsh_name="bot", region="bot", material="silicon")
add_gmsh_contact(mesh="toy", gmsh_name="top_contact", name="top_contact", region="top", material="metal")
add_gmsh_contact(mesh="toy", gmsh_name="bot_contact", name="bot_contact", region="bot", material="metal")
add_gmsh_interface(mesh="toy", gmsh_name="top_bot_interface", name="top_bot_interface", region0="top", region1="bot")
finalize_mesh(mesh="toy")
create_device(mesh="toy", device="toy")
write_devices(device="toy",file="pythonmesh1d.msh")

