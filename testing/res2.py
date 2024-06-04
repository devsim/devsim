# Copyright 2013 DEVSIM LLC
#
# SPDX-License-Identifier: Apache-2.0

import devsim
import test_common

device = "MyDevice"
regions = ("MySi1", "MySi2")
interface = "MyInt"
contacts = ("top", "bot")

test_common.CreateSimpleMeshWithInterface(
    device=device, region0=regions[0], region1=regions[1], interface=interface
)

for region in regions:
    test_common.SetupResistorConstants(device, region)
    test_common.SetupInitialResistorSystem(device, region, net_doping=1e16)

for contact in contacts:
    test_common.SetupInitialResistorContact(device, contact=contact)

test_common.SetupContinuousPotentialAtInterface(device, interface)

#####
##### Initial DC Solution
#####
devsim.set_parameter(name="topbias", value=0.0)
devsim.set_parameter(name="botbias", value=0.0)
devsim.solve(type="dc", absolute_error=1.0, relative_error=1e-10, maximum_iterations=30)

for region in regions:
    for name in ("Potential", "IntrinsicElectrons"):
        devsim.print_node_values(device=device, region=region, name=name)

for region in regions:
    test_common.SetupCarrierResistorSystem(device, region)

for contact in contacts:
    test_common.SetupCarrierResistorContact(device, contact=contact)

test_common.SetupContinuousElectronsAtInterface(device, interface)

for v in (0.0, 0.01, 0.02, 0.03, 0.04, 0.05, 0.06, 0.07, 0.08, 0.09, 0.10):
    devsim.set_parameter(name="topbias", value=v)
    devsim.solve(
        type="dc", absolute_error=1.0, relative_error=1e-10, maximum_iterations=30
    )

    test_common.printResistorCurrent(device=device, contact="top")
    test_common.printResistorCurrent(device=device, contact="bot")

for region in regions:
    devsim.print_node_values(device=device, region=region, name="Electrons")
    devsim.print_node_values(device=device, region=region, name="Potential")
    devsim.print_edge_values(device=device, region=region, name="ElectricField")
    devsim.print_edge_values(device=device, region=region, name="ElectronCurrent")

l = 1.0  # noqa: E741
q = 1.6e-19
n = 1.0e16
u = 400
R = l / (q * n * u)
print(0.1 / R)
