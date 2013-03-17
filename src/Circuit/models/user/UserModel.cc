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

#include "UserModel.hh"
#include "InstanceModel.hh"
#include <list>
#include <vector>
#include <iostream>

#warning "use OutputStream.hh"
class NodeKeeper;

// need to fix for window platform
#ifdef __WIN32__
#include <windows.h>
#else
#include <dlfcn.h>
#endif

namespace {
UserModelList uml;
std::vector<void *> closeList;
};

namespace UserModel {

InstanceModel *instantiateModel(NodeKeeper *nk, const std::string &mod, const std::string &nm, const std::vector<std::string> &nl, const std::vector<std::pair< std::string, double> > &p) {
    InstanceModel *imp = NULL;
    if (uml.count(mod)) {
#if 0
        const size_t nn = uml[mod].second;
        if (nl.size() != nn)
        {
            std::cout <<  "ERROR: Model " << mod << " instance " << nm << " requires " << nn << " nodes.\n";
        }
        else
        {
            imp = (uml[mod].first)(nk, nm, nl);
        }
#else
            imp = (uml[mod])(nk, nm, nl);
#endif
    }
    else {
        std::cout <<  "ERROR: Model " << mod << " not available.\n";
    }

    for (size_t i=0; i < p.size(); ++i) {
        imp->addParam(p[i].first, p[i].second);
    }
    return imp;
}

UserModelList getUserModelList() {
    return uml;
}

void generateModelList() {
    // Need to encode the number of nodes required
    // someday add pointer to help function
    // also create alternative constructor taking a list of nodes
//    uml["testmodel"] = std::make_pair(&create_testmodel, 2);
}

void loadModel(const std::string &model, const std::string &file) {
    std::string mname = model + "_create";
    std::cout << "Loading: " << model << " " << file << "\n";

#ifdef __WIN32__
    HINSTANCE__ *handle = LoadLibrary(file.c_str());
#else
    void *handle = dlopen(file.c_str(), RTLD_LAZY);
#endif

    const char *msg = ""; //dlerror();
    if (handle != NULL) {
        std::cout << file << " loaded successfully\n";
    } else {
        std::cout << file << " not loaded successfully\n";
        std::cout << msg << "\n";
#ifndef __WIN32__
        dlclose(handle);
#endif
        return;
    }

#ifndef __WIN32__
    void *fp = dlsym(handle, mname.c_str());
    msg = dlerror();
    if (msg == NULL) {
        std::cout << mname << " found\n";
    } else {
        std::cout << mname << " not found\n";
        std::cout << msg << "\n";
        dlclose(handle);
        return;
    }
#else
    void *fp = (void *) GetProcAddress(handle, mname.c_str());
    if (fp != NULL) {
        std::cout << mname << " found\n";
    } else {
        std::cout << mname << " not found\n";
        return;
    }
#endif

    CreateUserModelPtr cump = (CreateUserModelPtr)(fp);
    uml[model] = cump;
    closeList.push_back(handle);
}

void unloadModels() {
    for (size_t i=0; i < closeList.size(); ++i)
#ifndef __WIN32__
        dlclose(closeList[i]);
#else
#endif
}

}
