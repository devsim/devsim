/***
DEVSIM
Copyright 2013 DEVSIM LLC

SPDX-License-Identifier: Apache-2.0
***/


namespace dsGetArgs {
const char * optionTypeStrings[] = {"BOOLEAN", "STRING", "INTEGER", "FLOAT", "LIST"};

GetArgs::GetArgs(optionList opts) : options(opts)
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
    if (vit == optionMap.end())
    {
      dsAssert(vit != optionMap.end(), "UNEXPECTED");
    }
    else
    {
      const std::string &sval = (vit->second)->defaultValue;
      val = ObjectHolder(sval);
    }
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

void notConvertibleToType(const std::string &command, const std::string &arg, optionType ot, const ObjectHolder &val, std::string &error)
{
  std::ostringstream os;
  os << "Cannot convert \"" << val.GetString() << "\" to a " << optionTypeStrings[static_cast<size_t>(ot)] <<  " for argument " << arg;

  error = os.str();
}
}
}

