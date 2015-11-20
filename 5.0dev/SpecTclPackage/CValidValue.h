/*
    This software is Copyright by the Board of Trustees of Michigan
    State University (c) Copyright 2009.

    You may use this software under the terms of the GNU public license
    (GPL).  The terms of this license are described at:

     http://www.gnu.org/licenses/gpl.txt

     Author:
             Ron Fox
	     NSCL
	     Michigan State University
	     East Lansing, MI 48824-1321
*/

#ifndef _CVALIDVALUE_H
#define _CVALIDVALUE_H

#ifndef _STD_EXCEPTION
#include <exception>
#ifndef _STD_EXCEPTION
#define _STD_EXCEPTION
#endif
#endif

/**
 * @file CValidValue.h
 * @brief Valid value is a value that knows it's been set. (templated).
 */
class invalid_exception : public std::exception {
public:
  virtual const char* what() const throw() {
    return "Valid value used on lhs when not valid";
  }
};

/**
 * @class CValidValue
 *   
 *  This class is a template class that mimics a type that has all the numeric
 *  operators.  The value added by this wrapper is that it knows if it's value
 *  was modified since the last reset method call.
 */

template <class T>
class CValidValue {
private:
  T     m_value;
  bool  m_modified;

  // Canonicals
public:
  CValidValue() : m_modified(false) {}
  CValidValue(const T& initial) :
    m_value(initial), m_modified(true) {}
  virtual ~CValidValue() {}
  CValidValue(const CValidValue& rhs)  
    : m_value(rhs.m_value), m_modified(rhs.m_modified)
  {
    throwIfNotValid();
  }
  CValidValue& operator=(const CValidValue& rhs) {
    rhs.throwIfNotValid();
    m_value = rhs.m_value;
    m_modified = rhs.m_modified;
    return *this;
  }
  CValidValue& operator=(double v) {
    m_value = v;
    m_modified = true;
  }

  // Public methods:

public:
  void reset() { m_modified = false;}
  bool isvalid() const {
    return m_modified;
  }
  operator T() {
    throwIfNotValid();
    return m_value;
  }
 
private:
  void throwIfNotValid() const {
    if (!m_modified) {
      throw invalid_exception();
    }
  }
};


#endif
