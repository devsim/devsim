from devsim import *

def parse_gmsh_file(file):
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
                    physical_names = []
                else:
                    line = line.split()
                    name = line[2][1:-1]
                    physical_names.append((name, int(line[0]), int(line[1])))
                    section_count += 1
            elif state == '$Nodes':
                if not section_expected:
                    section_expected=int(line)
                    nodes = []
                else:
                    line = line.split()
                    nodes.append([int(line[0]), float(line[1]), float(line[2]), float(line[3])])
                    section_count += 1
            elif state == '$Elements':
                if not section_expected:
                    section_expected=int(line)
                    elements = [None]*section_expected
                else:
                    elements[section_count] = [int(x) for x in line.split()]
                    section_count += 1 
            else:
                raise RuntimeError("Unknown %s" % line)
    dimension = max([x[1] for x in physical_names])
    return {
        'dimension'   : dimension,
      'physical_names'       : physical_names,
      'coordinates' : nodes,
      'elements'    : elements
    }           

def read_gmsh_file(filename):
    '''reads gmsh file and converts to python representation'''
    data = parse_gmsh_file(filename)
    #namemap = {}
    print('%d dimension' % data['dimension'])
    print('%d coordinates' % len(data['coordinates']))
    #for i in sorted(data['physical_names'].keys()):
    #  print "%s %s %d" % (i, data['physical_names'][i], len(data['elements'][i]))
    #  namemap[data['physical_names'][i]] = i

    coordinate_indexes = [x[0] for x in data['coordinates']]
    max_coordinate_index = max(coordinate_indexes)
    coordinate_to_index = [-1] * (max_coordinate_index+1)
    for i,j in enumerate(coordinate_indexes):
        coordinate_to_index[j] = i 

    coordinates=[]
    for i in data['coordinates']:
        coordinates.extend(i[1:])

    all_physical_numbers = sorted(set([x[3] for x in data['elements']]))
    sorted_physical_names = sorted(data['physical_names'],key = lambda x : x[0])
    physical_number_map = {
        1 : {},
        2 : {},
        3 : {},
    }
    for i, j in enumerate(sorted_physical_names):
        # mapped dimension, physical number, new index
        physical_number_map[j[1]][j[2]] = i
    physical_names = [x[0] for x in sorted_physical_names]


    #gmsh format
    #elm-number elm-type number-of-tags < tag > ... node-number-list
    #our format
    elements = []
    for i in data['elements']:
        t = i[1]
        if (t == 4):
            #tetrahedron
            etype = 3
            dimension = 3
        elif (t == 2):
            #triangle
            etype = 2
            dimension = 2
        elif (t == 1):
            #edge
            etype = 1
            dimension = 1
        elif (t == 15):
            #point
            etype = 0
        else:
            raise RuntimeError("Cannot handle element type " % i)

        #physical number
        # will be remapped later
        pnum = physical_number_map[dimension][i[3]]

        #nodes
        # position 2 + number of tags
        nodes = i[i[2]+3:]
        nodes = [coordinate_to_index[x] for x in nodes]
        elements.extend([etype, pnum])
        elements.extend(nodes)

    return {
        'physical_names' : physical_names,
      'coordinates'    : coordinates,
      'elements'       : elements,
    }

