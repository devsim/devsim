# Copyright 2013 DEVSIM LLC
#
# SPDX-License-Identifier: Apache-2.0

import devsim
import test_common

# ssac circuit only
devsim.circuit_element(name="V1", n1=1, n2=0, value=1.0, acreal=1.0)
#lambda = 0.04
devsim.circuit_element(name="R1", n1=1, n2=2, value=5)
devsim.circuit_element(name="C1", n1=2, n2=0, value=5)

print("transient_dc")
devsim.solve(type="transient_dc", absolute_error=1.0, relative_error=1e-14, maximum_iterations=3)
test_common.print_circuit_solution()

devsim.circuit_alter(name="V1", value=0.0)

print("transient_tr")
devsim.solve(type="transient_tr", absolute_error=1.0, relative_error=1e-14, maximum_iterations=3, tdelta=1e-3, charge_error=1e-2)
test_common.print_circuit_solution()
devsim.solve(type="transient_tr", absolute_error=1.0, relative_error=1e-14, maximum_iterations=3, tdelta=1e-3, charge_error=1e-2)
test_common.print_circuit_solution()
devsim.solve(type="transient_tr", absolute_error=1.0, relative_error=1e-14, maximum_iterations=3, tdelta=1e-3, charge_error=1e-2)
test_common.print_circuit_solution()
devsim.solve(type="transient_tr", absolute_error=1.0, relative_error=1e-14, maximum_iterations=3, tdelta=1e-3, charge_error=1e-2)
test_common.print_circuit_solution()
devsim.solve(type="transient_tr", absolute_error=1.0, relative_error=1e-14, maximum_iterations=3, tdelta=1e-3, charge_error=1e-2)
test_common.print_circuit_solution()

