# Copyright 2013 DEVSIM LLC
#
# SPDX-License-Identifier: Apache-2.0
import devsim
devsim.set_parameter(name="threads_available", value=2)
import fpetest1

for name, value in (
    ("threads_available", 1),
  ("threads_available", 0),
  ("threads_available", -1),
  ("threads_available", 2),
):
    devsim.set_parameter(name=name, value=value)
    try:
        print(devsim.get_node_model_values(device=fpetest1.device, region=fpetest1.region, name="test2"))
    except devsim.error as x:
        print(x)

