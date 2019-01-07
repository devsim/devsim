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

#include "CheckFunctions.hh"
#include "CommandHandler.hh"
#include "GlobalData.hh"
#include "MeshKeeper.hh"
#include "Mesh1d.hh"
#include "Validate.hh"
#include <sstream>
namespace dsCommand {

bool mustBePositive(const std::string &optionName, std::string &errorString, const CommandHandler &args)
{
    bool ret = true;

    const double dval = args.GetDoubleOption(optionName);

    if (dval <= 0)
    {
        std::ostringstream os;
        os << "-" << optionName << " must be greater than 0.0\n";
        errorString += os.str();
        ret = false;
    }
    
    return ret;
}


bool stringCannotBeEmpty(const std::string &optionName, std::string &errorString, const CommandHandler &args)
{
    bool ret = true;

    const std::string &sval = args.GetStringOption(optionName);
    if (sval.empty())
    {
        std::ostringstream os;
        os << "-" << optionName << " cannot be empty string\n";
        errorString += os.str();
        ret = false;
    }
    
    return ret;
}

bool meshCannotExist(const std::string &optionName, std::string &errorString, const CommandHandler &args)
{
    bool ret = true;

    ret = stringCannotBeEmpty(optionName, errorString, args);

    if (ret)
    {
        const std::string &sval = args.GetStringOption(optionName);

        dsMesh::MeshKeeper &mdata = dsMesh::MeshKeeper::GetInstance();

        if (mdata.GetMesh(sval))
        {
            std::ostringstream os;
            os << "A mesh already exists with name \"" << sval << "\"\n";
            errorString += os.str();
            ret = false;
        }
    }

    return ret;
}

bool meshMustExist(const std::string &optionName, std::string &errorString, const CommandHandler &args)
{
    bool ret = true;

    ret = stringCannotBeEmpty(optionName, errorString, args);

    if (ret)
    {
        const std::string &sval = args.GetStringOption(optionName);

        dsMesh::MeshKeeper &mdata = dsMesh::MeshKeeper::GetInstance();

        if (!mdata.GetMesh(sval))
        {
            std::ostringstream os;
            os << "A mesh does not exist with name \"" << sval << "\"\n";
            errorString += os.str();
            ret = false;
        }
    }

    return ret;
}

bool meshMustNotBeFinalized(const std::string &optionName, std::string &errorString, const CommandHandler &args)
{
    bool ret = true;

    ret = meshMustExist(optionName, errorString, args);

    if (ret)
    {
        const std::string &sval = args.GetStringOption(optionName);

        dsMesh::MeshKeeper &mdata = dsMesh::MeshKeeper::GetInstance();

        if (mdata.GetMesh(sval)->IsFinalized())
        {
            std::ostringstream os;
            os << "Mesh \"" << sval << "\" must not be finalized\n";
            errorString += os.str();
            ret = false;
        }
    }

    return ret;
}

bool meshMustBeFinalized(const std::string &optionName, std::string &errorString, const CommandHandler &args)
{
    bool ret = true;

    ret = meshMustExist(optionName, errorString, args);

    if (ret)
    {
        const std::string &sval = args.GetStringOption(optionName);

        dsMesh::MeshKeeper &mdata = dsMesh::MeshKeeper::GetInstance();

        if (!mdata.GetMesh(sval)->IsFinalized())
        {
            std::ostringstream os;
            os << "Mesh \"" << sval << "\" must already be finalized\n";
            errorString += os.str();
            ret = false;
        }
    }

    return ret;
}

bool mustBeValidDevice(const std::string &optionName, std::string &errorString, const CommandHandler &args)
{
    bool ret = true;

    ret = stringCannotBeEmpty(optionName, errorString, args);

    const std::string &sval = args.GetStringOption(optionName);

    Device *dev = nullptr;

    std::string validate = dsValidate::ValidateDevice(sval, dev);
    if (!validate.empty())
    {
        errorString += validate;
        ret = false;
    }

    return ret;
}

bool mustNotBeValidDevice(const std::string &optionName, std::string &errorString, const CommandHandler &args)
{
    bool ret = true;

    ret = stringCannotBeEmpty(optionName, errorString, args);

    const std::string &sval = args.GetStringOption(optionName);

    Device *dev = nullptr;

    dsValidate::ValidateDevice(sval, dev);
    if (dev)
    {
        std::ostringstream os;
        os << "Device \"" << sval << "\" already exists\n";
        errorString += os.str();
        ret = false;
    }

    return ret;
}


bool mustBeValidRegion(const std::string &optionName, std::string &errorString, const CommandHandler &args)
{
    bool ret = true;

    ret = stringCannotBeEmpty("device", errorString, args) && stringCannotBeEmpty(optionName, errorString, args);

    if (ret)
    {
        const std::string &dname = args.GetStringOption("device");
        const std::string &rname = args.GetStringOption(optionName);

        Device *dev = nullptr;
        Region *reg = nullptr;

        std::string validate = dsValidate::ValidateDeviceAndRegion(dname, rname, dev, reg);
        if (!validate.empty())
        {
            errorString += validate;
            ret = false;
        }
    }
    return ret;
}

bool mustBeValidContact(const std::string &optionName, std::string &errorString, const CommandHandler &args)
{
    bool ret = true;

    ret = stringCannotBeEmpty("device", errorString, args) && stringCannotBeEmpty(optionName, errorString, args);

    if (ret)
    {
        const std::string &dname = args.GetStringOption("device");
        const std::string &cname = args.GetStringOption(optionName);

        Device *dev = nullptr;
        Contact *con = nullptr;

        std::string validate = dsValidate::ValidateDeviceAndContact(dname, cname, dev, con);
        if (!validate.empty())
        {
            errorString += validate;
            ret = false;
        }
    }
    return ret;
}

bool mustBeValidInterface(const std::string &optionName, std::string &errorString, const CommandHandler &args)
{
    bool ret = true;

    ret = stringCannotBeEmpty("device", errorString, args) && stringCannotBeEmpty(optionName, errorString, args);

    if (ret)
    {
        const std::string &dname = args.GetStringOption("device");
        const std::string &iname = args.GetStringOption(optionName);

        Device *dev = nullptr;
        Interface *interface = nullptr;

        std::string validate = dsValidate::ValidateDeviceAndInterface(dname, iname, dev, interface);
        if (!validate.empty())
        {
            errorString += validate;
            ret = false;
        }
    }
    return ret;
}


bool mustBeSpecifiedIfRegionSpecified(const std::string &optionName, std::string &errorString, const CommandHandler &args)
{
    bool ret = true;
    const std::string &dname = args.GetStringOption(optionName);
    const std::string &rname = args.GetStringOption("region");
    if (dname.empty() && !rname.empty())
    {
        std::ostringstream os;
        os << "Device name must be specified if region specified\n";
        errorString += os.str();
        ret = false;
    }
    return ret;
}
}
