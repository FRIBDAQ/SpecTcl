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
#ifndef __VALIDVALUE_H
#define __VALIDVALUE_H

#ifndef __HISTOTYPES_H
#include <histotypes.h>
#endif

#ifndef __STL_STRING
#include <string>
#ifndef __STL_STRING
#define __STL_STRING
#endif
#endif

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
      throw STD(string)("Attempted getValue of unset ValidValue object");
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
      throw STD(string)("Attempted getValue of unset ValidValue object");
    }
    return m_Value; 
  }
};

#endif
