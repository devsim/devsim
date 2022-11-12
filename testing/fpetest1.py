# Copyright 2013 DEVSIM LLC
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

