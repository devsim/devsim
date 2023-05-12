/***
DEVSIM
Copyright 2013 DEVSIM LLC

SPDX-License-Identifier: Apache-2.0
***/

#ifndef MESH_2D_STRUCTS_HH
#define MESH_2D_STRUCTS_HH
#include <string>
#include <map>
#include "MeshLoaderStructs.hh"

namespace dsMesh {

class BoundingBox {
    public:

    BoundingBox(double x0, double x1, double y0, double y1, double bl) : x0_(x0), x1_(x1), y0_(y0), y1_(y1), bloat_(bl)
    {
        if (x0 > x1)
        {
            x1_ = x0;
            x0_ = x1;
        }
        if (y0 > y1)
        {
            y1_ = y0;
            y0_ = y1;
        }
    }


    bool IsPointInside(double x, double y) const
    {
        return (
            (x > (x0_ - bloat_)) &&
            (x < (x1_ + bloat_)) &&
            (y > (y0_ - bloat_)) &&
            (y < (y1_ + bloat_))
            );
    }

        double GetX0() const
        {
            return x0_;
        }
        double GetX1() const
        {
            return x1_;
        }
        double GetY0() const
        {
            return y0_;
        }
        double GetY1() const
        {
            return y1_;
        }
        double GetBloat() const
        {
            return bloat_;
        }

    private:

    double x0_;
    double x1_;
    double y0_;
    double y1_;
    double bloat_;
};

typedef std::vector<BoundingBox> BoundingBoxList_t;


class MeshLine2d {
    public:

//// start out with non-terminating mesh lines
#if 0
        MeshLine2d(Direction /*dir*/, double /*p*/, double /*ps*/, double /*ns*/, const std::string &/*t*/);
#endif
        MeshLine2d(double /*p*/, double /*ps*/, double /*ns*/);

        bool operator<(const MeshLine2d &mp) const {
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

    private:
        MeshLine2d();

        double      position;
        double      posSpacing;
        double      negSpacing;

//// start out with non-terminating mesh lines
#if 0
        double      w0;
        double      w1;
#endif


//      MeshLine2d &operator=(MeshLine2d &);
//      MeshLine2d(const MeshLine2d &);

};

/// must do sanity checking to make sure that regions only meet on new
class MeshRegion2d {
    public:
        MeshRegion2d(const std::string &/*region*/, const std::string &/*material*/);

        void AddBoundingBox(const MeshRegion2d &mr);

        const std::string &GetName() const
        {
            return name_;
        }
        const std::string &GetMaterial() const
        {
            return material_;
        }

/*
        bool   IsCoordinateInside(const MeshCoordinate &) const;
*/
        bool IsPointInside(double /*x*/, double /*y*/) const;

        void AddBoundingBox(const MeshRegion &);
        void AddBoundingBox(const BoundingBox &bb)
        {
            bboxes_.push_back(bb);
#if 0
            std::cerr << "add bounding box " << name_ << " "
                << bb.GetX0() << " " << bb.GetY0() << " "
                << bb.GetX1() << " " << bb.GetY1() << " "
                << "\n";
#endif
        }


        MeshRegion2d();
    private:

        /// Rely on the default copy constructor and assignment operator
        std::string        name_;
        std::string        material_;
        BoundingBoxList_t  bboxes_;
};

/// here we assume that we contact both sides of the interface
class MeshInterface2d {
    public:
        //// We will use overlaps to detect interface connections
        MeshInterface2d(const std::string &/*name*/, const std::string &/*region0*/, const std::string &/*region1*/);
        const std::string &GetName() const
        {
            return name;
        }
        const std::string &GetRegion0() const
        {
            return Region0;
        }
        const std::string &GetRegion1() const
        {
            return Region1;
        }

        bool IsPointInside(double /*x*/, double /*y*/) const;

        void AddBoundingBox(const MeshInterface2d &);
        void AddBoundingBox(const BoundingBox &bb)
        {
            bboxes_.push_back(bb);
        }

        MeshInterface2d();
    private:
        std::string name;
        std::string Region0;
        std::string Region1;
        BoundingBoxList_t  bboxes_;
};

/// Must insist we are not at an interface
/// What would happen to equation permutations??? (may be really bad)
class MeshContact2d {
    public:
        //// We will use overlaps to detect interface connections
        MeshContact2d(const std::string &/*name*/, const std::string &/*material*/, const std::string &/*region*/);
        const std::string &GetName() const
        {
            return name;
        }
        const std::string &GetRegion() const
        {
            return Region;
        }

        const std::string &GetMaterial() const
        {
            return material;
        }

        void AddBoundingBox(const MeshContact2d &);
        void AddBoundingBox(const BoundingBox &bb)
        {
            bboxes_.push_back(bb);
        }

        bool IsPointInside(double /*x*/, double /*y*/) const;

        MeshContact2d();
    private:
        std::string name;
        std::string material;
        std::string Region;
        BoundingBoxList_t  bboxes_;
};

//// If we have coordinates which don't become material, do we create them with default region, or do we just not assign them to the final structure.
}
#endif
