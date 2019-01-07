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

#include "GeniusReader.hh"
#include "GeniusLoader.hh"
#include "MeshKeeper.hh"
#include "OutputStream.hh"
#include "cgnslib.h"
#include <sstream>
namespace dsGeniusParse {
//dsMesh::GeniusLoaderPtr    GeniusLoader = nullptr;
std::string errors;
bool fatal_cgns_error = false;
bool print_cgns_messages = true;

void GeniusErrorHandler(int level,  char *msg)
{
  std::ostringstream os;
  switch (level) {
    case -1:
      os << "CGNS FATAL: ";
      fatal_cgns_error = true;
      break;
    case 0:
      os << "CGNS WARNING: ";
      break;
    case 1:
      os << "CGNS ERROR: ";
      break;
  }
  if (fatal_cgns_error)
  {
    os << msg << "\n";
    OutputStream::WriteOut(OutputStream::OutputType::INFO, os.str().c_str());
    errors += os.str();
  }
  else if (print_cgns_messages)
  {
    os << msg << "\n";
    errors += os.str();
  }
}

/*
  make sure you are located at the desired place already
*/
bool has_descriptor(const std::string &str)
{
  bool ret = false; 
  int num_descriptors;
  cg_ndescriptors(&num_descriptors);
  char name[33];
  char *text;
  for (int zi = 1; zi <= num_descriptors; ++zi)
  {
    cg_descriptor_read(zi, name, &text);
    if (str == text)
    {
      ret = true;
      cg_free(text);
      break;
    }
    cg_free(text);
  }
  return ret;
}


bool ReadGeniusFile(const std::string &fname, const std::string &meshName, std::string &errorString, dsMesh::GeniusLoaderPtr GeniusLoader, std::vector<dsMesh::GeniusRegionPtr> &regions)
{
  bool ret = true;
  errors.clear();
  fatal_cgns_error = false;
  print_cgns_messages = true;
  std::string base_name;
  std::string base_path;
  std::string zone_path;
  std::string temp_path;
  std::string tstring;
  DataType_t dtype;
  int        dim;
  int        vlen;
  int ndescriptors;


  cgsize_t one = 1;


  // Set Error Handler
  cg_error_handler(GeniusErrorHandler);

  int fn;
  cg_open(fname.c_str(), CG_MODE_READ, &fn);
  if (!fn)
  {
    /* assume for now that the error string is populated by cgns */
    goto error_exit;
  }

  int B;
  cg_nbases(fn, &B);

  if (B != 1)
  {
    std::ostringstream os;
    os << "A Genius mesh can only have 1 base and " << B << " reported\n";
    errors += os.str();
    ret = false;
    goto error_exit;
  }

  char char_buf[33];
  char *cgns_buf;

  int cell_dim, phys_dim;
  cg_base_read(fn, B, char_buf, &cell_dim, &phys_dim);

  if (phys_dim != 3)
  {
    std::ostringstream os;
    os << "A Genius mesh must have a physical dimension of 3\n";
    errors += os.str();
    ret = false;
    goto error_exit;
  }

  base_name = char_buf;
  if ((base_name.find("GENIUS_Mesh_") == 0) && (base_name.size() > 12))
  {
    /// magic numbers less then 2008 for 2d
    GeniusLoader->SetMagicNumber(base_name.substr(12));
    base_path = std::string("/") + base_name + "/"; 
  }
  else
  {
    std::ostringstream os;
    os << "Genius base name " << base_name << "not a properly formatted\n";
    errors += os.str();
    ret = false;
    goto error_exit;
  }

  cg_goto(fn, B, "end");
  if (has_descriptor("ResistiveMetal"))
  {
    GeniusLoader->SetResistive(true);
  }

  int nzones;
  cg_nzones(fn, B, &nzones);

  if (nzones < 1)
  {
    std::ostringstream os;
    os << "Genius mesh must have at least one zone\n";
    errors += os.str();
    ret = false;
    goto error_exit;
  }


  {
    ZoneType_t ztype;
    int zinfo[3];
    int nd;
    for (int zi = 1; zi <= nzones; ++zi)
    {
      dsMesh::GeniusRegionPtr rinfop(new dsMesh::GeniusRegion);
      regions.push_back(rinfop);
      dsMesh::GeniusRegion &rinfo = *rinfop;
      cg_zone_type(fn, B, zi, &ztype);
      if (ztype != Unstructured)
      {
        std::ostringstream os;
        os << "Genius zones must all be unstructured\n";
        errors += os.str();
        ret = false;
        goto error_exit;
      }

      // Region Name
      cg_zone_read(fn, B, zi, char_buf, zinfo);
      rinfo.region = char_buf;
      zone_path = base_path + rinfo.region + "/" ;

      // Material Name
      cg_goto(fn , B, "Zone_t", zi, "end");
      cg_ndescriptors(&nd);
      //// assuming only one descriptor per zone
      if (nd)
      {
        cg_descriptor_read(nd, char_buf, &cgns_buf);
        rinfo.material = cgns_buf;
        cg_free(cgns_buf);
      }

      const int ncoord = zinfo[0];
      rinfo.vec_x.resize(ncoord);
      rinfo.vec_y.resize(ncoord);
      rinfo.vec_z.resize(ncoord);
      rinfo.global_node_index.resize(ncoord);

      // if something goes wrong, hopefully feeds into error hook
      // TODO: find out proper dimensions
      cg_coord_read(fn, B, zi, "CoordinateX", RealDouble, &one, &ncoord, &rinfo.vec_x[0]);
      cg_coord_read(fn, B, zi, "CoordinateY", RealDouble, &one, &ncoord, &rinfo.vec_y[0]);
      cg_coord_read(fn, B, zi, "CoordinateZ", RealDouble, &one, &ncoord, &rinfo.vec_z[0]);

      // global node index
      temp_path = zone_path + "Global_Node_Index";
      {
        cg_gopath(fn, temp_path.c_str());
        int narrays;
        cg_narrays(&narrays);
        if (narrays == 1)
        {
          cg_array_info(narrays, char_buf, &dtype, &dim, &vlen);
          if ((dtype == Integer) && (vlen == ncoord))
          {
            rinfo.global_node_index.resize(vlen);
            cg_array_read_as(narrays, dtype, &rinfo.global_node_index[0]);
          }
        }
      }

      /// now get the elements
      int nsections;
      cg_nsections(fn, B, zi, &nsections);
      if (nsections == 1)
      //read section information
      {
        ElementType_t  type;
        int            nbndry;
        int            parent_flag;
        cg_section_read(fn, B, zi, nsections, char_buf, &type, &one, &rinfo.nelem, &nbndry, &parent_flag);
        // GENIUS wants type to be MIXED);
      }

      {
        cgsize_t edata_len;
        cg_ElementDataSize(fn, B, zi, nsections, &edata_len);

        rinfo.edata.resize(edata_len);
        cgsize_t parentdata;
        cg_elements_read(fn, B, zi, nsections, &rinfo.edata[0], &parentdata);
      }

      //read all the solutions
      std::string solution_name;
      std::string field_name;
      int nsols;
      int nfields;
      GridLocation_t location;
      cg_nsols(fn, B, zi, &nsols);
      for (int si=1; si <= nsols; ++si)
      {
        cg_nfields(fn, B, zi, si, &nfields);
        cg_sol_info(fn, B, zi, si, char_buf, &location);
        /// TODO: check location for Vertex
        solution_name = char_buf;
        for (int fi=1; fi <= nfields; ++fi)
        {
          cg_field_info(fn, B, zi, si, fi, &dtype, char_buf);
          field_name = char_buf;
          if (dtype == RealDouble)
          {
            std::vector<double> &data_vec = rinfo.solutions[field_name];
            data_vec.resize(ncoord);
            cg_field_read(fn, B, zi, si, field_name.c_str(), dtype, &one, &ncoord, &data_vec[0]);
          }
          else
          {
             //// warn if field_name is not of type double and not loaded
            std::ostringstream os;
            os << "Field skipped since not of type double " << field_name << "\n";
            errors += os.str();
          }
          temp_path = zone_path;
          temp_path += solution_name;
          if (!cg_gopath(fn, temp_path.c_str()))
          {
            cg_ndescriptors(&ndescriptors);
            for (int di = 1; di <= ndescriptors; ++di)
            {
              cg_descriptor_read(di, char_buf, &cgns_buf);
              rinfo.units[char_buf] = cgns_buf;
              cg_free(cgns_buf);
            }
          }
        }
      }

      // read boundaries
      {
        BCType_t bocotype;
        PointSetType_t ptset_type;
        cgsize_t npnts;
        int NormalIndex;
        cgsize_t NormalListSize;
        DataType_t NormalDataType;
        int ndataset;

        int nbocos;
        cg_nbocos(fn, B, zi, &nbocos);
        rinfo.bocos.resize(nbocos);
        for (int bi = 1; bi <= nbocos; ++bi)
        {
          dsMesh::GeniusBoundaryPtr bp(new dsMesh::GeniusBoundary);
          rinfo.bocos[bi-1] = bp;
          dsMesh::GeniusBoundary &boco = *bp;
          cg_boco_info(fn, B, zi, bi, char_buf, &bocotype, &ptset_type, &npnts, &NormalIndex, &NormalListSize,
                &NormalDataType, &ndataset);
          boco.name = char_buf;

          /// get the points
          if (ptset_type == PointList)
          {
            boco.points.resize(npnts);
            cg_boco_read(fn, B, zi, bi, &boco.points[0], nullptr);
          }

          cg_goto(fn, B, "Zone_t", zi, "ZoneBC_t", 1, "BC_t", bi, "end");
          cg_ndescriptors(&ndescriptors);

          for (int di=1; di <= ndescriptors; ++di)
          {
            cg_descriptor_read(di, char_buf, &cgns_buf);
            tstring = char_buf;
            if(tstring == "bc_label")
            {
              boco.label = cgns_buf;
            }
            else if(tstring == "bc_settings")
            {
              boco.settings = cgns_buf;
            }
            cg_free(cgns_buf);
          }

          cg_goto(fn, B, "Zone_t", zi, "ZoneBC_t", 1, "BC_t", bi, "end");
          print_cgns_messages = false;
          if (!cg_gopath(fn, "extra_data_for_electrode"))
          {
            print_cgns_messages = true;
            boco.has_electrode = true;
            int narrays;
            cg_narrays(&narrays);
            for (int ai =1; ai <= narrays; ++ai)
            {
              cg_array_info(ai, char_buf, &dtype, &dim, &vlen);
              if ((dtype == RealDouble) && (vlen == 1))
              {
                tstring = char_buf;
                if (tstring == "electrode_potential")
                {
                  cg_array_read_as(ai, dtype, &boco.electrode_potential);
                }
                else if (tstring == "electrode_potential_old")
                {
                  cg_array_read_as(ai, dtype, &boco.electrode_potential_old);
                }
                else if (tstring == "electrode_vapp")
                {
                  cg_array_read_as(ai, dtype, &boco.electrode_vapp);
                }
                else if (tstring == "electrode_iapp" )
                {
                  cg_array_read_as(ai, dtype, &boco.electrode_iapp);
                }
              }
            }
          }
        }
      }
      GeniusLoader->AddRegion(rinfop);
    }
  }

//TODO: skipping ExtraBoundaryInfo
//TODO: skipping element_side_information
error_exit:

  if (fn)
  {
    cg_close(fn);
  }

  errorString += dsGeniusParse::errors;
  if (fatal_cgns_error)
  {
    ret = false;
  }

  return ret;
}


int processRegionElements(dsMesh::GeniusRegionPtr grp, std::string &errorString)
{
  int ret = 0;
  dsMesh::GeniusRegion &region = *grp;
  const std::vector<int> &edata = region.edata;
  const size_t edatalen = edata.size();
  const int nelem = region.nelem;
  size_t elem_count = 0;
  size_t eindex = 0;
  do
  {
    if ((edata[eindex] == TRI_3) && ((eindex + 4) <= edatalen))
    {
      region.AddShape(dsMesh::Shapes::ElementType_t::TRIANGLE, &edata[eindex + 1]);
      eindex += 4;
      elem_count += 1;
    }
    else if ((edata[eindex] == TETRA_4) && ((eindex + 5) <= edatalen))
    {
      region.AddShape(dsMesh::Shapes::ElementType_t::TETRAHEDRON, &edata[eindex + 1]);
      eindex += 5;
      elem_count += 1;
    }
    else
    {
      std::ostringstream os;
      os << "Region " << region.region << " can only have triangle or tetrahedron elements in DEVSIM.\n";
      errorString += os.str();
      ret += 1;
      break;
    }
  } while (eindex < edatalen);

  if (ret == 0)
  {
    if (elem_count != nelem)
    {
      std::ostringstream os;
      os << "Region " << region.region << " has " << elem_count << " elements and " << nelem << " were expected.\n";
      errorString += os.str();
      ret += 1;
    }
    if (region.GetNumberOfTypes() != 1)
    {
      std::ostringstream os;
      os << "Region " << region.region << " has elements of mixed dimensions.\n";
      errorString += os.str();
      ret += 1;
    }
  }
  
  return ret;
}

bool LoadMeshes(const std::string &fname, const std::string &meshName, std::string &errorString)
{
  dsMesh::GeniusLoaderPtr GeniusLoader = new dsMesh::GeniusLoader(meshName);
  dsMesh::MeshKeeper &mk = dsMesh::MeshKeeper::GetInstance();
  mk.AddMesh(GeniusLoader);

  std::vector<dsMesh::GeniusRegionPtr> regions;
  bool ret = ReadGeniusFile(fname, meshName, errorString, GeniusLoader, regions);


  if (ret)
  {
    int nerrors = 0;
    for (size_t i = 0; i < regions.size(); ++i)
    {
      nerrors += processRegionElements(regions[i], errorString);
    }
    ret = (nerrors == 0);

    int dimension = 0;
    if (ret)
    {
      if (regions.size() > 0)
      {
        dimension = regions[0]->GetDimension();
        for (size_t i = 1; i < regions.size(); ++i)
        {
          if (regions[i]->GetDimension() != dimension)
          {
            std::ostringstream os;
            os << "All regions must have same dimension\n";
            errorString += os.str();
            ret = false;
          }
        }
        GeniusLoader->SetDimension(dimension);
      }
      else
      {
        std::ostringstream os;
        os << "No regions exist in mesh\n";
        errorString += os.str();
        ret = false;
      }
    }
  }
  return ret;
}
}

