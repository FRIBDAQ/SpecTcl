/*
    This software is Copyright by the Board of Trustees of Michigan
    State University (c) Copyright 2005.

    You may use this software under the terms of the GNU public license
    (GPL).  The terms of this license are described at:

     http://www.gnu.org/licenses/gpl.txt

*/

#ifndef CLANGUAGECALLBACK_H
#define CLANGUAGECALLBACK_H

#include "CBufferCallback.h"

// The callback type is shown below:


typedef void (NSCLBufferCallback)(unsigned int nBufferType,
				  const void*  pBuffer,
				  void*        pUserData);

/*!
   This class allows you to attach an unbound function as a callback.
   The class serves as an adaptor between the object oriented callback scheme
   and the unbound function.  The function (presumably C?) will be called
   with the buffer type, the buffer pointer, and an uninterpreted user
   datum that must be sizeof(void*) and is kept, uninterpreted, as void*.

*/
class CLanguageCallback : public CBufferCallback
{
private:
  void*                m_pClientData;	// User parameter to the callback.
  NSCLBufferCallback*  m_pUserCallback;	// Pointer to the user's callback function.
public:
  CLanguageCallback(NSCLBufferCallback* pCallback,
		    void*               pUserData);
  CLanguageCallback(const CLanguageCallback& rhs);
  virtual ~CLanguageCallback();

  CLanguageCallback& operator=(const CLanguageCallback& rhs);
  int operator==(const CLanguageCallback& rhs) const;
  int operator!=(const CLanguageCallback& rhs) const;


  // Class operations:

  void operator()(unsigned int nBufferType,
		  const void*  pBuffer);
};


#endif
