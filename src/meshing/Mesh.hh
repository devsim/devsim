/***
DEVSIM
Copyright 2013 DEVSIM LLC

SPDX-License-Identifier: Apache-2.0
***/

#ifndef MESH_HH
#define MESH_HH
#include <string>
namespace dsMesh {
class Mesh {
    public:
        virtual ~Mesh() = 0;
        const std::string &GetName();

        bool Instantiate(const std::string &, std::string &);
        bool Finalize(std::string &);
        bool IsFinalized();

    protected:
        virtual bool Instantiate_(const std::string &, std::string &) = 0;
        virtual bool Finalize_(std::string &) = 0;

        explicit Mesh(const std::string &);
        void     SetFinalized();
    private:
        Mesh();
        Mesh(const Mesh &);
        Mesh &operator=(const Mesh&);

        std::string name;
        bool finalized;
};

}
#endif
