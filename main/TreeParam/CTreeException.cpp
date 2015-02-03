///////////////////////////////////////////////////////////
//  CTreeException.cpp
//  Implementation of the Class CTreeException
//  Created on:      30-Mar-2005 11:03:49 AM
//  Original author: Ron Fox
///////////////////////////////////////////////////////////
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
// This implementation of TreeParameter is based on the ideas and original code of::
//    Daniel Bazin
//    National Superconducting Cyclotron Lab
//    Michigan State University
//    East Lansing, MI 48824-1321
//


#include <config.h>
#include "CTreeException.h"


#ifdef HAVE_STD_NAMESPACE
using namespace std;
#endif

/*!
    Construct the tree exception:
    \param reason 
           Why the exception is being thrown.
    \param doing
           What was going on when the exception was thrown.
*/
CTreeException::CTreeException(CTreeException::TreeErrorReason reason,
			       string doing) :
  CException(doing),
  m_eReason(reason)
{

}


//!  Destructor: no action required.

CTreeException::~CTreeException()
{
}

/*!
   Copy constructor - deletgate to the baes class and copy the reason.
*/
CTreeException::CTreeException(const CTreeException& rhs) :
  CException(rhs),
  m_eReason(rhs.m_eReason)
{
}
/*!
    Assignment - delegates to base class and copies the reason.
   Don't need to copy the last reason text since that's rebuilt every time
   it's needed.
*/
CTreeException&
CTreeException::operator=(const CTreeException& rhs) 
{
  if(&rhs != this) {
    CException::operator=(rhs);
    m_eReason = rhs.m_eReason;
  }
  return *this;
}
/*!
  Equality comparison.
*/
int
CTreeException::operator==(const CTreeException& rhs) 
{
  return (CException::operator==(rhs)    &&
	  (m_eReason == rhs.m_eReason));
}

/*!
   Inequality is the logical inverse of equality comparison.
*/
int
CTreeException::operator!=(const CTreeException& rhs) 
{
  return !(operator==(rhs));
}

/**
 * Returns why the exception was thrown.
 */
const char* 
CTreeException::ReasonText() const
{

  m_sLastReasonText   = "TreeException: ";
  
  m_sLastReasonText  +=  ReasonValueToText(m_eReason);
  m_sLastReasonText  += " while: ";
  m_sLastReasonText  += WasDoing();
  
  return m_sLastReasonText.c_str();
  


}


/**
 * Return the integerized m_eReason value.
 */
Int_t 
CTreeException::ReasonCode() const
{

  return (Int_t)m_eReason;


}


/**
 * Converts an integerized reason to a reason code.
 * @param nReason    
 *        Integerized reason from the call to CTreeException::
 *        ReasonCode
 * 
 */
CTreeException::TreeErrorReason 
CTreeException::ReasonToCode(Int_t nReason)
{
  
  TreeErrorReason code = (TreeErrorReason)nReason;
  
  if((code >=0) && (code < Invalid)) {
    return code;
  }
  else {
    return Invalid;
  }
  
  
}


/**
 * Converts the reason value to a textual string.
 * @param eReason
 *        The enumerated reason for the exception.
 * 
 */
string 
CTreeException::ReasonValueToText(CTreeException::TreeErrorReason eReason) const
{

  switch (eReason) {
  case NotBound:
    return "Tree Parameter is not bound to a parameter in an event array and should be.";
  case Bound:
    return "Tree Parameter is bound to a parameter in an event array and should not be.";
  case NoParam:
    return  "Tree Parameter needs an underlying parameter and does not have one.";
  case HaveParam:
    return "Tree Parameter had an underlying parameter and should not have one.";
  case InvalidIndex:
    return "Tree Parameter index is outside the valid range of indices.";
  case UnitsMismatch:
    return "Units mismatch in Tree Parameter assignment";
  case Invalid:
  default:
    return "Unrecognized exception reason code";
  }
 
  
}


