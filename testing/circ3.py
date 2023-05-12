# Copyright 2013 DEVSIM LLC
#
# SPDX-License-Identifier: Apache-2.0

# basic linear circuit solved by itself
import devsim
import test_common

devsim.circuit_element(name="V1", n1=1, n2=0, value=1.0)
devsim.circuit_element(name="R1", n1=1, n2=2, value=5)
devsim.circuit_element(name="R2", n1=2, n2=0, value=5)

devsim.solve(type="dc", absolute_error=1.0, relative_error=1e-14, maximum_iterations=3)
test_common.print_circuit_solution()

devsim.circuit_alter(name="V1", value=2.0)
devsim.solve(type="dc", absolute_error=1.0, relative_error=1e-14, maximum_iterations=3)
test_common.print_circuit_solution()

