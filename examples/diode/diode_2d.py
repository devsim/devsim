# Copyright 2013 DEVSIM LLC
#
# SPDX-License-Identifier: Apache-2.0

from devsim import *
from devsim.python_packages.simple_physics import *
import diode_common

# dio1
#
# Make doping a step function
# print dat to text file for viewing in grace
# verify currents analytically
# in dio2 add recombination
#

device="MyDevice"
region="MyRegion"

diode_common.Create2DMesh(device, region)

diode_common.SetParameters(device=device, region=region)

diode_common.SetNetDoping(device=device, region=region)

diode_common.InitialSolution(device, region)

# Initial DC solution
solve(type="dc", absolute_error=1.0, relative_error=1e-12, maximum_iterations=30)

diode_common.DriftDiffusionInitialSolution(device, region)
###
### Drift diffusion simulation at equilibrium
###
solve(type="dc", absolute_error=1e10, relative_error=1e-10, maximum_iterations=30)

####
#### Ramp the bias to 0.5 Volts
####
v = 0.0
while v < 0.51:
    set_parameter(device=device, name=GetContactBiasName("top"), value=v)
    solve(type="dc", absolute_error=1e10, relative_error=1e-10, maximum_iterations=30)
    PrintCurrents(device, "top")
    PrintCurrents(device, "bot")
    v += 0.1

val = 10
for i in range(2):
    set_parameter(device=device, name=GetContactBiasName("top"), value=val)
    data = solve(type="dc", absolute_error=1e10, relative_error=1e-10, maximum_iterations=30, info=True)
    print(data['converged'])
    if not data['converged']:
      val = 0.6

print(data)
for i in data['iterations']:
    for d in i['devices']:
        for r in d['regions']:
            for e in r['equations']:
                print(e)
