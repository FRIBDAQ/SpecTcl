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

template <class T>
class CValidValue {
private:
  T m_Value;
  Bool_t m_Valid;
public:
  // Constructors and other cannonicals:
  //
  CValidValue() : m_Valid(kfFALSE)
  {
  }
  CValidValue(T& initValue, Bool_t isValid = kfFALSE) : m_Value(initValue),
							m_Valid(isValid)
  {
  }
  CValidValue(const CValidValue& rRHS) : 
    m_Value(rRHS.m_Value),
    m_Valid(rRHS.m_Valid)
  {
  }
  CValidValue& operator=(const CValidValue& rRHS) {
    if(this != &rRHS) {
      m_Value = rRHS.m_Value;
      m_Valid = rRHS.m_Valid;
    }
    return *this;
  }
  int operator==(const CValidValue& rRhs) const {
    if((!m_Valid) || (!rRhs.m_Valid)) return 0;	// Both must be valid.
    return (m_Value == rRhs.m_Value);           // and equal.
  }
  int operator!=(const CValidValue& rRhs) const {
    return !operator==(rRhs);
  }
  //
  //  Selectors:
  //
public:
  T& getValue() {
    return m_Value;
  }
  Bool_t isValid() const {
    return m_Valid;
  }
  //
  // Mutators:
  //
protected:
  void setValue(const T& rNew) {
    m_Value = rNew;
  }
  void setValid(const Bool_t Valid) {
    m_Valid = Valid;
  }
  // Object operations.
  //
public:
  void clear() { m_Valid = kfFALSE; }
  CValidValue& operator=(const T& rRhs) {
    m_Value = rRhs;
    m_Valid = kfTRUE;
  }
  operator T&() { return m_Value; }
};

#endif
