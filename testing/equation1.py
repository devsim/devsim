# Copyright 2013 DEVSIM LLC
#
# SPDX-License-Identifier: Apache-2.0

from devsim import *
import test_common
# basic linear circuit solved by itself
add_circuit_node(name="n1", variable_update="default")
add_circuit_node(name="n2", variable_update="default")


circuit_element(name="V1", n1="n1", n2="0", value=1.0)


def myassemble(what, timemode):
    n1 = get_circuit_equation_number(node="n1")
    n2 = get_circuit_equation_number(node="n2")
    G1=1.0
    G2=1.0

    rcv=[]
    rv=[]

    if timemode != "DC":
        return [rcv, rv]

    if  what != "MATRIXONLY":
        v1 = get_circuit_node_value(node="n1", solution="dcop")
        v2 = get_circuit_node_value(node="n2", solution="dcop")

        I1 = G1 * (v1 - v2)
        I2 = G2 * v2
        rv.extend([n1, I1])
        rv.extend([n2, I2])
        rv.extend([n2, -I1])

    if what !="RHS" :
        mG1 = -G1
        mG2 = -G2

        rcv.extend([n1, n1, G1])
        rcv.extend([n2, n2, G1])
        rcv.extend([n1, n2, mG1])
        rcv.extend([n2, n1, mG2])
        rcv.extend([n2, n2, G2])

    print(rcv)
    print(rv)
    return rcv, rv, False

custom_equation(name="test1", procedure=myassemble)
solve(type="dc", absolute_error=1.0, relative_error=1e-14, maximum_iterations=3)
test_common.print_circuit_solution()

circuit_alter(name="V1", value=2.0)
solve(type="dc", absolute_error=1.0, relative_error=1e-14, maximum_iterations=3)
test_common.print_circuit_solution()


