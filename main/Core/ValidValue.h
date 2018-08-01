/*
    This software is Copyright by the Board of Trustees of Michigan
    State University (c) Copyright 2005.

    You may use this software under the terms of the GNU public license
    (GPL).  The terms of this license are described at:

     http://www.gnu.org/licenses/gpl.txt

     Author:
             Ron Fox
	     NSCL
	     Michigan State University
	     East Lansing, MI 48824-1321
*/

//
// ValidValue.h:
//    Provides a class which contains a value and an indicator that the value
//    is valid (modified).
//    In SpecTcl, this is used to represent a parameter value in the CEvent
//    array.
//
#ifndef VALIDVALUE_H
#define VALIDVALUE_H

#include <histotypes.h>
#include <string>

template <class T>
class CValidValue {
private:
  T         m_Value;
  ULong_t   m_nSerial;
  ULong_t*  m_pSerial;		/* Allows for shared validity generation */
  ULong_t   m_nLastSerial;      /* Serial number of last set.            */

public:
  // Constructors and other cannonicals:
  //
  CValidValue() : 
    m_nSerial(1),
    m_pSerial(&m_nSerial),
    m_nLastSerial(0)
  {
  }
  CValidValue(T& initValue, Bool_t isValid = kfFALSE) : 
    m_Value(initValue),
    m_nSerial(1),
    m_pSerial(&m_nSerial),
    m_nLastSerial(isValid ? 0 : 1)
  {
  }

  CValidValue(ULong_t* sharedSerial) :
    m_pSerial(sharedSerial),
    m_nLastSerial(*sharedSerial - 1)
  {
  }

  CValidValue(const CValidValue& rRHS) : 
    m_Value(rRHS.m_Value),
    m_nSerial(rRHS.m_nSerial),
    m_pSerial(rRHS.m_pSerial),
    m_nLastSerial(rRHS.m_nLastSerial)
  {
  }
  CValidValue& operator=(const CValidValue& rRHS) {
    if(this != &rRHS) {
      m_Value        = rRHS.getValue();
    }
    m_nLastSerial    = *m_pSerial;
    return *this;
  }
  int operator==(const CValidValue& rRhs) const {
    if(!isValid() || !rRhs.isValid()) 
      return 0;			// Both must be valid.
    return (m_Value == rRhs.m_Value); // and equal.
  }
  int operator!=(const CValidValue& rRhs) const {
    return !operator==(rRhs);
  }
  //
  //  Selectors:
  //
public:
  T getValue() const {
    if(!isValid()) {
      throw std::string("Attempted getValue of unset ValidValue object");
    }
    return m_Value;
  }
  Bool_t isValid() const {
    return m_nLastSerial == *m_pSerial;
  }
  //
  // Mutators:
  //
protected:
  void setValue(const T& rNew) {
    m_Value = rNew;
  }
  void setValid(const Bool_t Valid) {
    m_nLastSerial = Valid ? *m_pSerial : *m_pSerial - 1;
  }
  // Object operations.
  //
public:
  void clear() {
    m_nLastSerial = *m_pSerial - 1;
  }
  CValidValue& operator=(const T& rRhs) {
    m_Value = rRhs;
    m_nLastSerial = *m_pSerial;
    return *this;
  }
  operator T&() { 
    if(!isValid()) {
      throw std::string("Attempted getValue of unset ValidValue object");
    }
    return m_Value; 
  }
};
/*!
   Class that implements a valid value with an associated index and dope
   vector.  This allows one to maintain a dope vector of
   the valid values in a vector that have been assigned a value.
*/
#ifndef __DOPEVECTOR_H
#include "DopeVector.h"
#endif
template <class T>
class DopedValidValue : public CValidValue<T>
{
  // data:
private:
  DopeVector&  m_dope;		// Dope vector to fill in.
  unsigned int m_index;		// My index to set in the dope vector.
  
  // Canonicals
public:
  DopedValidValue(DopeVector& dope, unsigned int index) :
    CValidValue<T>(),
    m_dope(dope),
    m_index(index) {}

  DopedValidValue(DopeVector& dope, unsigned int index,
		  T& initValue, Bool_t isValid = kfFALSE) :
    CValidValue<T>(initValue, isValid),
    m_dope(dope),
    m_index(index) {}

  DopedValidValue(DopeVector& dope, unsigned int index, 
		  ULong_t* sharedSerial) :
    CValidValue<T>(sharedSerial),
    m_dope(dope),
    m_index(index) {}

  DopedValidValue(const DopedValidValue& rhs) :
    CValidValue<T>(rhs),
    m_dope(rhs.m_dope),
    m_index(rhs.m_index) {}

  DopedValidValue& operator=(const DopedValidValue& rhs) {
    m_dope = rhs.m_dope;
    m_index = rhs.m_index;
    return *this;
  }


  int operator==(const DopedValidValue& rhs) const {
    return (m_dope == rhs.m_dope) &&
      (m_index == rhs.m_index)    &&
      CValidValue<T>::operator==(rhs);
  }
  int operator!=(const DopedValidValue& rhs) const {
    return !(*this == rhs);
  }

public:
  DopedValidValue<T>& operator=(const T& rhs) {
    // add to dope vector if not yet set:

    if (!CValidValue<T>::isValid()) {
      m_dope.append(m_index);
    }
    return reinterpret_cast<DopedValidValue&>(CValidValue<T>::operator=(rhs));
  }


};


#endif
