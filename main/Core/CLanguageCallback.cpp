/*
    This software is Copyright by the Board of Trustees of Michigan
    State University (c) Copyright 2005.

    You may use this software under the terms of the GNU public license
    (GPL).  The terms of this license are described at:

     http://www.gnu.org/licenses/gpl.txt

*/
#include <config.h>
#include "CLanguageCallback.h"

#ifdef HAVE_STD_NAMESPACE
using namespace std;
#endif

// Implementation of the C language (unbound function) callback class.
// See the header for more information.

/*!
   Construct a callback object.   The callback object contains a function pointer
   and a pointer to the user's callback specific data.  We really only need to save
   these items in class member data:
   \param pCallback (NSCLBUfferCallback* [in]):
       Pointer to the user's unbound callback function.
   \param pUserData  (void* [in]):
       User's data that will be passed as the final argument to the callback.
*/
CLanguageCallback::CLanguageCallback(NSCLBufferCallback* pCallback,
				     void*               pUserData) :
  m_pClientData(pUserData),
  m_pUserCallback(pCallback)
{}
/*!
   Copy construction is a no brainer... just copy the member data.
*/
CLanguageCallback::CLanguageCallback(const CLanguageCallback& rhs) :
  m_pClientData(rhs.m_pClientData),
  m_pUserCallback(rhs.m_pUserCallback)
{}
/*!
   Destruction is now a no-op:
*/
CLanguageCallback::~CLanguageCallback()
{
}
/*!
  Assignment is just a shallow copy since the user data is uninterpreted:
*/
CLanguageCallback&
CLanguageCallback::operator=(const CLanguageCallback& rhs)
{
  if(this != &rhs) {
    CBufferCallback::operator=(rhs);
    m_pClientData   = rhs.m_pClientData;
    m_pUserCallback = rhs.m_pUserCallback;
  }
  return *this;
}
/*!
  Callbacks are equal if their shallow member data are equal.
 */
int
CLanguageCallback::operator==(const CLanguageCallback& rhs) const
{
  return ((CBufferCallback::operator==(rhs))       &&
	  (m_pClientData  == rhs.m_pClientData)   &&
	  (m_pUserCallback== rhs.m_pUserCallback));
}
/*!
  Callbacks are inequal if they are not equal
*/
int
CLanguageCallback::operator!=(const CLanguageCallback& rhs) const
{
  return !(*this == rhs);
}

/*!
   The callback operation just delegates to the user callback:
   \param nBufferType (unsigned int [in]):
       The type of buffer on which we were established as a callback.
   \param pBuffer  (const void* [in]):
       The data buffer that triggered us.
*/
void
CLanguageCallback::operator()(unsigned int nBufferType,
			      const void*  pBuffer)
{
  (*m_pUserCallback)(nBufferType, pBuffer, m_pClientData); // that was easy.
}
