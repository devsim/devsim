%{
/***
DEVSIM
Copyright 2016 DEVSIM LLC

SPDX-License-Identifier: Apache-2.0
***/

#include "Gmshyystype.hh"
#include "GmshReader.hh"
#include "GmshLoader.hh"
#include "MeshKeeper.hh"
#include "MeshLoaderUtility.hh"
#include "OutputStream.hh"
#include <sstream>
#include <vector>

int Gmsherror(const char *msg);

int Gmshlex();

bool processElement(dsMesh::GmshLoader &, const std::vector<int> &);
%}

//%token HELP
%token <dval> FLOAT
%token <ival> INT
%token <str>  WORD MESHFORMAT
%token EOL
%token        BEG_MESHFORMAT    END_MESHFORMAT
%token        BEG_PHYSICALNAMES END_PHYSICALNAMES
%token        BEG_NODES         END_NODES
%token        BEG_ELEMENTS      END_ELEMENTS

%type <dval> number
%type <ilist> intlist

%%
all : | meshformat
    ;

meshformat :
        BEG_MESHFORMAT EOL MESHFORMAT EOL END_MESHFORMAT EOL
        {
          if (
            ($3 != "2.2 0 8") &&
            ($3 != "2.1 0 8")
          )
          {
            std::ostringstream os;
            os << "ERROR: MeshFormat " << $3 << " not supported\n";
            Gmsherror(os.str().c_str());
            YYABORT;
          }
        }
        | meshformat physicalnames
        | meshformat nodes
        | meshformat elements
        | meshformat EOL
	;

physicalnames :
        BEG_PHYSICALNAMES EOL INT EOL
        | physicalnames INT INT WORD EOL
        {
          if (($2 < 0) || ($2 > 3))
          {
            std::ostringstream os;
            os << "ERROR: PhysicalName mapping " << $3 << " to " << $4 << " cannot have dimension " << $2 << "\n";
            Gmsherror(os.str().c_str());
            YYABORT;
          }
          else if ($3 < 1)
          {
            std::ostringstream os;
            os << "ERROR: PhysicalName mapping " << $3 << " to " << $4 << " dimension " << $2 << " must refer to a positive index\n";
            Gmsherror(os.str().c_str());
            YYABORT;
          }
          else if (dsGmshParse::GmshLoader->HasPhysicalName($2, $3))
          {
            std::ostringstream os;
            os << "ERROR: PhysicalName mapping" << $3 << " to " << $4 << " dimension " << $2 << " cannot be used since " << $3 << " already maps to " << dsGmshParse::GmshLoader->GetPhysicalName($2, $3)<< "\n";
            Gmsherror(os.str().c_str());
            YYABORT;
          }
          else
          {
            dsGmshParse::GmshLoader->AddPhysicalName($2, $3, $4);
          }
        }
        | physicalnames END_PHYSICALNAMES EOL
        {
          /// Verify we get the expected number
        }
        ;

nodes :
        BEG_NODES EOL INT EOL
        | nodes INT number number number EOL
        {
          /// enforce that node index is a positive number
          if ($2 < 1)
          {
            std::ostringstream os;
            os << "ERROR: node index " << $2 << " must refer to a positive index\n";
            Gmsherror(os.str().c_str());
            YYABORT;
          }
          else
          {
            dsGmshParse::GmshLoader->AddCoordinate($2, dsMesh::MeshCoordinate($3, $4, $5));
          }
        }
        | nodes END_NODES
        {
          /// Verify we get the expected number
        }
        ;

elements :
        BEG_ELEMENTS EOL INT EOL
        | elements intlist EOL
        {
          /// here we are processing a list of numbers since the number we expect are based on element type and tag properties
          bool ok = processElement(*dsGmshParse::GmshLoader, $2);
          if (!ok)
          {
            YYABORT;
          }
        }
        | elements END_ELEMENTS
        {
          /// Verify we get the expected number
        }
        ;

intlist :
        INT
        {
          $$.clear();
          $$.push_back($1);
        }
        | intlist INT
        {
          $$.push_back($2);
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

%%


int Gmsherror(const char *msg) {
    std::ostringstream os;
    os <<  "line: " << dsGmshParse::meshlineno << ": " << msg << "\n";
    dsGmshParse::errors += os.str();
    return 0;
}

bool processElement(dsMesh::GmshLoader &gmsh, const std::vector<int> &ilist)
{

  const size_t ilist_size = ilist.size();

  if (ilist_size < 3)
  {
    std::ostringstream os;
    os << "ERROR: Element line only has " << ilist.size() << "entries.\n";
    Gmsherror(os.str().c_str());
    return false;
  }

  const int element_index = ilist[0];

  if (element_index < 1)
  {
    std::ostringstream os;
    os << "ERROR: element index " << element_index << " must be a positive index.\n";
    Gmsherror(os.str().c_str());
    return false;
  }

  const int element_number = ilist[1];

  dsMesh::Shapes::ElementType_t element_enum = dsMesh::Shapes::ElementType_t::UNKNOWN;

/*
  const char *TypeNames[] =
  {
    "unknown",
    "point",
    "line",
    "triangle",
    "tetrahedron"
  };
*/

  switch (element_number)
  {
    case 15:
      element_enum = dsMesh::Shapes::ElementType_t::POINT;
      break;
    case 1:
      element_enum = dsMesh::Shapes::ElementType_t::LINE;
      break;
    case 2:
      element_enum = dsMesh::Shapes::ElementType_t::TRIANGLE;
      break;
    case 4:
      element_enum = dsMesh::Shapes::ElementType_t::TETRAHEDRON;
      break;
    default:
      break;
  };

  if (element_enum == dsMesh::Shapes::ElementType_t::UNKNOWN)
  {
    std::ostringstream os;
    os << "ERROR: Unable to process element of type " << element_number << "\n";
    Gmsherror(os.str().c_str());
    return false;
  }

  const int number_tags = ilist[2];
  if (number_tags < 0)
  {
    std::ostringstream os;
    os << "ERROR: Number of tags for element " << number_tags << " cannot be less than 0\n";
    Gmsherror(os.str().c_str());
    return false;
  }

  size_t physical_number = size_t(-1);

  if (number_tags > 0)
  {
    if (ilist_size > 4)
    {
      if (ilist[3] > 0)
      {
        physical_number = ilist[3];
      }
      else
      {
        std::ostringstream os;
        os << "ERROR: physical number " << ilist[3] << " must be positive\n";
        Gmsherror(os.str().c_str());
        return false;
      }
    }
    else
    {
      std::ostringstream os;
      os << "ERROR: could not process number of element tags correctly\n";
      Gmsherror(os.str().c_str());
      return false;
    }
  }

  if (physical_number == size_t(-1))
  {
      std::ostringstream os;
      os << "ERROR: physical number must be positive specified\n";
      Gmsherror(os.str().c_str());
      return false;
  }

  const size_t element_node_begin = 3 + number_tags;

  std::vector<int> indexes;
  indexes.reserve(4);
  bool hasNonPositive = false;
  for (size_t i = element_node_begin; i < ilist_size; ++i)
  {
    const int index = ilist[i];
    if (index < 1)
    {
      hasNonPositive = true;
      break;
    }
    indexes.push_back(ilist[i]);
  }

  if (hasNonPositive)
  {
    std::ostringstream os;
    os << "ERROR: element has non-positive index for nodes\n";
    Gmsherror(os.str().c_str());
    return false;
  }

  const size_t indexes_size = indexes.size();
  if (
      ((element_enum == dsMesh::Shapes::ElementType_t::POINT) && (indexes_size == 1)) ||
      ((element_enum == dsMesh::Shapes::ElementType_t::LINE) && (indexes_size == 2)) ||
      ((element_enum == dsMesh::Shapes::ElementType_t::TRIANGLE) && (indexes_size == 3)) ||
      ((element_enum == dsMesh::Shapes::ElementType_t::TETRAHEDRON) && (indexes_size == 4))
     )
  {
    gmsh.AddElement(dsMesh::GmshElement(element_index, physical_number, element_enum, indexes));
    return true;
  }
  else
  {
    std::ostringstream os;
    os << "ERROR: element has wrong number of nodes\n";
    Gmsherror(os.str().c_str());
    return false;
  }
}


