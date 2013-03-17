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

set_parameter -name "threads_available" -value 2;
source fpetest1.tcl
set_parameter -name "threads_available" -value 1;
catch { set x [get_node_model_values -device $device -region $region -name test2]} y
puts $y
set_parameter -name "threads_available" -value 0;
catch { set x [get_node_model_values -device $device -region $region -name test2]} y
puts $y
set_parameter -name "threads_available" -value -1;
catch { set x [get_node_model_values -device $device -region $region -name test2]} y
puts $y
set_parameter -name "threads_available" -value 2;
catch { set x [get_node_model_values -device $device -region $region -name test2]} y
puts $y
