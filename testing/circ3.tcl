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

# basic linear circuit solved by itself

circuit_element -name V1 -n1 1 -n2 0 -value 1.0
circuit_element -name R1 -n1 1 -n2 2 -value 5
circuit_element -name R2 -n1 2 -n2 0 -value 5

solve -type dc -absolute_error 1.0 -relative_error 1e-14 -maximum_iterations 3

circuit_alter -name V1 -value 2.0
solve -type dc -absolute_error 1.0 -relative_error 1e-14 -maximum_iterations 30
