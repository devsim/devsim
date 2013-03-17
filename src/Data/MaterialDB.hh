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

      enum OpenType_t {READONLY, READWRITE, CREATE};
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

