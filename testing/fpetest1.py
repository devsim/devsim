# Copyright 2013 DEVSIM LLC
#
# SPDX-License-Identifier: Apache-2.0

import devsim
import test_common

device="MyDevice"
region="MyRegion"

test_common.CreateSimpleMesh(device, region)

for name, equation in (
    ("test1", "log(-1)"),
  ("test2", "log(x)"),
):
    devsim.node_model(device=device, region=region, name=name, equation=equation)
    try:
        print(devsim.get_node_model_values(device=device, region=region, name=name))
    except devsim.error as x:
        print(x)

