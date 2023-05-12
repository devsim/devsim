/***
DEVSIM
Copyright 2013 DEVSIM LLC

SPDX-License-Identifier: Apache-2.0
***/

#ifndef COMMAND_HANDLER_HH
#define COMMAND_HANDLER_HH

#include "ObjectHolder.hh"
#include <string>
#include <vector>
#include <map>

class ObjectHolder;
class CommandHandler;
namespace dsGetArgs
{
enum class optionType {BOOLEAN = 0, STRING, INTEGER, FLOAT, LIST};
enum class requiredType {OPTIONAL, REQUIRED};
namespace optionTypeString {
extern const char * optionTypeStrings[];
}


typedef bool (*checkFunction)(const std::string &/*optionname*/, std::string &/*errorString*/, const CommandHandler &);

struct Option {
    const char *name;
    //// always use "" instead of nullptr
    const char   *defaultValue;
    optionType    type;
    requiredType  rtype;
    checkFunction func;
};

typedef Option       * optionList;

}

class CommandHandler;
typedef void (*newcmd)(CommandHandler &);


class CommandHandler
{
  public:
    CommandHandler(void *command_info);
    ~CommandHandler();
    void SetErrorResult(const std::string &msg);
    void SetEmptyResult();
    const std::string &GetCommandName() const;

    void SetObjectResult(ObjectHolder);
    void SetStringListResult(const std::vector<std::string> &list);
    void SetIntListResult(const std::vector<int> &list);
    void SetDoubleListResult(const std::vector<double> &list);
    void SetStringResult(const std::string &);
    void SetDoubleResult(double v);
    void SetIntResult(int v);
    void SetListResult(ObjectHolderList_t &object_list);
    void SetMapResult(ObjectHolderMap_t &object_list);

    void *GetCommandInfo()
    {
      return command_info_;
    }

    int GetReturnCode() const
    {
      return return_code_;
    }

    const std::string &GetErrorString() const
    {
      return error_string_;
    }

    ObjectHolder GetReturnObject() const
    {
      return return_object_;
    }

    bool        processOptions(dsGetArgs::optionList opts, std::string &) const;
    std::string GetStringOption(const std::string &) const;
    double      GetDoubleOption(const std::string &) const;
    int         GetIntegerOption(const std::string &) const;
    bool        GetBooleanOption(const std::string &) const;
    ObjectHolder GetObjectHolder(const std::string &) const;


  private:

    void        *command_info_;
    std::string  command_name;
    std::string  error_string_;
    int          return_code_;
    ObjectHolder return_object_;
};

template <typename T, typename U> std::vector<T> GetKeys(const std::map<T, U> &tmap)
{
  std::vector<T> result;
  for (typename std::map<T, U>::const_iterator it = tmap.begin(); it != tmap.end(); ++it)
  {
    result.push_back(it->first);
  }
  return result;
}
#endif

