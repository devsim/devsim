/***
DEVSIM
Copyright 2013 DEVSIM LLC

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

#include "ExternalPreconditioner.hh"
#include "CompressedMatrix.hh"
#include "dsAssert.hh"
#include "ObjectHolder.hh"
#include "Interpreter.hh"
#include "GlobalData.hh"
#include "OutputStream.hh"
#include <utility>
#include <complex>
#include <algorithm>

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
    {"Ap", CreatePODArray(cm->GetAp())},
    {"Ai", CreatePODArray(cm->GetAi())},
    {"Ax", CreateDoublePODArray(cm->GetAx())},
    {"is_same_symbolic", ObjectHolder(cm->GetSymbolicStatus() == SymbolicStatus_t::SAME_SYMBOLIC)},
  };

  if (cm->GetMatrixType() == MatrixType::COMPLEX)
  {
    factor_args["is_complex"] = ObjectHolder(true);
  }
  else
  {
    factor_args["is_complex"] = ObjectHolder(false);
    factor_args["Az"] = CreateDoublePODArray(cm->GetAz());
  }

  Interpreter interpreter;
  bool ret = interpreter.RunCommand(command_handle_, factor_args);
  if (!ret)
  {
    std::string error = "while factorizing matrix using python solver\n";
    error += interpreter.GetErrorString();
    OutputStream::WriteOut(OutputStream::OutputType::INFO, error.c_str());
  }
  else
  {

  }

#warning "FINISH HERE WITH RETURN VALUE PROCESSING"

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
    std::string error = "while factorizing matrix using python solver\n";
    error += interpreter.GetErrorString();
    OutputStream::WriteOut(OutputStream::OutputType::INFO, error.c_str());
  }

#warning "FINISH HERE WITH RETURN VALUE PROCESSING"
}

template <typename DoubleType>
void ExternalPreconditioner<DoubleType>::DerivedLUSolve(ComplexDoubleVec_t<DoubleType> &x, const ComplexDoubleVec_t<DoubleType> &b) const
{
  dsAssert(command_handle_.IsCallable(), "python solver command is not callable\n");
  dsAssert(!command_data_.empty(), "python solver invalid data\n");

  std::vector<DoubleType> Xx(x.size());
  std::vector<DoubleType> Xz(x.size());
  std::vector<DoubleType> Bx(b.size());
  std::vector<DoubleType> Bz(b.size());

  auto realpart = [](auto &x){return x.real();};
  auto imagpart = [](auto &x){return x.imag();};
  std::transform(x.begin(), x.end(), Xx.begin(), realpart);
  std::transform(x.begin(), x.end(), Xz.begin(), imagpart);
  std::transform(b.begin(), b.end(), Bx.begin(), realpart);
  std::transform(b.begin(), b.end(), Bz.begin(), imagpart);

  const std::string return_keys[] = {
    "status",
    "message",
    "Xx",
    "Xz",
  };
  ObjectHolderMap_t factor_args = {
    {"action", ObjectHolder("solve")},
    {"solver_object", command_data_},
    {"is_complex", ObjectHolder(true)},
    {"Bx", CreateDoublePODArray(Bx)},
    {"Bz", CreateDoublePODArray(Bz)},
  };

  Interpreter interpreter;
  bool ret = interpreter.RunCommand(command_handle_, factor_args);
  if (!ret)
  {
    std::string error = "while factorizing matrix using python solver\n";
    error += interpreter.GetErrorString();
    OutputStream::WriteOut(OutputStream::OutputType::INFO, error.c_str());
  }

#warning "FINISH HERE WITH RETURN VALUE PROCESSING"
}
}


template class dsMath::ExternalPreconditioner<double>;
#ifdef DEVSIM_EXTENDED_PRECISION
#include "Float128.hh"
template class dsMath::ExternalPreconditioner<float128>;
#endif

