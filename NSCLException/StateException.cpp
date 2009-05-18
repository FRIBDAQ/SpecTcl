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
#include <config.h>
#include "StateException.h"

#ifdef HAVE_STD_NAMESPACE
using namespace std;
#endif

/////////////////////////////////////////////////////////////////////////////
/*!
   Construct a state exception. 
   \param state : const char*
       Describes the state the object is in.
   \param allowedStates : const char*
       Describes the state the object should be in to be able to perform
       the request.
   \param pAction : const char*
      Describes what the program is doing at the time of the exception.
*/
CStateException::CStateException(const char*    state,
				 const char*    allowedStates,
				 const char*    pAction) :
  CException(pAction)
{
  m_Reason  = "Invalid object state when: ";
  m_Reason +=  WasDoing();
  m_Reason += " relevant erroneous state: ";
  m_Reason += state;
  m_Reason += " relevant allowed state: ";
  m_Reason += allowedStates;
}
///////////////////////////////////////////////////////////////////////////
/*!
   Destruction is a no-op.
*/
CStateException::~CStateException()
{
  
}
//////////////////////////////////////////////////////////////////////////
/*!
   Copy construction.. just need to construct the base class and
   our reason string.
*/
CStateException::CStateException(const CStateException & rhs) :
  CException(rhs),
  m_Reason(rhs.m_Reason)
{}

//////////////////////////////////////////////////////////////////////////
/*!
     Asignment operator.   base class assign and then assign our
     reason text.
*/
CStateException&
CStateException::operator=(const CStateException& rhs) 
{
  if (this != &rhs) {
    CException::operator=(rhs);
    m_Reason = rhs.m_Reason;
  }
  return *this;
}
//////////////////////////////////////////////////////////////////////////
/*!
   Equality comparison... if base class and reason text are the same.
*/
int
CStateException::operator==(const CStateException& rhs) 
{
  return (CException::operator==(rhs)                   &&
	  m_Reason == rhs.m_Reason);
}
//////////////////////////////////////////////////////////////////////////
/*!
   Inequality comparision.. this is just the inverse of equality compare.
*/
int
CStateException::operator!=(const CStateException& rhs) 
{
  return !(*this == rhs);
}

//////////////////////////////////////////////////////////////////////////
/*!
   Return the reason for the exception. This was stored at construction
   time in m_Reason.

   \return const char*
   \retval  m_Reason.c_str()

*/
const  char*
CStateException::ReasonText() const
{
  return m_Reason.c_str();
}
/////////////////////////////////////////////////////////////////////////
/*!
   Reason code is always meaningless so we return a -1.
*/
Int_t
CStateException::ReasonCode() const
{
  return -1;
}
