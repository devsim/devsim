# Copyright 2013 DEVSIM LLC
#
# SPDX-License-Identifier: Apache-2.0

#Purpose: Fermi integral and inverse

import devsim
import test_common

device="MyDevice"
region="MyRegion"

test_common.CreateSimpleMesh(device, region)



devsim.node_model(device=device, region=region, name="zeta1", equation="x*70 - 70;")
devsim.node_model(device=device, region=region, name="g1", equation="gfi(zeta1, s);")
devsim.node_model(device=device, region=region, name="zeta2", equation="igfi(g1, s);")


devsim.node_model(device=device, region=region, name="dgfidx1", equation="diff(gfi(zeta1, s), zeta1);")
devsim.node_model(device=device, region=region, name="digfidx1", equation="diff(igfi(g1, s), g1);")
devsim.node_model(device=device, region=region, name="dgfidx2", equation="1/digfidx1;")

for s in (2,4,6,8):
    devsim.set_parameter(name="s", value=s)

    devsim.print_node_values(device=device, region=region, name="zeta1")
    devsim.print_node_values(device=device, region=region, name="g1")
    devsim.print_node_values(device=device, region=region, name="zeta2")

    devsim.print_node_values(device=device, region=region, name="dgfidx1")
    devsim.print_node_values(device=device, region=region, name="digfidx1")
    devsim.print_node_values(device=device, region=region, name="dgfidx2")
