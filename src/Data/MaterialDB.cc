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

#include "MaterialDB.hh"
#include "GlobalData.hh"
#include "ObjectHolder.hh"
#include "sqlite3.h"
#include "OutputStream.hh"
#include "dsAssert.hh"
#include <sstream>

MaterialDB *MaterialDB::instance = 0;

MaterialDB::MaterialDB() : sqlite_(nullptr)
{
}

//// Clear Out Entries and signal callbacks
void MaterialDB::ClearEntries()
{
  GlobalData &ginst = GlobalData::GetInstance();
  for (MaterialDataMap_t::const_iterator it = materialData.begin(); it != materialData.end(); ++it)
  {
    const std::string   &material_name  = (it->first);
    const ParameterDataMap_t &parameter_data = (it->second);
    for (ParameterDataMap_t::const_iterator jt = parameter_data.begin(); jt != parameter_data.end(); ++jt)
    {
      const std::string &parameter_name = (jt->first);
      ginst.SignalCallbacksOnMaterialChange(material_name, parameter_name);
    }
  }
  materialData.clear();
}

bool MaterialDB::OpenDB(const std::string &nm, OpenType_t ot, std::string &errorString)
{
  bool     ret = false;
  sqlite3 *db(nullptr);

  errorString.clear();

  if (sqlite_)
  {
    this->CloseDB();
  }

  ClearEntries();

  if (!nm.empty())
  {
    int s = SQLITE_OK;
    if (ot == OpenType_t::READONLY)
    {
      s = sqlite3_open_v2(nm.c_str(), &db, SQLITE_OPEN_READONLY, nullptr);
    }
    else if (ot == OpenType_t::READWRITE)
    {
      s = sqlite3_open_v2(nm.c_str(), &db, SQLITE_OPEN_READWRITE, nullptr);
    }
    else if (ot == OpenType_t::CREATE)
    {
      s = sqlite3_open_v2(nm.c_str(), &db, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, nullptr);
    }
    if (s == SQLITE_OK)
    {
      ret      = true;
      sqlite_  = db;
      db_name_ = nm;
    }
    else if (s == SQLITE_MISUSE)
    {
      dsAssert(0, "UNEXPECTED");
    }
    else
    {
      errorString = sqlite3_errmsg(db);
    }
  }
  return ret;
}

void MaterialDB::CloseDB()
{
  if (sqlite_)
  {
    sqlite3_close(sqlite_);
  }
  sqlite_ = nullptr;
  db_name_.clear();
}

MaterialDB &MaterialDB::GetInstance()
{
  if (!instance)
  {
    instance = new MaterialDB;
  }
  return *instance;
}

void MaterialDB::DestroyInstance()
{
  if (instance)
  {
    delete instance;
  }
  instance = 0;
}

MaterialDB::~MaterialDB()
{
  this->CloseDB();
}

MaterialDB::DBEntry_t MaterialDB::GetDBEntry(const std::string &material_name, const std::string &parameter_name)
{
  DBEntry_t ret = std::make_pair(false, MaterialDBEntry());

  MaterialDataMap_t::const_iterator it = materialData.find(material_name);
  if (it != materialData.end())
  {
    const ParameterDataMap_t &pdm = it->second;
    ParameterDataMap_t::const_iterator jt = pdm.find(parameter_name);
    if (jt != pdm.end())
    {
      ret.first  = true;
      ret.second = jt->second;
    }
  }

  if (!ret.first && sqlite_)
  {
    //// initialize as Undefined
//    materialData[material_name][parameter_name];

    const std::string query = "SELECT value, unit, description FROM materialdata WHERE material = $m and parameter = $p;";
    sqlite3_stmt *pstmt = nullptr;
    int err = sqlite3_prepare_v2(sqlite_, query.c_str(), query.size() + 1, &pstmt, nullptr);

    if (err != SQLITE_OK)
    {
      goto fail;
    }

    sqlite3_bind_text(pstmt, 1, material_name.c_str(),  material_name.size() + 1,  SQLITE_TRANSIENT);
    sqlite3_bind_text(pstmt, 2, parameter_name.c_str(), parameter_name.size() + 1, SQLITE_TRANSIENT);

    err = sqlite3_step(pstmt);
    //// Get count here!!!
    if ((err == SQLITE_ROW) && (sqlite3_column_count(pstmt) == 3))
    {
      const std::string v = reinterpret_cast<const char *>(sqlite3_column_text(pstmt, 0));
      const std::string u = reinterpret_cast<const char *>(sqlite3_column_text(pstmt, 1));
      const std::string d = reinterpret_cast<const char *>(sqlite3_column_text(pstmt, 2));

      materialData[material_name][parameter_name] = MaterialDBEntry(u, d, ObjectHolder(v), MaterialDBEntry::EntryType_t::FROMDB);
      ret = std::make_pair(true, materialData[material_name][parameter_name]);
    }

//    sqlite3_reset(pstmt);
#if 0
    err = sqlite3_step(pstmt);
    if (err != SQLITE_DONE)
    {
      std::ostringstream os; 
      os << "Material \"" << material_name
         << "\" database entry \"" << parameter_name
         << "\" has more than one row, using the first.\n";
      OutputStream::WriteOut(OutputStream::OutputType::INFO, os.str());
    }
#endif

    if (pstmt)
    {
      sqlite3_finalize(pstmt);
    }

    fail:;
  }

  return ret;
}

MaterialDB::DoubleDBEntry_t MaterialDB::GetDoubleDBEntry(const std::string &material_name, const std::string &parameter_name)
{
  DoubleDBEntry_t ret = std::make_pair(false, 0.0);

  DBEntry_t       dbent = GetDBEntry(material_name, parameter_name);

  if (dbent.first)
  {
    ObjectHolder::DoubleEntry_t dval = dbent.second.GetValue().GetDouble();
    if (dval.first)
    {
      ret = dval;
    }
    else
    {
      std::ostringstream os; 
      os << "Material \"" << material_name
         << "\" database entry \"" << parameter_name
         << "\" resolves to a string \"" << dbent.second.GetValue().GetString() <<  "\" when a number was expected\n";
      OutputStream::WriteOut(OutputStream::OutputType::INFO, os.str());
    }
  }
  return ret;
}

void MaterialDB::AddDBEntry(const std::string &material_name, const std::string &parameter_name, const MaterialDBEntry &dbentry)
{
  //// if this has never been a material parameter, then we don't need to signal a callback on it
  if (materialData.count(material_name) && materialData[material_name].count(parameter_name))
  {
    materialData[material_name][parameter_name] = dbentry;
    GlobalData &ginst = GlobalData::GetInstance();
    ginst.SignalCallbacksOnMaterialChange(material_name, parameter_name);
  }
  else
  {
    materialData[material_name][parameter_name] = dbentry;
  }
}

bool MaterialDB::SaveDB(std::string &errorString)
{
  bool ret = false;

  errorString.clear();

  const std::string &queryu = "UPDATE materialdata SET value = $v, unit = $u, description = $d WHERE material = $m AND parameter = $p;";
  const std::string &queryi = "INSERT INTO materialdata VALUES($m, $p, $v, $u, $d);";

  sqlite3_stmt *pstmtq = nullptr;
  sqlite3_stmt *pstmti = nullptr;

  int err = sqlite3_prepare_v2(sqlite_, queryu.c_str(), queryu.size() + 1, &pstmtq, nullptr);

  if (err != SQLITE_OK)
  {
    errorString += sqlite3_errmsg(sqlite_);
    goto fail;
  }

  err = sqlite3_prepare_v2(sqlite_, queryi.c_str(), queryi.size() + 1, &pstmti, nullptr);

  if (err != SQLITE_OK)
  {
    errorString += sqlite3_errmsg(sqlite_);
    goto fail;
  }

  for (MaterialDataMap_t::const_iterator it = materialData.begin(); it != materialData.end(); ++it)
  {
    const std::string   &material_name  = (it->first);
    const ParameterDataMap_t &parameter_data = (it->second);


    for (ParameterDataMap_t::const_iterator jt = parameter_data.begin(); jt != parameter_data.end(); ++jt)
    {
      const std::string &parameter_name = (jt->first);
      const MaterialDBEntry &mdb = jt->second;

      if ((mdb.GetEntryType() == MaterialDBEntry::EntryType_t::FROMDB)
          || (mdb.GetEntryType() == MaterialDBEntry::EntryType_t::UNDEFINED)
          )
      {
        continue;
      }

      sqlite3_bind_text(pstmtq, 1, mdb.GetValue().GetString().c_str(), -1, SQLITE_TRANSIENT);
      sqlite3_bind_text(pstmtq, 2, mdb.GetUnit().c_str(), -1, SQLITE_TRANSIENT);
      sqlite3_bind_text(pstmtq, 3, mdb.GetDescription().c_str(), -1, SQLITE_TRANSIENT);
      sqlite3_bind_text(pstmtq, 4, material_name.c_str(),  material_name.size() + 1,  SQLITE_TRANSIENT);
      sqlite3_bind_text(pstmtq, 5, parameter_name.c_str(), parameter_name.size() + 1, SQLITE_TRANSIENT);

      err = sqlite3_step(pstmtq);
      if (err != SQLITE_DONE)
      {
        errorString += sqlite3_errmsg(sqlite_);
        goto fail;
      }

      int changes = sqlite3_changes(sqlite_);
      //// TODO: error out here if more than 1 row changed
      if (changes > 0)
      {
        MaterialDBEntry m = mdb;
        m.SetType(MaterialDBEntry::EntryType_t::FROMDB);
        materialData[material_name][parameter_name] = m;
      }
      else if (changes == 0)
      {
/*
        int m = sqlite3_bind_parameter_index(pstmti, "$m");
        int p = sqlite3_bind_parameter_index(pstmti, "$p");
        int d = sqlite3_bind_parameter_index(pstmti, "$d");
        int u = sqlite3_bind_parameter_index(pstmti, "$u");
        int v = sqlite3_bind_parameter_index(pstmti, "$v");
*/
        sqlite3_bind_text(pstmti, 1, material_name.c_str(),  material_name.size() + 1,  SQLITE_TRANSIENT);
        sqlite3_bind_text(pstmti, 2, parameter_name.c_str(), parameter_name.size() + 1, SQLITE_TRANSIENT);
        //// TODO: is value guaranteed
        sqlite3_bind_text(pstmti, 3, mdb.GetValue().GetString().c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(pstmti, 4, mdb.GetUnit().c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(pstmti, 5, mdb.GetDescription().c_str(), -1, SQLITE_TRANSIENT);

        err = sqlite3_step(pstmti);
        if (err == SQLITE_DONE)
        {
          MaterialDBEntry m = mdb;
          m.SetType(MaterialDBEntry::EntryType_t::FROMDB);
          materialData[material_name][parameter_name] = m;
        }
        else
        {
          errorString += sqlite3_errmsg(sqlite_);
          goto fail;
        }
      }

      sqlite3_reset(pstmtq);
      sqlite3_reset(pstmti);
    }
  }

  fail:
    if (pstmtq)
    {
      sqlite3_finalize(pstmtq);
    }
    if (pstmti)
    {
      sqlite3_finalize(pstmti);
    }

  ret = errorString.empty();
  return ret;
}

bool MaterialDB::CreateDB(const std::string &filename, std::string &errorString)
{
  bool ret = false;
  errorString.clear();

  ret = OpenDB(filename, OpenType_t::CREATE, errorString);
  if (!ret)
  {
    return ret;
  }

  char *errmsg = nullptr;

  int err = sqlite3_exec(sqlite_, "DROP TABLE IF EXISTS materialdata; "
                        "CREATE TABLE materialdata (material text, parameter text, value text, unit text, description text);",
               nullptr, nullptr, &errmsg);
  if (err != SQLITE_OK)
  {
    errorString += errmsg;
    ret = false;
  }
  else
  {
    ret = true;
  }
  sqlite3_free(errmsg);

  return ret;
}

const MaterialDB::ParameterDataMap_t &MaterialDB::GetOpenEntries(const std::string &material_name) const
{
  MaterialDataMap_t::const_iterator it = materialData.find(material_name);

  const ParameterDataMap_t *pdm = &emptyData;

  if (it != materialData.end())
  {
    pdm = &(it->second);
  }

  return *pdm;
}


