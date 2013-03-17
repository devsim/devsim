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

#requires tcl thread package 
# Windows
if {$tcl_platform(pointerSize)==4} {
lappend auto_path C:/Tcl/lib/tcl8.5
} else {
lappend auto_path C:/Tcl64/lib/tcl8.5
}
# Linux
lappend auto_path /usr/lib/tcltk/
# Mac OS X
lappend auto_path /System/Library/Tcl/8.5
package require Thread
lappend ids [thread::create -joinable {after 100; puts [format "%d" 1 ] }]
lappend ids [thread::create -joinable {puts [format "%d" 2 ] }]
lappend ids [thread::create -joinable {puts [format "%d" 3 ] }]
lappend ids [thread::create -joinable {puts [format "%d" 4 ] }]
lappend ids [thread::create -joinable {puts [format "%d" 5 ] }]

foreach id $ids {
  thread::join $id
}
