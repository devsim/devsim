# Copyright 2013 DEVSIM LLC
#
# SPDX-License-Identifier: Apache-2.0

#puts [package require sqlite3]
# setting an index on material or parameter?

#sqlite3 db1 ./testdb
#db1 eval {DROP TABLE IF EXISTS t1}
#db1 eval {CREATE TABLE t1(material text, parameter text, value real, unit text, description text)}
#
#set x [db1 eval {INSERT INTO t1 VALUES('global', 'q',  '1.60217646e-19', 'coul', 'Electron Charge')}];
#set x [db1 eval {INSERT INTO t1 VALUES('global', 'pi', '3.14159265358979323846', '', 'Pi')}];
#set x [db1 eval {INSERT INTO t1 VALUES('global', 'm0', '9.10938188e-31', 'kg', 'Electron Mass')}];
#set x [db1 eval {SELECT * FROM t1 ORDER BY material}]
#puts $x
from devsim import *
create_db(filename="foodb")
add_db_entry(material="global", parameter="q", value=1.60217646e-19, unit="coul", description="Electron Charge")
add_db_entry(material="global", parameter="one", value=2, unit="", description="")
print((get_db_entry(material="global", parameter="q")))
print((get_db_entry(material="global", parameter="one")))
save_db()
print((get_db_entry(material="global", parameter="q")))
print((get_db_entry(material="global", parameter="one")))
close_db()
open_db(filename="foodb", permission="readwrite")
print((get_db_entry(material="global", parameter="q")))
add_db_entry(material="global", parameter="one", value=1, unit="", description="")
print((get_db_entry(material="global", parameter="one")))
save_db()
close_db()
open_db(filename="foodb", permission="readonly")
print((get_db_entry(material="global", parameter="q")))
print((get_db_entry(material="global", parameter="one")))
close_db()

# also test updating an existing parameter
