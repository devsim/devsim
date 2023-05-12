/***
DEVSIM
Copyright 2013 DEVSIM LLC

SPDX-License-Identifier: Apache-2.0
***/

#ifndef MESH_1D_STRUCTS_HH
#define MESH_1D_STRUCTS_HH
#include <string>
#include <map>
namespace dsMesh {
/// Will also need version for medici style spacing
class MeshLine1d {
    public:
        MeshLine1d(double /*p*/, double /*ps*/, double /*ns*/);

        MeshLine1d(double /*p*/, double /*ps*/, double /*ns*/, const std::string &/*t*/);

        bool operator<(const MeshLine1d &mp) const {
            return position < mp.position;
        }

        double getPosition()
        {
            return position;
        }
        double getPositiveSpacing()
        {
            return posSpacing;
        }
        double getNegativeSpacing()
        {
            return negSpacing;
        }

        const std::string &GetTag()
        {
            return tag;
        }

    private:
        MeshLine1d();
        /// Use default varieties of these
//      MeshLine1d &operator=(MeshLine1d &);
//      MeshLine1d(const MeshLine1d &);

        double      position;
        double      posSpacing;
        double      negSpacing;
        std::string tag;
};

/// must do sanity checking to make sure that regions only meet on new
class MeshRegion1d {
    public:
        MeshRegion1d(const std::string &, const std::string &, const std::string &, const std::string &);
        const std::string &GetName() const;
        const std::string &GetMaterial() const;
        const std::string &GetTag0() const;
        const std::string &GetTag1() const;
        const size_t &GetIndex0() const;
        const size_t &GetIndex1() const;

        void sortTags(const std::map<std::string, size_t> &);

    private:
        MeshRegion1d();
        /// Rely on the default copy constructor and assignment operator
        std::string name;
        std::string material;
        std::string tag0;
        std::string tag1;
        size_t      index0;
        size_t      index1;
        bool        tagssorted;
};

/// here we assume that we contact both sides of the interface
class MeshInterface1d {
    public:
        MeshInterface1d(const std::string &, const std::string &);
        const std::string &GetName();
        const std::string &GetTag();
    private:
        /// Rely on the default copy constructor and assignment operator
        std::string name;
        std::string tag;
};

class MeshContact1d {
    public:
        MeshContact1d(const std::string &, const std::string &, const std::string &);
        const std::string &GetName();
        const std::string &GetMaterial();
        const std::string &GetTag();
    private:
        /// Rely on the default copy constructor and assignment operator
        std::string name;
        std::string material;
        std::string tag;
};

}
#endif
