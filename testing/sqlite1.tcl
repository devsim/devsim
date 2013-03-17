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

package require sqlite3
# setting an index on material or parameter?

sqlite3 db1 ./testdb
db1 eval {DROP TABLE IF EXISTS t1} 
db1 eval {CREATE TABLE t1(material text, parameter text, value real, unit text, description text)} 

set x [db1 eval {INSERT INTO t1 VALUES('global', 'q',  '1.60217646e-19', 'coul', 'Electron Charge')}];
set x [db1 eval {INSERT INTO t1 VALUES('global', 'pi', '3.14159265358979323846', '', 'Pi')}];
set x [db1 eval {INSERT INTO t1 VALUES('global', 'm0', '9.10938188e-31', 'kg', 'Electron Mass')}];
set x [db1 eval {SELECT * FROM t1 ORDER BY material}] 
puts $x

