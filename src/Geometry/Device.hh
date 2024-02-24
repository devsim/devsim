/***
DEVSIM
Copyright 2013 DEVSIM LLC

SPDX-License-Identifier: Apache-2.0
***/

#ifndef DEVICE_HH
#define DEVICE_HH
#include "MathEnum.hh"
#ifdef DEVSIM_EXTENDED_PRECISION
#include "Float128.hh"
#endif
#include "dsMathTypes.hh"

#include <cstddef>
#include <string>
#include <vector>
#include <map>
#include <complex>


class PermutationEntry;
typedef std::map<size_t, PermutationEntry> PermutationMap;

namespace dsMath {
template <typename T> class RowColVal;

template <typename DoubleType>
using RealRowColVal = RowColVal<DoubleType>;

template <typename DoubleType>
using RealRowColValueVec = std::vector<RealRowColVal<DoubleType>>;

template <typename DoubleType>
using RHSEntry = std::pair<int, DoubleType>;

template <typename DoubleType>
using RHSEntryVec = std::vector<RHSEntry<DoubleType>>;
}

/**
    A device maintains all of the regions and boundary regions within it
*/
class Contact;
typedef Contact *ContactPtr;
typedef const Contact *ConstContactPtr;

class Interface;
typedef Interface *InterfacePtr;
typedef const Interface *ConstInterfacePtr;

class Coordinate;
typedef Coordinate *CoordinatePtr;

class Region;
typedef Region *RegionPtr;

class Device
{
   public:
      ~Device();

      typedef std::vector<CoordinatePtr> CoordinateList_t;
      typedef std::map<std::string, RegionPtr> RegionList_t;
      typedef std::map<std::string, ContactPtr> ContactList_t;
      typedef std::map<std::string, InterfacePtr> InterfaceList_t;

      /// Coordinate index to an interface pointer
      typedef std::map<size_t, std::vector<InterfacePtr> > CoordinateIndexToInterface_t;
      typedef std::map<size_t, std::vector<ContactPtr> > CoordinateIndexToContact_t;

      Device(std::string, size_t);

      /// If we ever add a delete method.  Interfaces would need to be removed automatically when their underlying regions are removed.
      void AddRegion(const RegionPtr &);

      template <typename DoubleType>
      void Update(const dsMath::DoubleVec_t<DoubleType> &/*result*/);
      template <typename DoubleType>
      void ACUpdate(const std::vector<dsMath::ComplexDouble_t<DoubleType>> &/*result*/);
      template <typename DoubleType>
      void NoiseUpdate(const std::string &/*output*/, const std::vector<PermutationEntry> &/*permvec*/, const std::vector<dsMath::ComplexDouble_t<DoubleType>> &/*result*/);

      void UpdateContacts();
      // Need to be careful with accessors and stuff
      // maintaining constness of contact
      void AddContact(const ContactPtr &);

      void AddInterface(const InterfacePtr &);

      void AddCoordinateList(const CoordinateList_t &);
      void AddCoordinate(CoordinatePtr);

      const std::string &GetName() const
      {
          return deviceName;
      }

      void SetBaseEquationNumber(size_t);
      size_t GetBaseEquationNumber();
      size_t CalcMaxEquationNumber(bool);

    template <typename DoubleType>
    void ContactAssemble(dsMath::RealRowColValueVec<DoubleType> &, dsMath::RHSEntryVec<DoubleType> &, PermutationMap &, dsMathEnum::WhatToLoad, dsMathEnum::TimeMode);

    template <typename DoubleType>
    void InterfaceAssemble(dsMath::RealRowColValueVec<DoubleType> &, dsMath::RHSEntryVec<DoubleType> &, PermutationMap &, dsMathEnum::WhatToLoad, dsMathEnum::TimeMode);

    template <typename DoubleType>
    void RegionAssemble(dsMath::RealRowColValueVec<DoubleType> &, dsMath::RHSEntryVec<DoubleType> &, dsMathEnum::WhatToLoad, dsMathEnum::TimeMode);

    template <typename DoubleType>
    DoubleType GetAbsError() const
    {
        return static_cast<DoubleType>(absError);
    }

    template <typename DoubleType>
    DoubleType GetRelError() const
    {
        return static_cast<DoubleType>(relError);
    }

    const CoordinateList_t &GetCoordinateList() const
    {
        return coordinateList;
    }

    const RegionList_t &GetRegionList() const
    {
        return regionList;
    }

    const InterfaceList_t &GetInterfaceList() const
    {
        return interfaceList;
    }

    const ContactList_t &GetContactList() const
    {
        return contactList;
    }

    RegionPtr GetRegion(const std::string &);

    InterfacePtr GetInterface(const std::string &) const ;

    ContactPtr GetContact(const std::string &) const;

    size_t GetNumberOfInterfacesOnCoordinate(const Coordinate &);
    size_t GetNumberOfContactsOnCoordinate(const Coordinate &);

    //// We guarantee that this list is sorted by pointer address to the interface
    const CoordinateIndexToInterface_t &GetCoordinateIndexToInterface() const
    {
        return coordinateIndexToInterface;
    }

    //// We guarantee that this list is sorted by pointer address to the contact
    const CoordinateIndexToContact_t   &GetCoordinateIndexToContact() const
    {
        return coordinateIndexToContact;
    }

    void BackupSolutions(const std::string &);
    void RestoreSolutions(const std::string &);

    size_t GetDimension() const
    {
      return dimension;
    }

    void SignalCallbacksOnInterface(const std::string &/*str*/, const Region *) const;

   private:
      Device();
      Device (const Device &);
      Device &operator= (const Device &);

      std::string deviceName;
      size_t dimension;

      RegionList_t regionList;

      ContactList_t contactList;
      InterfaceList_t interfaceList;

      CoordinateList_t coordinateList;

      CoordinateIndexToInterface_t coordinateIndexToInterface;
      CoordinateIndexToContact_t   coordinateIndexToContact;

      size_t baseeqnnum; // base equation number for this region

#ifdef DEVSIM_EXTENDED_PRECISION
      float128 relError;
      float128 absError;
#else
      double relError;
      double absError;
#endif
};

#endif
