///////////////////////////////////////////////////////////
//  CTreeException.h
//  Implementation of the Class CTreeException
//  Created on:      30-Mar-2005 11:03:49 AM
//  Original author: Ron Fox
///////////////////////////////////////////////////////////

#if !defined(__CTREEEXCEPTION_H)
#define __CTREEEXCEPTION_H


#ifndef __EXCEPTION_H
#include <Exception.h>
#endif

#ifndef __STL_STRING
#include <string>
#ifndef __STL_STRING
#define __STL_STRING
#endif
#endif

/**
 * Exception that can be thrown due to usage problems in the
 * treeparameter/treevariable and array classes.
 * Note that this class includes the definition of an enum (TreeErrorReason) which
 * has values:
 * 
 * - NotBound    - Tree Parameter is not bound to a parameter in an event array and
 *                 should be. 
 * - Bound       - Tree parameter is bound to a parameter in an event array and
 *                 should not be.
 * - NoParam     - Tree parameter needs an underlying paramter and does not have one. 
 * - HaveParam   - Tree Parameter had an underlying parameter and should not have one.
 * - InvalidIndex - Tree parameter array index is outside the valid range of indices.
 * - UnitsMismatch - An assignment attempted to violate the sancitity of units.
 * - Invalid         - Invalid exception reason (used for ReasonToCode mostly.
 *
 * @author Ron Fox
 * @version 1.0
 * @created 30-Mar-2005 11:03:49 AM
 */
class CTreeException : public CException
{
  // Data types:
public:
  typedef enum _TreeErrorReason {
    NotBound,
    Bound,
    NoParam,
    HaveParam,
    InvalidIndex,
    UnitsMismatch,
    Invalid			// It's important that this be the last value.
  } TreeErrorReason;
private:
  /**
   * Why the exception was thrown.  See the class comment header and class
   * definition body for possible values (this is an enum).
   */
  TreeErrorReason m_eReason;
  /**
   * Provides a properly scoped value from which to return reason texts.
   */
  mutable STD(string) m_sLastReasonText;	// This makes us thread-unsafe!!!!
public:
  CTreeException(TreeErrorReason reason, STD(string) doing);
  virtual         ~CTreeException();
  CTreeException(const CTreeException&);
  CTreeException& operator=(const CTreeException&);
  int operator==(const CTreeException& rhs);
  int operator!=(const CTreeException& rhs);

  virtual const char*   ReasonText() const;
  virtual Int_t   ReasonCode() const;
  static TreeErrorReason ReasonToCode(Int_t nReason);
  STD(string)          ReasonValueToText(TreeErrorReason eReason) const;
  
  
};


#endif
