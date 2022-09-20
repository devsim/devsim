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

