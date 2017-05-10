import numpy
def gmsh_reader():
  create_gmsh_mesh(file="gmsh_mos2d.msh", mesh="mos2d")
  add_gmsh_region    (mesh="mos2d", gmsh_name="bulk",    region="bulk", material="Silicon")
  add_gmsh_region    (mesh="mos2d", gmsh_name="oxide",    region="oxide", material="Silicon")
  add_gmsh_region    (mesh="mos2d", gmsh_name="gate",    region="gate", material="Silicon")
  add_gmsh_contact   (mesh="mos2d", gmsh_name="drain_contact",  region="bulk", name="drain", material="metal")
  add_gmsh_contact   (mesh="mos2d", gmsh_name="source_contact", region="bulk", name="source", material="metal")
  add_gmsh_contact   (mesh="mos2d", gmsh_name="body_contact",   region="bulk", name="body", material="metal")
  add_gmsh_contact   (mesh="mos2d", gmsh_name="gate_contact",   region="gate", name="gate", material="metal")
  add_gmsh_interface (mesh="mos2d", gmsh_name="gate_oxide_interface", region0="gate", region1="oxide", name="gate_oxide")
  add_gmsh_interface (mesh="mos2d", gmsh_name="bulk_oxide_interface", region0="bulk", region1="oxide", name="bulk_oxide")
  finalize_mesh(mesh="mos2d")
  create_device(mesh="mos2d", device="mos2d")

def split_elements(elements):
  regions = {}
  for idx, i in enumerate(elements):
    element_type = i[0]
    number_of_tags = i[1]
    if number_of_tags != 3:
      raise RuntimeError("Element %d, 3 tags expected and %d were available" % (idx, number_of_tags))
    physical_entity =i[2]
    #geometrical_entity = i[3]
    #number of mesh partitions = i[4]
    element_nodes = [x-1 for x in i[5:]]
    if element_type == 1:
      nexp = 2
    elif element_type == 2:
      nexp = 3
    elif element_type == 4:
      nexp = 4
    else:
      raise RuntimeError("Element %d, element type %d not expected" % (idx, element_type))
    if len(element_nodes) != nexp:
      raise RuntimeError("Element %d, element num nodes %d not expected" % (idx, len(element_nodes)))
    if not physical_entity in regions:
      regions[physical_entity] = []
    regions[physical_entity].append(element_nodes)
  return regions

def python_reader(file):
  sections=set(['$MeshFormat', '$PhysicalNames', '$Nodes', '$Elements'])
  lineno = 0
  section_count = 0
  section_expected = 0
  dimension = 0
  with open(file, 'r') as ih:
    state = 'begin' 
    for line in ih:
      line = line.rstrip()
      lineno += 1
      if state == 'begin':
        if line in sections:
          state = line
        else:
          raise RuntimeError("Error Line %d" % lineno)
      elif line.find('$End') == 0:
        state = 'begin'
        section_count = 0
        section_expected = None
      elif state == '$MeshFormat':
        continue
      elif state == '$PhysicalNames':
        if not section_expected:
          section_expected=int(line)
          physical_names = {}
        else:
          line = line.split()
          name = line[2][1:-1]
          physical_names[int(line[1])] = (name, int(line[0]))
          section_count += 1
      elif state == '$Nodes':
        if not section_expected:
          section_expected=int(line)
          nodes = numpy.array(numpy.zeros((section_expected, 3)))
        else:
          line = line.split()
          if int(line[0])-1 != section_count:
            raise RuntimeError("Node count %d" % section_count)
          nodes[section_count,:] = numpy.array(line[1:])
          section_count += 1
      elif state == '$Elements':
        if not section_expected:
          section_expected=int(line)
          elements = [None]*section_expected
        else:
          line = [int(x) for x in line.split()]
          if line[0]-1 != section_count:
            raise RuntimeError("Element count %d" % section_count)
          elements[section_count] = line[1:]
          section_count += 1 
      else:
        raise RuntimeError("Unknown %s" % line)
  dimension = max([physical_names[x][1] for x in physical_names.keys()])
  return {
    'dimension'   : dimension,
    'names'       : physical_names,
    'coordinates' : nodes,
    'elements'    : split_elements(elements)
  }           

data = python_reader('gmsh_mos2d.msh')
namemap = {}
print '%d dimension' % data['dimension']
print '%d coordinates' % len(data['coordinates'])
for i in sorted(data['names'].keys()):
  print "%s %s %d" % (i, data['names'][i], len(data['elements'][i]))
  namemap[data['names'][i]] = i

### TODO: only deal in flattened arrays
### TODO: Seriously consider just using text format to enter data (Extended precision bonus)
### TODO: Seriously consider allowing parser to read entire file as string
#create_general_mesh(mesh="mos2d", dimension=data['dimension'])
#  add_general_coordinates(mesh="mos2d", data['coordinates'])
#  for i in ('bulk', 'gate'):
#    add_general_region(mesh="mos2d", region=i, material='Silicon', elements=data['elements'][namemap[i])
#  add_general_region(mesh="mos2d", region='gate', material='Oxide', elements=data['elements'][namemap['gate'])
#  for i in ('drain', 'source', 'body'):
#    cname = i + "_contact"
#    add_general_contact(mesh="mos2d", name=cname,  region=i, material="metal", elements=data['elements'][namemap[cname]])
#  i = 'gate'
#  cname = i + "_contact"
#  add_general_contact(mesh="mos2d", name=cname,  region=i, material="metal", elements=data['elements'][namemap[cname]])
#  add_general_interface (mesh="mos2d", elements=data['elements'[namemap["gate_oxide_interface"]]], region0="gate", region1="oxide", name="gate_oxide")
#  add_general_interface (mesh="mos2d", elements=data['elements'[namemap["bulk_oxide_interface"]]], region0="bulk", region1="oxide", name="bulk_oxide")
#  finalize_mesh(mesh="mos2d")
#  create_device(mesh="mos2d", device="mos2d")
#
