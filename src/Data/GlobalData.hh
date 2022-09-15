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

#ifndef GLOBALDATA_HH
#define GLOBALDATA_HH
#include <map>
#include <string>
#include <vector>

class Region;
typedef Region *RegionPtr;
class Device;
typedef Device *DevicePtr;

class ObjectHolder;

// This class is supposed to contain everything
class GlobalData {
public:
  typedef std::map<std::string, ObjectHolder> GlobalDataMap_t;
  typedef std::map<std::string, GlobalDataMap_t> DeviceDataMap_t;
  typedef std::map<std::string, DeviceDataMap_t> RegionDataMap_t;

  typedef std::pair<bool, ObjectHolder> DBEntry_t;
  typedef std::pair<bool, double> DoubleDBEntry_t;

  //    typedef std::pair<bool, double> doubleDBEntry;

  typedef std::map<std::string, DevicePtr> DeviceList_t;
  static GlobalData &GetInstance();
  static void DestroyInstance();

  void AddDevice(DevicePtr);

  const DeviceList_t &GetDeviceList() { return deviceList; }

  DevicePtr GetDevice(const std::string &);

  //// Need callback system when material parameters change
  void AddDBEntryOnDevice(const std::string & /*device*/,
                          const std::string & /*name*/, ObjectHolder /*value*/);
  void AddDBEntryOnRegion(const std::string & /*device*/,
                          const std::string & /*region*/,
                          const std::string & /*name*/, ObjectHolder /*value*/);
  void AddDBEntryOnRegion(const Region *, const std::string & /*name*/,
                          ObjectHolder /*value*/);
  void AddDBEntryOnGlobal(const std::string & /*name*/, ObjectHolder /*value*/);

  GlobalData::DBEntry_t GetDBEntryOnDevice(const std::string & /*device*/,
                                           const std::string & /*name*/) const;
  GlobalData::DBEntry_t GetDBEntryOnRegion(const std::string & /*device*/,
                                           const std::string & /*region*/,
                                           const std::string & /*name*/) const;
  GlobalData::DBEntry_t GetDBEntryOnRegion(const Region * /*rp*/,
                                           const std::string & /*name*/) const;
  GlobalData::DBEntry_t GetDBEntryOnGlobal(const std::string & /*name*/) const;
  GlobalData::DoubleDBEntry_t
  GetDoubleDBEntryOnRegion(const Region * /*rp*/,
                           const std::string & /*name*/) const;

  //// Only retrieves the name of the parameters
  //// The list doesn't search recursively up the hierarchy
  std::vector<std::string> GetDBEntryListOnGlobal() const;
  std::vector<std::string>
  GetDBEntryListOnDevice(const std::string & /*device*/) const;
  std::vector<std::string>
  GetDBEntryListOnRegion(const std::string & /*device*/,
                         const std::string & /*region*/) const;

  //// TODO: Need to be able to signal everything everywhere sometime (sanity
  /// check) / SignalCallbacksEverything();
  void SignalCallbacksOnGlobal(const std::string &);
  void SignalCallbacksOnDevice(const std::string &, const std::string &);
  void SignalCallbacksOnRegion(const std::string &, const std::string &,
                               const std::string &);

  void SignalCallbacksOnMaterialChange(const std::string & /*material_name*/,
                                       const std::string & /*parameter_name*/);

  void SetInterpreter(void *);
  void *GetInterpreter();

  bool AddTclEquation(const std::string & /*name*/, ObjectHolder /*procedure*/,
                      std::string & /*error*/);

  typedef std::map<std::string, ObjectHolder> TclEquationList_t;

  const TclEquationList_t &GetTclEquationList();

private:
  void InitializeParameters();

  GlobalData();
  GlobalData(GlobalData &);
  GlobalData &operator=(GlobalData &);
  ~GlobalData();
  static GlobalData *instance;

  DeviceList_t deviceList;

  RegionDataMap_t regionData;
  DeviceDataMap_t deviceData;
  GlobalDataMap_t globalData;

  TclEquationList_t tclEquationList;

  void *tclInterp;
};
#endif
