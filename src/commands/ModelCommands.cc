/***
DEVSIM
Copyright 2013 DEVSIM LLC

SPDX-License-Identifier: Apache-2.0
***/

#include "ModelCommands.hh"
#include "dsCommand.hh"
#include "CommandHandler.hh"

#include "Interface.hh"
#include "EquationFunctions.hh"
#include "Device.hh"
#include "Region.hh"
#include "Contact.hh"
#include "NodeModel.hh"
#include "EdgeModel.hh"
#include "NodeSolution.hh"
#include "EdgeSubModel.hh"
#include "TriangleEdgeSubModel.hh"
#include "TetrahedronEdgeSubModel.hh"
#include "InterfaceNodeModel.hh"
#include "EdgeFromNodeModel.hh"
#include "TriangleEdgeModel.hh"
#include "TetrahedronEdgeModel.hh"
#include "VectorGradient.hh"
#include "CylindricalSurfaceArea.hh"

#include "TriangleEdgeFromEdgeModel.hh"
#include "TriangleEdgeFromEdgeModelDerivative.hh"
#include "TriangleEdgePairFromEdgeModel.hh"
#include "TriangleEdgePairFromEdgeModelDerivative.hh"

#include "TetrahedronEdgeFromEdgeModel.hh"
#include "TetrahedronEdgeFromEdgeModelDerivative.hh"
#include "TetrahedronEdgePairFromEdgeModel.hh"
#include "TetrahedronEdgePairFromEdgeModelDerivative.hh"

#include "TriangleEdgeFromNodeModel.hh"
#include "TetrahedronEdgeFromNodeModel.hh"

#include "InterfaceNormal.hh"

#include "CheckFunctions.hh"
#include "Validate.hh"

#include "OutputStream.hh"
#include "dsAssert.hh"
#include "MathEval.hh"

#include "TriangleCylindricalNodeVolume.hh"
#include "TriangleCylindricalEdgeCouple.hh"
#include "CylindricalNodeVolume.hh"
#include "CylindricalEdgeNodeVolume.hh"
#include "CylindricalEdgeCouple.hh"

#include "AverageEdgeModel.hh"

#include "VectorTriangleEdgeModel.hh"
#include "VectorTetrahedronEdgeModel.hh"

#include <sstream>
#include <iomanip>
#include <utility>
#include <functional>

using namespace dsValidate;

///// This can be removed once we move debug code elsewhere
//#include <iostream>
#include "Node.hh"
#include "Edge.hh"
#include "Triangle.hh"

#include <sstream>

namespace dsCommand {
void
createNodeSolutionCmd(CommandHandler &data)
{
    std::string errorString;
    const std::string &commandName = data.GetCommandName();

    using namespace dsGetArgs;
    static dsGetArgs::Option option[] =
    {
        {"device",   "", dsGetArgs::optionType::STRING, dsGetArgs::requiredType::REQUIRED, mustBeValidDevice},
        {"region",   "", dsGetArgs::optionType::STRING, dsGetArgs::requiredType::REQUIRED, mustBeValidRegion},
        {"name",     "", dsGetArgs::optionType::STRING, dsGetArgs::requiredType::REQUIRED, stringCannotBeEmpty},
        {nullptr,  nullptr, dsGetArgs::optionType::STRING, dsGetArgs::requiredType::OPTIONAL, nullptr}
    };

    bool error = data.processOptions(option, errorString);

    if (error)
    {
        data.SetErrorResult(errorString);
        return;
    }

    const std::string &deviceName = data.GetStringOption("device");
    const std::string &regionName = data.GetStringOption("region");
    const std::string &name = data.GetStringOption("name");

    Device *dev = nullptr;
    Region *reg = nullptr;
    errorString = ValidateDeviceAndRegion(deviceName, regionName, dev, reg);
    if (!errorString.empty())
    {
        data.SetErrorResult(errorString);
        return;
    }

    if (commandName == "node_solution")
    {
      CreateNodeSolution(name, reg, NodeModel::DisplayType::SCALAR);
    }
    else if (commandName == "edge_solution")
    {
      CreateEdgeSubModel(name, reg, EdgeModel::DisplayType::SCALAR);
    }
    else if (commandName == "element_solution")
    {
      const size_t dimension = dev->GetDimension();
      if (dimension == 1)
      {
        errorString += "1D not supported for " + commandName + "\n";
      }
      else if (dimension == 2)
      {
        CreateTriangleEdgeSubModel(name, reg, TriangleEdgeModel::DisplayType::SCALAR);
      }
      else if (dimension == 3)
      {
        CreateTetrahedronEdgeSubModel(name, reg, TetrahedronEdgeModel::DisplayType::SCALAR);
      }
    }
    else
    {
      errorString += "command not processed";
    }

    if (errorString.empty())
    {
      data.SetEmptyResult();
    }
    else
    {
      data.SetErrorResult(errorString);
    }

    return;
}

/// Leverages both node and edge model
void
createNodeModelCmd(CommandHandler &data)
{
    std::string errorString;
    dsHelper::ret_pair result = std::make_pair(false, errorString);

//    const std::string commandName = data.GetCommandName();

//    GlobalData &gdata = GlobalData::GetInstance();

    using namespace dsGetArgs;
    static dsGetArgs::Option option[] =
    {
        {"device",   "", dsGetArgs::optionType::STRING, dsGetArgs::requiredType::REQUIRED, mustBeValidDevice},
        {"region",   "", dsGetArgs::optionType::STRING, dsGetArgs::requiredType::REQUIRED, mustBeValidRegion},
        {"name",   "",   dsGetArgs::optionType::STRING, dsGetArgs::requiredType::REQUIRED, stringCannotBeEmpty},
        {"equation",   "", dsGetArgs::optionType::STRING, dsGetArgs::requiredType::REQUIRED, nullptr},
        {"display_type",   "", dsGetArgs::optionType::STRING, dsGetArgs::requiredType::OPTIONAL, nullptr},
        {nullptr,  nullptr, dsGetArgs::optionType::STRING, dsGetArgs::requiredType::OPTIONAL}
    };

    bool error = data.processOptions(option, errorString);

    if (error)
    {
        data.SetErrorResult(errorString);
        return;
    }

    const std::string commandName = data.GetCommandName();

    const std::string &deviceName = data.GetStringOption("device");
    const std::string &regionName = data.GetStringOption("region");
    const std::string &name = data.GetStringOption("name");
    const std::string &equation = data.GetStringOption("equation");
    std::string dtype = data.GetStringOption("display_type");

    Device *dev = nullptr;
    Region *reg = nullptr;
    errorString = ValidateDeviceAndRegion(deviceName, regionName, dev, reg);

    const size_t dimension = dev->GetDimension();

    NodeModel::DisplayType            ndt  = NodeModel::DisplayType::SCALAR;
    EdgeModel::DisplayType            edt  = EdgeModel::DisplayType::SCALAR;
    TriangleEdgeModel::DisplayType    trdt = TriangleEdgeModel::DisplayType::SCALAR;
    TetrahedronEdgeModel::DisplayType tedt = TetrahedronEdgeModel::DisplayType::SCALAR;

    if (!dtype.empty())
    {
      if (commandName == "node_model")
      {
        ndt = dsHelper::getNodeModelDisplayType(dtype);
      }
      else if (commandName == "edge_model")
      {
        edt = dsHelper::getEdgeModelDisplayType(dtype);
      }
      else if (commandName == "element_model")
      {
        if (dimension == 2)
        {
          trdt = dsHelper::getTriangleEdgeModelDisplayType(dtype);
        }
        else if (dimension == 3)
        {
          tedt = dsHelper::getTetrahedronEdgeModelDisplayType(dtype);
        }
      }

      if ((ndt == NodeModel::DisplayType::UNKNOWN) ||
          (edt == EdgeModel::DisplayType::UNKNOWN) ||
          (trdt == TriangleEdgeModel::DisplayType::UNKNOWN) ||
          (tedt == TetrahedronEdgeModel::DisplayType::UNKNOWN))
      {
        errorString += "display_type \"" + dtype + "\" is not a valid option\n";
      }
    }

    if (!errorString.empty())
    {
        data.SetErrorResult(errorString);
        return;
    }

    ConstNodeModelPtr existingNodeModel = reg->GetNodeModel(name);
    ConstEdgeModelPtr existingEdgeModel = reg->GetEdgeModel(name);
    ConstTriangleEdgeModelPtr existingTriangleEdgeModel = reg->GetTriangleEdgeModel(name);
    ConstTetrahedronEdgeModelPtr existingTetrahedronEdgeModel = reg->GetTetrahedronEdgeModel(name);
// TODO:"assert 2d or 3d"

    if (commandName == "node_model")
    {
        if (existingEdgeModel)
        {
          std::ostringstream os;
          os << "A node model cannot replace a edge model by the same name " << name << "\n";
          errorString += os.str();
        }
        else if (existingTriangleEdgeModel)
        {
          std::ostringstream os;
          os << "A node model cannot replace a triangle edge model by the same name " << name << "\n";
          errorString += os.str();
        }
        else if (existingTetrahedronEdgeModel)
        {
          std::ostringstream os;
          os << "A node model cannot replace a tetrahedron edge model by the same name " << name << "\n";
          errorString += os.str();
        }
        else
        {
          existingNodeModel.reset();
          result = dsHelper::CreateNodeExprModel(name, equation, reg, ndt);
          data.SetEmptyResult();
        }
    }
    else if (commandName == "edge_model")
    {

        if (existingNodeModel)
        {
          std::ostringstream os;
          os << "A node model cannot replace an edge model by the same name " << name << "\n";
          errorString += os.str();
        }
        else if (existingTriangleEdgeModel)
        {
          std::ostringstream os;
          os << "An edge model cannot replace a triangle edge model by the same name " << name << "\n";
          errorString += os.str();
        }
        else if (existingTetrahedronEdgeModel)
        {
          std::ostringstream os;
          os << "An edge model cannot replace a tetrahedron edge model by the same name " << name << "\n";
          errorString += os.str();
        }
        else
        {
          existingEdgeModel.reset();
          result = dsHelper::CreateEdgeExprModel(name, equation, reg, edt);
        }
    }
    else if (commandName == "element_model")
    {
      if (dimension == 1)
      {
        std::ostringstream os;
        os << commandName << " not supported in 1 D\n";
        errorString += os.str();
      }
      else if (dimension == 2)
      {
        if (existingNodeModel)
        {
          std::ostringstream os;
          os << "A triangle edge model cannot replace a node model by the same name " << name << "\n";
          errorString += os.str();
        }
        else if (existingEdgeModel)
        {
          std::ostringstream os;
          os << "A triangle edge model cannot replace a edge model by the same name " << name << "\n";
          errorString += os.str();
        }
        else if (existingTetrahedronEdgeModel)
        {
          std::ostringstream os;
          os << "An triangle edge model cannot replace a tetrahedron edge model by the same name " << name << "\n";
          errorString += os.str();
        }
        else
        {
          existingTriangleEdgeModel.reset();
          result = dsHelper::CreateTriangleEdgeExprModel(name, equation, reg, trdt);
        }
      }
      else if (dimension == 3)
      {
        if (existingNodeModel)
        {
          std::ostringstream os;
          os << "A tetrahedron edge model cannot replace a node model by the same name " << name << "\n";
          errorString += os.str();
        }
        else if (existingEdgeModel)
        {
          std::ostringstream os;
          os << "A tetrahedron edge model cannot replace a edge model by the same name " << name << "\n";
          errorString += os.str();
        }
        else if (existingTriangleEdgeModel)
        {
          std::ostringstream os;
          os << "An tetrahedron edge model cannot replace a triangle edge model by the same name " << name << "\n";
          errorString += os.str();
        }
        else
        {
          existingTetrahedronEdgeModel.reset();
          result = dsHelper::CreateTetrahedronEdgeExprModel(name, equation, reg, tedt);
        }
      }
    }
    else
    {
        dsAssert(0, "UNEXPECTED");
    }

    if (!result.first)
    {
      errorString += result.second;
    }

    if (!errorString.empty())
    {
        std::ostringstream os;
        os << "While creating equation " << onRegiononDevice(regionName, deviceName) << "\n";
        errorString = os.str() + errorString;
        data.SetErrorResult(errorString);
        return;
    }
    else
    {
        data.SetStringResult(result.second);
    }
}

/// Leverages both node and edge model
void
createContactNodeModelCmd(CommandHandler &data)
{
    std::string errorString;
    dsHelper::ret_pair result = std::make_pair(false, errorString);

//    const std::string commandName = data.GetCommandName();

//    GlobalData &gdata = GlobalData::GetInstance();

    using namespace dsGetArgs;
    static dsGetArgs::Option option[] =
    {
        {"device",   "", dsGetArgs::optionType::STRING, dsGetArgs::requiredType::REQUIRED, mustBeValidDevice},
        {"contact",   "", dsGetArgs::optionType::STRING, dsGetArgs::requiredType::REQUIRED, mustBeValidContact},
        {"name",   "",   dsGetArgs::optionType::STRING, dsGetArgs::requiredType::REQUIRED, stringCannotBeEmpty},
        {"equation",   "", dsGetArgs::optionType::STRING, dsGetArgs::requiredType::REQUIRED, nullptr},
        {"display_type",   "", dsGetArgs::optionType::STRING, dsGetArgs::requiredType::OPTIONAL, nullptr},
        {nullptr,  nullptr, dsGetArgs::optionType::STRING, dsGetArgs::requiredType::OPTIONAL}
    };

    bool error = data.processOptions(option, errorString);

    if (error)
    {
        data.SetErrorResult(errorString);
        return;
    }

    const std::string commandName = data.GetCommandName();

    const std::string &deviceName = data.GetStringOption("device");
    const std::string &contactName = data.GetStringOption("contact");
    const std::string &name = data.GetStringOption("name");
    const std::string &equation = data.GetStringOption("equation");
    std::string dtype = data.GetStringOption("display_type");

    Device *dev = nullptr;
    Contact *cp = nullptr;
    errorString = ValidateDeviceAndContact(deviceName, contactName, dev, cp);

    NodeModel::DisplayType ndt = NodeModel::DisplayType::SCALAR;
    EdgeModel::DisplayType edt = EdgeModel::DisplayType::VECTOR;

    if (commandName == "contact_node_model")
    {
      if (!dtype.empty())
      {
        ndt = dsHelper::getNodeModelDisplayType(dtype);
      }
      if (ndt == NodeModel::DisplayType::UNKNOWN)
      {
        errorString += "display_type \"" + dtype + "\" is not a valid option for contact_node_model\n";
      }
    }
    else if (commandName == "contact_edge_model")
    {
      if (!dtype.empty())
      {
        edt = dsHelper::getEdgeModelDisplayType(dtype);
      }
      if (edt == EdgeModel::DisplayType::UNKNOWN)
      {
        errorString += "display_type \"" + dtype + "\" is not a valid option for contact_edge_model\n";
      }
    }
    if (!errorString.empty())
    {
        data.SetErrorResult(errorString);
        return;
    }

    RegionPtr reg = const_cast<Region *>(cp->GetRegion());
    ConstNodeModelPtr existingNodeModel = reg->GetNodeModel(name);
    ConstEdgeModelPtr existingEdgeModel = reg->GetEdgeModel(name);

    if (commandName == "contact_node_model")
    {
        if (existingEdgeModel)
        {
          std::ostringstream os;
          os << "An edge model cannot replace a node model by the same name " << name << "\n";
          errorString += os.str();
        }
        else
        {
          existingNodeModel.reset();
          result = dsHelper::CreateNodeExprModel(name, equation, reg, ndt, cp);
        }
    }
    else if (commandName == "contact_edge_model")
    {
        if (existingNodeModel)
        {
          std::ostringstream os;
          os << "A node model cannot replace an edge model by the same name " << name << "\n";
          errorString += os.str();
        }
        else
        {
          existingEdgeModel.reset();
          result = dsHelper::CreateEdgeExprModel(name, equation, reg, edt, cp);
        }
    }
    else
    {
        dsAssert(0, "UNEXPECTED");
    }

    if (!result.first)
    {
      errorString += result.second;
    }

    if (!errorString.empty())
    {
        std::ostringstream os;
        os << "While creating equation " << onContactonDevice(contactName, deviceName) << "\n";
        errorString = os.str() + errorString;
        data.SetErrorResult(errorString);
        return;
    }
    else
    {
        data.SetStringResult(result.second);
    }

    return;
}

void
createCylindricalCmd(CommandHandler &data)
{
  std::string errorString;

  const std::string commandName = data.GetCommandName();

//    GlobalData &gdata = GlobalData::GetInstance();

  using namespace dsGetArgs;
  static dsGetArgs::Option option[] =
  {
    {"device",         "", dsGetArgs::optionType::STRING, dsGetArgs::requiredType::REQUIRED, mustBeValidDevice},
    {"region",         "", dsGetArgs::optionType::STRING, dsGetArgs::requiredType::REQUIRED, mustBeValidRegion},
    {nullptr,  nullptr, dsGetArgs::optionType::STRING, dsGetArgs::requiredType::OPTIONAL}
  };

  bool error = data.processOptions(option, errorString);

  if (error)
  {
      data.SetErrorResult(errorString);
      return;
  }

  const std::string &deviceName    = data.GetStringOption("device");
  const std::string &regionName    = data.GetStringOption("region");

  Device *dev = nullptr;
  Region *reg = nullptr;

  errorString = ValidateDeviceAndRegion(deviceName, regionName, dev, reg);
  if (!errorString.empty())
  {
    data.SetErrorResult(errorString);
    return;
  }

  const size_t dimension = dev->GetDimension();

  if (dimension != 2)
  {
    std::ostringstream os;
    os << "Device " << dev->GetName() << " is only supported in 2d\n";
    errorString += os.str();
  }

  if (!errorString.empty())
  {
    data.SetErrorResult(errorString);
    return;
  }

  if (commandName == "cylindrical_edge_couple")
  {
    CreateTriangleCylindricalEdgeCouple(reg);
    CreateCylindricalEdgeCouple(reg);
    data.SetEmptyResult();
  }
  else if  (commandName == "cylindrical_node_volume")
  {
    CreateTriangleCylindricalNodeVolume(reg);
    CreateCylindricalNodeVolume(reg);
    CreateCylindricalEdgeNodeVolume(reg);
    data.SetEmptyResult();
  }
  else if  (commandName == "cylindrical_surface_area")
  {
    CreateCylindricalSurfaceArea(reg);
    data.SetEmptyResult();
  }
  else
  {
    dsAssert(0, "UNEXPECTED");
  }
}

void
createEdgeFromNodeModelCmd(CommandHandler &data)
{
  std::string errorString;

//    const std::string commandName = data.GetCommandName();

//    GlobalData &gdata = GlobalData::GetInstance();

  using namespace dsGetArgs;
  static dsGetArgs::Option option[] =
  {
    {"device",     "", dsGetArgs::optionType::STRING, dsGetArgs::requiredType::REQUIRED, mustBeValidDevice},
    {"region",     "", dsGetArgs::optionType::STRING, dsGetArgs::requiredType::REQUIRED, mustBeValidRegion},
    {"node_model", "", dsGetArgs::optionType::STRING, dsGetArgs::requiredType::REQUIRED, stringCannotBeEmpty},
    {"calc_type", "default", dsGetArgs::optionType::STRING, dsGetArgs::requiredType::OPTIONAL, stringCannotBeEmpty},
    {nullptr,  nullptr, dsGetArgs::optionType::STRING, dsGetArgs::requiredType::OPTIONAL}
  };

  bool error = data.processOptions(option, errorString);

  if (error)
  {
      data.SetErrorResult(errorString);
      return;
  }

  const std::string commandName = data.GetCommandName();

  const std::string &deviceName = data.GetStringOption("device");
  const std::string &regionName = data.GetStringOption("region");
  const std::string &name = data.GetStringOption("node_model");

  Device *dev = nullptr;
  Region *reg = nullptr;

  errorString = ValidateDeviceAndRegion(deviceName, regionName, dev, reg);
  if (!errorString.empty())
  {
    data.SetErrorResult(errorString);
    return;
  }

  ConstNodeModelPtr existingNodeModel = reg->GetNodeModel(name);
  if (!existingNodeModel)
  {
    std::ostringstream os;
    os << name << " is not an existing node model";
    errorString = os.str();
    data.SetErrorResult(errorString);
    return;
  }

  if (commandName == "edge_from_node_model")
  {
    std::string em0 = name + "@n0";
    std::string em1 = name + "@n1";
    /// Need to test that these edge models don't already exist
    CreateEdgeFromNodeModel(em0, em1, name, reg);
    data.SetEmptyResult();
  }
  else if (commandName == "element_from_node_model")
  {
    const size_t dimension = dev->GetDimension();

    if (dimension == 1)
    {
      std::ostringstream os;
      os << commandName << " not supported in 1 D\n";
      errorString = os.str();
      data.SetErrorResult(errorString);
      return;
    }
    if (dimension == 2)
    {
      std::string em0 = name + "@en0";
      std::string em1 = name + "@en1";
      std::string em2 = name + "@en2";
      CreateTriangleEdgeFromNodeModel(em0, em1, em2, name, reg);
      data.SetEmptyResult();
    }
    else if (dimension == 3)
    {
      std::string em0 = name + "@en0";
      std::string em1 = name + "@en1";
      std::string em2 = name + "@en2";
      std::string em3 = name + "@en3";
      CreateTetrahedronEdgeFromNodeModel(em0, em1, em2, em3, name, reg);
      data.SetEmptyResult();
    }
  }
  else if (commandName == "vector_gradient")
  {
    const std::string &calctype = data.GetStringOption("calc_type");
    if (calctype == "avoidzero")
    {
      CreateVectorGradient(reg, name, VectorGradientEnum::AVOIDZERO);
      data.SetEmptyResult();
    }
    else if (calctype == "default")
    {
      CreateVectorGradient(reg, name, VectorGradientEnum::DEFAULT);
      data.SetEmptyResult();
    }
    else
    {
      errorString += "-calc_type must be \"avoidzero\" or \"default\"\n";
      data.SetErrorResult(errorString);
      return;
    }
  }
  else
  {
    dsAssert(0, "UNEXPECTED");
  }
}

void
createEdgeAverageModelCmd(CommandHandler &data)
{
  std::string errorString;

//    const std::string commandName = data.GetCommandName();

//    GlobalData &gdata = GlobalData::GetInstance();

  using namespace dsGetArgs;
  static dsGetArgs::Option option[] =
  {
    {"device",       "", dsGetArgs::optionType::STRING, dsGetArgs::requiredType::REQUIRED, mustBeValidDevice},
    {"region",       "", dsGetArgs::optionType::STRING, dsGetArgs::requiredType::REQUIRED, mustBeValidRegion},
    {"node_model",   "", dsGetArgs::optionType::STRING, dsGetArgs::requiredType::REQUIRED, stringCannotBeEmpty},
    {"edge_model",   "", dsGetArgs::optionType::STRING, dsGetArgs::requiredType::REQUIRED, stringCannotBeEmpty},
    {"average_type", "arithmetic", dsGetArgs::optionType::STRING, dsGetArgs::requiredType::OPTIONAL, stringCannotBeEmpty},
    {"derivative", "", dsGetArgs::optionType::STRING, dsGetArgs::requiredType::OPTIONAL, nullptr},
    {nullptr,  nullptr, dsGetArgs::optionType::STRING, dsGetArgs::requiredType::OPTIONAL}
  };

  bool error = data.processOptions(option, errorString);

  if (error)
  {
      data.SetErrorResult(errorString);
      return;
  }

//  const std::string commandName = data.GetCommandName();

  const std::string &deviceName       = data.GetStringOption("device");
  const std::string &regionName       = data.GetStringOption("region");
  const std::string &nodeModel        = data.GetStringOption("node_model");
  const std::string &edgeModel        = data.GetStringOption("edge_model");
  const std::string &derivativeModel  = data.GetStringOption("derivative");
  const std::string &averageType      = data.GetStringOption("average_type");

  Device *dev = nullptr;
  Region *reg = nullptr;

  errorString = ValidateDeviceAndRegion(deviceName, regionName, dev, reg);
  if (!errorString.empty())
  {
    data.SetErrorResult(errorString);
    return;
  }

  ConstNodeModelPtr existingNodeModel = reg->GetNodeModel(nodeModel);
  if (!existingNodeModel)
  {
    std::ostringstream os;
    os << nodeModel << " is not an existing node model";
    errorString = os.str();
    data.SetErrorResult(errorString);
    return;
  }

  if (!derivativeModel.empty())
  {
    ConstNodeModelPtr dModel = reg->GetNodeModel(derivativeModel);
    if (!dModel)
    {
      std::ostringstream os;
      os << derivativeModel << " is not an existing node model";
      errorString = os.str();
      data.SetErrorResult(errorString);
      return;
    }
  }

  AverageEdgeModelEnum::AverageType_t atype = AverageEdgeModelEnum::GetTypeName(averageType, errorString);
  if (atype == AverageEdgeModelEnum::UNKNOWN)
  {
    data.SetErrorResult(errorString);
    return;
  }

  if (derivativeModel.empty())
  {
    CreateAverageEdgeModel(edgeModel, nodeModel, atype, reg);
    data.SetEmptyResult();
  }
  else
  {
    CreateAverageEdgeModel(edgeModel, nodeModel, derivativeModel, atype, reg);
    data.SetEmptyResult();
  }

}

/// Leverages both node and edge model
void
createInterfaceNodeModelCmd(CommandHandler &data)
{
    std::string errorString;
    dsHelper::ret_pair result = std::make_pair(false, errorString);

//    const std::string commandName = data.GetCommandName();

//    GlobalData &gdata = GlobalData::GetInstance();

    using namespace dsGetArgs;
    static dsGetArgs::Option option[] =
    {
        {"device",    "", dsGetArgs::optionType::STRING, dsGetArgs::requiredType::REQUIRED, mustBeValidDevice},
        {"interface", "", dsGetArgs::optionType::STRING, dsGetArgs::requiredType::REQUIRED, stringCannotBeEmpty},
        {"name",      "", dsGetArgs::optionType::STRING, dsGetArgs::requiredType::REQUIRED, stringCannotBeEmpty},
        {"equation","", dsGetArgs::optionType::STRING, dsGetArgs::requiredType::REQUIRED, stringCannotBeEmpty},
        {nullptr,  nullptr, dsGetArgs::optionType::STRING, dsGetArgs::requiredType::OPTIONAL, nullptr}
    };

    bool error = data.processOptions(option, errorString);

    if (error)
    {
        data.SetErrorResult(errorString);
        return;
    }

//    const std::string commandName = data.GetCommandName();

    const std::string &deviceName = data.GetStringOption("device");
    const std::string &interfaceName = data.GetStringOption("interface");
    const std::string &name = data.GetStringOption("name");
    const std::string &equation = data.GetStringOption("equation");

    Device    *dev = nullptr;
    Interface *interface = nullptr;

    errorString = ValidateDeviceAndInterface(deviceName, interfaceName, dev, interface);
    if (!errorString.empty())
    {
        data.SetErrorResult(errorString);
        return;
    }

    result = dsHelper::CreateInterfaceNodeExprModel(name, equation, interface);

    if (!result.first)
    {
      errorString += result.second;
    }

    if (!errorString.empty())
    {
        std::ostringstream os;
        os << "While creating equation " << onInterfaceonDevice(interfaceName, deviceName) << "\n";
        errorString = os.str() + errorString;
        data.SetErrorResult(errorString);
        return;
    }
    else
    {
        data.SetStringResult(result.second);
    }
}

void
setNodeValuesCmd(CommandHandler &data)
{
    std::string errorString;

    const std::string commandName = data.GetCommandName();

    using namespace dsGetArgs;
    static dsGetArgs::Option option[] =
    {
        {"device",   "", dsGetArgs::optionType::STRING, dsGetArgs::requiredType::REQUIRED, mustBeValidDevice},
        {"region",   "", dsGetArgs::optionType::STRING, dsGetArgs::requiredType::REQUIRED, stringCannotBeEmpty},
        {"name",     "", dsGetArgs::optionType::STRING, dsGetArgs::requiredType::REQUIRED, stringCannotBeEmpty},
        {"init_from", "", dsGetArgs::optionType::STRING, dsGetArgs::requiredType::OPTIONAL, nullptr},
        {"values",    "", dsGetArgs::optionType::LIST,   dsGetArgs::requiredType::OPTIONAL, nullptr},
        {nullptr,  nullptr, dsGetArgs::optionType::STRING, dsGetArgs::requiredType::OPTIONAL}
    };

    bool error = data.processOptions(option, errorString);

    if (error)
    {
        data.SetErrorResult(errorString);
        return;
    }

    const std::string &deviceName = data.GetStringOption("device");
    const std::string &regionName = data.GetStringOption("region");
    const std::string &name = data.GetStringOption("name");
    const std::string &initializer = data.GetStringOption("init_from");

    std::vector<double> values;
    ObjectHolder vdata = data.GetObjectHolder("values");
    if (vdata.IsList())
    {
      if (!initializer.empty())
      {
        std::ostringstream os;
        os << "Options \"values\" and \"init_from\" should not be specified at the same time\n";
        errorString += os.str();
      }
      bool ok = vdata.GetDoubleList(values);
      if (!ok)
      {
        std::ostringstream os;
        os << "Option \"values\" could not be converted to a list of doubles\n";
        errorString += os.str();
      }
    }

    Device *dev = nullptr;
    Region *reg = nullptr;

    errorString = ValidateDeviceAndRegion(deviceName, regionName, dev, reg);

    if (!errorString.empty())
    {
        data.SetErrorResult(errorString);
        return;
    }

    const auto dimension = dev->GetDimension();

    size_t values_expected = 0;

    std::function<void(const std::vector<double> &)> setter;
#ifdef DEVSIM_EXTENDED_PRECISION
    std::function<void(const std::vector<float128> &)> extended_setter;
    std::vector<float128> extended_values;
#endif

    bool model_exists = false;
    bool initializer_exists = false;

    if (commandName == "set_node_values")
    {
      auto nm_name        = std::const_pointer_cast<NodeModel, const NodeModel>(reg->GetNodeModel(name));
      auto nm_initializer = reg->GetNodeModel(initializer);
      values_expected = reg->GetNumberNodes();

      model_exists       = static_cast<bool>(nm_name);
      initializer_exists = static_cast<bool>(nm_initializer);

      setter          = [nm_name](const std::vector<double> &v) {nm_name->SetValues(v);};
#ifdef DEVSIM_EXTENDED_PRECISION
      extended_setter = [nm_name](const std::vector<float128> &v) {nm_name->SetValues(v);};
#endif

      if (!initializer_exists)
      {
      }
      else if (std::dynamic_pointer_cast<NodeSolution<double>>(nm_name))
      {
          values = nm_initializer->GetScalarValues<double>();
      }
#ifdef DEVSIM_EXTENDED_PRECISION
      else if (std::dynamic_pointer_cast<NodeSolution<float128>>(nm_name))
      {
          extended_values = nm_initializer->GetScalarValues<float128>();
      }
#endif
    }
    else if (commandName == "set_edge_values")
    {
      auto nm_name        = std::const_pointer_cast<EdgeModel, const EdgeModel>(reg->GetEdgeModel(name));
      auto nm_initializer = reg->GetEdgeModel(initializer);
      values_expected = reg->GetNumberEdges();

      model_exists       = static_cast<bool>(nm_name);
      initializer_exists = static_cast<bool>(nm_initializer);

      setter          = [nm_name](const std::vector<double> &v) {nm_name->SetValues(v);};
#ifdef DEVSIM_EXTENDED_PRECISION
      extended_setter = [nm_name](const std::vector<float128> &v) {nm_name->SetValues(v);};
#endif

      if (!initializer_exists)
      {
      }
      else if (std::dynamic_pointer_cast<EdgeSubModel<double>>(nm_name))
      {
          values = nm_initializer->GetScalarValues<double>();
      }
#ifdef DEVSIM_EXTENDED_PRECISION
      else if (std::dynamic_pointer_cast<EdgeSubModel<float128>>(nm_name))
      {
          extended_values = nm_initializer->GetScalarValues<float128>();
      }
#endif
    }
    else if (commandName == "set_element_values")
    {
      if (dimension == 1)
      {
        errorString += "1D not supported for " + commandName + "\n";
      }
      else if (dimension == 2)
      {
        auto nm_name        = std::const_pointer_cast<TriangleEdgeModel, const TriangleEdgeModel>(reg->GetTriangleEdgeModel(name));
        auto nm_initializer = reg->GetTriangleEdgeModel(initializer);
        values_expected = (3 * reg->GetNumberTriangles());

        model_exists       = static_cast<bool>(nm_name);
        initializer_exists = static_cast<bool>(nm_initializer);

        setter          = [nm_name](const std::vector<double> &v) {nm_name->SetValues(v);};
#ifdef DEVSIM_EXTENDED_PRECISION
        extended_setter = [nm_name](const std::vector<float128> &v) {nm_name->SetValues(v);};
#endif

        if (!initializer_exists)
        {
        }
        else if (std::dynamic_pointer_cast<TriangleEdgeSubModel<double>>(nm_name))
        {
            values = nm_initializer->GetScalarValues<double>();
        }
#ifdef DEVSIM_EXTENDED_PRECISION
        else if (std::dynamic_pointer_cast<TriangleEdgeSubModel<float128>>(nm_name))
        {
            extended_values = nm_initializer->GetScalarValues<float128>();
        }
#endif
      }
      else if (dimension == 3)
      {
        auto nm_name        = std::const_pointer_cast<TetrahedronEdgeModel, const TetrahedronEdgeModel>(reg->GetTetrahedronEdgeModel(name));
        auto nm_initializer = reg->GetTetrahedronEdgeModel(initializer);
        values_expected = (6 * reg->GetNumberTetrahedrons());

        model_exists       = static_cast<bool>(nm_name);
        initializer_exists = static_cast<bool>(nm_initializer);

        setter          = [nm_name](const std::vector<double> &v) {nm_name->SetValues(v);};
#ifdef DEVSIM_EXTENDED_PRECISION
        extended_setter = [nm_name](const std::vector<float128> &v) {nm_name->SetValues(v);};
#endif

        if (!initializer_exists)
        {
        }
        else if (std::dynamic_pointer_cast<TetrahedronEdgeSubModel<double>>(nm_name))
        {
            values = nm_initializer->GetScalarValues<double>();
        }
#ifdef DEVSIM_EXTENDED_PRECISION
        else if (std::dynamic_pointer_cast<TetrahedronEdgeSubModel<float128>>(nm_name))
        {
            extended_values = nm_initializer->GetScalarValues<float128>();
        }
#endif
      }
    }

    if (!model_exists)
    {
      std::ostringstream os;
      os << "Model " << name << " does not exist\n";
      errorString += os.str();
    }
    else if ((!initializer.empty()) && (!initializer_exists))
    {
      std::ostringstream os;
      os << "-init_from " << initializer << " does not exist\n";
      errorString += os.str();
    }
    else if (values.size() == values_expected)
    {
      dsAssert(static_cast<bool>(setter), "UNEXPECTED");
      setter(values);
    }
#ifdef DEVSIM_EXTENDED_PRECISION
    else if (extended_values.size() == values_expected)
    {
      dsAssert(static_cast<bool>(setter), "UNEXPECTED");
      extended_setter(extended_values);
    }
#endif
    else
    {
      std::ostringstream os;
      os << "wrong number of elements\n";
      errorString += os.str();
    }

    if (!errorString.empty())
    {
      data.SetErrorResult(errorString);
    }
    else
    {
      data.SetEmptyResult();
    }
}

void
setNodeValueCmd(CommandHandler &data)
{
    std::string errorString;

    using namespace dsGetArgs;
    static dsGetArgs::Option option[] =
    {
        {"device",   "", dsGetArgs::optionType::STRING, dsGetArgs::requiredType::REQUIRED, mustBeValidDevice},
        {"region",   "", dsGetArgs::optionType::STRING, dsGetArgs::requiredType::REQUIRED, stringCannotBeEmpty},
        {"name",     "", dsGetArgs::optionType::STRING, dsGetArgs::requiredType::REQUIRED, stringCannotBeEmpty},
        {"index", "-1", dsGetArgs::optionType::INTEGER, dsGetArgs::requiredType::OPTIONAL, nullptr},
        {"value", "0.0", dsGetArgs::optionType::FLOAT, dsGetArgs::requiredType::REQUIRED,  nullptr},
        {nullptr,  nullptr, dsGetArgs::optionType::STRING, dsGetArgs::requiredType::OPTIONAL}
    };

    bool error = data.processOptions(option, errorString);

    if (error)
    {
        data.SetErrorResult(errorString);
        return;
    }

    const std::string &deviceName = data.GetStringOption("device");
    const std::string &regionName = data.GetStringOption("region");
    const std::string &name = data.GetStringOption("name");
    const int index = data.GetIntegerOption("index");
    const double value = data.GetDoubleOption("value");

    Device *dev = nullptr;
    Region *reg = nullptr;

    errorString = ValidateDeviceAndRegion(deviceName, regionName, dev, reg);

    if (!errorString.empty())
    {
        data.SetErrorResult(errorString);
        return;
    }

    ConstNodeModelPtr nm_name        = reg->GetNodeModel(name);

    if (!nm_name.get())
    {
        std::ostringstream os;
        os << "Model " << name << " does not exist\n";
        errorString += os.str();
    }
    else if ((index < -1) || ((index != -1) && (static_cast<size_t>(index) >= nm_name->GetRegion().GetNumberNodes())))
    {
        std::ostringstream os;
        os << "-index " << index << " does not exist\n";
        errorString += os.str();
    }
    else if (index == -1)
    {
      std::const_pointer_cast<NodeModel, const NodeModel>(nm_name)->SetValues(value);
    }
    else
    {
      std::const_pointer_cast<NodeModel, const NodeModel>(nm_name)->SetNodeValue(index, value);
    }

    if (!errorString.empty())
    {
        data.SetErrorResult(errorString);
        return;
    }
    else
    {
      data.SetEmptyResult();
    }
}

namespace {
void SetListAsResult(CommandHandler &data, const std::string &type, const std::string &name, const std::vector<double> &vals)
{

  if (vals.empty())
  {
    std::ostringstream os;
    os << type << " " << name << " is empty\n";
    data.SetErrorResult(os.str());
    return;
  }
  else
  {
    data.SetObjectResult(CreateDoublePODArray(vals));
  }
}
}

void
printNodeValuesCmd(CommandHandler &data)
{
  std::string errorString;

  const std::string commandName = data.GetCommandName();

  using namespace dsGetArgs;
  static dsGetArgs::Option option[] =
  {
      {"device",   "", dsGetArgs::optionType::STRING, dsGetArgs::requiredType::REQUIRED, mustBeValidDevice},
      {"region",   "", dsGetArgs::optionType::STRING, dsGetArgs::requiredType::REQUIRED, stringCannotBeEmpty},
      {"name",     "", dsGetArgs::optionType::STRING, dsGetArgs::requiredType::REQUIRED, stringCannotBeEmpty},
      {nullptr,  nullptr, dsGetArgs::optionType::STRING, dsGetArgs::requiredType::OPTIONAL}
  };

  bool error = data.processOptions(option, errorString);

  if (error)
  {
      data.SetErrorResult(errorString);
      return;
  }

  const std::string &deviceName = data.GetStringOption("device");
  const std::string &regionName = data.GetStringOption("region");
  const std::string &name = data.GetStringOption("name");

  Device *dev = nullptr;
  Region *reg = nullptr;

  errorString = ValidateDeviceAndRegion(deviceName, regionName, dev, reg);

  if (!errorString.empty())
  {
      data.SetErrorResult(errorString);
      return;
  }

  ConstNodeModelPtr nm_name        = reg->GetNodeModel(name);

  if (!nm_name.get())
  {
      std::ostringstream os;
      os << "Node Model " << name << " does not exist\n";
      errorString += os.str();
  }

  if (!errorString.empty())
  {
    data.SetErrorResult(errorString);
      return;
  }
  else
  {

    if (commandName == "print_node_values")
    {
      const NodeScalarList<double> &nsl = nm_name->GetScalarValues<double>();

      std::ostringstream os;
      os << name << "\n";
      os << std::scientific << std::setprecision(5);
      for (auto v : nsl)
      {
          os << v << "\n";
      }
      OutputStream::WriteOut(OutputStream::OutputType::INFO, os.str());
      data.SetEmptyResult();
    }
    else if (commandName == "get_node_model_values")
    {
      const NodeScalarList<double> &nsl = nm_name->GetScalarValues<double>();

      SetListAsResult(data, "Node Model", name, nsl);
    }
    else if (commandName == "delete_node_model")
    {
      // fix the reference count to prevent assertion in Region.cc
      nm_name.reset();
      reg->DeleteNodeModel(name);
      data.SetEmptyResult();
    }
  }
}

void
printEdgeValuesCmd(CommandHandler &data)
{
  std::string errorString;

  const std::string commandName = data.GetCommandName();

  using namespace dsGetArgs;
  static dsGetArgs::Option option[] =
  {
    {"device",   "", dsGetArgs::optionType::STRING, dsGetArgs::requiredType::REQUIRED, mustBeValidDevice},
    {"region",   "", dsGetArgs::optionType::STRING, dsGetArgs::requiredType::REQUIRED, stringCannotBeEmpty},
    {"name",     "", dsGetArgs::optionType::STRING, dsGetArgs::requiredType::REQUIRED, stringCannotBeEmpty},
    {nullptr,  nullptr, dsGetArgs::optionType::STRING, dsGetArgs::requiredType::OPTIONAL}
  };

  bool error = data.processOptions(option, errorString);

  if (error)
  {
      data.SetErrorResult(errorString);
      return;
  }

  const std::string &deviceName = data.GetStringOption("device");
  const std::string &regionName = data.GetStringOption("region");
  const std::string &name = data.GetStringOption("name");

  Device *dev = nullptr;
  Region *reg = nullptr;

  errorString = ValidateDeviceAndRegion(deviceName, regionName, dev, reg);

  if (!errorString.empty())
  {
      data.SetErrorResult(errorString);
      return;
  }

  ConstEdgeModelPtr nm_name        = reg->GetEdgeModel(name);

  if (!nm_name.get())
  {
    std::ostringstream os;
    os << "Edge Model " << name << " does not exist\n";
    errorString += os.str();
  }

  if (!errorString.empty())
  {
    data.SetErrorResult(errorString);
    return;
  }
  else
  {
    if (commandName == "print_edge_values")
    {
      const EdgeScalarList<double> &nsl = nm_name->GetScalarValues<double>();
      std::ostringstream os;
      os << name << "\n";
      os << std::scientific << std::setprecision(5);
      for (auto v : nsl)
      {
          os << v << "\n";
      }
      OutputStream::WriteOut(OutputStream::OutputType::INFO, os.str());
      data.SetEmptyResult();
    }
    else if (commandName == "get_edge_model_values")
    {
      const EdgeScalarList<double> &nsl = nm_name->GetScalarValues<double>();
      SetListAsResult(data, "Edge Model", name, nsl);
    }
    else if (commandName == "delete_edge_model")
    {
      // fix the reference count to prevent assertion in Region.cc
      nm_name.reset();
      reg->DeleteEdgeModel(name);
      data.SetEmptyResult();
    }
  }

  return;
}

void
printElementEdgeValuesCmd(CommandHandler &data)
{
  std::string errorString;

  const std::string commandName = data.GetCommandName();

  using namespace dsGetArgs;
  static dsGetArgs::Option option[] =
  {
    {"device",   "", dsGetArgs::optionType::STRING, dsGetArgs::requiredType::REQUIRED, mustBeValidDevice},
    {"region",   "", dsGetArgs::optionType::STRING, dsGetArgs::requiredType::REQUIRED, stringCannotBeEmpty},
    {"name",     "", dsGetArgs::optionType::STRING, dsGetArgs::requiredType::REQUIRED, stringCannotBeEmpty},
    {nullptr,  nullptr, dsGetArgs::optionType::STRING, dsGetArgs::requiredType::OPTIONAL}
  };

  bool error = data.processOptions(option, errorString);

  if (error)
  {
      data.SetErrorResult(errorString);
      return;
  }

  const std::string &deviceName = data.GetStringOption("device");
  const std::string &regionName = data.GetStringOption("region");
  const std::string &name = data.GetStringOption("name");

  Device *dev = nullptr;
  Region *reg = nullptr;

  errorString = ValidateDeviceAndRegion(deviceName, regionName, dev, reg);

  if (!errorString.empty())
  {
      data.SetErrorResult(errorString);
      return;
  }

  const size_t dimension = dev->GetDimension();

  ConstTriangleEdgeModelPtr    triangle_edge_model;
  ConstTetrahedronEdgeModelPtr tetrahedron_edge_model;
  if (dimension == 2)
  {
    triangle_edge_model = reg->GetTriangleEdgeModel(name);
  }
  else if (dimension == 3)
  {
    tetrahedron_edge_model = reg->GetTetrahedronEdgeModel(name);
  }

  if ((!triangle_edge_model) && (!tetrahedron_edge_model))
  {
    std::ostringstream os;
    os << "Element Edge Model " << name << " does not exist\n";
    errorString += os.str();
  }

  if (!errorString.empty())
  {
    data.SetErrorResult(errorString);
    return;
  }
  else
  {

    if (triangle_edge_model)
    {
      if (commandName == "print_element_values")
      {
        const TriangleEdgeScalarList<double> &nsl = triangle_edge_model->GetScalarValues<double>();

        std::ostringstream os;
        os << name << "\n";
        os << std::scientific << std::setprecision(5);
        for (auto v : nsl)
        {
            os << v << "\n";
        }
        OutputStream::WriteOut(OutputStream::OutputType::INFO, os.str());
        data.SetEmptyResult();
      }
      else if (commandName == "get_element_model_values")
      {
        const TriangleEdgeScalarList<double> &nsl = triangle_edge_model->GetScalarValues<double>();

        SetListAsResult(data, "Element Edge Model", name, nsl);
      }
      else if (commandName == "delete_element_model")
      {
        // fix the reference count to prevent assertion in Region.cc
        triangle_edge_model.reset();
        reg->DeleteTriangleEdgeModel(name);
        data.SetEmptyResult();
      }
    }
    else if (tetrahedron_edge_model)
    {
      if (commandName == "print_element_values")
      {
        const TetrahedronEdgeScalarList<double> &nsl = tetrahedron_edge_model->GetScalarValues<double>();

        std::ostringstream os;
        os << name << "\n";
        os << std::scientific << std::setprecision(5);
        for (auto v : nsl)
        {
            os << v << "\n";
        }
        OutputStream::WriteOut(OutputStream::OutputType::INFO, os.str());
        data.SetEmptyResult();
      }
      else if (commandName == "get_element_model_values")
      {
        const TetrahedronEdgeScalarList<double> &nsl = tetrahedron_edge_model->GetScalarValues<double>();

        SetListAsResult(data, "Element Edge Model", name, nsl);
      }
      else if (commandName == "delete_element_model")
      {
        // fix the reference count to prevent assertion in Region.cc
        tetrahedron_edge_model.reset();
        reg->DeleteTetrahedronEdgeModel(name);
        data.SetEmptyResult();
      }
    }
  }
}

void
getInterfaceValuesCmd(CommandHandler &data)
{
  std::string errorString;

//    const std::string commandName = data.GetCommandName();

//    GlobalData &gdata = GlobalData::GetInstance();

  using namespace dsGetArgs;
  static dsGetArgs::Option option[] =
  {
    {"device",    "", dsGetArgs::optionType::STRING, dsGetArgs::requiredType::REQUIRED, mustBeValidDevice},
    {"interface", "", dsGetArgs::optionType::STRING, dsGetArgs::requiredType::REQUIRED, stringCannotBeEmpty},
    {"name",      "", dsGetArgs::optionType::STRING, dsGetArgs::requiredType::REQUIRED, stringCannotBeEmpty},
    {nullptr,  nullptr, dsGetArgs::optionType::STRING, dsGetArgs::requiredType::OPTIONAL, nullptr}
  };

  bool error = data.processOptions(option, errorString);

  if (error)
  {
      data.SetErrorResult(errorString);
      return;
  }

  const std::string commandName = data.GetCommandName();

  const std::string &deviceName = data.GetStringOption("device");
  const std::string &interfaceName = data.GetStringOption("interface");
  const std::string &name = data.GetStringOption("name");

  Device    *dev = nullptr;
  Interface *interface = nullptr;

  errorString = ValidateDeviceAndInterface(deviceName, interfaceName, dev, interface);
  if (!errorString.empty())
  {
    data.SetErrorResult(errorString);
    return;
  }

  ConstInterfaceNodeModelPtr imp = interface->GetInterfaceNodeModel(name);

  if (!imp.get())
  {
    std::ostringstream os;
    os << "Interface Node Model " << name << " does not exist\n";
    errorString += os.str();
  }

  if (!errorString.empty())
  {
    data.SetErrorResult(errorString);
    return;
  }
  else
  {
    if (commandName == "get_interface_model_values")
    {
      const NodeScalarList<double> &nsl = imp->GetScalarValues<double>();
      SetListAsResult(data, "Interface Node Model", name, nsl);
    }
    else if (commandName == "delete_interface_model")
    {
      // fix the reference count to prevent assertion in Region.cc
      imp.reset();
      interface->DeleteInterfaceNodeModel(name);
      data.SetEmptyResult();
    }
  }

  return;
}

void
getNodeModelListCmd(CommandHandler &data)
{
  std::string errorString;

  const std::string commandName = data.GetCommandName();

  using namespace dsGetArgs;
  static dsGetArgs::Option option[] =
  {
      {"device",   "", dsGetArgs::optionType::STRING, dsGetArgs::requiredType::REQUIRED, mustBeValidDevice},
      {"region",   "", dsGetArgs::optionType::STRING, dsGetArgs::requiredType::REQUIRED, stringCannotBeEmpty},
      {nullptr,  nullptr, dsGetArgs::optionType::STRING, dsGetArgs::requiredType::OPTIONAL}
  };

  bool error = data.processOptions(option, errorString);

  if (error)
  {
      data.SetErrorResult(errorString);
      return;
  }

  const std::string &deviceName = data.GetStringOption("device");
  const std::string &regionName = data.GetStringOption("region");

  Device *dev = nullptr;
  Region *reg = nullptr;

  errorString = ValidateDeviceAndRegion(deviceName, regionName, dev, reg);

  if (!errorString.empty())
  {
      data.SetErrorResult(errorString);
      return;
  }

  if (commandName == "get_node_model_list")
  {
    const Region::NodeModelList_t &nml = reg->GetNodeModelList();
    data.SetStringListResult(GetKeys(nml));
  }
  else if (commandName == "get_edge_model_list")
  {
    const Region::EdgeModelList_t &nml = reg->GetEdgeModelList();
    data.SetStringListResult(GetKeys(nml));
  }
  else if (commandName == "get_element_model_list")
  {
    const size_t dimension = dev->GetDimension();
    if (dimension == 2)
    {
      const Region::TriangleEdgeModelList_t &nml = reg->GetTriangleEdgeModelList();
      data.SetStringListResult(GetKeys(nml));
    }
    else if (dimension == 3)
    {
      const Region::TetrahedronEdgeModelList_t &nml = reg->GetTetrahedronEdgeModelList();
      data.SetStringListResult(GetKeys(nml));
    }
  }
}

void
getInterfaceModelListCmd(CommandHandler &data)
{
    std::string errorString;

//    const std::string commandName = data.GetCommandName();

//    GlobalData &gdata = GlobalData::GetInstance();

    using namespace dsGetArgs;
    static dsGetArgs::Option option[] =
    {
        {"device",    "", dsGetArgs::optionType::STRING, dsGetArgs::requiredType::REQUIRED, mustBeValidDevice},
        {"interface", "", dsGetArgs::optionType::STRING, dsGetArgs::requiredType::REQUIRED, stringCannotBeEmpty},
        {nullptr,  nullptr, dsGetArgs::optionType::STRING, dsGetArgs::requiredType::OPTIONAL, nullptr}
    };

    bool error = data.processOptions(option, errorString);

    if (error)
    {
        data.SetErrorResult(errorString);
        return;
    }

//    const std::string commandName = data.GetCommandName();

    const std::string &deviceName = data.GetStringOption("device");
    const std::string &interfaceName = data.GetStringOption("interface");

    Device    *dev = nullptr;
    Interface *interface = nullptr;

    errorString = ValidateDeviceAndInterface(deviceName, interfaceName, dev, interface);
    if (!errorString.empty())
    {
        data.SetErrorResult(errorString);
        return;
    }

    const Interface::NameToInterfaceNodeModelMap_t &nml = interface->GetInterfaceNodeModelList();
    data.SetStringListResult(GetKeys(nml));
}

void
createVectorElementModelCmd(CommandHandler &data)
{
    std::string errorString;

//    const std::string commandName = data.GetCommandName();

//    GlobalData &gdata = GlobalData::GetInstance();

    using namespace dsGetArgs;
    static dsGetArgs::Option option[] =
    {
        {"device",     "", dsGetArgs::optionType::STRING, dsGetArgs::requiredType::REQUIRED, mustBeValidDevice},
        {"region",     "", dsGetArgs::optionType::STRING, dsGetArgs::requiredType::REQUIRED, mustBeValidRegion},
        {"element_model", "", dsGetArgs::optionType::STRING, dsGetArgs::requiredType::REQUIRED, stringCannotBeEmpty},
        {nullptr,  nullptr, dsGetArgs::optionType::STRING, dsGetArgs::requiredType::OPTIONAL}
    };

    bool error = data.processOptions(option, errorString);

    if (error)
    {
        data.SetErrorResult(errorString);
        return;
    }

    const std::string commandName = data.GetCommandName();

    const std::string &deviceName = data.GetStringOption("device");
    const std::string &regionName = data.GetStringOption("region");
    const std::string &name = data.GetStringOption("element_model");

    Device *dev = nullptr;
    Region *reg = nullptr;

    errorString = ValidateDeviceAndRegion(deviceName, regionName, dev, reg);
    if (!errorString.empty())
    {
        data.SetErrorResult(errorString);
        return;
    }

    const size_t dimension = dev->GetDimension();

    if (dimension == 1)
    {
      errorString += commandName + " not supported in 1 Dimension\n";
      data.SetErrorResult(errorString);
      return;
    }
    else if (dimension == 2)
    {
      ConstTriangleEdgeModelPtr existingModel = reg->GetTriangleEdgeModel(name);
      if (!existingModel)
      {
        std::ostringstream os;
        os << name << " is not an existing element model";
        errorString = os.str();
        data.SetErrorResult(errorString);
        return;
      }
      else
      {
        CreateVectorTriangleEdgeModel(name, reg);
        data.SetEmptyResult();
      }
    }
    else if (dimension == 3)
    {
      ConstTetrahedronEdgeModelPtr existingModel = reg->GetTetrahedronEdgeModel(name);
      if (!existingModel)
      {
        std::ostringstream os;
        os << name << " is not an existing element model";
        errorString = os.str();
        data.SetErrorResult(errorString);
        return;
      }
      else
      {
        CreateVectorTetrahedronEdgeModel(name, reg);
        data.SetEmptyResult();
      }
    }
}

void
createTriangleFromEdgeModelCmd(CommandHandler &data)
{
    std::string errorString;

//    const std::string commandName = data.GetCommandName();

//    GlobalData &gdata = GlobalData::GetInstance();

    using namespace dsGetArgs;
    static dsGetArgs::Option option[] =
    {
        {"device",     "", dsGetArgs::optionType::STRING, dsGetArgs::requiredType::REQUIRED, mustBeValidDevice},
        {"region",     "", dsGetArgs::optionType::STRING, dsGetArgs::requiredType::REQUIRED, mustBeValidRegion},
        {"edge_model", "", dsGetArgs::optionType::STRING, dsGetArgs::requiredType::REQUIRED, stringCannotBeEmpty},
        {"derivative", "", dsGetArgs::optionType::STRING, dsGetArgs::requiredType::OPTIONAL, nullptr},
        {nullptr,  nullptr, dsGetArgs::optionType::STRING, dsGetArgs::requiredType::OPTIONAL}
    };

    bool error = data.processOptions(option, errorString);

    if (error)
    {
        data.SetErrorResult(errorString);
        return;
    }

    const std::string commandName = data.GetCommandName();

    const std::string &deviceName = data.GetStringOption("device");
    const std::string &regionName = data.GetStringOption("region");
    const std::string &name = data.GetStringOption("edge_model");
    const std::string &derivative = data.GetStringOption("derivative");

    Device *dev = nullptr;
    Region *reg = nullptr;

    errorString = ValidateDeviceAndRegion(deviceName, regionName, dev, reg);
    if (!errorString.empty())
    {
        data.SetErrorResult(errorString);
        return;
    }

    const size_t dimension = dev->GetDimension();

    if (derivative.empty())
    {
      ConstEdgeModelPtr existingEdgeModel = reg->GetEdgeModel(name);
      if (!existingEdgeModel)
      {
        std::ostringstream os;
        os << name << " is not an existing edge model";
        errorString = os.str();
        data.SetErrorResult(errorString);
        return;
      }
    }
    else
    {
      const std::string name0 = name + ":" + derivative + "@n0";
      const std::string name1 = name + ":" + derivative + "@n1";
      ConstEdgeModelPtr existingEdgeModel0 = reg->GetEdgeModel(name0);
      ConstEdgeModelPtr existingEdgeModel1 = reg->GetEdgeModel(name1);
      if ((!existingEdgeModel0) || (!existingEdgeModel1))
      {
        std::ostringstream os;
        if (!existingEdgeModel0)
        {
          os << name0 << " is not an existing edge model";
        }
        if (!existingEdgeModel1)
        {
          os << name1 << " is not an existing edge model";
        }

        errorString = os.str();
        data.SetErrorResult(errorString);
        return;
      }
    }

    if (dimension == 1)
    {
      errorString += commandName + " not supported in 1 Dimension\n";
      data.SetErrorResult(errorString);
      return;
    }
    if (dimension == 2)
    {
      if (commandName == "element_from_edge_model")
      {
        if (derivative.empty())
        {
          CreateTriangleEdgeFromEdgeModel(name, reg);
        }
        else
        {
          CreateTriangleEdgeFromEdgeModelDerivative(name, derivative, reg);
        }
      }
      else if (commandName == "element_pair_from_edge_model")
      {
        if (derivative.empty())
        {
          CreateTriangleEdgePairFromEdgeModel(name, reg);
        }
        else
        {
          CreateTriangleEdgePairFromEdgeModelDerivative(name, derivative, reg);
        }
      }
      data.SetEmptyResult();
    }
    else if (dimension == 3)
    {
      if (commandName == "element_from_edge_model")
      {
        if (derivative.empty())
        {
          CreateTetrahedronEdgeFromEdgeModel(name, reg);
        }
        else
        {
          CreateTetrahedronEdgeFromEdgeModelDerivative(name, derivative, reg);
        }
      }
      else if (commandName == "element_pair_from_edge_model")
      {
        if (derivative.empty())
        {
          CreateTetrahedronEdgePairFromEdgeModel(name, reg);
        }
        else
        {
          CreateTetrahedronEdgePairFromEdgeModelDerivative(name, derivative, reg);
        }
      }
      data.SetEmptyResult();
    }
}

void
debugTriangleCmd(CommandHandler &data)
{
    std::string errorString;

//    const std::string commandName = data.GetCommandName();

//    GlobalData &gdata = GlobalData::GetInstance();

    using namespace dsGetArgs;
    static dsGetArgs::Option option[] =
    {
        {"device",     "", dsGetArgs::optionType::STRING, dsGetArgs::requiredType::REQUIRED, mustBeValidDevice},
        {"region",     "", dsGetArgs::optionType::STRING, dsGetArgs::requiredType::REQUIRED, mustBeValidRegion},
        {nullptr,  nullptr, dsGetArgs::optionType::STRING, dsGetArgs::requiredType::OPTIONAL}
    };

    bool error = data.processOptions(option, errorString);

    if (error)
    {
        data.SetErrorResult(errorString);
        return;
    }

//    const std::string commandName = data.GetCommandName();

    const std::string &deviceName = data.GetStringOption("device");
    const std::string &regionName = data.GetStringOption("region");

    Device *dev = nullptr;
    Region *reg = nullptr;

    errorString = ValidateDeviceAndRegion(deviceName, regionName, dev, reg);
    if (!errorString.empty())
    {
        data.SetErrorResult(errorString);
        return;
    }

    //// TODO: make this more generally useful

    const ConstTriangleList &triangleList = reg->GetTriangleList();
    const Region::TriangleEdgeModelList_t &triangleEdgeModelList = reg->GetTriangleEdgeModelList();
    const Region::TriangleToConstEdgeList_t &ttelist = reg->GetTriangleToEdgeList();
    for (size_t i = 0; i < triangleList.size(); i += 100)
    {
      std::ostringstream os;
      const Triangle &triangle = *triangleList[i];
      size_t ti = triangle.GetIndex();
      os << "Triangle: " << i << " " << ti << "\n";
      const std::vector<ConstNodePtr> &nodeList = triangle.GetNodeList();
      for (size_t ni = 0; ni < nodeList.size(); ++ni)
      {
        const Node &node = *nodeList[ni];
        os << "Node " << ni << " index " << node.GetIndex() << " position " << node.Position() << "\n";
      }

      const std::vector<ConstEdgePtr> &edgeList = ttelist[i];
      for (size_t ei = 0; ei < edgeList.size(); ++ei)
      {
        const Edge &edge = *edgeList[ei];
        os << "Edge " << ei << " index " << edge.GetIndex()
                  << " nodeindex0 " << edge.GetHead()->GetIndex()
                  << " nodeindex1 " << edge.GetTail()->GetIndex()
                  << "\n";
      }
      for (Region::TriangleEdgeModelList_t::const_iterator tmit = triangleEdgeModelList.begin(); tmit != triangleEdgeModelList.end(); ++tmit)
      {
        os << "Model: " << tmit->first << "\n";
        for (size_t ei = 0; ei < 3; ++ei)
        {
          os << "    " << (tmit->second)->GetScalarValues<double>()[3*ti + ei];
        }
        os << "\n";
      }
      OutputStream::WriteOut(OutputStream::OutputType::INFO, os.str());
      data.SetEmptyResult();
    }
}

void
createInterfaceNormalModelCmd(CommandHandler &data)
{
    std::string errorString;
    dsHelper::ret_pair result = std::make_pair(false, errorString);

    const std::string commandName = data.GetCommandName();

//    GlobalData &gdata = GlobalData::GetInstance();

    using namespace dsGetArgs;
    static dsGetArgs::Option option[] =
    {
        {"device",    "", dsGetArgs::optionType::STRING, dsGetArgs::requiredType::REQUIRED, mustBeValidDevice},
        {"region",    "", dsGetArgs::optionType::STRING, dsGetArgs::requiredType::REQUIRED, mustBeValidRegion},
        {"interface", "", dsGetArgs::optionType::STRING, dsGetArgs::requiredType::REQUIRED, stringCannotBeEmpty},
        {nullptr,  nullptr, dsGetArgs::optionType::STRING, dsGetArgs::requiredType::OPTIONAL, nullptr}
    };

    bool error = data.processOptions(option, errorString);

    if (error)
    {
        data.SetErrorResult(errorString);
        return;
    }

//    const std::string commandName = data.GetCommandName();

    const std::string &deviceName    = data.GetStringOption("device");
    const std::string &regionName = data.GetStringOption("region");
    const std::string &interfaceName = data.GetStringOption("interface");

    Device    *dev       = nullptr;
    Region    *region    = nullptr;
    Interface *interface = nullptr;

    errorString = ValidateDeviceRegionAndInterface(deviceName, regionName, interfaceName, dev, region, interface);
    if (dev && (dev->GetDimension() == 1 && commandName == "interface_normal_model"))
    {
      errorString += "1D not supported for " + commandName + "\n";
    }

    if (dev && (dev->GetDimension() != 2 && commandName == "interface_cylindrical_surface_area"))
    {
      errorString += "only 2D supported for " + commandName + "\n";
    }

    if (!errorString.empty())
    {
        data.SetErrorResult(errorString);
        return;
    }

    if (commandName == "interface_normal_model")
    {
      std::string idist(interfaceName +  "_distance");
      std::string inormx(interfaceName + "_normal_x");
      std::string inormy(interfaceName + "_normal_y");
      std::string inormz(interfaceName + "_normal_z");

      CreateInterfaceNormal(interfaceName, idist, inormx, inormy, inormz, region);
      data.SetEmptyResult();
    }
}

void
symdiffCmd(CommandHandler &data)
{
  std::string errorString;
  dsHelper::ret_pair result = std::make_pair(false, errorString);

//    const std::string commandName = data.GetCommandName();

//    GlobalData &gdata = GlobalData::GetInstance();

  using namespace dsGetArgs;
  static dsGetArgs::Option option[] =
  {
    {"expr",   "", dsGetArgs::optionType::STRING, dsGetArgs::requiredType::REQUIRED},
    {nullptr,  nullptr, dsGetArgs::optionType::STRING, dsGetArgs::requiredType::OPTIONAL}
  };

  bool error = data.processOptions(option, errorString);

  if (error)
  {
      data.SetErrorResult(errorString);
      return;
  }

//  const std::string commandName = data.GetCommandName();

  const std::string &expr = data.GetStringOption("expr");

  result = dsHelper::SymdiffEval(expr);

  if (!result.first)
  {
    errorString += result.second;
  }

  if (!errorString.empty())
  {
    std::ostringstream os;
    os << "While calling symdiff interpreter\n";
    errorString = os.str() + errorString;
    data.SetErrorResult(errorString);
    return;
  }
  else
  {
    data.SetStringResult(result.second);
  }
}

void
registerFunctionCmd(CommandHandler &data)
{
  std::string errorString;
  dsHelper::ret_pair result = std::make_pair(false, errorString);

//    const std::string commandName = data.GetCommandName();

//    GlobalData &gdata = GlobalData::GetInstance();

  using namespace dsGetArgs;
  static dsGetArgs::Option option[] =
  {
    {"name",      "", dsGetArgs::optionType::STRING, dsGetArgs::requiredType::REQUIRED},
    {"procedure", "", dsGetArgs::optionType::STRING, dsGetArgs::requiredType::REQUIRED},
    {"nargs",     "", dsGetArgs::optionType::INTEGER, dsGetArgs::requiredType::REQUIRED},
    {nullptr,     nullptr, dsGetArgs::optionType::STRING, dsGetArgs::requiredType::OPTIONAL}
  };

  bool error = data.processOptions(option, errorString);

  if (error)
  {
      data.SetErrorResult(errorString);
      return;
  }

//  const std::string commandName = data.GetCommandName();

  const std::string &name  = data.GetStringOption("name");
  const int nargs          = data.GetIntegerOption("nargs");
  ObjectHolder procedure   = data.GetObjectHolder("procedure");

  int num = nargs;

  if (num < 1)
  {
    errorString = "Must have at least 1 argument specified";
    data.SetErrorResult(errorString);
    return;
  }

#ifdef DEVSIM_EXTENDED_PRECISION
  MathEval<float128>::GetInstance().AddTclMath(name, procedure, static_cast<size_t>(nargs), errorString);
#endif
  MathEval<double>::GetInstance().AddTclMath(name, procedure, static_cast<size_t>(nargs), errorString);

  if (!errorString.empty())
  {
    data.SetErrorResult(errorString);
    return;
  }

  data.SetEmptyResult();
}
//// This node and edge model commands include contact
//get_node_model_command
//get_edge_model_command
//get_element_model_command
//get_interface_model_command
}

