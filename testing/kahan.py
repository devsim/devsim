# Copyright 2013 DEVSIM LLC
#
# SPDX-License-Identifier: Apache-2.0

import devsim
import test_common

device="MyDevice"
region="MyRegion"

test_common.CreateSimpleMesh(device, region)

for name, equation in (
    ("NetDoping1", "kahan3(1e20, -1e20, x);"),
  ("NetDoping2", "kahan4(1e20, -1e20, x,1e14);"),
  ("NetDoping3", "kahan3(x, 1e20, -1e20);"),
  ("NetDoping4", "kahan4(x, 1e20, -1e20, 1e14);"),
):
    devsim.node_model(device=device, region=region, name=name, equation=equation)
    print(devsim.get_node_model_values(device=device, region=region, name=name))

print(devsim.symdiff(expr="diff(kahan3(a,b,c),c);"))
print(devsim.symdiff(expr="diff(kahan4(a,b,c,d),c);"))
print(devsim.symdiff(expr="diff(kahan4(a,b,c,c),c);"))

