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

#### Plan is to use this file for full restart, including equations
set device mymos
load_devices -file mos_2d_dd.msh
source mos_2d_params.tcl

set_parameter -name debug_level -value info
set_parameter -device $device -region gate -name debug_level -value verbose


solve -type dc -absolute_error 1.0e30 -relative_error 1e-5 -maximum_iterations 30


write_devices -file mos_2d_restart2.msh -type devsim

#set_parameter -device mymos -region gate -name gatebias -value 0.1
#solve -type dc -absolute_error 1.0e30 -relative_error 1e-9 -maximum_iterations 30
