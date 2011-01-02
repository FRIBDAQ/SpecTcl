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

#ifndef __STATEEXCEPTION_H
#define __STATEEXCEPTION_H

#ifndef __EXCEPTION_H
#include <Exception.h>
#endif

#ifndef __STL_STRING
#include <string>
#ifndef __STL_STRING
#define __STL_STRING
#endif
#endif

/*!
   This class provides an exception that can be thrown if
   a program detects a 'state mismatch'. A state mismatch
   is said to occur when something is requested of an object,
   but its internal state forbids that service from being rendered to
   the client.  In particular, this was developed to deal with
   services being requested in the TCL class library of objects
   that require an interpreter to satisfy, when objects were not
   (yet) bound to an interpreter.
*/
class CStateException : public CException
{
private:
  std::string m_Reason;
public:
  // Constructors and other canonicals:

  CStateException(const char* state, const char* allowedStates,
		  const char* pAction);
  virtual ~CStateException();
  CStateException(const CStateException& rhs);
  CStateException& operator=(const CStateException& rhs);
  int operator==(const CStateException& rhs);
  int operator!=(const CStateException& rhs);


  // Overrides so this can function correctly as a CException:

  virtual  const char* ReasonText() const;
  virtual  Int_t ReasonCode() const;

};

#endif
