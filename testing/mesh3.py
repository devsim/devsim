# Copyright 2013 DEVSIM LLC
#
# SPDX-License-Identifier: Apache-2.0

import devsim
devsim.load_devices(file="mesh2.msh")
devsim.write_devices(file="mesh3.msh", type="devsim_data", device="MyDevice")

