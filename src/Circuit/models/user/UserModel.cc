/***
DEVSIM
Copyright 2013 DEVSIM LLC

SPDX-License-Identifier: Apache-2.0
***/

#include "UserModel.hh"
#include "InstanceModel.hh"
#include <list>
#include <vector>
#include <iostream>

#warning "use OutputStream.hh"
class NodeKeeper;

// need to fix for window platform
#ifdef _WIN32
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
    InstanceModel *imp = nullptr;
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

#ifdef _WIN32
    HINSTANCE__ *handle = LoadLibrary(file.c_str());
#else
    void *handle = dlopen(file.c_str(), RTLD_LAZY);
#endif

    const char *msg = ""; //dlerror();
    if (handle != nullptr) {
        std::cout << file << " loaded successfully\n";
    } else {
        std::cout << file << " not loaded successfully\n";
        std::cout << msg << "\n";
#ifndef _WIN32
        dlclose(handle);
#endif
        return;
    }

#ifndef _WIN32
    void *fp = dlsym(handle, mname.c_str());
    msg = dlerror();
    if (msg == nullptr) {
        std::cout << mname << " found\n";
    } else {
        std::cout << mname << " not found\n";
        std::cout << msg << "\n";
        dlclose(handle);
        return;
    }
#else
    void *fp = (void *) GetProcAddress(handle, mname.c_str());
    if (fp != nullptr) {
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
#ifndef _WIN32
        dlclose(closeList[i]);
#else
#endif
}

}
