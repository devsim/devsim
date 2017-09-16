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

#ifndef MATERIALDB_HH
#define MATERIALDB_HH
#include <string>
#include <map>

class Region;
typedef Region *RegionPtr;
class Device;
typedef Device *DevicePtr;

extern "C" {
struct Tcl_Interp;
}

#include "MaterialDBEntry.hh"

extern "C" {
typedef struct sqlite3 sqlite3;
typedef struct sqlite3_stmt sqlite3_stmt;
}


class MaterialDB;
typedef std::pair<bool, MaterialDBEntry> MaterialDBEntry_t;

// This class is supposed to contain everything
class MaterialDB
{
    public:
    ///// material, parameter name, db entry
      typedef std::map<std::string, MaterialDBEntry>     ParameterDataMap_t;
      typedef std::map<std::string, ParameterDataMap_t > MaterialDataMap_t;

      typedef std::pair<bool, double>       DoubleDBEntry_t;
      typedef std::pair<bool, MaterialDBEntry> DBEntry_t;

      static MaterialDB &GetInstance();
      static void DestroyInstance();

      enum class OpenType_t {READONLY, READWRITE, CREATE};
      bool OpenDB(const std::string &/*filename*/, OpenType_t /*flags*/, std::string &/*errorString*/);

      void AddDBEntry(const std::string &/*material_name*/, const std::string &/*parameter_name*/, const MaterialDBEntry &/*dbentry*/);

      DoubleDBEntry_t GetDoubleDBEntry(const std::string &/*material_name*/, const std::string &/*parameter_name*/);
      DBEntry_t       GetDBEntry(const std::string &/*material_name*/, const std::string &/*parameter_name*/);

      bool SaveDB(std::string &/*errorString*/);

      bool CreateDB(const std::string &/*filename*/, std::string &/*errorString*/);

      void CloseDB();

      const ParameterDataMap_t &GetOpenEntries(const std::string &/*MaterialName*/) const;

    private:
      void ClearEntries();

      MaterialDB();
      MaterialDB(MaterialDB &);
      MaterialDB &operator=(MaterialDB &);
      ~MaterialDB();
      static MaterialDB *instance;

      MaterialDataMap_t  materialData;
      ParameterDataMap_t emptyData;
      std::string       db_name_;
      sqlite3          *sqlite_;
};
#endif

