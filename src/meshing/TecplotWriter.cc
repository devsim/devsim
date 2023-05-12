/***
DEVSIM
Copyright 2013 DEVSIM LLC

SPDX-License-Identifier: Apache-2.0
***/

#include "TecplotWriter.hh"
#include "GlobalData.hh"
#include "Device.hh"
#include "Coordinate.hh"
#include "Region.hh"
#include "Node.hh"
#include "Edge.hh"
#include "Triangle.hh"
#include "Tetrahedron.hh"
#include "Contact.hh"
#include "Interface.hh"
#include "NodeModel.hh"
#include "EdgeModel.hh"
#include "MeshUtil.hh"
#include "dsAssert.hh"
#include "OutputStream.hh"
#include "TriangleEdgeModel.hh"
#include "TetrahedronEdgeModel.hh"
#include <sstream>
#include <fstream>
#include <iomanip>
#include <set>
namespace Tecplot {
void BreakLine(std::ostream &myfile, const std::string &output_string)
{
  if (output_string.empty())
  {
    return;
  }

  size_t len = output_string.size();

  if (len < 30000)
  {
    myfile << output_string;
  }
  else
  {
    size_t start = 0;

    bool done = false;
    while (!done)
    {
      size_t seek  = start + 29999;

      if (seek > (len - 1))
      {
        myfile << output_string.substr(start);
        done = true;
      }
      else
      {
        size_t pos = output_string.rfind(' ', seek);
        if (pos != std::string::npos)
        {
          /// we drop the whitespace character and replace
          myfile << output_string.substr(start, pos - start) << "\n";
          start = pos + 1;
        }
        else
        {
          pos = output_string.rfind('\n', seek);
          if (pos != std::string::npos)
          {
            myfile << output_string.substr(start, pos - start) << "\n";
            start = pos + 1;
          }
          else
          {
            myfile << output_string.substr(start, seek) << "\n";
            start = seek + 1;

          }
        }
      }
      //// This is one past the end of the string
      if (start >= len)
      {
        done = true;
      }
    }
  }
}


void WriteBlock(std::ostream &os, const std::vector<double> &values)
{
  NodeScalarList<double>::const_iterator it = values.begin();
  if (it != values.end())
  {
    os << *it;
    ++it;
  }
  for (; it != values.end(); ++it)
  {
    os << " " << *it;
  }
}

void WriteUniform(std::ostream &os, const size_t length, const double uniform_value)
{
#if 0
  if (uniform_value == 0.0)
  {
    os << length << "*0";
  }
  else
  {
    os << length << "*" << uniform_value;
  }
#else
  if (length != 0)
  {
    os << uniform_value;
  }
  for (size_t i = 1; i < length; ++i)
  {
    os << " " << uniform_value;
  }
#endif
}

void WriteNodeBlock(std::ostream &myfile, const Region &reg, const std::string &model_name)
{
  const size_t number_nodes = reg.GetNumberNodes();
  ConstNodeModelPtr nmp = reg.GetNodeModel(model_name);

  std::ostringstream os;
  os.copyfmt(myfile);
  if (nmp)
  {
    if (nmp->IsUniform())
    {
      WriteUniform(os, number_nodes, nmp->GetUniformValue<double>());
    }
    else
    {
      const NodeScalarList<double> &values = nmp->GetScalarValues<double>();
      WriteBlock(os, values);
    }
  }
  else
  {
    WriteUniform(os, number_nodes, 0.0);
  }
  os << "\n";

  BreakLine(myfile, os.str());
}

void WriteEdgeBlockScalar(std::ostream &myfile, const Region &reg, const std::string &model_name)
{
  std::ostringstream os;
  os.copyfmt(myfile);
  const size_t number_nodes = reg.GetNumberNodes();
  ConstEdgeModelPtr emp = reg.GetEdgeModel(model_name);

  /// in case the model doesn't exist in our region
  if (emp)
  {
//    dsAssert(emp->GetDisplayType() == EdgeModel::DisplayType::SCALAR, "UNEXPECTED");
    if (emp->IsUniform())
    {
      WriteUniform(os, number_nodes, emp->GetUniformValue<double>());
    }
    else
    {
      const NodeScalarList<double> &values = emp->GetScalarValuesOnNodes<double>();
      WriteBlock(os, values);
    }
  }
  else
  {
    WriteUniform(os, number_nodes, 0.0);
  }
  os << "\n";

  BreakLine(myfile, os.str());
}

void WriteTriangleEdgeBlockScalar(std::ostream &myfile, const Region &reg, const std::string &model_name)
{
  std::ostringstream os;
  os.copyfmt(myfile);

//  const size_t number_nodes = reg.GetNumberNodes();
  const size_t number_triangles = reg.GetNumberTriangles();
  ConstTriangleEdgeModelPtr emp = reg.GetTriangleEdgeModel(model_name);

  /// in case the model doesn't exist in our region
  if (emp)
  {
//    dsAssert(emp->GetDisplayType() == TriangleEdgeModel::DisplayType::SCALAR, "UNEXPECTED");

    if (emp->IsUniform())
    {
      WriteUniform(os, number_triangles, emp->GetUniformValue<double>());
    }
    else
    {
      std::vector<double> values;
      emp->GetScalarValuesOnElements<double>(values);
      WriteBlock(os, values);
    }
  }
  else
  {
    WriteUniform(os, number_triangles, 0.0);
  }
  os << "\n";
  BreakLine(myfile, os.str());
}

void WriteTetrahedronEdgeBlockScalar(std::ostream &myfile, const Region &reg, const std::string &model_name)
{
  std::ostringstream os;
  os.copyfmt(myfile);
//  const size_t number_nodes = reg.GetNumberNodes();
  const size_t number_tetrahedrons = reg.GetNumberTetrahedrons();
  ConstTetrahedronEdgeModelPtr emp = reg.GetTetrahedronEdgeModel(model_name);

  /// in case the model doesn't exist in our region
  if (emp)
  {
//    dsAssert(emp->GetDisplayType() == TriangleEdgeModel::DisplayType::SCALAR, "UNEXPECTED");

    if (emp->IsUniform())
    {
      WriteUniform(os, number_tetrahedrons, emp->GetUniformValue<double>());
    }
    else
    {
      std::vector<double> values;
      emp->GetScalarValuesOnElements<double>(values);
      WriteBlock(os, values);
    }
  }
  else
  {
    WriteUniform(os, number_tetrahedrons, 0.0);
  }
  os << "\n";
  BreakLine(myfile, os.str());
}


void WriteEdgeBlockVector(std::ostream &myfile, const Region &reg, const std::string &model_name)
{
  const size_t number_nodes = reg.GetNumberNodes();
  const size_t dimension    = reg.GetDimension();

  ConstEdgeModelPtr emp = reg.GetEdgeModel(model_name);

  std::ostringstream osx;
  std::ostringstream osy;
  std::ostringstream osz;

  osx.copyfmt(myfile);
  osy.copyfmt(myfile);
  osz.copyfmt(myfile);


  /// in case the model doesn't exist in our region
  /// TODO: make sure it is the same type in each region
  if (emp)
  {
    dsAssert(emp->GetDisplayType() == EdgeModel::DisplayType::VECTOR, "UNEXPECTED");

    const NodeVectorList<double> &values = emp->GetVectorValuesOnNodes<double>();
    dsAssert(values.size() > 0, "UNEXPECTED");

    NodeVectorList<double>::const_iterator it = values.begin();
    if (it != values.end())
    {
      osx << " " << (*it).Getx();
      osy << " " << (*it).Gety();
      osz << " " << (*it).Getz();
      ++it;
    }
    for (; it != values.end(); ++it)
    {
      osx << " " << (*it).Getx();
      osy << " " << (*it).Gety();
      osz << " " << (*it).Getz();
    }
  }
  else
  {
    WriteUniform(osx, number_nodes, 0.0);
    WriteUniform(osy, number_nodes, 0.0);
    WriteUniform(osz, number_nodes, 0.0);
  }

  osx << "\n";
  BreakLine(myfile, osx.str());

  if (dimension > 1)
  {
    osy << "\n";
    BreakLine(myfile, osy.str());
  }
  if (dimension > 2)
  {
    osz << "\n";
    BreakLine(myfile, osz.str());
  }
}

void WriteNodesAndSolutions(std::ostream &myfile, const Region &reg, const std::set<std::string> &solutions)
{
  for (std::set<std::string>::const_iterator it = solutions.begin(); it != solutions.end(); ++it)
  {
    WriteNodeBlock(myfile, reg, *it);
  }
}

void WriteScalarEdgeModels(std::ostream &myfile, const Region &reg, const std::set<std::string> &solutions)
{
  for (std::set<std::string>::const_iterator it = solutions.begin(); it != solutions.end(); ++it)
  {
    WriteEdgeBlockScalar(myfile, reg, *it);
  }
}

void WriteVectorEdgeModels(std::ostream &myfile, const Region &reg, const std::set<std::string> &solutions)
{
  for (std::set<std::string>::const_iterator it = solutions.begin(); it != solutions.end(); ++it)
  {
    WriteEdgeBlockVector(myfile, reg, *it);
  }
}

void WriteScalarElementEdgeModels(std::ostream &myfile, const Region &reg, const std::set<std::string> &solutions)
{
  const size_t dimension = reg.GetDimension();
  if (dimension == 2)
  {
    for (std::set<std::string>::const_iterator it = solutions.begin(); it != solutions.end(); ++it)
    {
//      std::cerr << "WRITING " << *it << std::endl;
      WriteTriangleEdgeBlockScalar(myfile, reg, *it);
    }
  }
  else if (dimension == 3)
  {
    for (std::set<std::string>::const_iterator it = solutions.begin(); it != solutions.end(); ++it)
    {
      WriteTetrahedronEdgeBlockScalar(myfile, reg, *it);
    }
  }
}

void WriteEdges(std::ostream &myfile, const Region &reg)
{
    const ConstEdgeList &ctl = reg.GetEdgeList();
    for (ConstEdgeList::const_iterator tit = ctl.begin(); tit != ctl.end(); ++tit)
    {
        const ConstNodeList &nlist = (*tit)->GetNodeList();
        myfile <<  (nlist[0]->GetIndex() + 1)
        << " " <<  (nlist[1]->GetIndex() + 1)
        << "\n";
    }
}

void WriteTriangles(std::ostream &myfile, const Region &reg)
{
    const ConstTriangleList &ctl = reg.GetTriangleList();
    for (ConstTriangleList::const_iterator tit = ctl.begin(); tit != ctl.end(); ++tit)
    {
        const ConstNodeList &nlist = (*tit)->GetFENodeList();
        myfile <<  (nlist[0]->GetIndex() + 1)
        << " " <<  (nlist[1]->GetIndex() + 1)
        << " " <<  (nlist[2]->GetIndex() + 1)
        << "\n";
    }
}

void WriteTetrahedra(std::ostream &myfile, const Region &reg)
{
    const ConstTetrahedronList &ctl = reg.GetTetrahedronList();
    for (ConstTetrahedronList::const_iterator tit = ctl.begin(); tit != ctl.end(); ++tit)
    {
        const ConstNodeList &nlist = (*tit)->GetFENodeList();
        myfile <<  (nlist[0]->GetIndex() + 1)
        << " " <<  (nlist[1]->GetIndex() + 1)
        << " " <<  (nlist[2]->GetIndex() + 1)
        << " " <<  (nlist[3]->GetIndex() + 1)
        << "\n";
    }
}


bool WriteSingleDevice(const std::string &dname, std::ostream &myfile, std::string &errorString)
{
  bool ret = true;
  std::ostringstream os;
  os.copyfmt(myfile);

//    myfile << std::setprecision(15) << std::scientific;
  GlobalData   &gdata = GlobalData::GetInstance();

  DevicePtr dp = gdata.GetDevice(dname);

  if (!dp)
  {
    ret = false;
    os << "ERROR: Device " << dname << " does not exist\n";
  }
  else
  {
    Device &dev = *dp;

    size_t dimension = dev.GetDimension();

    //// write solutions line
    std::set<std::string> nodeModelsNoDisplay;
    std::set<std::string> nodeModelsScalar;
    std::set<std::string> edgeModelsNoDisplay;
    std::set<std::string> edgeModelsScalar;
    std::set<std::string> edgeModelsVector;
    std::set<std::string> elementEdgeModelsScalar;
    {
      const Device::RegionList_t &rlist = dev.GetRegionList();
      for (Device::RegionList_t::const_iterator rit = rlist.begin(); rit != rlist.end(); ++rit)
      {
        const Region      &reg   = *(rit->second);
        const Region::NodeModelList_t nmlist = reg.GetNodeModelList();

        for (Region::NodeModelList_t::const_iterator nit = nmlist.begin(); nit != nmlist.end(); ++nit)
        {
          const std::string &name = nit->first;
          if ((name == "x" || name == "y" || name == "z"))
          {
            continue;
          }

          ConstNodeModelPtr emp = nit->second;

          NodeModel::DisplayType display_type=emp->GetDisplayType();

          const bool s_display = (nodeModelsScalar.find(name) != nodeModelsScalar.end());
          const bool n_display = (nodeModelsNoDisplay.find(name) != nodeModelsNoDisplay.end());

          std::string conflicting_type;

          if (display_type == NodeModel::DisplayType::NODISPLAY)
          {
            if (s_display)
            {
              conflicting_type = "scalar";
            }
            else
            {
              nodeModelsNoDisplay.insert(name);
            }
          }
          else if (display_type == NodeModel::DisplayType::SCALAR)
          {
            if (n_display)
            {
              conflicting_type = "nodisplay";
            }
            else
            {
              nodeModelsScalar.insert(name);
            }
          }
          else
          {
            dsAssert(0, "UNEXPECTED");
          }

          if (!conflicting_type.empty())
          {
            std::stringstream os;
            os << "Writing model " << name << " as " << conflicting_type << " since it was the first type encountered\n";
            OutputStream::WriteOut(OutputStream::OutputType::INFO, os.str());
          }
        }

        const Region::EdgeModelList_t emlist = reg.GetEdgeModelList();
        for (Region::EdgeModelList_t::const_iterator eit = emlist.begin(); eit != emlist.end(); ++eit)
        {
          const std::string name = eit->first;
          ConstEdgeModelPtr emp = eit->second;

          EdgeModel::DisplayType display_type=emp->GetDisplayType();

          const bool n_display = (edgeModelsNoDisplay.find(name) != edgeModelsNoDisplay.end());
          const bool v_display = (edgeModelsVector.find(name) != edgeModelsVector.end());
          const bool s_display = (edgeModelsScalar.find(name) != edgeModelsScalar.end());

          std::string conflicting_type;

          if (display_type == EdgeModel::DisplayType::NODISPLAY)
          {
            if (v_display)
            {
              conflicting_type = "vector";
            }
            else if (s_display)
            {
              conflicting_type = "scalar";
            }
            else
            {
              edgeModelsNoDisplay.insert(name);
            }
          }
          else if (display_type == EdgeModel::DisplayType::SCALAR)
          {
            if (v_display)
            {
              conflicting_type = "vector";
            }
            else if (n_display)
            {
              conflicting_type = "nodisplay";
            }
            else
            {
              edgeModelsScalar.insert(name);
            }
          }
          else if (display_type == EdgeModel::DisplayType::VECTOR)
          {
            if (s_display)
            {
              conflicting_type = "scalar";
            }
            else if (n_display)
            {
              conflicting_type = "nodisplay";
            }
            else
            {
              edgeModelsVector.insert(name);
            }
          }
          else
          {
            dsAssert(0, "UNEXPECTED");
          }

          if (!conflicting_type.empty())
          {
            std::stringstream os;
            os << "Writing model " << name << " as " << conflicting_type << " since it was the first type encountered\n";
            OutputStream::WriteOut(OutputStream::OutputType::INFO, os.str());
          }
        }

        const Region::TriangleEdgeModelList_t triangle_edge_model_list = reg.GetTriangleEdgeModelList();
        const Region::TetrahedronEdgeModelList_t tetrahedron_edge_model_list = reg.GetTetrahedronEdgeModelList();

        if (dimension == 2)
        {
          dsAssert(tetrahedron_edge_model_list.empty(), "UNEXPECTED");
        }
        else if (dimension == 3)
        {
          dsAssert(triangle_edge_model_list.empty(), "UNEXPECTED");
        }

        for (Region::TriangleEdgeModelList_t::const_iterator trit = triangle_edge_model_list.begin(); trit != triangle_edge_model_list.end(); ++trit)
        {
          if (trit->second->GetDisplayType() == TriangleEdgeModel::DisplayType::SCALAR)
          {
            elementEdgeModelsScalar.insert(trit->first);
          }
          else
          {
            dsAssert(trit->second->GetDisplayType() == TriangleEdgeModel::DisplayType::NODISPLAY, "UNEXPECTED");
          }
        }
        for (Region::TetrahedronEdgeModelList_t::const_iterator teit = tetrahedron_edge_model_list.begin(); teit != tetrahedron_edge_model_list.end(); ++teit)
        {
          if (teit->second->GetDisplayType() == TetrahedronEdgeModel::DisplayType::SCALAR)
          {
            elementEdgeModelsScalar.insert(teit->first);
          }
          else
          {
            dsAssert(teit->second->GetDisplayType() == TetrahedronEdgeModel::DisplayType::NODISPLAY, "UNEXPECTED");
          }
        }
      }
    }

    myfile << "TITLE = \"" << dname << "\"\n";


    std::ostringstream os;
    os.copyfmt(myfile);

    size_t dim = dev.GetDimension();

    //// position of cell centered quantity
    size_t varloc = 0;
    std::vector<size_t> cell_variables;

    if (dim == 1)
    {
      os << "VARIABLES = " << "\"x\"";
      varloc += 1;
    }
    else if (dim == 2)
    {
      os << "VARIABLES = " << "\"x\", \"y\"";
      varloc += 2;
    }
    else
    {
      os << "VARIABLES = " << "\"x\", \"y\", \"z\"";
      varloc += 3;
    }

    for (std::set<std::string>::iterator nmit = nodeModelsScalar.begin(); nmit != nodeModelsScalar.end(); ++nmit)
    {
        os << ", \"" << *nmit << "\"";
        varloc += 1;
    }

    for (std::set<std::string>::iterator it = edgeModelsScalar.begin(); it != edgeModelsScalar.end(); ++it)
    {
        os << ", \"" << *it << "\"";
        varloc += 1;
    }
    for (std::set<std::string>::iterator it = edgeModelsVector.begin(); it != edgeModelsVector.end(); ++it)
    {
      if (dim == 1)
      {
        os
          << ", \"" << *it << "_x_onNode" << "\""
        ;
        varloc += 1;
      }
      else if (dim == 2)
      {
        os
          << ", \"" << *it << "_x_onNode" << "\""
          << ", \"" << *it << "_y_onNode" << "\""
        ;
        varloc += 2;
      }
      else if (dim == 3)
      {
        os
          << ", \"" << *it << "_x_onNode" << "\""
          << ", \"" << *it << "_y_onNode" << "\""
          << ", \"" << *it << "_z_onNode" << "\""
        ;
        varloc += 3;
      }
      else
      {
        dsAssert(0, "UNEXPECTED");
      }
    }

    for (std::set<std::string>::iterator it = elementEdgeModelsScalar.begin(); it != elementEdgeModelsScalar.end(); ++it)
    {
        os << ", \"" << *it << "\"";
        varloc += 1;
        cell_variables.push_back(varloc);
    }

    os << "\n";
    BreakLine(myfile, os.str());

    const Device::RegionList_t &rlist = dev.GetRegionList();

    std::string cell_variables_string;
    if (!cell_variables.empty())
    {
      std::ostringstream os;
      os.copyfmt(myfile);
      os << ", VARLOCATION=([";
      os << cell_variables[0];
      for (size_t i = 1; i < cell_variables.size(); ++i)
      {
        os << "," << cell_variables[i];
      }
      os << "]=CELLCENTERED)";
      cell_variables_string = os.str();
    }

    for (Device::RegionList_t::const_iterator rit = rlist.begin(); rit != rlist.end(); ++rit)
    {
        const std::string &rname = rit->first;
        const Region      &reg   = *(rit->second);

        const size_t numnodes = reg.GetNumberNodes();

        myfile << "ZONE T=\"" << rname << "\" NODES=" << numnodes;

        if (dim == 1)
        {
          const size_t numelements = reg.GetNumberEdges();
          myfile << ", ELEMENTS=" << numelements
            << ", DATAPACKING=BLOCK, ZONETYPE=FELINESEG"
            << cell_variables_string << "\n";
          WriteNodeBlock(myfile, reg, "x");
        }
        else if (dim == 2)
        {
          const size_t numelements = reg.GetNumberTriangles();
          myfile << ", ELEMENTS=" << numelements
            << ", DATAPACKING=BLOCK, ZONETYPE=FETRIANGLE"
            << cell_variables_string << "\n";
          WriteNodeBlock(myfile, reg, "x");
          WriteNodeBlock(myfile, reg, "y");
        }
        else
        {
          const size_t numelements = reg.GetNumberTetrahedrons();
          myfile << ", ELEMENTS=" << numelements
            << ", DATAPACKING=BLOCK, ZONETYPE=FETETRAHEDRON"
            << cell_variables_string << "\n";
          WriteNodeBlock(myfile, reg, "x");
          WriteNodeBlock(myfile, reg, "y");
          WriteNodeBlock(myfile, reg, "z");
        }


        WriteNodesAndSolutions(myfile, reg, nodeModelsScalar);

        WriteScalarEdgeModels(myfile, reg, edgeModelsScalar);

        WriteVectorEdgeModels(myfile, reg, edgeModelsVector);

        WriteScalarElementEdgeModels(myfile, reg, elementEdgeModelsScalar);

        if (dim == 1)
        {
          WriteEdges(myfile, reg);
        }
        else if (dim == 2)
        {
          WriteTriangles(myfile, reg);
        }
        else
        {
          WriteTetrahedra(myfile, reg);
        }

    }

//TODO:  how about text fields, like parameters?
//TODO:  "interface edges"
//TODO:  "contacts edges"
    }
    myfile << "\n";
    errorString += os.str();
    return ret;

}
}

TecplotWriter::~TecplotWriter()
{
}

bool TecplotWriter::WriteMesh_(const std::string &deviceName, const std::string &filename, std::string &errorString)
{
    bool ret = true;
    std::ostringstream os;

    std::ofstream myfile;
    myfile.open (filename.c_str(), std::ios::out | std::ios::trunc | std::ios::binary);
    if (myfile.bad())
    {
        ret = false;
        os << "Could not open " << filename << " for writing\n";
    }
    else
    {
        ret = Tecplot::WriteSingleDevice(deviceName, myfile, errorString);
    }
    errorString += os.str();
    return ret;
}

bool TecplotWriter::WriteMeshes_(const std::string &filename, std::string &errorString)
{
    bool ret = true;
    std::ostringstream os;

    std::ofstream myfile;
    myfile.open (filename.c_str(), std::ios::out | std::ios::trunc | std::ios::binary);
    if (myfile.bad())
    {
        ret = false;
        os << "Could not open " << filename << " for writing\n";
    }

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
            ret = Tecplot::WriteSingleDevice(dname, myfile, errorString);
        }
    }
    myfile.close();

    errorString += os.str();
    return ret;
}

