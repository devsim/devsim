import devsim
from devsim.python_packages.simple_physics import *
import diode_common

def print_circuit_solution():
    for node in get_circuit_node_list():
        r = get_circuit_node_value(solution='dcop', node=node)
        print("%s\t%1.15e" % (node, r))

device = "MyDevice"
region = "MyRegion"

# Set extended parameters
set_parameter(name="extended_solver", value=True)
set_parameter(name="extended_model", value=True)
set_parameter(name="extended_equation", value=True)

#This requires a circuit element to integrated current
voltage = 0.0
circuit_element(name="V1", n1=GetContactBiasName("top"), n2=0, value=voltage, acreal=1.0, acimag=0.0)

diode_common.CreateMesh2(device=device, region=region)
diode_common.SetParameters(device=device, region=region)

#Übergeben der Werte an SetNetDoping
diode_common.SetNetDoping(device=device, region=region)

diode_common.InitialSolution(device, region, circuit_contacts="top")

# Initial DC solution
devsim.solve(type="dc", absolute_error=1.0, relative_error=1e-12, maximum_iterations=30)

diode_common.DriftDiffusionInitialSolution(device, region, circuit_contacts=["top"])

devsim.solve(type="transient_dc", absolute_error=1.0, relative_error=1e-14, maximum_iterations=30)

print_circuit_solution()

circuit_alter(name="V1", value=0.7)

time_step = 1e-3
total_time = 1e-2
current_time = 0

while current_time < total_time:
    devsim.solve(type="transient_bdf1", absolute_error=1e10, relative_error=1e-10, maximum_iterations=30, tdelta=time_step, charge_error=1)

    print_circuit_solution()

    current_time += time_step
