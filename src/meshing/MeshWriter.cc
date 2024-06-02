/***
DEVSIM
Copyright 2013 DEVSIM LLC

SPDX-License-Identifier: Apache-2.0
***/

#include "MeshWriter.hh"
#include "Interpreter.hh"

MeshWriter::~MeshWriter() {}

class MeshWriterTestFunctor {

    public:
        MeshWriterTestFunctor(ObjectHolder oh) : object_holder_(oh), call_function_(false)
        {
        }

        bool initialize(std::string &errorString)
        {
            bool ret = false;
            if (object_holder_.IsCallable())
            {
                Interpreter interp;
                std::vector<ObjectHolder> args{ObjectHolder("")};
                ret = interp.RunCommand(object_holder_, args);
                if (!ret)
                {
                    errorString += "\"include_test\" is not a callable one argument function\n";
                    errorString += interp.GetErrorString();
                }
                else if (auto x = interp.GetResult().GetBoolean(); x.first == false)
                {
                    ret = false;
                    errorString += "\"include_test\" does not return boolean\n";
                }
                else
                {
                    ret = true;
                    call_function_ = true;
                }
            }
            else if (object_holder_.GetString().empty())
            {
                ret = true;
            }
            else
            {
                errorString += "\"include_test\" is not a callable function\n";
                ret = false;
            }
            return ret;
        }

        bool run_function(const std::string &arg)
        {
            Interpreter interp;
            std::vector<ObjectHolder> args{ObjectHolder(arg)};
            bool ok = interp.RunCommand(object_holder_, args);
            bool ret = false;
            if (ok)
            {
                auto x = interp.GetResult().GetBoolean();
                ret = x.first && x.second;
            }
            return ret;
        }

        bool operator()(const std::string &arg)
        {
            bool ret = true;

            if (call_function_)
            {
                ret = run_function(arg);
            }

            return ret;
        }

    private:
        ObjectHolder object_holder_;
        bool         call_function_;
};

bool MeshWriter::WriteMesh(const std::string &deviceName, const std::string &filename, ObjectHolder test, std::string &errorString)
{
    MeshWriterTestFunctor test_functor(test);
    bool ret = test_functor.initialize(errorString);
    if (ret)
    {
        ret = this->WriteMesh_(deviceName, filename, test_functor, errorString);
    }
    return ret;
}

bool MeshWriter::WriteMeshes(const std::string &filename, ObjectHolder test, std::string &errorString)
{
    MeshWriterTestFunctor test_functor(test);
    bool ret = test_functor.initialize(errorString);
    if (ret)
    {
        ret = this->WriteMeshes_(filename, test_functor, errorString);
    }
    return ret;
}

