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

#ifndef INTERFACE_MODEL_EXPR_DATA_HH
#define INTERFACE_MODEL_EXPR_DATA_HH

#include <memory>
#include <vector>

template <typename T, typename U> class ScalarData;

class InterfaceNodeModel;

template<typename T>
using InterfaceNodeScalarData = ScalarData<InterfaceNodeModel, T>;

namespace IMEE {

enum datatype {NODEDATA=0, DOUBLE, INVALID};
extern const char *const datatypename[];

template <typename DoubleType>
class ScalarValuesType {
  public:
  ScalarValuesType() : vals(0), uval(0.0), isuniform(false), length(0) {}
  ScalarValuesType(const std::vector<DoubleType> &v) : vals(&v), uval(0.0), isuniform(false) {length = v.size();}

  ScalarValuesType(DoubleType v, size_t len) : vals(0), uval(v), isuniform(true), length(len) {}

  bool IsUniform() const
  {
    return isuniform;
  }

  const std::vector<DoubleType> &GetVector() const {
    return *vals;
  }

  DoubleType GetScalar() const {
    return uval;
  }

  size_t GetLength() const {
    return length;
  }

  private:
    const std::vector<DoubleType> *vals;
    DoubleType                     uval;
    bool isuniform;
    size_t                     length;
};

template <typename DoubleType>
class InterfaceModelExprData {
    public:

        InterfaceModelExprData(const InterfaceModelExprData &);
        InterfaceModelExprData(DoubleType x) : val(x), type(DOUBLE) {};
        InterfaceModelExprData(const InterfaceNodeScalarData<DoubleType> &);

        InterfaceModelExprData() : val(0.0), type(INVALID) {};

        InterfaceModelExprData &operator=(const InterfaceModelExprData &);

        template <typename T>
        InterfaceModelExprData &op_equal(const InterfaceModelExprData &, const T &);

        InterfaceModelExprData &operator*=(const InterfaceModelExprData &);
        InterfaceModelExprData &operator+=(const InterfaceModelExprData &);

        // test or will get undefined data
        DoubleType GetDoubleValue() const
        {
            return val;
        }
        datatype GetType() const {
            return type;
        }


        ScalarValuesType<DoubleType> GetScalarValues() const;

        void makeUnique();

    private:
        typedef std::shared_ptr<InterfaceNodeScalarData<DoubleType> > nsd_ptr;
        // These are the types of data we can keep
        std::shared_ptr<InterfaceNodeScalarData<DoubleType> > nsd;
        // This is a reference to existing data
        DoubleType         val;
        datatype           type;
};
}
#endif
