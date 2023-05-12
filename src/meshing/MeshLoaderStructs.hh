/***
DEVSIM
Copyright 2013 DEVSIM LLC

SPDX-License-Identifier: Apache-2.0
***/

#ifndef MESH_LOADER_STRUCTS_HH
#define MESH_LOADER_STRUCTS_HH
#include "Vector.hh"
#include "ObjectHolder.hh"
#include <string>
#include <vector>
#include <map>
#include <algorithm>
namespace dsMesh {
class Solution;
typedef Solution *SolutionPtr;
typedef std::map<std::string, SolutionPtr> MeshSolutionList_t;

class Equation;
typedef Equation *EquationPtr;
typedef std::map<std::string, EquationPtr> MeshEquationList_t;

class MeshCoordinate;
class MeshNode;
class MeshEdge;
class MeshTriangle;
class MeshTetrahedron;

typedef MeshCoordinate *MeshCoordinatePtr;
typedef const MeshCoordinate *ConstMeshCoordinatePtr;
typedef MeshNode *MeshNodePtr;
typedef MeshEdge *MeshEdgePtr;
typedef MeshTriangle *MeshTrianglePtr;
typedef MeshTetrahedron *MeshTetrahedronPtr;

class MeshRegion;
class MeshContact;
class MeshInterface;

typedef MeshRegion *MeshRegionPtr;
typedef MeshContact *MeshContactPtr;
typedef MeshInterface *MeshInterfacePtr;

typedef std::vector<MeshNode>        MeshNodeList_t;
typedef std::vector<MeshEdge>        MeshEdgeList_t;
typedef std::vector<MeshTriangle>    MeshTriangleList_t;
typedef std::vector<MeshTetrahedron> MeshTetrahedronList_t;


class Solution {
    public:
        enum class ModelType {MUNDEFINED = 0, NODE, EDGE, TRIANGLEEDGE, TETRAHEDRONEDGE, INTERFACENODE};
        enum class DataType  {DUNDEFINED = 0, BUILTIN, DATAPARENT, UNIFORM, DATA, COMMAND};

        static const char *ModelTypeString[];
        static const char *DataTypeString[];
        //// Should get size reservation from number of nodes in region
        Solution(const std::string &n) : name(n), model_type(ModelType::MUNDEFINED), data_type(DataType::DUNDEFINED), uniform_value(0.0), reserve_size(0) {
        }

        bool HasValues() {
            return !values.empty();
        }

        const std::string &GetName() {
            return name;
        }

        const std::string &GetCommandName() const {
            return command_name;
        }

        const std::vector<std::pair<std::string, ObjectHolder> > &GetCommandArgs() const {
            return command_args;
        }

        void SetCommandName(const std::string &p) {
            command_name = p;
        }

        void AddCommandOption(const std::string &n, ObjectHolder o)
        {
          command_args.push_back(std::make_pair(n, o));
        }

        const std::string &GetParent() {
            return parent;
        }

        void SetParent(const std::string &p) {
            parent = p;
        }

        void AddValue(double x)
        {
            values.push_back(x);
        }

        ModelType GetModelType() const  {
            return model_type;
        }

        void SetModelType(ModelType mt) {
          model_type = mt;
        }

        DataType GetDataType() const  {
            return data_type;
        }

        void SetDataType(DataType dt) {
          data_type = dt;
          if (data_type == DataType::DATA)
          {
            values.reserve(reserve_size);
          }
        }

        double GetUniformValue() const
        {
          return uniform_value;
        }

        void SetUniformValue(double v)
        {
          uniform_value = v;
        }

        typedef std::vector<double> values_t;

        const values_t &GetValues() const
        {
            return values;
        }

        void SetReserve(size_t rs)
        {
          reserve_size = rs;
        }

    private:
        std::string name;
        std::string command_name;
        std::vector<std::pair<std::string, ObjectHolder> > command_args;
        std::string parent;
        ModelType   model_type;
        DataType    data_type;
        values_t    values;
        double      uniform_value;
        size_t      reserve_size;
};

class Equation {
    public:
        Equation(const std::string &n) : name(n) {
        }


        const std::string &GetName() {
            return name;
        }

        const std::string &GetCommandName() const {
            return command_name;
        }

        const std::vector<std::pair<std::string, ObjectHolder> > &GetCommandArgs() const {
            return command_args;
        }

        void SetCommandName(const std::string &p) {
            command_name = p;
        }

        void AddCommandOption(const std::string &n, ObjectHolder o)
        {
          command_args.push_back(std::make_pair(n, o));
        }

    private:
        std::string name;
        std::string command_name;
        std::vector<std::pair<std::string, ObjectHolder> > command_args;
};

//// This refers to the index of the coordinate
class MeshNode
{
    public:
        MeshNode(size_t i) : index(i) {}
        size_t Index() const
        {
            return index;
        }

        bool operator==(const MeshNode &mn) const
        {
          return (this->index == mn.Index());
        }

        bool operator<(const MeshNode &mn) const
        {
          return (this->index < mn.Index());
        }
    private:
        //// This is the coordinate index
        size_t index;
};

//// This refers to the index of the two nodes
class MeshEdge
{
    public:
        MeshEdge(size_t i, size_t j) : index0(i), index1(j)
        {
          if ( j < i)
          {
             index0 = j;
             index1 = i;
          }
        }
        size_t Index0() const
        {
            return index0;
        }
        size_t Index1() const
        {
            return index1;
        }

        bool operator==(const MeshEdge &me) const
        {
          return (this->index0 == me.Index0()) && (this->index1 == me.Index1());
        }

        bool operator<(const MeshEdge &me) const
        {
          bool ret = false;
          if (this->index0 < me.Index0())
          {
            ret = true;
          }
          else if (this->index0 == me.Index0())
          {
            ret = (this->index1 < me.Index1());
          }
          return ret;
        }

    private:
        //// These are the node indices
        size_t index0;
        size_t index1;
};

class MeshTriangle
{
    public:
        MeshTriangle(size_t i, size_t j, size_t k) : index0(i), index1(j), index2(k)
        {
          std::vector<size_t> vec;
          vec.reserve(3);
          vec.push_back(i);
          vec.push_back(j);
          vec.push_back(k);
          std::sort(vec.begin(), vec.end());
          index0 = vec[0];
          index1 = vec[1];
          index2 = vec[2];
        }

        bool operator==(const MeshTriangle &mt) const
        {
          return (this->index0 == mt.Index0())
          && (this->index1 == mt.Index1())
          && (this->index2 == mt.Index2());
        }

        bool operator< (const MeshTriangle &mt) const
        {
          bool ret = false;
          if (this->index0 < mt.Index0())
          {
            ret = true;
          }
          else if (this->index0 == mt.Index0())
          {
            if (this->index1 < mt.Index1())
            {
              ret = true;
            }
            else if (this->index1 == mt.Index1())
            {
              ret = (this->index2 < mt.Index2());
            }
          }

          return ret;
        }

        size_t Index0() const
        {
            return index0;
        }
        size_t Index1() const
        {
            return index1;
        }
        size_t Index2() const
        {
            return index2;
        }
    private:
        //// These are the node indices
        size_t index0;
        size_t index1;
        size_t index2;
};

class MeshTetrahedron
{
    public:
        MeshTetrahedron(size_t i, size_t j, size_t k, size_t l) : index0(i), index1(j), index2(k), index3(l)
        {
          std::vector<size_t> vec;
          vec.reserve(4);
          vec.push_back(i);
          vec.push_back(j);
          vec.push_back(k);
          vec.push_back(l);
          std::sort(vec.begin(), vec.end());
          index0 = vec[0];
          index1 = vec[1];
          index2 = vec[2];
          index3 = vec[3];
        }

        bool operator==(const MeshTetrahedron &mt) const
        {
          return (this->index0 == mt.Index0())
          && (this->index1 == mt.Index1())
          && (this->index2 == mt.Index2())
          && (this->index3 == mt.Index3())
          ;
        }

        bool operator< (const MeshTetrahedron &mt) const
        {
          bool ret = false;
          if (this->index0 < mt.Index0())
          {
            ret = true;
          }
          else if (this->index0 == mt.Index0())
          {
            if (this->index1 < mt.Index1())
            {
              ret = true;
            }
            else if (this->index1 == mt.Index1())
            {
              if (this->index2 < mt.Index2())
              {
                ret = true;
              }
              else if (this->index2 == mt.Index2())
              {
                ret = (this->index3 < mt.Index3());
              }
            }
          }

          return ret;
        }

        size_t Index0() const
        {
            return index0;
        }
        size_t Index1() const
        {
            return index1;
        }
        size_t Index2() const
        {
            return index2;
        }
        size_t Index3() const
        {
            return index3;
        }
    private:
        //// These are the node indices
        size_t index0;
        size_t index1;
        size_t index2;
        size_t index3;
};

class MeshCoordinate {
    public:
        MeshCoordinate(double x=0.0, double y=0.0, double z=0.0)
        : xpos(x), ypos(y), zpos(z)
        {
        }
        double GetX() const {
            return xpos;
        }
        double GetY() const {
            return ypos;
        }
        double GetZ() const {
            return zpos;
        }
        Vector<double> GetVector() const
        {
            return Vector<double>(xpos, ypos, zpos);
        }


    private:
        double xpos;
        double ypos;
        double zpos;
};


class MeshRegion {
    public:
        MeshRegion(const std::string &n, const std::string &m) : name(n), material(m)
        {
            nodes.reserve(1000);
            edges.reserve(1000);
        }

//      MeshRegion();

        ~MeshRegion();

        const std::string &GetName() const
        {
            return name;
        }

        const std::string &GetMaterial() const
        {
            return material;
        }

        void AddNode(const MeshNode &mn)
        {
            nodes.push_back(mn);
        }

        void AddEdge(const MeshEdge &me)
        {
            edges.push_back(me);
        }

        void AddTriangle(const MeshTriangle &mt)
        {
            triangles.push_back(mt);
        }

        void AddTetrahedron(const MeshTetrahedron &mt)
        {
            tetrahedra.push_back(mt);
        }

        bool HasNodes() const {
            return !nodes.empty();
        }

        bool HasEdges() const {
            return !edges.empty();
        }

        bool HasTriangles() const {
            return !triangles.empty();
        }

        bool HasTetrahedra() const {
            return !tetrahedra.empty();
        }

        const MeshNodeList_t &GetNodes() const
        {
            return nodes;
        }

        const MeshEdgeList_t &GetEdges() const
        {
            return edges;
        }

        const MeshTriangleList_t &GetTriangles() const
        {
            return triangles;
        }

        const MeshTetrahedronList_t &GetTetrahedra() const
        {
            return tetrahedra;
        }

        bool IsSolution(const std::string &n)
        {
            return solutionList.count(n) != 0;
        }

        void AddSolution(SolutionPtr sp)
        {
            solutionList[sp->GetName()] = sp;
        }

        const MeshSolutionList_t &GetSolutionList() const
        {
            return solutionList;
        }

        bool IsEquation(const std::string &n)
        {
            return equationList.count(n) != 0;
        }

        void AddEquation(EquationPtr sp)
        {
            equationList[sp->GetName()] = sp;
        }

        const MeshEquationList_t &GetEquationList() const
        {
            return equationList;
        }

        // break down shapes into edges and nodes
        // Similar to the one used in the dsMesh::Shapes class
        void DecomposeAndUniquify();

    private:
        MeshRegion();
//      MeshRegion(const MeshRegion &);
//      MeshRegion &operator=(const MeshRegion &);

        std::string         name;
        std::string         material;
        //// These correspond to coordinate indices
        MeshNodeList_t        nodes;
        MeshEdgeList_t        edges;
        MeshTriangleList_t    triangles;
        MeshTetrahedronList_t tetrahedra;
        MeshSolutionList_t       solutionList;
        MeshEquationList_t       equationList;
};

class MeshInterfaceNodePair
{
    public:
        MeshInterfaceNodePair(size_t i, size_t j) : index0(i), index1(j) {}

        void SetIndex0(size_t i)
        {
          index0 = i;
        }

        void SetIndex1(size_t i)
        {
          index1 = i;
        }

        size_t Index0() const
        {
            return index0;
        }

        size_t Index1() const
        {
            return index1;
        }
    private:
        size_t index0;
        size_t index1;
};

class MeshInterface {
    public:
        MeshInterface(const std::string &n, const std::string &r0, const std::string &r1) : name(n), region0(r0, ""), region1(r1, "")
        {
        }

        ~MeshInterface();

        void AddNodePair(const MeshInterfaceNodePair &np)
        {
            nodePairs.push_back(np);
        }

        void AddEdgePair(const MeshEdge &t0, const MeshEdge &t1)
        {
          region0.AddEdge(t0);
          region1.AddEdge(t1);
        }

        void AddTrianglePair(const MeshTriangle &t0, const MeshTriangle &t1)
        {
          region0.AddTriangle(t0);
          region1.AddTriangle(t1);
        }

        bool HasNodesPairs() const
        {
            return !nodePairs.empty();
        }

        bool HasEdges() const
        {
          return region0.HasEdges();
        }

        bool HasTriangles() const
        {
          return region0.HasTriangles();
        }

        const std::string &GetName() const
        {
            return name;
        }
        const std::string &GetRegion0() const
        {
            return region0.GetName();
        }

        const std::string &GetRegion1() const
        {
            return region1.GetName();
        }

        typedef std::vector<MeshInterfaceNodePair > NodePairList_t;
        const NodePairList_t &GetNodePairs()
        {
            return nodePairs;
        }

        const MeshEdgeList_t &GetEdges0() const
        {
          return region0.GetEdges();
        }

        const MeshEdgeList_t &GetEdges1() const
        {
          return region1.GetEdges();
        }

        const MeshTriangleList_t &GetTriangles0() const
        {
          return region0.GetTriangles();
        }

        const MeshTriangleList_t &GetTriangles1() const
        {
          return region1.GetTriangles();
        }

        bool IsSolution(const std::string &n)
        {
            return solutionList.count(n) != 0;
        }

        void AddSolution(SolutionPtr sp)
        {
            solutionList[sp->GetName()] = sp;
        }

        const MeshSolutionList_t &GetSolutionList() const
        {
            return solutionList;
        }

        bool IsEquation(const std::string &n)
        {
            return equationList.count(n) != 0;
        }

        void AddEquation(EquationPtr sp)
        {
            equationList[sp->GetName()] = sp;
        }

        const MeshEquationList_t &GetEquationList() const
        {
            return equationList;
        }

        void DecomposeAndUniquify();

        MeshRegion &GetMeshRegion0()
        {
          return region0;
        }

        MeshRegion &GetMeshRegion1()
        {
          return region1;
        }

    private:
        std::string name;
        MeshRegion region0;
        MeshRegion region1;
        NodePairList_t nodePairs;
        MeshSolutionList_t       solutionList;
        MeshEquationList_t       equationList;
};

class MeshContact {
    public:
        MeshContact(const std::string &n, const std::string &r, const std::string &m) : name(n), region(r, ""), material(m)
        {
        }

        ~MeshContact();

        void AddNode(const MeshNode &n)
        {
            region.AddNode(n);
        }

        void AddEdge(const MeshEdge &e)
        {
            region.AddEdge(e);
        }

        void AddTriangle(const MeshTriangle &t)
        {
            region.AddTriangle(t);
        }

        bool HasNodes() const
        {
            return region.HasNodes();
        }

        bool HasEdges() const
        {
            return region.HasEdges();
        }

        bool HasTriangles()
        {
            return region.HasTriangles();
        }

        const std::string &GetName() const
        {
            return name;
        }

        const std::string &GetRegion() const
        {
            return region.GetName();
        }

        const std::string &GetMaterial() const
        {
            return material;
        }

        const MeshNodeList_t &GetNodes()
        {
            return region.GetNodes();
        }

        const MeshEdgeList_t &GetEdges()
        {
            return region.GetEdges();
        }

        const MeshTriangleList_t &GetTriangles()
        {
            return region.GetTriangles();
        }

        bool IsEquation(const std::string &n)
        {
            return equationList.count(n) != 0;
        }

        void AddEquation(EquationPtr sp)
        {
            equationList[sp->GetName()] = sp;
        }

        const MeshEquationList_t &GetEquationList() const
        {
            return equationList;
        }

        void DecomposeAndUniquify();

        MeshRegion &GetMeshRegion()
        {
          return region;
        }

    private:
        std::string               name;
        MeshRegion                region;
        std::string               material;
        MeshEquationList_t        equationList;
};

}
#endif

