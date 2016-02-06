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

template <typename T> class ScalarData;
class InterfaceNodeModel;
typedef ScalarData<InterfaceNodeModel> InterfaceNodeScalarData;

namespace IMEE {
class InterfaceModelExprData {
    public:
        /// Start with Node data.  Maybe someday allow flux integration
        enum datatype {NODEDATA=0, DOUBLE, INVALID};
        static const char *const datatypename[];

        InterfaceModelExprData(const InterfaceModelExprData &);
        InterfaceModelExprData(double x) : val(x), type(DOUBLE) {};
        InterfaceModelExprData(const InterfaceNodeScalarData &);

        InterfaceModelExprData() : val(0.0), type(INVALID) {};

        InterfaceModelExprData &operator=(const InterfaceModelExprData &);

        template <typename T>
        InterfaceModelExprData &op_equal(const InterfaceModelExprData &, const T &);

        InterfaceModelExprData &operator*=(const InterfaceModelExprData &);
        InterfaceModelExprData &operator+=(const InterfaceModelExprData &);

        // test or will get undefined data
        double GetDoubleValue() const
        {
            return val;
        }
        datatype GetType() const {
            return type;
        }

        class ScalarValuesType {
          public:
          ScalarValuesType() : vals(0), uval(0.0), isuniform(false), length(0) {}
          ScalarValuesType(const std::vector<double> &v) : vals(&v), uval(0.0), isuniform(false) {length = v.size();}

          ScalarValuesType(double v, size_t len) : vals(0), uval(v), isuniform(true), length(len) {}

          bool IsUniform() const
          {
            return isuniform;
          }

          const std::vector<double> &GetVector() const {
            return *vals;
          }

          double GetScalar() const {
            return uval;
          }

          size_t GetLength() const {
            return length;
          }

          private:
            const std::vector<double> *vals;
            double                     uval;
            bool isuniform;
            size_t                     length;
        };

        ScalarValuesType GetScalarValues() const;

        void makeUnique();

    private:
        friend class InterfaceModelExprEval;

        typedef std::shared_ptr<InterfaceNodeScalarData> nsd_ptr;
        // These are the types of data we can keep
        std::shared_ptr<InterfaceNodeScalarData> nsd;
        // This is a reference to existing data
        double             val;
        datatype           type;
};
}
#endif
