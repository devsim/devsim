%{
/***
DEVSIM
Copyright 2016 DEVSIM LLC

SPDX-License-Identifier: Apache-2.0
***/

#include "Devsimyystype.hh"
#include "DevsimReader.hh"
#include "DevsimLoader.hh"
#include "MeshKeeper.hh"
#include "OutputStream.hh"
#include <sstream>

int Devsimerror(const char *msg);

int Devsimlex();
%}

//%token HELP
%token <dval> FLOAT
%token <ival> INT
%token <str>  WORD COMMAND_OPTION
%token COMMAND_EOL
%token        BEG_DEVICE BEG_COORD BEG_REGION BEG_NODE
%token        BEG_EDGE BEG_TRIANGLE BEG_TETRAHEDRON BEG_NODESOL BEG_EDGESOL
%token        BEG_CONTACT BEG_INTERFACE
%token        END_DEVICE END_COORD END_REGION END_NODE
%token        END_EDGE END_TRIANGLE END_TETRAHEDRON END_NODESOL END_EDGESOL
%token        END_CONTACT END_INTERFACE
%token        DATASECTION BUILTINSECTION DATAPARENTSECTION UNIFORMSECTION COMMANDSECTION
%token        BEG_NODEMODEL END_NODEMODEL
%token        BEG_EDGEMODEL END_EDGEMODEL
%token        BEG_TRIANGLEEDGEMODEL END_TRIANGLEEDGEMODEL
%token        BEG_TETRAHEDRONEDGEMODEL END_TETRAHEDRONEDGEMODEL
%token        BEG_INTERFACENODEMODEL END_INTERFACENODEMODEL
%token        BEG_INTERFACEEQUATION END_INTERFACEEQUATION
%token        BEG_REGIONEQUATION END_REGIONEQUATION
%token        BEG_CONTACTEQUATION END_CONTACTEQUATION

%type <dval> number

%%
all : | all device
    ;

device :
        begin_devicecmd |
        device end_devicecmd |
        device coordinates |
        device region |
        device interface |
        device contact
        ;


begin_devicecmd : BEG_DEVICE WORD  {
    dsMesh::MeshKeeper &mk = dsMesh::MeshKeeper::GetInstance();
    if (dsDevsimParse::DevsimLoader)
    {
        std::ostringstream os;
        os << "ERROR: currently trying to load another device " << dsDevsimParse::DevsimLoader->GetName() << "\n";
        Devsimerror(os.str().c_str());
        YYABORT;
    }
    else if (mk.GetMesh($2))
    {
        std::ostringstream os;
        os << "ERROR: a mesh already exists by the name " << $2 << "\n";
        Devsimerror(os.str().c_str());
        YYABORT;
    }
    else
    {
        dsDevsimParse::DevsimLoader = new dsMesh::DevsimLoader($2);
        dsMesh::MeshKeeper &mk = dsMesh::MeshKeeper::GetInstance();
        mk.AddMesh(dsDevsimParse::DevsimLoader);
    }
}
;

end_devicecmd : END_DEVICE {
  std::string errorString;
  bool ret = dsDevsimParse::DevsimLoader->Finalize(errorString);
  if (ret)
  {
    std::string deviceName = dsDevsimParse::DevsimLoader->GetName();
    //// should we finalize and instantiate by default ????
    //// or should this be the last step for the user
    ret = dsDevsimParse::DevsimLoader->Instantiate(dsDevsimParse::DevsimLoader->GetName(), errorString);
    dsDevsimParse::DevsimLoader = nullptr;
  }

  if (!ret)
  {
    Devsimerror(errorString.c_str());
    YYABORT;
  }

  dsDevsimParse::DeletePointers();
}

coordinates : BEG_COORD
        | coordinates coordinate
        | coordinates END_COORD
        ;

coordinate : number number number
        {
            dsDevsimParse::DevsimLoader->AddCoordinate(dsMesh::MeshCoordinate($1, $2, $3));
        }
        ;

number : FLOAT {
            $$ = $1;
        } |
        INT
        {
            $$ = $1;
        }
        ;

region : BEG_REGION WORD WORD  {
            if (dsDevsimParse::MeshRegion)
            {
                std::ostringstream os;
                os << "ERROR: currently trying to load another region " << dsDevsimParse::MeshRegion->GetName() << "\n";
                Devsimerror(os.str().c_str());
                YYABORT;
            }
            else if (dsDevsimParse::DevsimLoader->IsMeshRegion($2))
            {
                std::ostringstream os;
                os << "ERROR: region already exists on device " << $2 << "\n";
                Devsimerror(os.str().c_str());
                YYABORT;
            }
            else
            {
                dsDevsimParse::MeshRegion = new dsMesh::MeshRegion($2, $3);
            }
        } |
        region END_REGION  {
            if (!dsDevsimParse::MeshRegion)
            {
                std::ostringstream os;
                os << "ERROR: No region to end " << dsDevsimParse::MeshRegion->GetName() << "\n";
                Devsimerror(os.str().c_str());
                YYABORT;
            }
            else
            {
                dsDevsimParse::DevsimLoader->AddRegion(dsDevsimParse::MeshRegion);
                dsDevsimParse::MeshRegion = nullptr;
            }
        } |
        region nodes |
        region edges |
        region triangles |
        region tetrahedra |
        region nodesol |
        region edgesol |
        region nodemodel |
        region edgemodel |
        region triangleedgemodel |
        region tetrahedronedgemodel |
        region regionequation
        ;

nodes : BEG_NODE
        {
            if (dsDevsimParse::MeshRegion->HasNodes())
            {
                std::ostringstream os;
                os << "ERROR: Region " << dsDevsimParse::MeshRegion->GetName() << " already has nodes\n";
                Devsimerror(os.str().c_str());
                YYABORT;
            }
        } |
        nodes INT
        {
            if ($2 < 0)
            {
                std::ostringstream os;
                os << "ERROR: Cannot have negative node index " << $2 << "\n";
                Devsimerror(os.str().c_str());
                YYABORT;
            }
            else
            {
//              std::ostringstream os;
//              os << "add node with coord index " << $2 << "\n";
//              OutputStream::WriteOut(OutputStream::INFO, os.str());
                dsDevsimParse::MeshRegion->AddNode(dsMesh::MeshNode($2));
            }
        } |
        nodes END_NODE ;
        ;

contact_nodes : BEG_NODE
        {
            if (dsDevsimParse::MeshContact->HasNodes())
            {
                std::ostringstream os;
                os << "ERROR: Region " << dsDevsimParse::MeshContact->GetName() << " already has nodes\n";
                Devsimerror(os.str().c_str());
                YYABORT;
            }
        } |
        contact_nodes INT
        {
            if ($2 < 0)
            {
                std::ostringstream os;
                os << "ERROR: Cannot have negative contact node index " << $2 << "\n";
                Devsimerror(os.str().c_str());
                YYABORT;
            }
            else
            {
                dsDevsimParse::MeshContact->AddNode(dsMesh::MeshNode($2));
            }
        } |
        contact_nodes END_NODE;
        ;

contact_edges : BEG_EDGE
        {
            if (dsDevsimParse::MeshContact->HasEdges())
            {
                std::ostringstream os;
                os << "ERROR: Region " << dsDevsimParse::MeshContact->GetName() << " already has edges\n";
                Devsimerror(os.str().c_str());
                YYABORT;
            }
        } |
        contact_edges INT INT
        {
            int nodes[2] = {$2, $3};
            for (size_t i = 0; i < 2; ++i)
            {
              if (nodes[i] < 0)
              {
                  std::ostringstream os;
                  os << "ERROR: Cannot have negative contact node index " << nodes[i] << "\n";
                  Devsimerror(os.str().c_str());
                  YYABORT;
              }
            }

            dsDevsimParse::MeshContact->AddEdge(dsMesh::MeshEdge($2, $3));
        } |
        contact_edges END_EDGE;
        ;

contact_triangles : BEG_TRIANGLE
        {
            if (dsDevsimParse::MeshContact->HasTriangles())
            {
                std::ostringstream os;
                os << "ERROR: Region " << dsDevsimParse::MeshContact->GetName() << " already has triangles\n";
                Devsimerror(os.str().c_str());
                YYABORT;
            }
        } |
        contact_triangles INT INT INT
        {
            int nodes[3] = {$2, $3, $4};
            for (size_t i = 0; i < 3; ++i)
            {
              if (nodes[i] < 0)
              {
                  std::ostringstream os;
                  os << "ERROR: Cannot have negative contact node index " << nodes[i] << "\n";
                  Devsimerror(os.str().c_str());
                  YYABORT;
              }
            }

            dsDevsimParse::MeshContact->AddTriangle(dsMesh::MeshTriangle(nodes[0], nodes[1], nodes[2]));
        } |
        contact_triangles END_TRIANGLE;
        ;

interface_nodes : BEG_NODE
        {
            if (dsDevsimParse::MeshInterface->HasNodesPairs())
            {
                std::ostringstream os;
                os << "ERROR: Interface " << dsDevsimParse::MeshInterface->GetName() << " already has nodes\n";
                Devsimerror(os.str().c_str());
                YYABORT;
            }
        } |
        interface_nodes INT INT
        {
            if ($2 < 0)
            {
                std::ostringstream os;
                os << "ERROR: Cannot have negative interface node index " << $2 << "\n";
                Devsimerror(os.str().c_str());
                YYABORT;
            }
            else if ($3 < 0)
            {
                std::ostringstream os;
                os << "ERROR: Cannot have negative interface node index " << $3 << "\n";
                Devsimerror(os.str().c_str());
                YYABORT;
            }
            else
            {
                dsDevsimParse::MeshInterface->AddNodePair(dsMesh::MeshInterfaceNodePair($2, $3));
            }
        } |
        interface_nodes END_NODE ;
        ;

interface_edges : BEG_EDGE
        {
            if (dsDevsimParse::MeshInterface->HasEdges())
            {
                std::ostringstream os;
                os << "ERROR: Interface " << dsDevsimParse::MeshInterface->GetName() << " already has edges\n";
                Devsimerror(os.str().c_str());
                YYABORT;
            }
        } |
        interface_edges INT INT INT INT
        {
            int nodes[4] = {$2, $3, $4, $5};
            for (size_t i = 0; i < 4; ++i)
            {
              if (nodes[i] < 0)
              {
                  std::ostringstream os;
                  os << "ERROR: Cannot have negative interface node index " << nodes[i] << "\n";
                  Devsimerror(os.str().c_str());
                  YYABORT;
              }
            }

            dsDevsimParse::MeshInterface->AddEdgePair(dsMesh::MeshEdge($2, $3), dsMesh::MeshEdge($4, $5));
        } |
        interface_edges END_EDGE ;
        ;

interface_triangles : BEG_TRIANGLE
        {
            if (dsDevsimParse::MeshInterface->HasTriangles())
            {
                std::ostringstream os;
                os << "ERROR: Interface " << dsDevsimParse::MeshInterface->GetName() << " already has triangles\n";
                Devsimerror(os.str().c_str());
                YYABORT;
            }
        } |
        interface_triangles INT INT INT INT INT INT
        {
            int nodes[6] = {$2, $3, $4, $5, $6, $7};
            for (size_t i = 0; i < 6; ++i)
            {
              if (nodes[i] < 0)
              {
                  std::ostringstream os;
                  os << "ERROR: Cannot have negative interface node index " << nodes[i] << "\n";
                  Devsimerror(os.str().c_str());
                  YYABORT;
              }
            }

            dsDevsimParse::MeshInterface->AddTrianglePair(dsMesh::MeshTriangle($2, $3, $4), dsMesh::MeshTriangle($5, $6, $7));
        } |
        interface_triangles END_TRIANGLE ;
        ;


edges : BEG_EDGE
        {
            if (dsDevsimParse::MeshRegion->HasEdges())
            {
                std::ostringstream os;
                os << "ERROR: Region " << dsDevsimParse::MeshRegion->GetName() << " already has edges\n";
                Devsimerror(os.str().c_str());
                YYABORT;
            }
        } |
        edges INT INT
        {
            if ($2 < 0)
            {
                std::ostringstream os;
                os << "ERROR: Cannot have negative edge index " << $2 << "\n";
                Devsimerror(os.str().c_str());
                YYABORT;
            }
            else if ($3 < 0)
            {
                std::ostringstream os;
                os << "ERROR: Cannot have negative edge index " << $3 << "\n";
                Devsimerror(os.str().c_str());
                YYABORT;
            }
            else
            {
//              std::ostringstream os;
//              os << "add edge with node index " << $2 << "\t" << $3 << "\n";
//              OutputStream::WriteOut(OutputStream::INFO, os.str());
                dsDevsimParse::MeshRegion->AddEdge(dsMesh::MeshEdge($2, $3));
            }
        } |
        edges END_EDGE ;
        ;

triangles : BEG_TRIANGLE
        {
            if (dsDevsimParse::MeshRegion->HasTriangles())
            {
                std::ostringstream os;
                os << "ERROR: Region " << dsDevsimParse::MeshRegion->GetName() << " already has triangles\n";
                Devsimerror(os.str().c_str());
                YYABORT;
            }
        } |
        triangles INT INT INT
        {
            if ($2 < 0)
            {
                std::ostringstream os;
                os << "ERROR: Cannot have negative triangle index " << $2 << "\n";
                Devsimerror(os.str().c_str());
                YYABORT;
            }
            else if ($3 < 0)
            {
                std::ostringstream os;
                os << "ERROR: Cannot have negative triangle index " << $3 << "\n";
                Devsimerror(os.str().c_str());
                YYABORT;
            }
            else if ($4 < 0)
            {
                std::ostringstream os;
                os << "ERROR: Cannot have negative triangle index " << $4 << "\n";
                Devsimerror(os.str().c_str());
                YYABORT;
            }
            else
            {
                dsDevsimParse::MeshRegion->AddTriangle(dsMesh::MeshTriangle($2, $3, $4));
            }
        } |
        triangles END_TRIANGLE ;
        ;

tetrahedra : BEG_TETRAHEDRON
        {
            if (dsDevsimParse::MeshRegion->HasTetrahedra())
            {
                std::ostringstream os;
                os << "ERROR: Region " << dsDevsimParse::MeshRegion->GetName() << " already has tetrahedra\n";
                Devsimerror(os.str().c_str());
                YYABORT;
            }
        } |
        tetrahedra INT INT INT INT
        {
            if ($2 < 0)
            {
                std::ostringstream os;
                os << "ERROR: Cannot have negative triangle index " << $2 << "\n";
                Devsimerror(os.str().c_str());
                YYABORT;
            }
            else if ($3 < 0)
            {
                std::ostringstream os;
                os << "ERROR: Cannot have negative triangle index " << $3 << "\n";
                Devsimerror(os.str().c_str());
                YYABORT;
            }
            else if ($4 < 0)
            {
                std::ostringstream os;
                os << "ERROR: Cannot have negative triangle index " << $4 << "\n";
                Devsimerror(os.str().c_str());
                YYABORT;
            }
            else if ($5 < 0)
            {
                std::ostringstream os;
                os << "ERROR: Cannot have negative triangle index " << $5 << "\n";
                Devsimerror(os.str().c_str());
                YYABORT;
            }
            else
            {
                dsDevsimParse::MeshRegion->AddTetrahedron(dsMesh::MeshTetrahedron($2, $3, $4, $5));
            }
        } |
        tetrahedra END_TETRAHEDRON ;
        ;

contact : BEG_CONTACT WORD WORD WORD  {
            if (dsDevsimParse::MeshContact)
            {
                std::ostringstream os;
                os << "ERROR: currently trying to load another contact " << dsDevsimParse::MeshContact->GetName() << "\n";
                Devsimerror(os.str().c_str());
                YYABORT;
            }
            else if (dsDevsimParse::DevsimLoader->IsMeshContact($2))
            {
                std::ostringstream os;
                os << "ERROR: contact already exists on device " << $2 << "\n";
                Devsimerror(os.str().c_str());
                YYABORT;
            }
            else
            {
                dsDevsimParse::MeshContact = new dsMesh::MeshContact($2, $3, $4);
            }
        } |
        contact END_CONTACT  {
            if (!dsDevsimParse::MeshContact)
            {
                std::ostringstream os;
                os << "ERROR: No contact to end " << dsDevsimParse::MeshContact->GetName() << "\n";
                Devsimerror(os.str().c_str());
                YYABORT;
            }
            else
            {
                dsDevsimParse::DevsimLoader->AddContact(dsDevsimParse::MeshContact);
                dsDevsimParse::MeshContact = nullptr;
            }
        } |
        contact contact_nodes |
        contact contact_edges |
        contact contact_triangles |
        contact contactequation
        ;

interface : BEG_INTERFACE WORD WORD WORD  {
            if (dsDevsimParse::MeshInterface)
            {
                std::ostringstream os;
                os << "ERROR: currently trying to load another interface " << dsDevsimParse::MeshInterface->GetName() << "\n";
                Devsimerror(os.str().c_str());
                YYABORT;
            }
            else if (dsDevsimParse::DevsimLoader->IsMeshInterface($2))
            {
                std::ostringstream os;
                os << "ERROR: interface already exists on device " << $2 << "\n";
                Devsimerror(os.str().c_str());
                YYABORT;
            }
            else
            {
                dsDevsimParse::MeshInterface = new dsMesh::MeshInterface($2, $3, $4);
            }
        } |
        interface END_INTERFACE  {
            if (!dsDevsimParse::MeshInterface)
            {
                std::ostringstream os;
                os << "ERROR: No interface to end " << dsDevsimParse::MeshContact->GetName() << "\n";
                Devsimerror(os.str().c_str());
                YYABORT;
            }
            else
            {
                dsDevsimParse::DevsimLoader->AddInterface(dsDevsimParse::MeshInterface);
                dsDevsimParse::MeshInterface = nullptr;
            }
        } |
        interface interface_nodes |
        interface interface_edges |
        interface interface_triangles |
        interface interfacenodemodel |
        interface interfaceequation
        ;


nodesol : BEG_NODESOL WORD  {
            if (dsDevsimParse::Sol)
            {
                std::ostringstream os;
                os << "ERROR: Solution is currently being loaded " << dsDevsimParse::Sol->GetName() << "\n";
                Devsimerror(os.str().c_str());
                YYABORT;
            }
            else if (dsDevsimParse::MeshRegion->IsSolution($2))
            {
                std::ostringstream os;
                os << "ERROR: Solution already loaded " << $2 << "\n";
                Devsimerror(os.str().c_str());
                YYABORT;
            }
            else
            {
                dsDevsimParse::Sol = new dsMesh::Solution($2);
                dsDevsimParse::Sol->SetModelType(dsMesh::Solution::ModelType::NODE);
                dsDevsimParse::Sol->SetReserve(dsDevsimParse::MeshRegion->GetNodes().size());
                dsDevsimParse::Sol->SetDataType(dsMesh::Solution::DataType::DATA);
            }
        } |
        nodesol number  {
            dsDevsimParse::Sol->AddValue($2);
        } |
        nodesol END_NODESOL  {
            dsDevsimParse::MeshRegion->AddSolution(dsDevsimParse::Sol);
            dsDevsimParse::Sol = nullptr;
        }
        ;

regionequation : BEG_REGIONEQUATION WORD  {
            if (dsDevsimParse::Equation)
            {
                std::ostringstream os;
                os << "ERROR: Equation is currently being loaded " << dsDevsimParse::Equation->GetName() << "\n";
                Devsimerror(os.str().c_str());
                YYABORT;
            }
            else if (dsDevsimParse::MeshRegion->IsEquation($2))
            {
                std::ostringstream os;
                os << "ERROR: Equation already loaded " << $2 << "\n";
                Devsimerror(os.str().c_str());
                YYABORT;
            }
            else
            {
              dsDevsimParse::Equation = new dsMesh::Equation($2);
            }
        } |
        regionequation equation_command |
        regionequation END_REGIONEQUATION  {
            dsDevsimParse::MeshRegion->AddEquation(dsDevsimParse::Equation);
            dsDevsimParse::Equation = nullptr;
        }
        ;

contactequation : BEG_CONTACTEQUATION WORD  {
            if (dsDevsimParse::Equation)
            {
                std::ostringstream os;
                os << "ERROR: Equation is currently being loaded " << dsDevsimParse::Equation->GetName() << "\n";
                Devsimerror(os.str().c_str());
                YYABORT;
            }
            else if (dsDevsimParse::MeshContact->IsEquation($2))
            {
                std::ostringstream os;
                os << "ERROR: Equation already loaded " << $2 << "\n";
                Devsimerror(os.str().c_str());
                YYABORT;
            }
            else
            {
              dsDevsimParse::Equation = new dsMesh::Equation($2);
            }
        } |
        contactequation equation_command |
        contactequation END_CONTACTEQUATION  {
            dsDevsimParse::MeshContact->AddEquation(dsDevsimParse::Equation);
            dsDevsimParse::Equation = nullptr;
        }
        ;

interfaceequation : BEG_INTERFACEEQUATION WORD  {
            if (dsDevsimParse::Equation)
            {
                std::ostringstream os;
                os << "ERROR: Equation is currently being loaded " << dsDevsimParse::Equation->GetName() << "\n";
                Devsimerror(os.str().c_str());
                YYABORT;
            }
            else if (dsDevsimParse::MeshInterface->IsEquation($2))
            {
                std::ostringstream os;
                os << "ERROR: Equation already loaded " << $2 << "\n";
                Devsimerror(os.str().c_str());
                YYABORT;
            }
            else
            {
              dsDevsimParse::Equation = new dsMesh::Equation($2);
            }
        } |
        interfaceequation equation_command |
        interfaceequation END_INTERFACEEQUATION  {
            dsDevsimParse::MeshInterface->AddEquation(dsDevsimParse::Equation);
            dsDevsimParse::Equation = nullptr;
        }
        ;

nodemodel : BEG_NODEMODEL WORD  {
            if (dsDevsimParse::Sol)
            {
                std::ostringstream os;
                os << "ERROR: Solution is currently being loaded " << dsDevsimParse::Sol->GetName() << "\n";
                Devsimerror(os.str().c_str());
                YYABORT;
            }
            else if (dsDevsimParse::MeshRegion->IsSolution($2))
            {
                std::ostringstream os;
                os << "ERROR: Solution already loaded " << $2 << "\n";
                Devsimerror(os.str().c_str());
                YYABORT;
            }
            else
            {
                dsDevsimParse::Sol = new dsMesh::Solution($2);
                dsDevsimParse::Sol->SetModelType(dsMesh::Solution::ModelType::NODE);
                dsDevsimParse::Sol->SetReserve(dsDevsimParse::MeshRegion->GetNodes().size());
            }
        } |
        nodemodel datasection |
        nodemodel builtin |
        nodemodel dataparent |
        nodemodel uniform |
        nodemodel command |
        nodemodel END_NODEMODEL  {
            dsDevsimParse::MeshRegion->AddSolution(dsDevsimParse::Sol);
            dsDevsimParse::Sol = nullptr;
        }
        ;

edgemodel : BEG_EDGEMODEL WORD  {
            if (dsDevsimParse::Sol)
            {
                std::ostringstream os;
                os << "ERROR: Solution is currently being loaded " << dsDevsimParse::Sol->GetName() << "\n";
                Devsimerror(os.str().c_str());
                YYABORT;
            }
            else if (dsDevsimParse::MeshRegion->IsSolution($2))
            {
                std::ostringstream os;
                os << "ERROR: Solution already loaded " << $2 << "\n";
                Devsimerror(os.str().c_str());
                YYABORT;
            }
            else
            {
                dsDevsimParse::Sol = new dsMesh::Solution($2);
                dsDevsimParse::Sol->SetModelType(dsMesh::Solution::ModelType::EDGE);
                dsDevsimParse::Sol->SetReserve(dsDevsimParse::MeshRegion->GetEdges().size());
            }
        } |
        edgemodel datasection |
        edgemodel builtin |
        edgemodel dataparent |
        edgemodel uniform |
        edgemodel command |
        edgemodel END_EDGEMODEL  {
            dsDevsimParse::MeshRegion->AddSolution(dsDevsimParse::Sol);
            dsDevsimParse::Sol = nullptr;
        }
        ;

triangleedgemodel : BEG_TRIANGLEEDGEMODEL WORD  {
            if (dsDevsimParse::Sol)
            {
                std::ostringstream os;
                os << "ERROR: Solution is currently being loaded " << dsDevsimParse::Sol->GetName() << "\n";
                Devsimerror(os.str().c_str());
                YYABORT;
            }
            else if (dsDevsimParse::MeshRegion->IsSolution($2))
            {
                std::ostringstream os;
                os << "ERROR: Solution already loaded " << $2 << "\n";
                Devsimerror(os.str().c_str());
                YYABORT;
            }
            else
            {
                dsDevsimParse::Sol = new dsMesh::Solution($2);
                dsDevsimParse::Sol->SetModelType(dsMesh::Solution::ModelType::TRIANGLEEDGE);
                dsDevsimParse::Sol->SetReserve(3*dsDevsimParse::MeshRegion->GetTriangles().size());
            }
        } |
        triangleedgemodel datasection |
        triangleedgemodel builtin |
        triangleedgemodel dataparent |
        triangleedgemodel uniform |
        triangleedgemodel command |
        triangleedgemodel END_TRIANGLEEDGEMODEL  {
            dsDevsimParse::MeshRegion->AddSolution(dsDevsimParse::Sol);
            dsDevsimParse::Sol = nullptr;
        }
        ;

tetrahedronedgemodel : BEG_TETRAHEDRONEDGEMODEL WORD  {
            if (dsDevsimParse::Sol)
            {
                std::ostringstream os;
                os << "ERROR: Solution is currently being loaded " << dsDevsimParse::Sol->GetName() << "\n";
                Devsimerror(os.str().c_str());
                YYABORT;
            }
            else if (dsDevsimParse::MeshRegion->IsSolution($2))
            {
                std::ostringstream os;
                os << "ERROR: Solution already loaded " << $2 << "\n";
                Devsimerror(os.str().c_str());
                YYABORT;
            }
            else
            {
                dsDevsimParse::Sol = new dsMesh::Solution($2);
                dsDevsimParse::Sol->SetModelType(dsMesh::Solution::ModelType::TETRAHEDRONEDGE);
                dsDevsimParse::Sol->SetReserve(6*dsDevsimParse::MeshRegion->GetTetrahedra().size());
            }
        } |
        tetrahedronedgemodel datasection |
        tetrahedronedgemodel builtin |
        tetrahedronedgemodel dataparent |
        tetrahedronedgemodel uniform |
        tetrahedronedgemodel command |
        tetrahedronedgemodel END_TETRAHEDRONEDGEMODEL  {
            dsDevsimParse::MeshRegion->AddSolution(dsDevsimParse::Sol);
            dsDevsimParse::Sol = nullptr;
        }
        ;

interfacenodemodel : BEG_INTERFACENODEMODEL WORD  {
            if (dsDevsimParse::Sol)
            {
                std::ostringstream os;
                os << "ERROR: Solution is currently being loaded " << dsDevsimParse::Sol->GetName() << "\n";
                Devsimerror(os.str().c_str());
                YYABORT;
            }
            else if (dsDevsimParse::MeshInterface->IsSolution($2))
            {
                std::ostringstream os;
                os << "ERROR: Solution already loaded " << $2 << "\n";
                Devsimerror(os.str().c_str());
                YYABORT;
            }
            else
            {
                dsDevsimParse::Sol = new dsMesh::Solution($2);
                dsDevsimParse::Sol->SetModelType(dsMesh::Solution::ModelType::INTERFACENODE);
            }
        } |
        interfacenodemodel command |
        interfacenodemodel END_INTERFACENODEMODEL  {
            dsDevsimParse::MeshInterface->AddSolution(dsDevsimParse::Sol);
            dsDevsimParse::Sol = nullptr;
        }
        ;


datasection : DATASECTION {
          //// Make sure reserve is set before DATA
          dsDevsimParse::Sol->SetDataType(dsMesh::Solution::DataType::DATA);
        } |
        datasection number {
          dsDevsimParse::Sol->AddValue($2);
        }
        ;

builtin : BUILTINSECTION {
          dsDevsimParse::Sol->SetDataType(dsMesh::Solution::DataType::BUILTIN);
        }

dataparent : DATAPARENTSECTION WORD {
          dsDevsimParse::Sol->SetDataType(dsMesh::Solution::DataType::DATAPARENT);
          dsDevsimParse::Sol->SetParent($2);
        }

uniform : UNIFORMSECTION number {
          dsDevsimParse::Sol->SetDataType(dsMesh::Solution::DataType::UNIFORM);
          dsDevsimParse::Sol->SetUniformValue($2);
        }

command : command_recursive |
          command COMMAND_EOL
          ;

command_recursive:
          COMMANDSECTION WORD {
            dsDevsimParse::Sol->SetDataType(dsMesh::Solution::DataType::COMMAND);
            dsDevsimParse::Sol->SetCommandName($2);
          } |
          command_recursive COMMAND_OPTION FLOAT {
            dsDevsimParse::Sol->AddCommandOption($2, ObjectHolder($3));
          } |
          command_recursive COMMAND_OPTION INT {
            dsDevsimParse::Sol->AddCommandOption($2, ObjectHolder($3));
          } |
          command_recursive COMMAND_OPTION WORD {
            dsDevsimParse::Sol->AddCommandOption($2, ObjectHolder($3));
          };

equation_command : equation_command_recursive |
          equation_command COMMAND_EOL
          ;


equation_command_recursive :
        COMMANDSECTION WORD {
          dsDevsimParse::Equation->SetCommandName($2);
          } |
          equation_command_recursive COMMAND_OPTION FLOAT {
            dsDevsimParse::Equation->AddCommandOption($2, ObjectHolder($3));
          } |
          equation_command_recursive COMMAND_OPTION INT {
            dsDevsimParse::Equation->AddCommandOption($2, ObjectHolder($3));
          } |
          equation_command_recursive COMMAND_OPTION WORD {
            dsDevsimParse::Equation->AddCommandOption($2, ObjectHolder($3));
          };



edgesol : BEG_EDGESOL WORD  {
            if (dsDevsimParse::Sol)
            {
                std::ostringstream os;
                os << "ERROR: Solution is currently being loaded " << dsDevsimParse::Sol->GetName() << "\n";
                Devsimerror(os.str().c_str());
            }
            else if (dsDevsimParse::MeshRegion->IsSolution($2))
            {
                std::ostringstream os;
                os << "ERROR: Solution already loaded " << $2 << "\n";
                Devsimerror(os.str().c_str());
            }
            else
            {
                dsDevsimParse::Sol = new dsMesh::Solution($2);
                dsDevsimParse::Sol->SetModelType(dsMesh::Solution::ModelType::EDGE);
                dsDevsimParse::Sol->SetReserve(dsDevsimParse::MeshRegion->GetEdges().size());
                dsDevsimParse::Sol->SetDataType(dsMesh::Solution::DataType::DATA);
            }
        } |
        edgesol number  {
            dsDevsimParse::Sol->AddValue($2);
        } |
        edgesol END_EDGESOL  {
            dsDevsimParse::MeshRegion->AddSolution(dsDevsimParse::Sol);
            dsDevsimParse::Sol = nullptr;
        }
        ;

%%


int Devsimerror(const char *msg) {
    std::ostringstream os;
    os <<  "line: " << dsDevsimParse::meshlineno << ": " << msg << "\n";
    dsDevsimParse::errors += os.str();
    return 0;
}

