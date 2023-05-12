/***
DEVSIM
Copyright 2013 DEVSIM LLC

SPDX-License-Identifier: Apache-2.0
***/

#include "VTKWriter.hh"
#include "GlobalData.hh"
#include "Device.hh"
#include "Region.hh"
#include "Coordinate.hh"
#include "Vector.hh"
#include "Node.hh"
#include "Edge.hh"
#include "Triangle.hh"
#include "Tetrahedron.hh"
#include "NodeModel.hh"
#include "EdgeModel.hh"
#include "TriangleEdgeModel.hh"
#include "TetrahedronEdgeModel.hh"
#include "MeshUtil.hh"
#include "dsAssert.hh"
#include "base64.hh"
#include <sstream>
#include <fstream>
#include <iomanip>
#include <set>
#include <vector>
#include <string>

namespace VTK {

void WriteHeader(std::ostream &myfile)
{
  myfile <<
  "<?xml version=\"1.0\"?>\n"
  "<VTKFile type=\"UnstructuredGrid\" version=\"0.1\" byte_order=\"LittleEndian\" compressor=\"vtkZLibDataCompressor\">\n"
  "<UnstructuredGrid>\n"
  ;
}

void WriteFooter(std::ostream &myfile)
{
  myfile <<
  "</UnstructuredGrid>\n"
  "</VTKFile>\n"
  ;
}

void WriteDataArray(const std::vector<double> &nsl, const std::string &name, const size_t number_components, std::ostream &myfile)
{
  myfile << "<DataArray type=\"Float64\"";

  if (!name.empty())
  {
    myfile << " Name=\"" << name << "\"";
  }
  if (number_components != 1)
  {
    myfile << " NumberOfComponents=\"" << number_components << "\"";
  }

  myfile << " format=\"binary\">\n"
         << dsUtility::convertVectorToZlibBase64(nsl)
         << "\n</DataArray>\n";
}

void WritePoints(const Region &reg, std::ostream &myfile)
{
  myfile << "<Points>\n";

  const ConstNodeList &cnl = reg.GetNodeList();
  std::vector<double> points;
  points.reserve(3*cnl.size());
  for (ConstNodeList::const_iterator it = cnl.begin(); it != cnl.end(); ++it)
  {
    const Vector<double> &pos = (*it)->Position();
    points.push_back(pos.Getx());
    points.push_back(pos.Gety());
    points.push_back(pos.Getz());
  }

  WriteDataArray(points, std::string(), 3, myfile);

#if 0
  for (ConstNodeList::const_iterator it = cnl.begin(); it != cnl.end(); ++it)
  {
    const Vector<double> &pos = (*it)->Position();
    myfile
      << " " << pos.Getx()
      << " " << pos.Gety()
      << " " << pos.Getz()
      ;
  }
#endif

  myfile << "</Points>\n";
}

void WritePointData(const Region &reg, std::ostream &myfile)
{

  const Region::NodeModelList_t            &node_models             = reg.GetNodeModelList();
  const Region::EdgeModelList_t            &edge_models             = reg.GetEdgeModelList();
#if 0
  const Region::TriangleEdgeModelList_t    &triangle_edge_models    = reg.GetTriangleEdgeModelList();
  const Region::TetrahedronEdgeModelList_t &tetrahedron_edge_models = reg.GetTetrahedronEdgeModelList();
#endif

//  if (node_models.empty() && edge_models.empty() && triangle_edge_models.empty() && tetrahedron_edge_models.empty())
  if (node_models.empty() && edge_models.empty())
  {
    return;
  }

  myfile << "<PointData>\n";

  if (!node_models.empty())
  {
    for (Region::NodeModelList_t::const_iterator it=node_models.begin(); it != node_models.end(); ++it)
    {
      const std::string &nm = it->first;
      const NodeModel   &em = *(it->second);

      if (em.GetDisplayType() == NodeModel::DisplayType::SCALAR)
      {
        const NodeScalarList<double> &nsl = em.GetScalarValues<double>();
        WriteDataArray(nsl, nm, 1, myfile);
      }
      else if (em.GetDisplayType() == NodeModel::DisplayType::NODISPLAY)
      {
      }
      else
      {
        dsAssert(0, "UNEXPECTED display type");
      }
    }
  }

  //// Edge Scalar Data
  if (!edge_models.empty())
  {
    // Strange paraview bug requires scalar before vector data
    for (Region::EdgeModelList_t::const_iterator it=edge_models.begin(); it != edge_models.end(); ++it)
    {
      const std::string &nm = it->first;
      const EdgeModel &em = *(it->second);

      if (em.GetDisplayType() == EdgeModel::DisplayType::SCALAR)
      {
        const NodeScalarList<double> &nsl = em.GetScalarValuesOnNodes<double>();
        WriteDataArray(nsl, nm, 1, myfile);
      }
    }
  }


#if 0
  //// Require user to use calculator filter
  if (!triangle_edge_models.empty())
  {
    // Strange paraview bug requires scalar before vector data
    std::vector<double> nsl;
    for (Region::TriangleEdgeModelList_t::const_iterator it=triangle_edge_models.begin(); it != triangle_edge_models.end(); ++it)
    {
      const std::string &nm = it->first;
      const TriangleEdgeModel &em = *(it->second);

      if (em.GetDisplayType() == TriangleEdgeModel::DisplayType::SCALAR)
      {
        em.GetScalarValuesOnNodes<double>(TriangleEdgeModel::COUPLE, nsl);
        WriteDataArray(nsl, nm, 1, myfile);
      }
      else if (em.GetDisplayType() == TriangleEdgeModel::DisplayType::NODISPLAY)
      {
      }
      else
      {
        dsAssert(0, "UNEXPECTED display type");
      }
    }
  }

  if (!tetrahedron_edge_models.empty())
  {
    // Strange paraview bug requires scalar before vector data
    for (Region::TetrahedronEdgeModelList_t::const_iterator it=tetrahedron_edge_models.begin(); it != tetrahedron_edge_models.end(); ++it)
    {
      const std::string &nm = it->first;
      const TetrahedronEdgeModel &em = *(it->second);

      std::vector<double> nsl;
      if (em.GetDisplayType() == TetrahedronEdgeModel::DisplayType::SCALAR)
      {
        em.GetScalarValuesOnNodes<double>(TetrahedronEdgeModel::COUPLE, nsl);
        WriteDataArray(nsl, nm, 1, myfile);
      }
      else if (em.GetDisplayType() == TetrahedronEdgeModel::DisplayType::NODISPLAY)
      {
      }
      else
      {
        dsAssert(0, "UNEXPECTED display type");
      }
    }
  }
#endif

  //// Vector<double> Edge Scalar Data
  if (!edge_models.empty())
  {
    // Strange paraview bug requires scalar before vector data
    for (Region::EdgeModelList_t::const_iterator it=edge_models.begin(); it != edge_models.end(); ++it)
    {
      const std::string &nm = it->first;
      const EdgeModel &em = *(it->second);

      if (em.GetDisplayType() == EdgeModel::DisplayType::VECTOR)
      {
        const NodeVectorList<double> &nvl = em.GetVectorValuesOnNodes<double>();


        std::vector<double> points;
        points.reserve(3*nvl.size());

        const size_t len = nvl.size();
        for (size_t i = 0; i < len; ++i)
        {
          const Vector<double> &val = nvl[i];
          points.push_back(val.Getx());
          points.push_back(val.Gety());
          points.push_back(val.Getz());
        }

        WriteDataArray(points, nm, 3, myfile);
      }
      else if (em.GetDisplayType() == EdgeModel::DisplayType::SCALAR)
      {
      }
      else if (em.GetDisplayType() == EdgeModel::DisplayType::NODISPLAY)
      {
      }
      else
      {
        dsAssert(0, "UNEXPECTED display type");
      }
    }
  }

  myfile << "</PointData>\n";
}

#if 0
void WriteLineData(const Region &reg, std::ostream &myfile)
{
  const Region::EdgeModelList_t &nml = reg.GetEdgeModelList();

  const std::vector<Vector<double>> unit = MeshUtil::GetUnitVector(reg);

  if (!nml.empty())
  {
    myfile << "<CellData>\n";

    // Strange paraview bug requires scalar before vector data
    for (Region::EdgeModelList_t::const_iterator it=nml.begin(); it != nml.end(); ++it)
    {
      const std::string &nm = it->first;
      const EdgeModel &em = *(it->second);

      if (em.GetDisplayType() == EdgeModel::DisplayType::SCALAR)
      {
        const EdgeScalarList<double> &nsl = em.GetScalarValues<double>();

        myfile << "<DataArray Name=\"" << nm << "\" type=\"Float64\" format=\"binary\">\n";

        myfile << convertVectorToZlibBase64(nsl);

#if 0
        dsAssert(nsl.size() == unit.size(), "UNEXPECTED");
        const size_t len = unit.size();
        for (size_t i = 0; i < len; ++i)
        {
          const double val = nsl[i];
          myfile << " " << val;
        }
#endif
        myfile << "\n</DataArray>\n";
      }
    }
    for (Region::EdgeModelList_t::const_iterator it=nml.begin(); it != nml.end(); ++it)
    {
      const std::string &nm = it->first;
      const EdgeModel &em = *(it->second);

      if (em.GetDisplayType() == EdgeModel::DisplayType::VECTOR)
      {
        const EdgeScalarList<double> &nsl = em.GetScalarValues<double>();

        myfile << "<DataArray Name=\"" << nm << "\" type=\"Float64\" NumberOfComponents=\"3\" format=\"binary\">\n";

        std::vector<double> points;
        points.reserve(3*nsl.size());

        dsAssert(nsl.size() == unit.size(), "UNEXPECTED");
        const size_t len = unit.size();
        for (size_t i = 0; i < len; ++i)
        {
          Vector<double> val = unit[i];
          val *= nsl[i];
          points.push_back(val.Getx());
          points.push_back(val.Gety());
          points.push_back(val.Getz());
        }
        myfile << convertVectorToZlibBase64(points);
#if 0
        dsAssert(nsl.size() == unit.size(), "UNEXPECTED");
        const size_t len = unit.size();
        for (size_t i = 0; i < len; ++i)
        {
          Vector<double> val = unit[i];
          val *= nsl[i];
          myfile << " " << val.Getx()
                 << " " << val.Gety()
                 << " " << val.Getz();
        }
#endif
        myfile << "\n</DataArray>\n";
      }
    }
    myfile << "</CellData>\n";
  }
}
#endif

void WriteElementData(const Region &reg, std::ostream &myfile)
{
  const Region::TriangleEdgeModelList_t    &triangle_edge_models    = reg.GetTriangleEdgeModelList();
  const Region::TetrahedronEdgeModelList_t &tetrahedron_edge_models = reg.GetTetrahedronEdgeModelList();

  if (triangle_edge_models.empty() && tetrahedron_edge_models.empty())
  {
    return;
  }

  myfile << "<CellData>\n";
  if (!triangle_edge_models.empty())
  {
    std::vector<double> nsl;
    for (Region::TriangleEdgeModelList_t::const_iterator it=triangle_edge_models.begin(); it != triangle_edge_models.end(); ++it)
    {
      const std::string &nm = it->first;
      const TriangleEdgeModel &em = *(it->second);

      if (em.GetDisplayType() == TriangleEdgeModel::DisplayType::SCALAR)
      {
        em.GetScalarValuesOnElements<double>(nsl);
        WriteDataArray(nsl, nm, 1, myfile);
      }
      else if (em.GetDisplayType() == TriangleEdgeModel::DisplayType::NODISPLAY)
      {
      }
      else
      {
        dsAssert(0, "UNEXPECTED display type");
      }
    }
  }

  if (!tetrahedron_edge_models.empty())
  {
    for (Region::TetrahedronEdgeModelList_t::const_iterator it=tetrahedron_edge_models.begin(); it != tetrahedron_edge_models.end(); ++it)
    {
      const std::string &nm = it->first;
      const TetrahedronEdgeModel &em = *(it->second);

      std::vector<double> nsl;
      if (em.GetDisplayType() == TetrahedronEdgeModel::DisplayType::SCALAR)
      {
        em.GetScalarValuesOnElements<double>(nsl);
        WriteDataArray(nsl, nm, 1, myfile);
      }
      else if (em.GetDisplayType() == TetrahedronEdgeModel::DisplayType::NODISPLAY)
      {
      }
      else
      {
        dsAssert(0, "UNEXPECTED display type");
      }
    }
  }

  myfile << "</CellData>\n";
}

void WriteLines(const Region &reg, std::ostream &myfile)
{
  const ConstEdgeList &cel = reg.GetEdgeList();

  std::ostringstream connectivity;
  std::ostringstream offsets;
  std::ostringstream types;

  size_t off = 2;
  for (ConstEdgeList::const_iterator it = cel.begin(); it != cel.end(); ++it)
  {
    size_t index0 = (*it)->GetHead()->GetIndex();
    size_t index1 = (*it)->GetTail()->GetIndex();
    connectivity << " " << index0 << " " << index1;
    offsets << " " << off;
    types << " 3";

    off += 2;
  }

  myfile <<
      "<Cells>\n";

  myfile <<
      "<DataArray type=\"Int32\" Name=\"connectivity\" format=\"ascii\">\n"
      << connectivity.str() <<
      "\n</DataArray>\n";

  myfile <<
      "<DataArray type=\"Int32\" Name=\"offsets\" format=\"ascii\">\n"
      << offsets.str() <<
      "\n</DataArray>\n";

  myfile <<
      "<DataArray type=\"UInt8\" Name=\"types\" format=\"ascii\">\n"
      << types.str() <<
      "\n</DataArray>\n";


  myfile << "</Cells>\n";
}

void WriteTriangles(const Region &reg, std::ostream &myfile)
{
  const ConstTriangleList &ctl = reg.GetTriangleList();

  std::ostringstream connectivity;
  std::ostringstream offsets;
  std::ostringstream types;

  size_t off = 3;
  for (ConstTriangleList::const_iterator it = ctl.begin(); it != ctl.end(); ++it)
  {

    const std::vector<ConstNodePtr> &nl = (*it)->GetNodeList();

    connectivity
              << " " << nl[0]->GetIndex()
              << " " << nl[1]->GetIndex()
              << " " << nl[2]->GetIndex();
    offsets << " " << off;
    types << " 5";

    off += 3;
  }

  myfile <<
      "<Cells>\n";

  myfile <<
      "<DataArray type=\"Int32\" Name=\"connectivity\" format=\"ascii\">\n"
      << connectivity.str() <<
      "\n</DataArray>\n";

  myfile <<
      "<DataArray type=\"Int32\" Name=\"offsets\" format=\"ascii\">\n"
      << offsets.str() <<
      "\n</DataArray>\n";

  myfile <<
      "<DataArray type=\"UInt8\" Name=\"types\" format=\"ascii\">\n"
      << types.str() <<
      "\n</DataArray>\n";


  myfile << "</Cells>\n";
}

void WriteTetrahedrons(const Region &reg, std::ostream &myfile)
{
  const ConstTetrahedronList &ctl = reg.GetTetrahedronList();

  std::ostringstream connectivity;
  std::ostringstream offsets;
  std::ostringstream types;

  size_t off = 4;
  for (ConstTetrahedronList::const_iterator it = ctl.begin(); it != ctl.end(); ++it)
  {

    const std::vector<ConstNodePtr> &nl = (*it)->GetNodeList();

    connectivity
              << " " << nl[0]->GetIndex()
              << " " << nl[1]->GetIndex()
              << " " << nl[2]->GetIndex()
              << " " << nl[3]->GetIndex();
    offsets << " " << off;
    types << " 10";

    off += 4;
  }

  myfile <<
      "<Cells>\n";

  myfile <<
      "<DataArray type=\"Int32\" Name=\"connectivity\" format=\"ascii\">\n"
      << connectivity.str() <<
      "\n</DataArray>\n";

  myfile <<
      "<DataArray type=\"Int32\" Name=\"offsets\" format=\"ascii\">\n"
      << offsets.str() <<
      "\n</DataArray>\n";

  myfile <<
      "<DataArray type=\"UInt8\" Name=\"types\" format=\"ascii\">\n"
      << types.str() <<
      "\n</DataArray>\n";


  myfile << "</Cells>\n";
}

void WriteRegionWithEdgeData(const Region &reg, std::ostream &myfile)
{
  const ConstNodeList &cnl = reg.GetNodeList();
  const size_t num_points = cnl.size();

  size_t num_cells = 0;
  const size_t dim = reg.GetDimension();

  if (dim == 1)
  {
    const ConstEdgeList &cel = reg.GetEdgeList();
    num_cells = cel.size();
  }
  else if (dim == 2)
  {
    const ConstTriangleList &cel = reg.GetTriangleList();
    num_cells = cel.size();
  }
  else if (dim == 3)
  {
    const ConstTetrahedronList &cel = reg.GetTetrahedronList();
    num_cells = cel.size();
  }

  myfile <<
         "<Piece NumberOfPoints=\"" << num_points << "\""
         " NumberOfCells=\"" << num_cells << "\""
         ">\n";

  WritePoints(reg, myfile);

  if (dim == 1)
  {
    WriteLines(reg, myfile);
  }
  else if (dim == 2)
  {
    WriteTriangles(reg, myfile);
  }
  else if (dim == 3)
  {
    WriteTetrahedrons(reg, myfile);
  }

  WritePointData(reg, myfile);

//// deprecated
#if 0
  WriteLineData(reg, myfile);
#endif
  if ((dim == 2) || (dim == 3))
  {
    WriteElementData(reg, myfile);
  }


  myfile <<
         "</Piece>\n"
         ;
}

#if 0
void WriteRegionWithTriangleData(const Region &reg, std::ostream &myfile)
{
  const ConstNodeList &cnl = reg.GetNodeList();
  const size_t num_points = cnl.size();

  const ConstTriangleList &ctl = reg.GetTriangleList();
  const size_t num_cells = ctl.size();

  myfile <<
         "<Piece NumberOfPoints=\"" << num_points << "\""
         " NumberOfCells=\"" << num_cells << "\""
         ">\n";

  WritePoints(reg, myfile);
  WriteTriangles(reg, myfile);

  myfile <<
         "</Piece>\n"
         ;
}
#endif

#if 0
void WriteRegion(const Region &r, std::ostream &myfile)
{
  WriteRegionWithEdgeData(r, myfile);
  WriteRegionWithTriangleData(r, myfile);
}
#endif

bool WriteSingleDevice(const std::string &dname, const std::string &filename, std::string &errorString)
{
  bool ret = true;
  std::ostringstream os;

  GlobalData   &gdata = GlobalData::GetInstance();

  DevicePtr dp = gdata.GetDevice(dname);

  std::string vtmfilename = filename + ".vtm";
  std::string visitfilename = filename + ".visit";


  if (!dp)
  {
    ret = false;
    os << "ERROR: Device " << dname << " does not exist\n";
  }
  else
  {
    std::ofstream vtmfile;
    std::ofstream visitfile;
    vtmfile.open (vtmfilename.c_str(), std::ios::out | std::ios::trunc | std::ios::binary);
    visitfile.open (visitfilename.c_str(), std::ios::out | std::ios::trunc | std::ios::binary);
    if (vtmfile.bad())
    {
      ret = false;
      os << "Could not open " << vtmfilename << " for writing\n";
    }
    else if (visitfile.bad())
    {
      ret = false;
      os << "Could not open " << visitfilename << " for writing\n";
    }
    else
    {
      vtmfile << std::setprecision(15) << std::scientific;

      Device &dev = *dp;

      std::vector<std::string> vtufiles;

      const Device::RegionList_t &rlist = dev.GetRegionList();
      size_t i = 0;
      for (Device::RegionList_t::const_iterator rit = rlist.begin(); rit != rlist.end(); ++rit)
      {
        std::ostringstream istring;
        istring << i;

        const std::string vtufilename = filename + "_" + istring.str() + ".vtu";

        std::ofstream vtufile;
        vtufile.open (vtufilename.c_str(), std::ios::out | std::ios::trunc | std::ios::binary);
        if (vtufile.bad())
        {
            ret = false;
            os << "Could not open " << vtufilename << " for writing\n";
            vtufile.close();
            break;
        }
        else
        {
          vtufile << std::setprecision(15) << std::scientific;

          WriteHeader(vtufile);
          //// TODO: create separate file for triangle data when we have some
          WriteRegionWithEdgeData(*(rit->second), vtufile);
          WriteFooter(vtufile);

          vtufile << "\n";
          vtufile.close();
          vtufiles.push_back(vtufilename);
        }
        ++i;
      }

      if (ret)
      {
        vtmfile <<
"<?xml version=\"1.0\"?>\n"
"<VTKFile type=\"vtkMultiBlockDataSet\" version=\"0.1\" byte_order=\"LittleEndian\" compressor=\"vtkZLibDataCompressor\">\n"
"  <vtkMultiBlockDataSet>\n";
        visitfile << "!NBLOCKS " << vtufiles.size() << "\n";
        size_t i = 0;
        for (std::vector<std::string>::iterator it = vtufiles.begin(); it != vtufiles.end(); ++it)
        {
          vtmfile <<
"    <DataSet group=\"" << i << "\" dataset=\"" << 0 << "\" file=\"" << *it << "\"/>\n";
          ++i;
          visitfile << *it << "\n";
        }
        vtmfile <<
"  </vtkMultiBlockDataSet>\n"
"</VTKFile>\n";
      }

    }
    vtmfile.close();
    visitfile.close();
  }
  errorString += os.str();
  return ret;
}
}

VTKWriter::~VTKWriter()
{
}

bool VTKWriter::WriteMesh_(const std::string &deviceName, const std::string &filename, std::string &errorString)
{
    bool ret = true;
    std::ostringstream os;

#if 0
    std::ofstream myfile;
    myfile.open (filename.c_str(), std::ios::out | std::ios::trunc | std::ios::binary);
    if (myfile.bad())
    {
        ret = false;
        os << "Could not open " << filename << " for writing\n";
    }
    else
    {
#endif
        ret = VTK::WriteSingleDevice(deviceName, filename, errorString);
#if 0
    }
#endif
    errorString += os.str();
    return ret;
}

bool VTKWriter::WriteMeshes_(const std::string &filename, std::string &errorString)
{
    bool ret = true;
    std::ostringstream os;

#if 0
    std::ofstream myfile;
    myfile.open (filename.c_str(), std::ios::out | std::ios::trunc | std::ios::binary);
    if (myfile.bad())
    {
        ret = false;
        os << "Could not open " << filename << " for writing\n";
    }
#endif

    GlobalData   &gdata = GlobalData::GetInstance();
    const GlobalData::DeviceList_t &dlist = gdata.GetDeviceList();

    if (dlist.size() > 1)
    {
        ret = false;
        os << "More than 1 device in simulation when output format only supports one device.\n";
    }
    else
    {
        for (GlobalData::DeviceList_t::const_iterator dit = dlist.begin(); dit != dlist.end(); ++dit)
        {
            const std::string &dname = dit->first;
            ret = VTK::WriteSingleDevice(dname, filename, errorString);
        }
    }
#if 0
    myfile.close();
#endif

    errorString += os.str();
    return ret;
}
