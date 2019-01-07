/***
DEVSIM
Copyright 2013 Devsim LLC

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
***/

#include "Python.h"
#include "ceval.h"
#include "frameobject.h"

#include "PythonCommands.hh"
#include "Interpreter.hh"
#include "CommandHandler.hh"
#include "GetArgs.hh"
#include "dsCommand.hh"
#include "Validate.hh"

#include "GeometryCommands.hh"
#include "MaterialCommands.hh"
#include "ModelCommands.hh"
#include "MathCommands.hh"
#include "EquationCommands.hh"
#include "MeshingCommands.hh"
#include "CircuitCommands.hh"
#include "dsException.hh"
#include "FPECheck.hh"
#include "OutputStream.hh"
#include "import.hh"

#include <new>
#include <sstream>

// Stringify
// borrowed from dsAssert.hh
#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)
#define PASTETOKENS(x, y) x ## y
// Module initialization function
#if PY_MAJOR_VERSION >= 3
#define HELPER1(x) PASTETOKENS(PyInit_, x)
#else
#define HELPER1(x) PASTETOKENS(init, x)
#endif
#define DEVSIM_MODULE_INIT HELPER1(DEVSIM_MODULE_NAME)
#define DEVSIM_MODULE_STRING TOSTRING(DEVSIM_MODULE_NAME)

// Defined in devsim_py.cc
void devsim_initialization();

using namespace dsValidate;

namespace dsPy {
namespace {
struct module_state {
  module_state() : error(nullptr) {};
  PyObject *error;
};

#if PY_MAJOR_VERSION >= 3
#define GETSTATE(m) ((struct module_state*)PyModule_GetState(m))
#else
#define GETSTATE(m) (&_state)
static struct module_state _state;
#endif
}


static PyObject * CmdDispatch(PyObject *m, PyObject *args, PyObject *kwargs, const char *name, newcmd fptr)
{
  PyObject *ret = nullptr;

  FPECheck::ClearFPE();

  std::string command_name = name;


  if (args)
  {
    if (PyTuple_CheckExact(args))
    {
      size_t argc = PyTuple_Size(args);
      if (argc != 0)
      {
        std::ostringstream os;
        os << "Command " << command_name << " does not take positional arguments";
        PyErr_SetString(GETSTATE(m)->error, os.str().c_str());
        return ret;
      }
    }
    else
    {
      PyErr_SetString(GETSTATE(m)->error, "UNEXPECTED");
      return ret;
    }
  }

  try
  {

    dsGetArgs::CommandInfo command_info;
    command_info.self_         =  m;
    command_info.args_         =  args;
    if (kwargs) {
    Py_INCREF(kwargs);
    }
    command_info.kwargs_       =  kwargs;
    command_info.command_name_ =  command_name;
    command_info.exception_    =  GETSTATE(m)->error;

    CommandHandler data(&command_info);
    command_info.command_handler_ = &data;
    fptr(data);
    ret = reinterpret_cast<PyObject *>(data.GetReturnObject().GetObject());
    if (ret)
    {
      Py_INCREF(ret);
    }
  }
  catch (const dsException &x)
  {
    ret = nullptr;
    PyErr_SetString(GETSTATE(m)->error, x.what());
  }
  catch (std::bad_alloc &)
  {
    ret = nullptr;
    PyErr_SetString(GETSTATE(m)->error, const_cast<char *>("OUT OF MEMORY"));
  }
  catch (std::exception &)
  {
    ret = nullptr;
    PyErr_SetString(GETSTATE(m)->error, const_cast<char *>("UNEXPECTED ERROR"));
  }


  if (FPECheck::CheckFPE())
  {
    std::ostringstream os;
    os << "There was an uncaught floating point exception of type"" << FPECheck::getFPEString() << ""n";
    FPECheck::ClearFPE();
    ret = nullptr;
    OutputStream::WriteOut(OutputStream::OutputType::ERROR, os.str().c_str());
  }

  return ret;
}

// https://gcc.gnu.org/onlinedocs/cpp/Stringification.html
// https://gcc.gnu.org/onlinedocs/cpp/Concatenation.html
#define MyNewPyPtr(name, fptr) \
PyObject * name ## CmdDispatch(PyObject *self, PyObject *args, PyObject *kwargs) \
{ \
  return CmdDispatch(self, args, kwargs, # name, fptr); \
}

// https://stackoverflow.com/questions/28857522/python-c-extension-keyword-arguments
#define MYCOMMAND(name, fptr) {#name, reinterpret_cast<PyCFunction>(name ## CmdDispatch), METH_KEYWORDS | METH_VARARGS, name ## _doc}


// Geometry Commands
MyNewPyPtr(get_device_list,    dsCommand::getDeviceListCmd);
MyNewPyPtr(get_region_list,    dsCommand::getRegionListCmd);
MyNewPyPtr(get_interface_list, dsCommand::getRegionListCmd);
MyNewPyPtr(get_contact_list,   dsCommand::getRegionListCmd);
MyNewPyPtr(get_element_node_list, dsCommand::getElementNodeListCmd);
// Material Commands
MyNewPyPtr(set_parameter,      dsCommand::getParameterCmd);
MyNewPyPtr(get_parameter,      dsCommand::getParameterCmd);
MyNewPyPtr(get_parameter_list, dsCommand::getParameterCmd);
MyNewPyPtr(set_material,       dsCommand::getParameterCmd);
MyNewPyPtr(get_material,       dsCommand::getParameterCmd);
MyNewPyPtr(create_db,          dsCommand::openDBCmd);
MyNewPyPtr(open_db,            dsCommand::openDBCmd);
MyNewPyPtr(close_db,           dsCommand::openDBCmd);
MyNewPyPtr(save_db,            dsCommand::openDBCmd);
MyNewPyPtr(add_db_entry,       dsCommand::addDBEntryCmd);
MyNewPyPtr(get_db_entry,       dsCommand::getDBEntryCmd);
MyNewPyPtr(get_dimension,      dsCommand::getParameterCmd);
// Model Commands
MyNewPyPtr(contact_edge_model,         dsCommand::createContactNodeModelCmd);
MyNewPyPtr(contact_node_model,         dsCommand::createContactNodeModelCmd);
MyNewPyPtr(debug_triangle_models,      dsCommand::debugTriangleCmd);
MyNewPyPtr(delete_edge_model,          dsCommand::printEdgeValuesCmd);
MyNewPyPtr(delete_element_model,       dsCommand::printElementEdgeValuesCmd);
MyNewPyPtr(delete_interface_model,     dsCommand::getInterfaceValuesCmd);
MyNewPyPtr(delete_node_model,          dsCommand::printNodeValuesCmd);
MyNewPyPtr(edge_from_node_model,       dsCommand::createEdgeFromNodeModelCmd);
MyNewPyPtr(edge_average_model,         dsCommand::createEdgeAverageModelCmd);
MyNewPyPtr(edge_model,                 dsCommand::createNodeModelCmd);
MyNewPyPtr(element_from_edge_model,    dsCommand::createTriangleFromEdgeModelCmd);
MyNewPyPtr(element_from_node_model,    dsCommand::createEdgeFromNodeModelCmd);
MyNewPyPtr(vector_element_model,       dsCommand::createVectorElementModelCmd);
MyNewPyPtr(element_model,              dsCommand::createNodeModelCmd);
MyNewPyPtr(get_edge_model_list,        dsCommand::getNodeModelListCmd);
MyNewPyPtr(get_edge_model_values,      dsCommand::printEdgeValuesCmd);
MyNewPyPtr(get_element_model_list,     dsCommand::getNodeModelListCmd);
MyNewPyPtr(get_element_model_values,   dsCommand::printElementEdgeValuesCmd);
MyNewPyPtr(get_interface_model_list,   dsCommand::getInterfaceModelListCmd);
MyNewPyPtr(get_interface_model_values, dsCommand::getInterfaceValuesCmd);
MyNewPyPtr(get_node_model_list,        dsCommand::getNodeModelListCmd);
MyNewPyPtr(get_node_model_values,      dsCommand::printNodeValuesCmd);
MyNewPyPtr(interface_model,            dsCommand::createInterfaceNodeModelCmd);
MyNewPyPtr(interface_normal_model,     dsCommand::createInterfaceNormalModelCmd);
MyNewPyPtr(node_model,                 dsCommand::createNodeModelCmd);
MyNewPyPtr(node_solution,              dsCommand::createNodeSolutionCmd);
MyNewPyPtr(print_edge_values,          dsCommand::printEdgeValuesCmd);
MyNewPyPtr(print_element_values,       dsCommand::printElementEdgeValuesCmd);
MyNewPyPtr(print_node_values,          dsCommand::printNodeValuesCmd);
MyNewPyPtr(register_function,          dsCommand::registerFunctionCmd);
MyNewPyPtr(set_node_values,            dsCommand::setNodeValuesCmd);
MyNewPyPtr(set_node_value,             dsCommand::setNodeValueCmd);
MyNewPyPtr(symdiff,                    dsCommand::symdiffCmd);
MyNewPyPtr(vector_gradient,            dsCommand::createEdgeFromNodeModelCmd);
MyNewPyPtr(cylindrical_edge_couple,    dsCommand::createCylindricalCmd);
MyNewPyPtr(cylindrical_node_volume,    dsCommand::createCylindricalCmd);
MyNewPyPtr(cylindrical_surface_area,   dsCommand::createCylindricalCmd);
// Math Commands
MyNewPyPtr(get_contact_current,        dsCommand::getContactCurrentCmd);
MyNewPyPtr(get_contact_charge,         dsCommand::getContactCurrentCmd);
MyNewPyPtr(solve,                      dsCommand::solveCmd);
// Equation Commands
MyNewPyPtr(equation,                       dsCommand::createEquationCmd);
MyNewPyPtr(interface_equation,             dsCommand::createInterfaceEquationCmd);
MyNewPyPtr(contact_equation,               dsCommand::createContactEquationCmd);
MyNewPyPtr(custom_equation,                dsCommand::createCustomEquationCmd);
MyNewPyPtr(get_equation_numbers,           dsCommand::getEquationNumbersCmd);
MyNewPyPtr(get_equation_list,              dsCommand::getEquationListCmd);
MyNewPyPtr(get_interface_equation_list,    dsCommand::getInterfaceEquationListCmd);
MyNewPyPtr(get_contact_equation_list,      dsCommand::getContactEquationListCmd);
MyNewPyPtr(delete_equation,                dsCommand::deleteEquationCmd);
MyNewPyPtr(delete_interface_equation,      dsCommand::deleteInterfaceEquationCmd);
MyNewPyPtr(delete_contact_equation,        dsCommand::deleteContactEquationCmd);
MyNewPyPtr(get_equation_command,           dsCommand::deleteEquationCmd);
MyNewPyPtr(get_contact_equation_command,   dsCommand::deleteContactEquationCmd);
MyNewPyPtr(get_interface_equation_command, dsCommand::deleteInterfaceEquationCmd);
// Meshing Commands
MyNewPyPtr(create_1d_mesh,                dsCommand::create1dMeshCmd);
MyNewPyPtr(finalize_mesh,                 dsCommand::finalizeMeshCmd);
MyNewPyPtr(add_1d_mesh_line,              dsCommand::add1dMeshLineCmd);
MyNewPyPtr(add_1d_interface,              dsCommand::add1dInterfaceCmd);
MyNewPyPtr(add_1d_contact,                dsCommand::add1dContactCmd);
MyNewPyPtr(add_1d_region,                 dsCommand::add1dRegionCmd);
MyNewPyPtr(create_2d_mesh,                dsCommand::create1dMeshCmd);
MyNewPyPtr(add_2d_mesh_line,              dsCommand::add2dMeshLineCmd);
MyNewPyPtr(add_2d_region,                 dsCommand::add2dRegionCmd);
MyNewPyPtr(add_2d_interface,              dsCommand::add2dInterfaceCmd);
MyNewPyPtr(add_2d_contact,                dsCommand::add2dContactCmd);
MyNewPyPtr(create_device,                 dsCommand::createDeviceCmd);
MyNewPyPtr(load_devices,                  dsCommand::loadDevicesCmd);
MyNewPyPtr(write_devices,                 dsCommand::writeDevicesCmd);
MyNewPyPtr(create_gmsh_mesh,              dsCommand::createGmshMeshCmd);
MyNewPyPtr(add_gmsh_contact,              dsCommand::addGmshContactCmd);
MyNewPyPtr(add_gmsh_interface,            dsCommand::addGmshInterfaceCmd);
MyNewPyPtr(add_gmsh_region,               dsCommand::addGmshRegionCmd);
MyNewPyPtr(create_genius_mesh,            dsCommand::createGeniusMeshCmd);
MyNewPyPtr(add_genius_contact,            dsCommand::addGeniusContactCmd);
MyNewPyPtr(add_genius_interface,          dsCommand::addGeniusInterfaceCmd);
MyNewPyPtr(add_genius_region,             dsCommand::addGeniusRegionCmd);
MyNewPyPtr(create_contact_from_interface, dsCommand::createContactFromInterfaceCmd);
// Circuit Commands
MyNewPyPtr(add_circuit_node,            dsCommand::addCircuitNodeCmd);
MyNewPyPtr(circuit_element,             dsCommand::circuitElementCmd);
MyNewPyPtr(circuit_alter,               dsCommand::circuitAlterCmd);
MyNewPyPtr(circuit_node_alias,          dsCommand::circuitNodeAliasCmd);
MyNewPyPtr(get_circuit_node_list,       dsCommand::circuitGetCircuitNodeListCmd);
MyNewPyPtr(get_circuit_solution_list,   dsCommand::circuitGetCircuitSolutionListCmd);
MyNewPyPtr(get_circuit_node_value,      dsCommand::circuitGetCircuitNodeValueCmd);
MyNewPyPtr(set_circuit_node_value,      dsCommand::circuitGetCircuitNodeValueCmd);
MyNewPyPtr(get_circuit_equation_number, dsCommand::circuitGetCircuitEquationNumberCmd);

#include "DevsimDoc.cc"

static struct PyMethodDef devsim_methods[] = {
//    {"devsim", reinterpret_cast<PyCFunction>(CmdDispatch), METH_KEYWORDS, "devsim interpreter"},
// Geometry Commands
MYCOMMAND(get_device_list,    dsCommand::getDeviceListCmd),
MYCOMMAND(get_region_list,    dsCommand::getRegionListCmd),
MYCOMMAND(get_interface_list, dsCommand::getRegionListCmd),
MYCOMMAND(get_contact_list,   dsCommand::getRegionListCmd),
MYCOMMAND(get_element_node_list, dsCommand::getElementNodeListCmd),
// Material Commands
MYCOMMAND(set_parameter,      dsCommand::getParameterCmd),
MYCOMMAND(get_parameter,      dsCommand::getParameterCmd),
MYCOMMAND(get_parameter_list, dsCommand::getParameterCmd),
MYCOMMAND(set_material,       dsCommand::getParameterCmd),
MYCOMMAND(get_material,       dsCommand::getParameterCmd),
MYCOMMAND(create_db,          dsCommand::openDBCmd),
MYCOMMAND(open_db,            dsCommand::openDBCmd),
MYCOMMAND(close_db,           dsCommand::openDBCmd),
MYCOMMAND(save_db,            dsCommand::openDBCmd),
MYCOMMAND(add_db_entry,       dsCommand::addDBEntryCmd),
MYCOMMAND(get_db_entry,       dsCommand::getDBEntryCmd),
MYCOMMAND(get_dimension,      dsCommand::getParameterCmd),
// Model Commands
MYCOMMAND(contact_edge_model,         dsCommand::createContactNodeModelCmd),
MYCOMMAND(contact_node_model,         dsCommand::createContactNodeModelCmd),
MYCOMMAND(debug_triangle_models,      dsCommand::debugTriangleCmd),
MYCOMMAND(delete_edge_model,          dsCommand::printEdgeValuesCmd),
MYCOMMAND(delete_element_model,       dsCommand::printElementEdgeValuesCmd),
MYCOMMAND(delete_interface_model,     dsCommand::getInterfaceValuesCmd),
MYCOMMAND(delete_node_model,          dsCommand::printNodeValuesCmd),
MYCOMMAND(edge_from_node_model,       dsCommand::createEdgeFromNodeModelCmd),
MYCOMMAND(edge_average_model,         dsCommand::createEdgeAverageModelCmd),
MYCOMMAND(edge_model,                 dsCommand::createNodeModelCmd),
MYCOMMAND(element_from_edge_model,    dsCommand::createTriangleFromEdgeModelCmd),
MYCOMMAND(element_from_node_model,    dsCommand::createEdgeFromNodeModelCmd),
MYCOMMAND(vector_element_model,       dsCommand::createVectorElementModelCmd),
MYCOMMAND(element_model,              dsCommand::createNodeModelCmd),
MYCOMMAND(get_edge_model_list,        dsCommand::getNodeModelListCmd),
MYCOMMAND(get_edge_model_values,      dsCommand::printEdgeValuesCmd),
MYCOMMAND(get_element_model_list,     dsCommand::getNodeModelListCmd),
MYCOMMAND(get_element_model_values,   dsCommand::printElementEdgeValuesCmd),
MYCOMMAND(get_interface_model_list,   dsCommand::getInterfaceModelListCmd),
MYCOMMAND(get_interface_model_values, dsCommand::getInterfaceValuesCmd),
MYCOMMAND(get_node_model_list,        dsCommand::getNodeModelListCmd),
MYCOMMAND(get_node_model_values,      dsCommand::printNodeValuesCmd),
MYCOMMAND(interface_model,            dsCommand::createInterfaceNodeModelCmd),
MYCOMMAND(interface_normal_model,     dsCommand::createInterfaceNormalModelCmd),
MYCOMMAND(node_model,                 dsCommand::createNodeModelCmd),
MYCOMMAND(node_solution,              dsCommand::createNodeSolutionCmd),
MYCOMMAND(print_edge_values,          dsCommand::printEdgeValuesCmd),
MYCOMMAND(print_element_values,       dsCommand::printElementEdgeValuesCmd),
MYCOMMAND(print_node_values,          dsCommand::printNodeValuesCmd),
MYCOMMAND(register_function,          dsCommand::registerFunctionCmd),
MYCOMMAND(set_node_values,            dsCommand::setNodeValuesCmd),
MYCOMMAND(set_node_value,             dsCommand::setNodeValueCmd),
MYCOMMAND(symdiff,                    dsCommand::symdiffCmd),
MYCOMMAND(vector_gradient,            dsCommand::createEdgeFromNodeModelCmd),
MYCOMMAND(cylindrical_edge_couple,    dsCommand::createCylindricalCmd),
MYCOMMAND(cylindrical_node_volume,    dsCommand::createCylindricalCmd),
MYCOMMAND(cylindrical_surface_area,   dsCommand::createCylindricalCmd),
// Math Commands
MYCOMMAND(get_contact_current,        dsCommand::getContactCurrentCmd),
MYCOMMAND(get_contact_charge,         dsCommand::getContactCurrentCmd),
MYCOMMAND(solve,                      dsCommand::solveCmd),
// Equation Commands
MYCOMMAND(equation,                       dsCommand::createEquationCmd),
MYCOMMAND(interface_equation,             dsCommand::createInterfaceEquationCmd),
MYCOMMAND(contact_equation,               dsCommand::createContactEquationCmd),
MYCOMMAND(custom_equation,                dsCommand::createCustomEquationCmd),
MYCOMMAND(get_equation_numbers,           dsCommand::getEquationNumbersCmd),
MYCOMMAND(get_equation_list,              dsCommand::getEquationListCmd),
MYCOMMAND(get_interface_equation_list,    dsCommand::getInterfaceEquationListCmd),
MYCOMMAND(get_contact_equation_list,      dsCommand::getContactEquationListCmd),
MYCOMMAND(delete_equation,                dsCommand::deleteEquationCmd),
MYCOMMAND(delete_interface_equation,      dsCommand::deleteInterfaceEquationCmd),
MYCOMMAND(delete_contact_equation,        dsCommand::deleteContactEquationCmd),
MYCOMMAND(get_equation_command,           dsCommand::deleteEquationCmd),
MYCOMMAND(get_contact_equation_command,   dsCommand::deleteContactEquationCmd),
MYCOMMAND(get_interface_equation_command, dsCommand::deleteInterfaceEquationCmd),
// Meshing Commands
MYCOMMAND(create_1d_mesh,                dsCommand::create1dMeshCmd),
MYCOMMAND(finalize_mesh,                 dsCommand::finalizeMeshCmd),
MYCOMMAND(add_1d_mesh_line,              dsCommand::add1dMeshLineCmd),
MYCOMMAND(add_1d_interface,              dsCommand::add1dInterfaceCmd),
MYCOMMAND(add_1d_contact,                dsCommand::add1dContactCmd),
MYCOMMAND(add_1d_region,                 dsCommand::add1dRegionCmd),
MYCOMMAND(create_2d_mesh,                dsCommand::create1dMeshCmd),
MYCOMMAND(add_2d_mesh_line,              dsCommand::add2dMeshLineCmd),
MYCOMMAND(add_2d_region,                 dsCommand::add2dRegionCmd),
MYCOMMAND(add_2d_interface,              dsCommand::add2dInterfaceCmd),
MYCOMMAND(add_2d_contact,                dsCommand::add2dContactCmd),
MYCOMMAND(create_device,                 dsCommand::createDeviceCmd),
MYCOMMAND(load_devices,                  dsCommand::loadDevicesCmd),
MYCOMMAND(write_devices,                 dsCommand::writeDevicesCmd),
MYCOMMAND(create_gmsh_mesh,              dsCommand::createGmshMeshCmd),
MYCOMMAND(add_gmsh_contact,              dsCommand::addGmshContactCmd),
MYCOMMAND(add_gmsh_interface,            dsCommand::addGmshInterfaceCmd),
MYCOMMAND(add_gmsh_region,               dsCommand::addGmshRegionCmd),
MYCOMMAND(create_genius_mesh,            dsCommand::createGeniusMeshCmd),
MYCOMMAND(add_genius_contact,            dsCommand::addGeniusContactCmd),
MYCOMMAND(add_genius_interface,          dsCommand::addGeniusInterfaceCmd),
MYCOMMAND(add_genius_region,             dsCommand::addGeniusRegionCmd),
MYCOMMAND(create_contact_from_interface, dsCommand::createContactFromInterfaceCmd),
// Circuit Commands
MYCOMMAND(add_circuit_node,            dsCommand::addCircuitNodeCmd),
MYCOMMAND(circuit_element,             dsCommand::circuitElementCmd),
MYCOMMAND(circuit_alter,               dsCommand::circuitAlterCmd),
MYCOMMAND(circuit_node_alias,          dsCommand::circuitNodeAliasCmd),
MYCOMMAND(get_circuit_node_list,       dsCommand::circuitGetCircuitNodeListCmd),
MYCOMMAND(get_circuit_solution_list,   dsCommand::circuitGetCircuitSolutionListCmd),
MYCOMMAND(get_circuit_node_value,      dsCommand::circuitGetCircuitNodeValueCmd),
MYCOMMAND(set_circuit_node_value,      dsCommand::circuitGetCircuitNodeValueCmd),
MYCOMMAND(get_circuit_equation_number, dsCommand::circuitGetCircuitEquationNumberCmd),
{nullptr, nullptr, 0, nullptr}
};




#if PY_MAJOR_VERSION >= 3
static int devsim_traverse(PyObject *m, visitproc visit, void *arg) {
    Py_VISIT(GETSTATE(m)->error);
    return 0;
}

static int devsim_clear(PyObject *m) {
    Py_CLEAR(GETSTATE(m)->error);
    return 0;
}


static struct PyModuleDef moduledef = {
        PyModuleDef_HEAD_INIT,
        DEVSIM_MODULE_STRING,
        nullptr,
        sizeof(struct module_state),
        devsim_methods,
        nullptr,
        devsim_traverse,
        devsim_clear,
        nullptr
};

#define INITERROR return nullptr

//PyMODINIT_FUNC // this next line is used instead to use DLL_PUBLIC macro
extern "C" DLL_PUBLIC PyObject *
DEVSIM_MODULE_INIT(void)
#else
#define INITERROR return
extern "C" void DLL_PUBLIC DEVSIM_MODULE_INIT()
#endif
{
#if PY_MAJOR_VERSION >= 3
  PyObject *module = PyModule_Create(&moduledef);
#else
  PyObject *module = Py_InitModule(DEVSIM_MODULE_STRING, devsim_methods);
#endif

    if (module == nullptr)
    {
      INITERROR;
    }

    // TODO: modify symdiff to use this approach
    struct module_state *st = GETSTATE(module);
    st->error = PyErr_NewException(const_cast<char *>(DEVSIM_MODULE_STRING ".error"), nullptr, nullptr);
    if (st->error == nullptr) {
        Py_DECREF(module);
        INITERROR;
    }

    PyModule_AddObject(module, "error", st->error);

    //https://www.python.org/dev/peps/pep-0396/
    PyDict_SetItemString(PyModule_GetDict(module), "__version__", PyUnicode_FromString(DEVSIM_VERSION_STRING));

    devsim_initialization();

#if PY_MAJOR_VERSION >=3
  return module;
#endif
}

#if 0
// https://docs.python.org/3/howto/cporting.html
// https://docs.python.org/3/extending/embedding.html
using namespace std;
bool 
Commands_Init() {
#if PY_MAJOR_VERSION >=3
#if 0
  PyObject *module = PyInit_ds();
  if (module)
  {
#endif
    PyImport_AppendInittab("ds", &PyInit_ds);
#if 0
  }
#endif
#else
  initds();
#endif

  return true;
}
#endif
}

