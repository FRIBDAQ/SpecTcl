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
#include "CXdrMemInputStream.h"
#include <string.h>
#ifdef HAVE_STD_NAMESPACE
using namespace std;
#endif

/*!
   Construct an XdrMemInputStream.
   This is done with an arbitrary name in order to force the dummy
   connect.
   \param nBuffersize (int [in]):
       Number of bytes in the buffer.
   \param pBuffer (void* [in]):
       The buffer to xdr read from.
*/
CXdrMemInputStream::CXdrMemInputStream(int nBuffersize,
				       void* pBuffer) :
  CXdrInputStream(string("dummy"), nBuffersize) ,
    m_pBuffer(pBuffer),
    m_fHaveRead(false)
{
}
/*!
   Destructor is a no-op in this implementation.
*/
CXdrMemInputStream::~CXdrMemInputStream() 
{}

/*!
   Connect the stream..  This just sets the state of the
   stream to connected.
*/
void
CXdrMemInputStream::Connect() throw (string)
{
  setOpen(true);		// Set state to open and
  DoRead();			// do the one and only read.
}
/*!
  Disconnect the stream.. just sets the state of the stream
  to closed.
*/
void
CXdrMemInputStream::Disconnect() throw (string)
{
  setOpen(false);
}
/*!
   Read a buffer from the stream.
   If the m_fHaveRead flag is true, then m_pBuffer is copied
   where we've been told to put it.  Otherwise, 0 is returned
   indicating an end file condition.
*/
int
CXdrMemInputStream::Read(void* pBuffer)
{
  if(!m_fHaveRead) {
    memcpy(pBuffer, m_pBuffer, getBufferSize());
    m_fHaveRead = true;
    return getBufferSize();
  }
  else {
    return 0;
  }
}
