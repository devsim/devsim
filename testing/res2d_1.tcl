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

####
#### Meshing
####
#### a manual mesh is where we use simple commands
#### future potential types are non-terminating quadtree, et al.
#### This one will be the basis for the mesh reader/writer
#### Will need to support reading in a node solution
create_2d_mesh -mesh dog -type manual
add_2d -mesh dog -coordinates {
### need to make sure tcl interpreter can tell us if we got a name instead of a number
# coordinate index followed by x and y
    0	0.0 0.0
    1	1.0 0.0
    2	2.0 0.0
}
### nodes reference coordinates
add_2d -mesh dog -region cat -nodes {
# node index followed by coordinate index
    0 0
    1 1
}
add_2d -mesh dog -region mouse -nodes {
    0 1
    1 2
}

# do we really need to specify the nodes at the interface, or
# should we assume all of them: maybe -nodes0 -nodes1
add_2d -mesh dog -interface bear -region0 cat -region1 mouse \
    -nodes0 {1} \
    -nodes1 {0}

add_2d -mesh dog -region cat -edges {
    # edge node node
    1 0 0
}

add_2d -mesh dog -region mouse -edges {
    # edge node node
    1 0 0
}

# in order to get the coupling length we need this
# or should we allow that to be a model
add_2d -mesh dog -region cat -triangles {
    #triangle edge, edge, edge
}
add_2d -mesh dog -region dog -triangles {
    #triangle edge, edge, edge
}

add_2d -mesh dog -contact top -nodes {0}
add_2d -mesh dog -contact bot -nodes {1}

#### node solutions
#### serializer would have to set tcl_precision
add_2d -mesh dog -nodesolution NetDoping {
    0 1e10
    1 2e10
}
add_2d -mesh dog -nodesolution Electrons {
    0 1e10
    1 2e10
}

#### equations, models, etc.

# finalize things
finalize_2d_mesh -mesh dog
create_2d_device -mesh dog -device bar

### should we have edges for 1d and triangles for 2d????
### If we store edge solutions, should triangle provide directions, or explicitly state edges
