
from devsim import *
import itertools

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

class MeshWriter:
    def __init__(self, device):
        # gets the mesh input for existing devices
        self.device = device
        self.physical_names = []
        self.elements = []
        self.coordinates = []
        self.coordinates_xyz = []
        self.region_to_coordinate_indexes = {}
        self.name_to_physical_index = {}


    def get_element_list(self, shapes, physical_index, region_coordinate_indexes):
        # Element Type (float)
        #     0 node
        #     1 edge
        #     2 triangle
        #     3 tetrahedron
        element_list = []
        for shape in shapes:
            element_list.append(len(shape)-1)
            element_list.append(physical_index)
            element_list.extend([region_coordinate_indexes[i] for i in shape])
        return element_list



    def get_global_coordinates(self):
        # this is the global list of coordinates

        for r in get_region_list(device=self.device):
          region_coordinates = [int(x) for x in get_node_model_values(device=self.device, region=r, name='coordinate_index')]
          self.region_to_coordinate_indexes[r] = region_coordinates

          # expand global coordinate list as necessary
          clen = max(region_coordinates) + 1
          if clen > len(self.coordinates_xyz):
            self.coordinates_xyz.extend([None] * (clen - len(self.coordinates_xyz)))

          node_positions = zip(get_node_model_values(device=self.device, region=r, name='x'),
                               get_node_model_values(device=self.device, region=r, name='y'),
                               get_node_model_values(device=self.device, region=r, name='z'))

          for i, p in enumerate(node_positions):
              self.coordinates_xyz[region_coordinates[i]] = p

        self.coordinates = list(itertools.chain.from_iterable(self.coordinates_xyz))


    def get_elements(self):
        for r in get_region_list(device=self.device):
            rtc = self.region_to_coordinate_indexes[r]
            physical_index = self.name_to_physical_index[r]
            elist = self.get_element_list(get_element_node_list(device=self.device, region=r), physical_index, rtc)
            self.elements.extend(elist)

        for c in get_contact_list(device=self.device):
            r = get_region_list(device=self.device, contact=c)[0]
            rtc = self.region_to_coordinate_indexes[r]
            physical_index = self.name_to_physical_index[c]
            elist = self.get_element_list(get_element_node_list(device=self.device, region=r, contact=c), physical_index, rtc)
            self.elements.extend(elist)

        # warning, this will not work for the special periodic boundary condition
        for i in get_interface_list(device=self.device):
            elists = []
            for r in get_region_list(device=self.device, interface=i):
                rtc = self.region_to_coordinate_indexes[r]
                physical_index = self.name_to_physical_index[i]
                elist = self.get_element_list(get_element_node_list(device=self.device, region=r, interface=i), physical_index, rtc)
                elists.append(elist)
            if elists[0] != elists[1]:
                raise RuntimeError(f"Error processing interface {i}.  If you are using the periodic interface boundary condition, then please contact support for modifying script.")
            self.elements.extend(elists[0])


    def populate_physical_names(self):

        def add_name(name):
            if name in self.name_to_physical_index:
                raise RuntimeError(f"Cannot use name {name} twice")
            self.physical_names.append(name)
            self.name_to_physical_index[name] = len(self.physical_names) - 1

        for r in get_region_list(device=self.device):
            add_name(r)

        for c in get_contact_list(device=self.device):
            add_name(c)

        for i in get_interface_list(device=self.device):
            add_name(i)

    def process_mesh(self):
        self.populate_physical_names()
        self.get_global_coordinates()
        self.get_elements()


mw = MeshWriter(device="toy")
mw.process_mesh()
print(mw.physical_names)
print(mw.coordinates)
print(mw.elements)
