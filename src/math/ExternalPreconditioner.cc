/***
DEVSIM
Copyright 2013 DEVSIM LLC

SPDX-License-Identifier: Apache-2.0
***/

#include "ExternalPreconditioner.hh"
#include "CompressedMatrix.hh"
#include "dsAssert.hh"
#include "ObjectHolder.hh"
#include "Interpreter.hh"
#include "GlobalData.hh"
#include "OutputStream.hh"
#include <utility>
#include <algorithm>

namespace dsMath {
namespace
{
void preswap(std::vector<double> &xin, std::vector<double> &xout)
{
  xin.swap(xout);
}
void convertToType(std::vector<double> &xin, std::vector<double> &xout)
{
  xout.swap(xin);
}
void preswap(ComplexDoubleVec_t<double> &xin, ComplexDoubleVec_t<double> &xout)
{
  xin.swap(xout);
}
void convertToType(ComplexDoubleVec_t<double> &xin, ComplexDoubleVec_t<double> &xout)
{
  xout.swap(xin);
}
}
}

#ifdef DEVSIM_EXTENDED_PRECISION
#include "Float128.hh"
namespace dsMath {
namespace
{
void preswap(std::vector<double> &xin, std::vector<float128> &xout)
{
}
void convertToType(std::vector<double> &xin, std::vector<float128> &xout)
{
  xout.resize(xin.size());
  std::copy(xin.begin(), xin.end(), xout.begin());
}
void preswap(ComplexDoubleVec_t<double> &xin, ComplexDoubleVec_t<float128> &xout)
{
}
void convertToType(ComplexDoubleVec_t<double> &xin, ComplexDoubleVec_t<float128> &xout)
{
  xout.resize(xin.size());
  std::transform(xin.begin(), xin.end(), xout.begin(), [](auto x){return static_cast<ComplexDouble_t<float128>>(x);});
}
}
}
#endif


namespace dsMath {
template <typename DoubleType>
ExternalPreconditioner<DoubleType>::ExternalPreconditioner(size_t sz, PEnum::TransposeType_t transpose) : Preconditioner<DoubleType>(sz, transpose)
{
}

template <typename DoubleType>
dsMath::CompressionType ExternalPreconditioner<DoubleType>::GetRealMatrixCompressionType() const
{
  return compression_type_;
}

template <typename DoubleType>
dsMath::CompressionType ExternalPreconditioner<DoubleType>::GetComplexMatrixCompressionType() const
{
  return compression_type_;
}

template <typename DoubleType>
ExternalPreconditioner<DoubleType>::~ExternalPreconditioner()
{
}

template <typename DoubleType>
bool ExternalPreconditioner<DoubleType>::init(ObjectHolder oh, std::string &error_string)
{
  if (!oh.IsCallable())
  {
    error_string += "python solver object \"" + oh.GetString() + "\" is not callable\n";
    return false;
  }
  const std::string return_keys[] = {
    "matrix_format",
    "solver_object",
    "status",
    "message",
  };
  ObjectHolderMap_t init_args = {
    {"action", ObjectHolder("init")},
    {"transpose", ObjectHolder(this->GetTransposeSolve())},
    {"n", ObjectHolder(static_cast<int>(this->size()))},
  };

  command_handle_ = oh;
  Interpreter interpreter;
  bool ret = interpreter.RunCommand(command_handle_, init_args);
  if (!ret)
  {
    error_string = interpreter.GetErrorString();
  }
  else
  {
    ObjectHolderMap_t result_dictionary;
    auto result = interpreter.GetResult();
    ret = result.GetHashMap(result_dictionary);
    if (!ret)
    {
      error_string += "python solver object did not return a dictionary\n";
    }
    else
    {
      for (const auto &arg: return_keys)
      {
        if (auto it = result_dictionary.find(arg); it == result_dictionary.end())
        {
          error_string += "python solver object did not return a dictionary containing \"" + arg + "\"\n";
          ret = false;
        }
      }
      if (ret)
      {
        auto matrix_format = result_dictionary["matrix_format"].GetString();
        if (matrix_format == "csc")
        {
          compression_type_ = dsMath::CompressionType::CCM;
        }
        else if (matrix_format == "csr")
        {
          compression_type_ = dsMath::CompressionType::CRM;
        }
        else
        {
          error_string += R"(python solver object did not return a dictionary containing "csc" or "csr" for "matrix_format")" "\n";
          ret = false;
        }
        auto status = result_dictionary["status"].GetBoolean().second;
        auto message = result_dictionary["message"].GetString();
        error_string += message;
        dsAssert(status, error_string);
      }
      command_data_ = result_dictionary["solver_object"];
    }
  }
  return ret;
}

template <typename DoubleType>
bool ExternalPreconditioner<DoubleType>::DerivedLUFactor(Matrix<DoubleType> *m)
{
  dsAssert(command_handle_.IsCallable(), "python solver command is not callable\n");
  dsAssert(!command_data_.empty(), "python solver invalid data\n");

  CompressedMatrix<DoubleType> *cm = dynamic_cast<CompressedMatrix<DoubleType> *>(m);
  dsAssert(cm, "UNEXPECTED");
  dsAssert(cm->GetCompressionType() == compression_type_, "UNEXPECTED");

  const std::string return_keys[] = {
    "status",
    "message",
  };

  ObjectHolderMap_t factor_args = {
    {"action", ObjectHolder("factor")},
    {"solver_object", command_data_},
    {"Ap", CreateIntPODArray(cm->GetAp())},
    {"Ai", CreateIntPODArray(cm->GetAi())},
    {"is_same_symbolic", ObjectHolder(cm->GetSymbolicStatus() == SymbolicStatus_t::SAME_SYMBOLIC)},
  };

  if (cm->GetMatrixType() == MatrixType::COMPLEX)
  {
    const auto &ax = cm->GetAx();
    const auto &az = cm->GetAz();
    dsAssert(ax.size() == az.size(), "UNEXPECTED");
    std::vector<DoubleType> ac(2*ax.size());
    for (size_t i = 0, j = 0; i < ax.size(); ++i)
    {
      ac[j++] = ax[i];
      ac[j++] = az[i];
    }
    factor_args["is_complex"] = ObjectHolder(true);
    factor_args["Ax"] = CreateDoublePODArray(ac);
  }
  else
  {
    factor_args["is_complex"] = ObjectHolder(false);
    factor_args["Ax"] = CreateDoublePODArray(cm->GetAx());
  }

  Interpreter interpreter;
  bool ret = interpreter.RunCommand(command_handle_, factor_args);
  if (!ret)
  {
    std::string error = "while factorizing matrix using python solver\n";
    error += interpreter.GetErrorString();
    OutputStream::WriteOut(OutputStream::OutputType::FATAL, error.c_str());
  }
  else
  {
    std::string error_string;
    ObjectHolderMap_t result_dictionary;
    auto result = interpreter.GetResult();
    ret = result.GetHashMap(result_dictionary);
    if (!ret)
    {
      error_string += "python solver object did not return a dictionary\n";
    }
    else
    {
      for (const auto &arg: return_keys)
      {
        if (auto it = result_dictionary.find(arg); it == result_dictionary.end())
        {
          error_string += "python solver object did not return a dictionary containing \"" + arg + "\"\n";
          ret = false;
        }
      }
      auto status = result_dictionary["status"].GetBoolean().second;
      auto message = result_dictionary["message"].GetString();
      error_string += message;
      dsAssert(status, error_string);
    }
  }
  return ret;
}

template <typename DoubleType>
void ExternalPreconditioner<DoubleType>::DerivedLUSolve(DoubleVec_t<DoubleType> &x, const DoubleVec_t<DoubleType> &b) const
{
  dsAssert(command_handle_.IsCallable(), "python solver command is not callable\n");
  dsAssert(!command_data_.empty(), "python solver invalid data\n");

  const std::string return_keys[] = {
    "status",
    "message",
    "x",
  };
  ObjectHolderMap_t factor_args = {
    {"action", ObjectHolder("solve")},
    {"solver_object", command_data_},
    {"is_complex", ObjectHolder(false)},
    {"b", CreateDoublePODArray(b)},
  };

  Interpreter interpreter;
  bool ret = interpreter.RunCommand(command_handle_, factor_args);
  if (!ret)
  {
    std::string error = "while solving matrix using python solver\n";
    error += interpreter.GetErrorString();
    OutputStream::WriteOut(OutputStream::OutputType::FATAL, error.c_str());
  }
  else
  {
    std::string error_string;
    ObjectHolderMap_t result_dictionary;
    auto result = interpreter.GetResult();
    ret = result.GetHashMap(result_dictionary);
    if (!ret)
    {
      error_string += "python solver object did not return a dictionary\n";
    }
    else
    {
      for (const auto &arg: return_keys)
      {
        if (auto it = result_dictionary.find(arg); it == result_dictionary.end())
        {
          error_string += "python solver object did not return a dictionary containing \"" + arg + "\"\n";
          ret = false;
        }
      }
      auto status = result_dictionary["status"].GetBoolean().second;
      auto message = result_dictionary["message"].GetString();
      error_string += message;
      dsAssert(status, error_string);
      std::vector<double> xv;
      preswap(xv, x);
      bool ret = result_dictionary["x"].GetDoubleList(xv);
      convertToType(xv, x);
      dsAssert(ret && (x.size() == b.size()), "Mismatch in returned x");
    }
  }
}

template <typename DoubleType>
void ExternalPreconditioner<DoubleType>::DerivedLUSolve(ComplexDoubleVec_t<DoubleType> &x, const ComplexDoubleVec_t<DoubleType> &b) const
{
  dsAssert(command_handle_.IsCallable(), "python solver command is not callable\n");
  dsAssert(!command_data_.empty(), "python solver invalid data\n");

  const std::string return_keys[] = {
    "status",
    "message",
    "x",
  };

  ObjectHolderMap_t factor_args = {
    {"action", ObjectHolder("solve")},
    {"solver_object", command_data_},
    {"is_complex", ObjectHolder(true)},
    {"b", CreateComplexDoublePODArray(b)},
  };

  Interpreter interpreter;
  bool ret = interpreter.RunCommand(command_handle_, factor_args);
  if (!ret)
  {
    std::string error = "while solving matrix using python solver\n";
    error += interpreter.GetErrorString();
    OutputStream::WriteOut(OutputStream::OutputType::FATAL, error.c_str());
  }
  else
  {
    std::string error_string;
    ObjectHolderMap_t result_dictionary;
    auto result = interpreter.GetResult();
    ret = result.GetHashMap(result_dictionary);
    if (!ret)
    {
      error_string += "python solver object did not return a dictionary\n";
    }
    else
    {
      for (const auto &arg: return_keys)
      {
        if (auto it = result_dictionary.find(arg); it == result_dictionary.end())
        {
          error_string += "python solver object did not return a dictionary containing \"" + arg + "\"\n";
          ret = false;
        }
      }
      auto status = result_dictionary["status"].GetBoolean().second;
      auto message = result_dictionary["message"].GetString();
      error_string += message;
      dsAssert(status, error_string);

      bool ret = false;
      ComplexDoubleVec_t<double> xv;
      preswap(xv, x);
      ret = result_dictionary["x"].GetComplexDoubleList(xv);
      convertToType(xv, x);
      dsAssert(ret && (x.size() == b.size()), "Mismatch in returned x");
    }
  }
}
}


template class dsMath::ExternalPreconditioner<double>;
#ifdef DEVSIM_EXTENDED_PRECISION
#include "Float128.hh"
template class dsMath::ExternalPreconditioner<float128>;
#endif

