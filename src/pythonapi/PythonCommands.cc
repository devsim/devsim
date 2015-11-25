/***
DEVSIM
Copyright 2013 Devsim LLC

This file is part of DEVSIM.

DEVSIM is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation, version 3.

DEVSIM is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with DEVSIM.  If not, see <http://www.gnu.org/licenses/>.
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

#include <new>
#include <sstream>
#include <iostream>

using namespace dsValidate;

namespace dsPy {

namespace {
  std::map<std::string, newcmd> CommandMap;
  ObjectHolder devsim_exception;
}

#if 0
PyObject *
CmdDispatch(PyObject *self, PyObject *args, PyObject *kwargs)
{
  //// return exception
  PyObject *ret = NULL;

  FPECheck::ClearFPE();

  std::string command_name;


  if (args)
  {
    if (PyTuple_CheckExact(args))
    {
      size_t argc = PyTuple_Size(args);
      if (argc == 1)
      {
        PyObject *lobj = PyTuple_GetItem(args, 0);
        if (PyString_CheckExact(lobj))
        {
          command_name = PyString_AsString(lobj);
        }
        else
        {
          PyErr_SetString(reinterpret_cast<PyObject *>(devsim_exception.GetObject()), "UNEXPECTED");
          return ret;
        }
      }
      else if (argc > 1)
      {
        std::ostringstream os;
        os << "Command " << command_name << " does not take positional arguments";
        PyErr_SetString(reinterpret_cast<PyObject *>(devsim_exception.GetObject()), os.str().c_str());
        return ret;
      }
    }
    else
    {
      PyErr_SetString(reinterpret_cast<PyObject *>(devsim_exception.GetObject()), "UNEXPECTED");
      return ret;
    }
  }

  std::map<std::string, newcmd>::iterator it = CommandMap.find(command_name);
  newcmd cmdptr = (it->second);
  if (it == CommandMap.end())
  {
    PyErr_SetString(reinterpret_cast<PyObject *>(devsim_exception.GetObject()), "UNEXPECTED");
    return ret;
  }

  try
  {

    dsGetArgs::CommandInfo command_info;
    command_info.self_         =  self;
    command_info.args_         =  args;
    command_info.kwargs_       =  kwargs;
    command_info.command_name_ =  command_name;
    command_info.exception_    =  reinterpret_cast<PyObject *>(devsim_exception.GetObject());

    CommandHandler data(&command_info);
    command_info.command_handler_ = &data;
    cmdptr(data);
    ret = reinterpret_cast<PyObject *>(data.GetReturnObject().GetObject());
    if (ret)
    {
      Py_INCREF(ret);
    }
  }
  catch (const dsException &x)
  {
    ret = NULL;
    PyErr_SetString(reinterpret_cast<PyObject *>(devsim_exception.GetObject()), x.what());
  }
  catch (std::bad_alloc &)
  {
    ret = NULL;
    PyErr_SetString(reinterpret_cast<PyObject *>(devsim_exception.GetObject()), const_cast<char *>("OUT OF MEMORY"));
  }
  catch (std::exception &)
  {
    ret = NULL;
    PyErr_SetString(reinterpret_cast<PyObject *>(devsim_exception.GetObject()), const_cast<char *>("UNEXPECTED ERROR"));
  }


  if (FPECheck::CheckFPE())
  {
    std::ostringstream os;
    os << "There was an uncaught floating point exception of type \"" << FPECheck::getFPEString() << "\"\n";
    FPECheck::ClearFPE();
    ret = NULL;
    OutputStream::WriteOut(OutputStream::ERROR, os.str().c_str());
  }


  return ret;
}
#endif

static PyObject * CmdDispatch(PyObject *self, PyObject *args, PyObject *kwargs, const char *name, newcmd fptr)
{
  PyObject *ret = NULL;

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
        PyErr_SetString(reinterpret_cast<PyObject *>(devsim_exception.GetObject()), os.str().c_str());
        return ret;
      }
    }
    else
    {
      PyErr_SetString(reinterpret_cast<PyObject *>(devsim_exception.GetObject()), "UNEXPECTED");
      return ret;
    }
  }

  try
  {

    dsGetArgs::CommandInfo command_info;
    command_info.self_         =  self;
    command_info.args_         =  args;
    if (kwargs) {
    Py_INCREF(kwargs);
    }
    command_info.kwargs_       =  kwargs;
    command_info.command_name_ =  command_name;
    command_info.exception_    =  reinterpret_cast<PyObject *>(devsim_exception.GetObject());

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
    ret = NULL;
    PyErr_SetString(reinterpret_cast<PyObject *>(devsim_exception.GetObject()), x.what());
  }
  catch (std::bad_alloc &)
  {
    ret = NULL;
    PyErr_SetString(reinterpret_cast<PyObject *>(devsim_exception.GetObject()), const_cast<char *>("OUT OF MEMORY"));
  }
  catch (std::exception &)
  {
    ret = NULL;
    PyErr_SetString(reinterpret_cast<PyObject *>(devsim_exception.GetObject()), const_cast<char *>("UNEXPECTED ERROR"));
  }


  if (FPECheck::CheckFPE())
  {
    std::ostringstream os;
    os << "There was an uncaught floating point exception of type"" << FPECheck::getFPEString() << ""n";
    FPECheck::ClearFPE();
    ret = NULL;
    OutputStream::WriteOut(OutputStream::ERROR, os.str().c_str());
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


#define MYCOMMAND(name, fptr) {#name, reinterpret_cast<PyCFunction>(name ## CmdDispatch), METH_KEYWORDS, name ## _doc}

#if 0
namespace {
bool CreateCommand(Interpreter &interp, const std::string &name)
{
  std::ostringstream os;
  os << "def " << name << "(*args, **kwargs):\n"
    "  return ds.devsim('" << name << "', *args, **kwargs)\n"
    "ds." << name << "=" << name << "\n"
    "del " << name << "\n"
    ;
  bool ok = interp.RunCommand(os.str());
  return ok;
}
}
#endif

// Geometry Commands
MyNewPyPtr(get_device_list,    dsCommand::getDeviceListCmd);
MyNewPyPtr(get_region_list,    dsCommand::getRegionListCmd);
MyNewPyPtr(get_interface_list, dsCommand::getRegionListCmd);
MyNewPyPtr(get_contact_list,   dsCommand::getRegionListCmd);
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
{NULL, NULL, 0, NULL}
};

void AddCommands()
{

  PyObject *module = Py_InitModule("ds", devsim_methods);
  PyObject *ex = PyErr_NewException(const_cast<char *>("ds.error"), NULL, NULL);
  devsim_exception = ObjectHolder(ex);
  PyModule_AddObject(module, "error", ex);

#if 0
  Interpreter interp;
  interp.RunCommand("import ds\n");
  dsCommand::Commands **tlist = clistlist;
  for ( ; (*tlist) != NULL; ++tlist)
  {
    dsCommand::Commands *clist = *tlist;
    for ( ; (clist->name) != NULL; ++clist)
    {
      CommandMap[clist->name] = clist->command;
      CreateCommand(interp, clist->name);
    }
  }
#endif
}

bool 
Commands_Init() {
#if 0
    bool ok = false;

    ok = true;
    if (ok)
    {
      dsCommand::Commands *clistlist[] =
      {
        dsCommand::GeometryCommands,
        dsCommand::MaterialCommands,
        dsCommand::ModelCommands,
        dsCommand::MathCommands,
        dsCommand::EquationCommands,
        dsCommand::MeshingCommands,
        dsCommand::CircuitCommands,
        NULL
      };
      AddCommands(clistlist);
    }
#endif
  AddCommands();

  return true;
}
}

