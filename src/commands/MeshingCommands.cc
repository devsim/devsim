/***
DEVSIM
Copyright 2013 DEVSIM LLC

SPDX-License-Identifier: Apache-2.0
***/

#include "MeshingCommands.hh"
#include "dsCommand.hh"
#include "CommandHandler.hh"
#include "Validate.hh"
#include "GlobalData.hh"
#include "CheckFunctions.hh"
#include "MeshKeeper.hh"
#include "Mesh1d.hh"
#include "Mesh2d.hh"
#include "DevsimReader.hh"
#include "DevsimWriter.hh"
#include "DevsimRestartWriter.hh"
#include "FloodsWriter.hh"
#include "VTKWriter.hh"
#include "TecplotWriter.hh"
#include "dsAssert.hh"
#include "GmshReader.hh"
#include "GmshLoader.hh"

#include "Device.hh"
#include "Region.hh"
#include "Interface.hh"
#include "Contact.hh"

#include <sstream>

using namespace dsValidate;

namespace dsCommand {

void
create1dMeshCmd(CommandHandler &data)
{
    std::string errorString;

    const std::string commandName = data.GetCommandName();

    using namespace dsGetArgs;
    static dsGetArgs::Option option[] =
    {
        {"mesh", "",   dsGetArgs::optionType::STRING, dsGetArgs::requiredType::REQUIRED, meshCannotExist},
        {nullptr,   nullptr, dsGetArgs::optionType::STRING, dsGetArgs::requiredType::OPTIONAL, nullptr}
    };

    bool error = data.processOptions(option, errorString);

    if (error)
    {
        data.SetErrorResult(errorString);
        return;
    }

    dsMesh::MeshKeeper &mdata = dsMesh::MeshKeeper::GetInstance();

    const std::string &meshName = data.GetStringOption("mesh");

    dsMesh::MeshPtr mp = nullptr;
    if (commandName == "create_1d_mesh")
    {
        mp = new dsMesh::Mesh1d(meshName);
    }
    else if (commandName == "create_2d_mesh")
    {
        mp = new dsMesh::Mesh2d(meshName);
    }
    else
    {
        dsAssert(0, "UNEXPECTED");
    }
    mdata.AddMesh(mp);
    data.SetEmptyResult();
}

void
finalizeMeshCmd(CommandHandler &data)
{
    std::string errorString;

    const std::string commandName = data.GetCommandName();

    using namespace dsGetArgs;
    static dsGetArgs::Option option[] = {
        {"mesh", "",   dsGetArgs::optionType::STRING, dsGetArgs::requiredType::REQUIRED, meshMustNotBeFinalized},
        {nullptr,   nullptr, dsGetArgs::optionType::STRING, dsGetArgs::requiredType::OPTIONAL, nullptr}
    };

    bool error = data.processOptions(option, errorString);

    if (error)
    {
        data.SetErrorResult(errorString);
        return;
    }

    dsMesh::MeshKeeper &mdata = dsMesh::MeshKeeper::GetInstance();

    const std::string &meshName = data.GetStringOption("mesh");

    dsMesh::MeshPtr mp = mdata.GetMesh(meshName);
    {
        bool ret = mp->Finalize(errorString);
        if (!ret)
        {
          data.SetErrorResult(errorString);
          return;
        }
    }
    data.SetEmptyResult();
}

void
deleteMeshCmd(CommandHandler &data)
{
    std::string errorString;

    const std::string commandName = data.GetCommandName();

    using namespace dsGetArgs;
    static dsGetArgs::Option option[] = {
        {"mesh", "",   dsGetArgs::optionType::STRING, dsGetArgs::requiredType::REQUIRED, meshMustExist},
        {nullptr,   nullptr, dsGetArgs::optionType::STRING, dsGetArgs::requiredType::OPTIONAL, nullptr}
    };

    bool error = data.processOptions(option, errorString);

    if (error)
    {
        data.SetErrorResult(errorString);
        return;
    }

    dsMesh::MeshKeeper &mdata = dsMesh::MeshKeeper::GetInstance();

    const std::string &meshName = data.GetStringOption("mesh");

    bool ret = mdata.DeleteMesh(meshName);

    if (!ret)
    {
      errorString = "Could not delete mesh \"" + meshName + "\"\n";
      data.SetErrorResult(errorString);
      return;
    }
    data.SetEmptyResult();
}

void
getMeshListCmd(CommandHandler &data)
{
    std::string errorString;

    using namespace dsGetArgs;
    static dsGetArgs::Option option[] = {
        {nullptr,   nullptr, dsGetArgs::optionType::STRING, dsGetArgs::requiredType::OPTIONAL, nullptr}
    };

    bool error = data.processOptions(option, errorString);

    if (error)
    {
        data.SetErrorResult(errorString);
        return;
    }

    dsMesh::MeshKeeper &mdata = dsMesh::MeshKeeper::GetInstance();
    const auto &meshList = mdata.GetMeshList();

    data.SetStringListResult(GetKeys(meshList));
}

void
add1dMeshLineCmd(CommandHandler &data)
{
    std::string errorString;

    const std::string commandName = data.GetCommandName();

    using namespace dsGetArgs;
    static dsGetArgs::Option option[] = {
        {"mesh",   "", dsGetArgs::optionType::STRING, dsGetArgs::requiredType::REQUIRED, meshMustNotBeFinalized},
        {"tag",    "", dsGetArgs::optionType::STRING, dsGetArgs::requiredType::OPTIONAL, nullptr},
        {"pos",    "0", dsGetArgs::optionType::FLOAT, dsGetArgs::requiredType::REQUIRED, nullptr},
/// this will default to plus spacing
        {"ns",     "0", dsGetArgs::optionType::FLOAT, dsGetArgs::requiredType::OPTIONAL, nullptr},
        {"ps",     "0", dsGetArgs::optionType::FLOAT, dsGetArgs::requiredType::REQUIRED, mustBePositive},
        {nullptr,  nullptr, dsGetArgs::optionType::STRING, dsGetArgs::requiredType::OPTIONAL,  nullptr}
    };

    bool error = data.processOptions(option, errorString);

    if (error)
    {
        data.SetErrorResult(errorString);
        return;
    }

    const std::string &meshName = data.GetStringOption("mesh");
    const std::string &tagName  = data.GetStringOption("tag");
    double  pos = data.GetDoubleOption("pos");
    double  ns  = data.GetDoubleOption("ns");
    double  ps  = data.GetDoubleOption("ps");

    if (ns <= 0.0)
    {
        ns = ps;
    }

    dsMesh::MeshKeeper &mdata = dsMesh::MeshKeeper::GetInstance();
    dsMesh::MeshPtr mp = mdata.GetMesh(meshName);
    dsMesh::Mesh1dPtr m1dp = dynamic_cast<dsMesh::Mesh1dPtr>(mp);
    if (!m1dp)
    {
      std::ostringstream os;
      os << meshName << " is not a 1D mesh\n";
      data.SetErrorResult(os.str());
      return;
    }
    else
    {
        if (!tagName.empty())
        {
            m1dp->AddLine(dsMesh::MeshLine1d(pos, ps, ns, tagName));
            data.SetEmptyResult();
        }
        else
        {
            m1dp->AddLine(dsMesh::MeshLine1d(pos, ps, ns));
            data.SetEmptyResult();
        }
    }
}

void
add2dMeshLineCmd(CommandHandler &data)
{
    std::string errorString;

    const std::string commandName = data.GetCommandName();

    using namespace dsGetArgs;
    static dsGetArgs::Option option[] = {
        {"mesh",   "", dsGetArgs::optionType::STRING, dsGetArgs::requiredType::REQUIRED, meshMustNotBeFinalized},
        {"pos",    "0", dsGetArgs::optionType::FLOAT, dsGetArgs::requiredType::REQUIRED, nullptr},
/// this will default to plus spacing
        {"ns",     "0", dsGetArgs::optionType::FLOAT, dsGetArgs::requiredType::OPTIONAL, nullptr},
        {"ps",     "0", dsGetArgs::optionType::FLOAT, dsGetArgs::requiredType::REQUIRED, mustBePositive},
        {"dir",    "",  dsGetArgs::optionType::STRING, dsGetArgs::requiredType::REQUIRED, nullptr},
        {nullptr,  nullptr, dsGetArgs::optionType::STRING, dsGetArgs::requiredType::OPTIONAL,  nullptr}
    };

    bool error = data.processOptions(option, errorString);

    if (error)
    {
        data.SetErrorResult(errorString);
        return;
    }

    const std::string &meshName = data.GetStringOption("mesh");
    const std::string &dirName  = data.GetStringOption("dir");
    double  pos = data.GetDoubleOption("pos");
    double  ns  = data.GetDoubleOption("ns");
    double  ps  = data.GetDoubleOption("ps");

    if (ns <= 0.0)
    {
        ns = ps;
    }

    dsMesh::MeshKeeper &mdata = dsMesh::MeshKeeper::GetInstance();
    dsMesh::MeshPtr mp = mdata.GetMesh(meshName);
    dsMesh::Mesh2dPtr m2dp = dynamic_cast<dsMesh::Mesh2dPtr>(mp);
    if (!m2dp)
    {
      std::ostringstream os;
      os << meshName << " is not a 2D mesh\n";
      data.SetErrorResult(os.str());
      return;
    }
    else
    {
        if (dirName == "x")
        {
            dsMesh::MeshLine2dPtr mlp(new dsMesh::MeshLine2d(pos, ps, ns));
            m2dp->AddLine(dsMesh::LineDirection::XDIR, mlp);
            data.SetEmptyResult();
        }
        else if (dirName == "y")
        {
            dsMesh::MeshLine2dPtr mlp(new dsMesh::MeshLine2d(pos, ps, ns));
            m2dp->AddLine(dsMesh::LineDirection::YDIR, mlp);
            data.SetEmptyResult();
        }
        else
        {
            std::ostringstream os;
            os << dirName << " is not a valid mesh direction\n";
            data.SetErrorResult(os.str());
            return;
        }
    }

}

void
add1dInterfaceCmd(CommandHandler &data)
{
    std::string errorString;

    const std::string commandName = data.GetCommandName();

    using namespace dsGetArgs;

    static dsGetArgs::Option option[] = {
        {"mesh",   "", dsGetArgs::optionType::STRING, dsGetArgs::requiredType::REQUIRED, meshMustNotBeFinalized},
        {"name",   "", dsGetArgs::optionType::STRING, dsGetArgs::requiredType::REQUIRED, stringCannotBeEmpty},
        {"tag",    "", dsGetArgs::optionType::STRING, dsGetArgs::requiredType::REQUIRED, stringCannotBeEmpty},
        {nullptr,  nullptr,  dsGetArgs::optionType::STRING, dsGetArgs::requiredType::OPTIONAL,  nullptr}
    };

    bool error = data.processOptions(option, errorString);

    if (error)
    {
        data.SetErrorResult(errorString);
        return;
    }

    const std::string &meshName = data.GetStringOption("mesh");
    const std::string &name = data.GetStringOption("name");
    const std::string &tagName  = data.GetStringOption("tag");

    dsMesh::MeshKeeper &mdata = dsMesh::MeshKeeper::GetInstance();
    dsMesh::MeshPtr mp = mdata.GetMesh(meshName);
    dsMesh::Mesh1dPtr m1dp = dynamic_cast<dsMesh::Mesh1dPtr>(mp);
    if (!m1dp)
    {
        std::ostringstream os;
        os << meshName << " is not a 1D mesh\n";
        data.SetErrorResult(os.str());
        return;
    }
    else
    {
      m1dp->AddInterface(dsMesh::MeshInterface1d(name, tagName));
      data.SetEmptyResult();
    }
}

void
add1dContactCmd(CommandHandler &data)
{
    std::string errorString;

    const std::string commandName = data.GetCommandName();

    using namespace dsGetArgs;

    static dsGetArgs::Option option[] = {
        {"mesh",   "", dsGetArgs::optionType::STRING, dsGetArgs::requiredType::REQUIRED, meshMustNotBeFinalized},
        {"name",   "", dsGetArgs::optionType::STRING, dsGetArgs::requiredType::REQUIRED, stringCannotBeEmpty},
        {"material", "", dsGetArgs::optionType::STRING, dsGetArgs::requiredType::REQUIRED, stringCannotBeEmpty},
        {"tag",    "", dsGetArgs::optionType::STRING, dsGetArgs::requiredType::REQUIRED, stringCannotBeEmpty},
        {nullptr,  nullptr,  dsGetArgs::optionType::STRING, dsGetArgs::requiredType::OPTIONAL,  nullptr}
    };

    bool error = data.processOptions(option, errorString);

    if (error)
    {
        data.SetErrorResult(errorString);
        return;
    }

    const std::string &meshName = data.GetStringOption("mesh");
    const std::string &name = data.GetStringOption("name");
    const std::string &material = data.GetStringOption("material");
    const std::string &tagName  = data.GetStringOption("tag");

    dsMesh::MeshKeeper &mdata = dsMesh::MeshKeeper::GetInstance();
    dsMesh::MeshPtr mp = mdata.GetMesh(meshName);
    dsMesh::Mesh1dPtr m1dp = dynamic_cast<dsMesh::Mesh1dPtr>(mp);
    if (!m1dp)
    {
        std::ostringstream os;
        os << meshName << " is not a 1D mesh\n";
        data.SetErrorResult(os.str());
        return;
    }
    else
    {
      m1dp->AddContact(dsMesh::MeshContact1d(name, material, tagName));
      data.SetEmptyResult();
    }
}

void
add2dInterfaceCmd(CommandHandler &data)
{
    std::string errorString;

    const std::string commandName = data.GetCommandName();

    using namespace dsGetArgs;

    static dsGetArgs::Option option[] = {
        {"mesh",   "", dsGetArgs::optionType::STRING, dsGetArgs::requiredType::REQUIRED, meshMustNotBeFinalized},
        {"name",   "", dsGetArgs::optionType::STRING, dsGetArgs::requiredType::REQUIRED, stringCannotBeEmpty},
        {"region0",    "", dsGetArgs::optionType::STRING, dsGetArgs::requiredType::REQUIRED, stringCannotBeEmpty},
        {"region1",    "", dsGetArgs::optionType::STRING, dsGetArgs::requiredType::REQUIRED, stringCannotBeEmpty},
        {"xl", "-MAXDOUBLE", dsGetArgs::optionType::FLOAT, dsGetArgs::requiredType::OPTIONAL, nullptr},
        {"xh", "MAXDOUBLE",  dsGetArgs::optionType::FLOAT, dsGetArgs::requiredType::OPTIONAL, nullptr},
        {"yl", "-MAXDOUBLE", dsGetArgs::optionType::FLOAT, dsGetArgs::requiredType::OPTIONAL, nullptr},
        {"yh", "MAXDOUBLE",  dsGetArgs::optionType::FLOAT, dsGetArgs::requiredType::OPTIONAL, nullptr},
        {"bloat", "1.0e-10",  dsGetArgs::optionType::FLOAT, dsGetArgs::requiredType::OPTIONAL, nullptr},
        {nullptr,  nullptr,  dsGetArgs::optionType::STRING, dsGetArgs::requiredType::OPTIONAL,  nullptr}
    };

    bool error = data.processOptions(option, errorString);

    if (error)
    {
        data.SetErrorResult(errorString);
        return;
    }

    const std::string &meshName = data.GetStringOption("mesh");
    const std::string &name = data.GetStringOption("name");
    const std::string &region0Name  = data.GetStringOption("region0");
    const std::string &region1Name  = data.GetStringOption("region1");
    const double xl = data.GetDoubleOption("xl");
    const double xh = data.GetDoubleOption("xh");
    const double yl = data.GetDoubleOption("yl");
    const double yh = data.GetDoubleOption("yh");
    const double bl = data.GetDoubleOption("bloat");

    dsMesh::MeshKeeper &mdata = dsMesh::MeshKeeper::GetInstance();
    dsMesh::MeshPtr mp = mdata.GetMesh(meshName);
    dsMesh::Mesh2dPtr m2dp = dynamic_cast<dsMesh::Mesh2dPtr>(mp);
    if (!m2dp)
    {
        std::ostringstream os;
        os << meshName << " is not a 2D mesh\n";
        data.SetErrorResult(os.str());
        return;
    }
    else
    {
        if (region0Name == region1Name)
        {
            std::ostringstream os;
            os << region0Name << " cannot be specified for both region0 and region1\n";
            data.SetErrorResult(os.str());
            return;
        }
        else
        {
            dsMesh::MeshInterface2dPtr ip(new dsMesh::MeshInterface2d(name, region0Name, region1Name));
            m2dp->AddInterface(ip);
            dsMesh::BoundingBox bb(xl, xh, yl, yh, bl);
            ip->AddBoundingBox(bb);
            data.SetEmptyResult();
        }
    }
}

void
add2dContactCmd(CommandHandler &data)
{
    std::string errorString;

    const std::string commandName = data.GetCommandName();

    using namespace dsGetArgs;

    static dsGetArgs::Option option[] = {
        {"mesh",   "", dsGetArgs::optionType::STRING, dsGetArgs::requiredType::REQUIRED, meshMustNotBeFinalized},
        {"name",   "", dsGetArgs::optionType::STRING, dsGetArgs::requiredType::REQUIRED, stringCannotBeEmpty},
        {"material", "", dsGetArgs::optionType::STRING, dsGetArgs::requiredType::REQUIRED, stringCannotBeEmpty},
        {"region",  "", dsGetArgs::optionType::STRING, dsGetArgs::requiredType::REQUIRED, stringCannotBeEmpty},
        {"xl", "-MAXDOUBLE", dsGetArgs::optionType::FLOAT, dsGetArgs::requiredType::OPTIONAL, nullptr},
        {"xh", "MAXDOUBLE",  dsGetArgs::optionType::FLOAT, dsGetArgs::requiredType::OPTIONAL, nullptr},
        {"yl", "-MAXDOUBLE", dsGetArgs::optionType::FLOAT, dsGetArgs::requiredType::OPTIONAL, nullptr},
        {"yh", "MAXDOUBLE",  dsGetArgs::optionType::FLOAT, dsGetArgs::requiredType::OPTIONAL, nullptr},
        {"bloat", "1.0e-10",  dsGetArgs::optionType::FLOAT, dsGetArgs::requiredType::OPTIONAL, nullptr},
        {nullptr,  nullptr,  dsGetArgs::optionType::STRING, dsGetArgs::requiredType::OPTIONAL,  nullptr}
    };

    bool error = data.processOptions(option, errorString);

    if (error)
    {
        data.SetErrorResult(errorString);
        return;
    }

    const std::string &meshName = data.GetStringOption("mesh");
    const std::string &name = data.GetStringOption("name");
    const std::string &regionName  = data.GetStringOption("region");
    const std::string &materialName = data.GetStringOption("material");
    const double xl = data.GetDoubleOption("xl");
    const double xh = data.GetDoubleOption("xh");
    const double yl = data.GetDoubleOption("yl");
    const double yh = data.GetDoubleOption("yh");
    const double bl = data.GetDoubleOption("bloat");

    dsMesh::MeshKeeper &mdata = dsMesh::MeshKeeper::GetInstance();
    dsMesh::MeshPtr mp = mdata.GetMesh(meshName);
    dsMesh::Mesh2dPtr m2dp = dynamic_cast<dsMesh::Mesh2dPtr>(mp);
    if (!m2dp)
    {
        std::ostringstream os;
        os << meshName << " is not a 2D mesh\n";
        data.SetErrorResult(os.str());
        return;
    }
    else
    {
        dsMesh::MeshContact2dPtr cp(new dsMesh::MeshContact2d(name, materialName, regionName));
        m2dp->AddContact(cp);
        dsMesh::BoundingBox bb(xl, xh, yl, yh, bl);
        cp->AddBoundingBox(bb);
        data.SetEmptyResult();
    }
}

void
add1dRegionCmd(CommandHandler &data)
{
    std::string errorString;

    const std::string commandName = data.GetCommandName();

    using namespace dsGetArgs;
    static dsGetArgs::Option option[] = {
        {"mesh",     "", dsGetArgs::optionType::STRING, dsGetArgs::requiredType::REQUIRED, meshMustNotBeFinalized},
        {"region",   "", dsGetArgs::optionType::STRING, dsGetArgs::requiredType::REQUIRED, stringCannotBeEmpty},
        {"material", "", dsGetArgs::optionType::STRING, dsGetArgs::requiredType::REQUIRED, stringCannotBeEmpty},
        {"tag1",     "", dsGetArgs::optionType::STRING, dsGetArgs::requiredType::REQUIRED, stringCannotBeEmpty},
        {"tag2",     "", dsGetArgs::optionType::STRING, dsGetArgs::requiredType::REQUIRED, stringCannotBeEmpty},
        {nullptr,  nullptr,    dsGetArgs::optionType::STRING, dsGetArgs::requiredType::OPTIONAL, nullptr}
    };

    bool error = data.processOptions(option, errorString);

    if (error)
    {
        data.SetErrorResult(errorString);
        return;
    }

    const std::string &meshName = data.GetStringOption("mesh");
    const std::string &regionName = data.GetStringOption("region");
    const std::string &materialName = data.GetStringOption("material");
    const std::string &tagName1 = data.GetStringOption("tag1");
    const std::string &tagName2 = data.GetStringOption("tag2");

    dsMesh::MeshKeeper &mdata = dsMesh::MeshKeeper::GetInstance();
    dsMesh::MeshPtr mp = mdata.GetMesh(meshName);
    dsMesh::Mesh1dPtr m1dp = dynamic_cast<dsMesh::Mesh1dPtr>(mp);
    if (!m1dp)
    {
        std::ostringstream os;
        os << meshName << " is not a 1D mesh\n";
        data.SetErrorResult(os.str());
        return;
    }
    else
    {
        m1dp->AddRegion(dsMesh::MeshRegion1d(regionName, materialName, tagName1, tagName2));
        data.SetEmptyResult();
    }
}

void
add2dRegionCmd(CommandHandler &data)
{
    std::string errorString;

    const std::string commandName = data.GetCommandName();

    using namespace dsGetArgs;
    static dsGetArgs::Option option[] = {
        {"mesh",     "", dsGetArgs::optionType::STRING, dsGetArgs::requiredType::REQUIRED, meshMustNotBeFinalized},
        {"region",   "", dsGetArgs::optionType::STRING, dsGetArgs::requiredType::REQUIRED, stringCannotBeEmpty},
        {"material", "", dsGetArgs::optionType::STRING, dsGetArgs::requiredType::REQUIRED, stringCannotBeEmpty},
        {"xl", "-MAXDOUBLE", dsGetArgs::optionType::FLOAT, dsGetArgs::requiredType::OPTIONAL, nullptr},
        {"xh", "MAXDOUBLE",  dsGetArgs::optionType::FLOAT, dsGetArgs::requiredType::OPTIONAL, nullptr},
        {"yl", "-MAXDOUBLE", dsGetArgs::optionType::FLOAT, dsGetArgs::requiredType::OPTIONAL, nullptr},
        {"yh", "MAXDOUBLE",  dsGetArgs::optionType::FLOAT, dsGetArgs::requiredType::OPTIONAL, nullptr},
        {"bloat", "1.0e-10",  dsGetArgs::optionType::FLOAT, dsGetArgs::requiredType::OPTIONAL, nullptr},
        {nullptr,  nullptr, dsGetArgs::optionType::STRING, dsGetArgs::requiredType::OPTIONAL, nullptr}
    };

    bool error = data.processOptions(option, errorString);

    if (error)
    {
        data.SetErrorResult(errorString);
        return;
    }

    const std::string &meshName = data.GetStringOption("mesh");
    const std::string &regionName = data.GetStringOption("region");
    const std::string &materialName = data.GetStringOption("material");
    const double xl = data.GetDoubleOption("xl");
    const double xh = data.GetDoubleOption("xh");
    const double yl = data.GetDoubleOption("yl");
    const double yh = data.GetDoubleOption("yh");
    const double bl = data.GetDoubleOption("bloat");

    dsMesh::MeshKeeper &mdata = dsMesh::MeshKeeper::GetInstance();
    dsMesh::MeshPtr mp = mdata.GetMesh(meshName);
    dsMesh::Mesh2dPtr m2dp = dynamic_cast<dsMesh::Mesh2dPtr>(mp);
    if (!m2dp)
    {
        std::ostringstream os;
        os << meshName << " is not a 2D mesh\n";
        data.SetErrorResult(os.str());
        return;
    }
    else
    {
        dsMesh::MeshRegion2dPtr rp(new dsMesh::MeshRegion2d(regionName, materialName));
        m2dp->AddRegion(rp);
        dsMesh::BoundingBox bb(xl, xh, yl, yh, bl);
        rp->AddBoundingBox(bb);
        data.SetEmptyResult();
    }
}

void
createDeviceCmd(CommandHandler &data)
{
    std::string errorString;

    const std::string commandName = data.GetCommandName();

    using namespace dsGetArgs;
    static dsGetArgs::Option option[] = {
        {"mesh",     "", dsGetArgs::optionType::STRING, dsGetArgs::requiredType::REQUIRED, meshMustBeFinalized},
        {"device",   "", dsGetArgs::optionType::STRING, dsGetArgs::requiredType::REQUIRED, mustNotBeValidDevice},
        {nullptr,  nullptr, dsGetArgs::optionType::STRING, dsGetArgs::requiredType::OPTIONAL}
    };

    bool error = data.processOptions(option, errorString);

    if (error)
    {
        data.SetErrorResult(errorString);
        return;
    }

    const std::string &meshName = data.GetStringOption("mesh");
    const std::string &deviceName = data.GetStringOption("device");

    dsMesh::MeshKeeper &mdata = dsMesh::MeshKeeper::GetInstance();
    dsMesh::MeshPtr mp = mdata.GetMesh(meshName);

    bool ret = mp->Instantiate(deviceName, errorString);
    if (!ret)
    {
      data.SetErrorResult(errorString);
      return;
    }
    else
    {
      data.SetEmptyResult();
    }
}

void
deleteDeviceCmd(CommandHandler &data)
{
    std::string errorString;

    const std::string commandName = data.GetCommandName();

    using namespace dsGetArgs;
    static dsGetArgs::Option option[] = {
        {"device",   "", dsGetArgs::optionType::STRING, dsGetArgs::requiredType::REQUIRED, mustBeValidDevice},
        {nullptr,  nullptr, dsGetArgs::optionType::STRING, dsGetArgs::requiredType::OPTIONAL}
    };

    bool error = data.processOptions(option, errorString);

    if (error)
    {
        data.SetErrorResult(errorString);
        return;
    }

    const std::string &deviceName = data.GetStringOption("device");

    auto &gdata = GlobalData::GetInstance();
    bool ret = gdata.DeleteDevice(deviceName);

    if (!ret)
    {
      errorString = "Could not delete device \"" + deviceName + "\"\n";
      data.SetErrorResult(errorString);
      return;
    }
    else
    {
      data.SetEmptyResult();
    }
}

void
loadDevicesCmd(CommandHandler &data)
{
    std::string errorString;

    const std::string commandName = data.GetCommandName();

    using namespace dsGetArgs;
// TODO:  "specify list of solutions to read/write"
// TODO:  "delete device if there is a problem"
// TODO:  "specify option to only load the mesh creator, but don't instantiate"
    static dsGetArgs::Option option[] = {
        {"file",     "", dsGetArgs::optionType::STRING, dsGetArgs::requiredType::REQUIRED, nullptr},
        {nullptr,  nullptr, dsGetArgs::optionType::STRING, dsGetArgs::requiredType::OPTIONAL, nullptr}
    };

    bool error = data.processOptions(option, errorString);

    if (error)
    {
        data.SetErrorResult(errorString);
        return;
    }

    const std::string &fileName = data.GetStringOption("file");

    bool ret = dsDevsimParse::LoadMeshes(fileName, errorString);
    if (!ret)
    {
      data.SetErrorResult(errorString);
      return;
    }
    else
    {
      data.SetEmptyResult();
    }
}

void
writeDevicesCmd(CommandHandler &data)
{
    std::string errorString;

    const std::string commandName = data.GetCommandName();

    using namespace dsGetArgs;
    static dsGetArgs::Option option[] = {
        {"file",     "", dsGetArgs::optionType::STRING, dsGetArgs::requiredType::REQUIRED, nullptr},
        {"device",     "", dsGetArgs::optionType::STRING, dsGetArgs::requiredType::OPTIONAL, nullptr},
        {"type",     "", dsGetArgs::optionType::STRING, dsGetArgs::requiredType::OPTIONAL, nullptr},
        {nullptr,  nullptr, dsGetArgs::optionType::STRING, dsGetArgs::requiredType::OPTIONAL, nullptr}
    };

    bool error = data.processOptions(option, errorString);

    if (error)
    {
        data.SetErrorResult(errorString);
        return;
    }

    const std::string &fileName = data.GetStringOption("file");
    const std::string &device   = data.GetStringOption("device");
    const std::string &type = data.GetStringOption("type");

    std::unique_ptr<MeshWriter> mw;

    if (type.empty() || (type == "devsim"))
    {
        mw = std::unique_ptr<MeshWriter>(new DevsimRestartWriter());
    }
    else if (type == "devsim_data")
    {
        mw = std::unique_ptr<MeshWriter>(new DevsimWriter());
    }
    else if (type == "floops")
    {
        mw = std::unique_ptr<MeshWriter>(new FloodsWriter());
    }
    else if (type == "vtk")
    {
#ifdef VTKWRITER
        mw = std::unique_ptr<MeshWriter>(new VTKWriter());
#else
        errorString += "VTK support was not built into this version.  Please select from \"devsim\", \"devsim_data\", \"floops\", or \"tecplot\".\n";
        data.SetErrorResult(errorString);
        return;
#endif
    }
    else if (type == "tecplot")
    {
        mw = std::unique_ptr<MeshWriter>(new TecplotWriter());
    }
    else
    {
        errorString += "type: " + type + " is not a valid type.  Please select from \"devsim\", \"devsim_data\", \"floops\", \"vtk\", or \"tecplot\".\n";
        data.SetErrorResult(errorString);
        return;
    }

    bool ret = true;
    if (device.empty())
    {
        ret = mw->WriteMeshes(fileName, errorString);
    }
    else
    {
        ret = mw->WriteMesh(device, fileName, errorString);
    }

    if (!ret)
    {
      data.SetErrorResult(errorString);
      return;
    }
    else
    {
      data.SetEmptyResult();
    }
}

void
createGmshMeshCmd(CommandHandler &data)
{
    std::string errorString;

//    const std::string commandName = data.GetCommandName();

    using namespace dsGetArgs;
    static dsGetArgs::Option option[] =
    {
        {"mesh", "",   dsGetArgs::optionType::STRING, dsGetArgs::requiredType::REQUIRED, meshCannotExist},
        {"file", "", dsGetArgs::optionType::STRING, dsGetArgs::requiredType::OPTIONAL, nullptr},
        {"coordinates",    "", dsGetArgs::optionType::LIST, dsGetArgs::requiredType::OPTIONAL, nullptr},
        {"physical_names", "", dsGetArgs::optionType::LIST, dsGetArgs::requiredType::OPTIONAL, nullptr},
        {"elements",   "", dsGetArgs::optionType::LIST, dsGetArgs::requiredType::OPTIONAL, nullptr},
        {nullptr,   nullptr, dsGetArgs::optionType::STRING, dsGetArgs::requiredType::OPTIONAL, nullptr}
    };

    bool error = data.processOptions(option, errorString);

    if (error)
    {
        data.SetErrorResult(errorString);
        return;
    }

    const std::string &fileName = data.GetStringOption("file");
    const std::string &meshName = data.GetStringOption("mesh");
    std::vector<double>      coordinates;
    std::vector<std::string> physical_names;
    std::vector<size_t>      elements;

    {
      ObjectHolder cdata = data.GetObjectHolder("coordinates");
      if (cdata.IsList())
      {
        bool ok = cdata.GetDoubleList(coordinates);
        if (!ok)
        {
          std::ostringstream os;
          os << "Option \"coordinates\" could not be converted to a list of doubles\n";
          errorString += os.str();
        }
      }
    }

    {
      ObjectHolder pdata = data.GetObjectHolder("physical_names");
      if (pdata.IsList())
      {
        bool ok = pdata.GetStringList(physical_names);
        if (!ok)
        {
          std::ostringstream os;
          os << "Option \"physical_names\" could not be converted to a list of strings\n";
          errorString += os.str();
        }
      }
    }

    {
      ObjectHolder edata = data.GetObjectHolder("elements");
      if (edata.IsList())
      {
        bool ok = edata.GetUnsignedLongList(elements);
        if (!ok)
        {
          std::ostringstream os;
          os << "Option \"elements\" could not be converted to a list of unsigned\n";
          errorString += os.str();
        }
      }
    }

    if (!errorString.empty())
    {
      data.SetErrorResult(errorString);
      return;
    }

    bool load_file = (!fileName.empty());
    size_t load_argcount = !coordinates.empty();
    load_argcount += !physical_names.empty();
    load_argcount += !elements.empty();

    bool ret = true;

    if (load_file && (load_argcount > 0))
    {
      std::ostringstream os;
      os << "Option \"file\" cannot be mixed with \"coordinates\", \"elements\", and \"physical_names\"\n";
      errorString += os.str();
      ret = false;
    }
    else if (load_file)
    {
      ret = dsGmshParse::LoadMeshesFromFile(fileName, meshName, errorString);
    }
    else if (load_argcount == 3)
    {
      ret = dsGmshParse::LoadMeshesFromArgs(meshName, coordinates, physical_names, elements, errorString);
    }
    else if (load_argcount > 0)
    {
      std::ostringstream os;
      os << "\"coordinates\", \"elements\", and \"physical_names\" are all required together\n";
      errorString += os.str();
      ret = false;
    }
    else
    {
      std::ostringstream os;
      os << "Unable to create mesh\n";
      errorString += os.str();
      ret = false;
    }

    if (!ret)
    {
      data.SetErrorResult(errorString);
      return;
    }
    else
    {
      data.SetEmptyResult();
    }
}

void
addGmshInterfaceCmd(CommandHandler &data)
{
    std::string errorString;

    const std::string commandName = data.GetCommandName();

    using namespace dsGetArgs;

    static dsGetArgs::Option option[] = {
        {"mesh",   "", dsGetArgs::optionType::STRING, dsGetArgs::requiredType::REQUIRED, meshMustNotBeFinalized},
        {"name",   "", dsGetArgs::optionType::STRING, dsGetArgs::requiredType::REQUIRED, stringCannotBeEmpty},
        {"gmsh_name",    "", dsGetArgs::optionType::STRING, dsGetArgs::requiredType::REQUIRED, stringCannotBeEmpty},
        {"region0",    "", dsGetArgs::optionType::STRING, dsGetArgs::requiredType::REQUIRED, stringCannotBeEmpty},
        {"region1",    "", dsGetArgs::optionType::STRING, dsGetArgs::requiredType::REQUIRED, stringCannotBeEmpty},
        {nullptr,  nullptr,  dsGetArgs::optionType::STRING, dsGetArgs::requiredType::OPTIONAL,  nullptr}
    };

    bool error = data.processOptions(option, errorString);

    if (error)
    {
        data.SetErrorResult(errorString);
        return;
    }

    const std::string &meshName = data.GetStringOption("mesh");
    const std::string &name = data.GetStringOption("name");
    const std::string &gmshName  = data.GetStringOption("gmsh_name");
    const std::string &regionName0  = data.GetStringOption("region0");
    const std::string &regionName1  = data.GetStringOption("region1");

    dsMesh::MeshKeeper &mdata = dsMesh::MeshKeeper::GetInstance();
    dsMesh::MeshPtr mp = mdata.GetMesh(meshName);
    dsMesh::GmshLoaderPtr gmp = dynamic_cast<dsMesh::GmshLoaderPtr>(mp);
    if (!gmp)
    {
        std::ostringstream os;
        os << meshName << " is not a gmsh mesh\n";
        data.SetErrorResult(os.str());
        return;
    }
    else
    {
      gmp->MapPhysicalNameToInterface(gmshName, name, regionName0, regionName1);
      data.SetEmptyResult();
    }
}

void
addGmshContactCmd(CommandHandler &data)
{
    std::string errorString;

    const std::string commandName = data.GetCommandName();

    using namespace dsGetArgs;

    static dsGetArgs::Option option[] = {
        {"mesh",   "", dsGetArgs::optionType::STRING, dsGetArgs::requiredType::REQUIRED, meshMustNotBeFinalized},
        {"name",   "", dsGetArgs::optionType::STRING, dsGetArgs::requiredType::REQUIRED, stringCannotBeEmpty},
        {"material",    "", dsGetArgs::optionType::STRING, dsGetArgs::requiredType::REQUIRED, stringCannotBeEmpty},
        {"gmsh_name",    "", dsGetArgs::optionType::STRING, dsGetArgs::requiredType::REQUIRED, stringCannotBeEmpty},
        {"region",    "", dsGetArgs::optionType::STRING, dsGetArgs::requiredType::REQUIRED, stringCannotBeEmpty},
        {nullptr,  nullptr,  dsGetArgs::optionType::STRING, dsGetArgs::requiredType::OPTIONAL,  nullptr}
    };

    bool error = data.processOptions(option, errorString);

    if (error)
    {
        data.SetErrorResult(errorString);
        return;
    }

    const std::string &meshName = data.GetStringOption("mesh");
    const std::string &name = data.GetStringOption("name");
    const std::string &gmshName  = data.GetStringOption("gmsh_name");
    const std::string &regionName  = data.GetStringOption("region");
    const std::string &materialName  = data.GetStringOption("material");

    dsMesh::MeshKeeper &mdata = dsMesh::MeshKeeper::GetInstance();
    dsMesh::MeshPtr mp = mdata.GetMesh(meshName);
    dsMesh::GmshLoaderPtr gmp = dynamic_cast<dsMesh::GmshLoaderPtr>(mp);
    if (!gmp)
    {
        std::ostringstream os;
        os << meshName << " is not a gmsh mesh\n";
        data.SetErrorResult(os.str());
        return;
    }
    else
    {
      gmp->MapPhysicalNameToContact(gmshName, name, regionName, materialName);
      data.SetEmptyResult();
    }
}

void
addGmshRegionCmd(CommandHandler &data)
{
    std::string errorString;

    const std::string commandName = data.GetCommandName();

    using namespace dsGetArgs;

    static dsGetArgs::Option option[] = {
        {"mesh",   "", dsGetArgs::optionType::STRING, dsGetArgs::requiredType::REQUIRED, meshMustNotBeFinalized},
        {"region",   "", dsGetArgs::optionType::STRING, dsGetArgs::requiredType::REQUIRED, stringCannotBeEmpty},
        {"gmsh_name",    "", dsGetArgs::optionType::STRING, dsGetArgs::requiredType::REQUIRED, stringCannotBeEmpty},
        {"material",    "", dsGetArgs::optionType::STRING, dsGetArgs::requiredType::REQUIRED, stringCannotBeEmpty},
        {nullptr,  nullptr,  dsGetArgs::optionType::STRING, dsGetArgs::requiredType::OPTIONAL,  nullptr}
    };

    bool error = data.processOptions(option, errorString);

    if (error)
    {
        data.SetErrorResult(errorString);
        return;
    }

    const std::string &meshName = data.GetStringOption("mesh");
    const std::string &gmshName  = data.GetStringOption("gmsh_name");
    const std::string &regionName  = data.GetStringOption("region");
    const std::string &materialName  = data.GetStringOption("material");

    dsMesh::MeshKeeper &mdata = dsMesh::MeshKeeper::GetInstance();
    dsMesh::MeshPtr mp = mdata.GetMesh(meshName);
    dsMesh::GmshLoaderPtr gmp = dynamic_cast<dsMesh::GmshLoaderPtr>(mp);
    if (!gmp)
    {
        std::ostringstream os;
        os << meshName << " is not a gmsh mesh\n";
        data.SetErrorResult(os.str());
        return;
    }
    else
    {
      gmp->MapPhysicalNameToRegion(gmshName, regionName, materialName);
      data.SetEmptyResult();
    }
}

/**
Unlike the other mesh commands, this one is to be called after the device has been instantiated
*/
void
createContactFromInterfaceCmd(CommandHandler &data)
{
    std::string errorString;

    const std::string commandName = data.GetCommandName();

    using namespace dsGetArgs;

    static dsGetArgs::Option option[] = {
        {"name",       "", dsGetArgs::optionType::STRING, dsGetArgs::requiredType::REQUIRED, stringCannotBeEmpty},
        {"material",   "", dsGetArgs::optionType::STRING, dsGetArgs::requiredType::REQUIRED, stringCannotBeEmpty},
        {"device",     "", dsGetArgs::optionType::STRING, dsGetArgs::requiredType::REQUIRED, mustBeValidDevice},
        {"region",     "", dsGetArgs::optionType::STRING, dsGetArgs::requiredType::REQUIRED, mustBeValidRegion},
        {"interface",  "", dsGetArgs::optionType::STRING, dsGetArgs::requiredType::REQUIRED, mustBeValidInterface},
        {nullptr,  nullptr,  dsGetArgs::optionType::STRING, dsGetArgs::requiredType::OPTIONAL,  nullptr}
    };

    bool error = data.processOptions(option, errorString);

    if (error)
    {
        data.SetErrorResult(errorString);
        return;
    }

    Device    *device    = nullptr;
    Contact   *contact   = nullptr;
    Interface *interface = nullptr;
    Region    *region    = nullptr;

    const std::string &contactName   = data.GetStringOption("name");
    const std::string &materialName  = data.GetStringOption("material");
    const std::string &deviceName    = data.GetStringOption("device");
    const std::string &regionName    = data.GetStringOption("region");
    const std::string &interfaceName = data.GetStringOption("interface");

    errorString = ValidateDeviceAndContact(deviceName, contactName, device, contact);
    if (contact)
    {
      std::ostringstream os;
      os << onContactonDevice(contactName, deviceName) << " already exists.\n";
      errorString = os.str();
      data.SetErrorResult(errorString);
      return;
    }

    ValidateDeviceAndRegion(deviceName, regionName, device, region);
    ValidateDeviceAndInterface(deviceName, interfaceName, device, interface);
    if (interface->GetRegion0() == region)
    {
      contact = new Contact(contactName, region, interface->GetNodes0(), materialName);
      contact->AddTriangles(interface->GetTriangles0());
      contact->AddEdges(interface->GetEdges0());
    }
    else if (interface->GetRegion1() == region)
    {
      contact = new Contact(contactName, region, interface->GetNodes1(), materialName);
      contact->AddTriangles(interface->GetTriangles1());
      contact->AddEdges(interface->GetEdges1());
    }
    else
    {
      std::ostringstream os;
      os << onInterfaceonDevice(interfaceName, deviceName) << " is not attached to region \"" << regionName << "\".\n";
      errorString = os.str();
      data.SetErrorResult(errorString);
      return;
    }

    device->AddContact(contact);
    data.SetEmptyResult();
}

void createInterfaceFromNodesCmd(CommandHandler &data)
{
  std::string errorString;

  const std::string commandName = data.GetCommandName();

  using namespace dsGetArgs;
  static dsGetArgs::Option option[] =
  {
      {"device",  "", dsGetArgs::optionType::STRING, dsGetArgs::requiredType::REQUIRED, mustBeValidDevice},
      {"name",    "", dsGetArgs::optionType::STRING, dsGetArgs::requiredType::REQUIRED, stringCannotBeEmpty},
      {"region0", "", dsGetArgs::optionType::STRING, dsGetArgs::requiredType::REQUIRED, mustBeValidRegion},
      {"region1", "", dsGetArgs::optionType::STRING, dsGetArgs::requiredType::REQUIRED, mustBeValidRegion},
      {"nodes0",  "", dsGetArgs::optionType::LIST,   dsGetArgs::requiredType::REQUIRED, nullptr},
      {"nodes1",  "", dsGetArgs::optionType::LIST,   dsGetArgs::requiredType::REQUIRED, nullptr},
      {nullptr,  nullptr, dsGetArgs::optionType::STRING, dsGetArgs::requiredType::OPTIONAL}
  };

  bool error = data.processOptions(option, errorString);

  if (error)
  {
      data.SetErrorResult(errorString);
      return;
  }

  const std::string &deviceName = data.GetStringOption("device");
  const std::string &name       = data.GetStringOption("name");

  std::string regionNames[2];
  regionNames[0] = data.GetStringOption("region0");
  regionNames[1] = data.GetStringOption("region1");

  Device *dev = nullptr;
  Region *reg[2] = {};

  for (size_t i = 0; i < 2; ++i)
  {
    errorString += ValidateDeviceAndRegion(deviceName, regionNames[i], dev, reg[i]);
  }

  if (!errorString.empty())
  {
      data.SetErrorResult(errorString);
      return;
  }

  std::vector<size_t> nodes[2];
  for (size_t i = 0; i < 2; ++i)
  {
    std::string oname("nodes");
    oname += std::to_string(i);
    ObjectHolder vdata = data.GetObjectHolder(oname);
    if (vdata.IsList())
    {
      bool ok = vdata.GetUnsignedLongList(nodes[i]);
      if (!ok)
      {
        std::ostringstream os;
        os << "Option \"" << oname << "\" could not be converted to a list of unsigned integers\n";
        errorString += os.str();
      }
    }
  }

  if (!errorString.empty())
  {
      data.SetErrorResult(errorString);
      return;
  }

  if ((nodes[0].empty() || nodes[1].empty()) || (nodes[0].size() != nodes[1].size()))
  {
    std::ostringstream os;
    os << "Node lists \"nodes0\", \"nodes1\" are empty or not the same size " << nodes[0].size() << " " << nodes[1].size() <<"\n";
    errorString = os.str();
    data.SetErrorResult(errorString);
    return;
  }

  size_t error_count = 0;
  std::vector<ConstNodePtr> inodes[2];
  for (size_t i = 0; i < 2; ++i)
  {
    inodes[i].resize(nodes[i].size(), nullptr);
    auto &nlist = reg[i]->GetNodeList();
    for (size_t j = 0; j < nodes[i].size(); ++j)
    {
      if (nodes[i][j] < nlist.size())
      {
        inodes[i][j] = nlist[nodes[i][j]];
      }
      else
      {
        ++error_count;
      }
    }
  }
  if (error_count > 0)
  {
    std::ostringstream os;
    os << "Node lists have invalid entries\n";
    errorString = os.str();
    data.SetErrorResult(errorString);
    return;
  }

  auto interface = new Interface(name, reg[0], reg[1], inodes[0], inodes[1]);
  dev->AddInterface(interface);
  data.SetEmptyResult();
}
}

