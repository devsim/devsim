/***
DEVSIM
Copyright 2013 Devsim LLC

This file is part of DEVSIM.

DEVSIM is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation, version 3.

DEVSIM is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with DEVSIM.  If not, see <http://www.gnu.org/licenses/>.
***/


namespace dsGetArgs {
const char * Types::optionTypeStrings[] = {"BOOLEAN", "STRING", "INTEGER", "FLOAT"};

GetArgs::GetArgs(optionList opts, switchList sl) : options(opts), switches(sl)
{
  Option *it = options;
  while ((*it).name != NULL)
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

void notConvertibleToType(const std::string &command, const std::string &arg, Types::optionType ot, const ObjectHolder &val, std::string &error)
{
  std::ostringstream os;
  os << "Cannot convert \"" << val.GetString() << "\" to a " << Types::optionTypeStrings[ot] <<  " for argument " << arg;

  error = os.str();
}
}
}

