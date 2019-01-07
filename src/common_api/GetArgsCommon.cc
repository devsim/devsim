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


namespace dsGetArgs {
const char * optionTypeStrings[] = {"BOOLEAN", "STRING", "INTEGER", "FLOAT", "LIST"};

GetArgs::GetArgs(optionList opts, switchList sl) : options(opts), switches(sl)
{
  Option *it = options;
  while ((*it).name != nullptr)
  {
    optionMap[(*it).name] = it;
    ++it;
  }
} 

ObjectHolder GetArgs::GetObjectHolder(const std::string &s) const
{
  ObjectHolder val;
  ObjectHolderMap_t::const_iterator it = selections.find(s);
  if (it != selections.end())
  {
    val = (it->second);
  }
  else
  {
    OptionMap_t::const_iterator vit = optionMap.find(s);
    dsAssert(vit != optionMap.end(), "UNEXPECTED");
    const std::string &sval = (vit->second)->defaultValue;
    val = ObjectHolder(sval);
  }
  return val;
}

std::string GetArgs::GetStringOption(const std::string &s) const
{
  const ObjectHolder &tobj = GetObjectHolder(s);
  return tobj.GetString();
}

double GetArgs::GetDoubleOption(const std::string &s) const
{
  double val = 0.0;

  const ObjectHolder &tobj = GetObjectHolder(s);

  ObjectHolder::DoubleEntry_t ret = tobj.GetDouble();

  if (ret.first)
  {
    val = ret.second;
  }
  else
  {
    const std::string &sval = tobj.GetString();

    if (sval == "MAXDOUBLE")
    {
      val = std::numeric_limits<double>::max();
    }
    else if (sval == "-MAXDOUBLE")
    {
      val = -std::numeric_limits<double>::max();
    }
  }
  return val;
}

bool GetArgs::GetBooleanOption(const std::string &s) const
{
  bool val = false;

  const ObjectHolder &tobj = GetObjectHolder(s);

  ObjectHolder::BooleanEntry_t ret = tobj.GetBoolean();

  if (ret.first)
  {
    val = ret.second;
  }

  return val;
}

int GetArgs::GetIntegerOption(const std::string &s) const
{
  int val = 0;

  const ObjectHolder &tobj = GetObjectHolder(s);

  ObjectHolder::IntegerEntry_t ret = tobj.GetInteger();

  if (ret.first)
  {
    val = ret.second;
  }

  return val;
}

namespace {
void alreadySpecified(const std::string &command, const std::string &arg, const ObjectHolder &val, std::string &error)
{
  std::ostringstream os;
  os << command << " already passed option " << arg;
  os << " with value " << val.GetString() << " as an option" << "\n";
  error = os.str();
}
#if 0
void alreadySpecified(const std::string &command, const std::string &arg, const double val, std::string &error)
{
  std::ostringstream os;
  os << command << " already passed option " << arg << " with value " << val << " as an option" << "\n";
  error = os.str();
}
#endif

void notSpecified(const std::string &command, const std::string &arg, std::string &error)
{
  std::ostringstream os;
  os << command << " does not specify a value for option " << arg << "\n";
  error = os.str();
}

void notConvertibleToType(const std::string &command, const std::string &arg, optionType ot, const ObjectHolder &val, std::string &error)
{
  std::ostringstream os;
  os << "Cannot convert \"" << val.GetString() << "\" to a " << optionTypeStrings[static_cast<size_t>(ot)] <<  " for argument " << arg;

  error = os.str();
}
}
}

