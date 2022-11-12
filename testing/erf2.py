# Copyright 2013 DEVSIM LLC
#
# Licensed under the Apache License, Version 2.0 (the "License")
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
# the mesh range is 0 to 1
# range is -1 to 1
myexp1 = "0.999*(x-0.5)"
# range is 0 to 2
myexp2 = "1.999*x+0.0001"


for i, j in (
      ("erf_inv", "erf_inv(x)"),
      ("d_erf_inv_dx", "diff(erf_inv(x),x)"),
      ("erfc_inv", "erfc_inv(x)"),
      ("d_erfc_inv_dx", "diff(erfc_inv(x),x)"),
      ("erf_inv", "erf_inv(%s)" % myexp1),
      ("d_erf_inv_dx", "diff(erf_inv(%s),x)" % myexp1),
      ("erfc_inv", "erfc_inv(%s)" % myexp2),
      ("d_erfc_inv_dx", "diff(erfc_inv(%s),x)" % myexp2),
    ):
    devsim.node_model(device=device, region=region, name=i, equation=j)
    print()
    try:
        devsim.print_node_values(device=device, region=region, name=i)
    except:
        pass



