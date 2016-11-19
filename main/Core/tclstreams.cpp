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
#include <tcl.h>
#include "tclstreams.h"
#include <string.h>

// Implementation of Tcl Channel based streambufs.
// see tclstreams.h for the general big picture:

///////////////////////////////////////////////////////////////////////////////

/*!
   Construct a Tcl_Channel output streambuf (tcloutbuf).
   We just tell the base class to initialize it's buffer
   management pointers using our m_Buffer.
   \param Channel (Tcl_Channel):
       The Tcl Channel to which I/O (writes) will be done.
*/
tcloutbuf::tcloutbuf(Tcl_Channel Channel) :
  m_Channel(Channel)
{
  setp(m_Buffer, m_Buffer+m_BufferLength);
}
/*!
  Destroy a Tcl_Channel output stream buf... We just need to call
  overflow with EOF to ensure that nay partial buffer gets written.

 */
tcloutbuf::~tcloutbuf()
{
  overflow(EOF);
}
/*!
    Called to attempt to put a character in the output buffer.
    I believe this is usually just called when the bufer is full
    to flush the buffer out.
    - If the character is an EOF we just flush the buffer, reset the pointers
      and return EOF.
    - If the character is not an EOF and there's room in the buffer, we just
      put the character in the buffer and increment the put pointer.
    - If there's no room in the buffer, we get sneaky by calling ourselves with EOF
      to get the buffer flushed and then again with the character to get the
      character inserted.

      \param c (int_type):
          The character to insert, unless the value is EOF in which case the
	  output buffer just gets flushed.
      \return int_type
      \retval   EOF     If output fails.
      \retval  other    Success.
*/
int_type
tcloutbuf::overflow(int_type c)
{
  char* p = pptr();
  char* e = epptr();
  char* b = pbase();

  if(c == EOF) {
    if(pptr() != pbase()) {
      int status = Tcl_WriteChars(m_Channel, pbase(), pptr() - pbase());
      setp(m_Buffer, m_Buffer+m_BufferLength);
    }
    return EOF;
  }
  else {
    if(pptr() < epptr()) {	// There's space
      *pptr() = static_cast<char>(c);
      pbump(1);
      return c;
    }
    else {
      overflow(EOF);
      return overflow(c);
    }
  }
}
////////////////////////////////////////////////////////////////////////////////

/*!
  Constructs a Tcl_Channel based input stream.  For details about the
  characteristics of this stream, see the header.  Note that since the
  channel is already open, and the user may do putbacks we initially allocated
  a putback buffer of a size designated by the streambuf creator (defaulting to 128
  chars).
  \param Channel (Tcl_Channel):
       Tcl channel the buffer will be read from to provide the read ahead buffer.
  \param nPutBackSize (int):
      bytes in the putback region.

 */
tclinbuf::tclinbuf(Tcl_Channel Channel, int nPutBackSize) :
  m_Channel(Channel),
  m_nPutBackSize(nPutBackSize),
  m_pBuffer(0)
{
  m_pBuffer = new char[m_nPutBackSize];
  setg(m_pBuffer, m_pBuffer+m_nPutBackSize, m_pBuffer+m_nPutBackSize);
}
/*!
   Destroys a tclinbuf streambuf.  This requires the deletion of the
   current input read-ahead buffer:
*/
tclinbuf::~tclinbuf()
{
  delete []m_pBuffer;
}
/*!
   Usually called by the iostreams library when the input buffer
   is empty and a character must be returned.  The function
   is responsible for, if necessary reading the next block of data
   from the channel, buffering it for subsequent reads.  The read is done
   to a buffer that has m_nPutBackSize free bytes on the front to allow
   for putbacks.

   \return int
   \retval   EOF   - read failed indicating the end of file or an error.
   \retval    c    - The first character read.

   NOTE:
     I see a sample of the fdsostream that seems to believe there might be
     bytes in the putback buffer, but I think that's a mistake on their part.
     since if there are bytes in the putback buffer they are returnable.

*/
int_type
tclinbuf::underflow()
{
  // If there are characters, return the one at the gptr position:

  if(gptr() < egptr()) {
    return static_cast<int_type>(*gptr());
  }
  // We need to read data from the channel.  This is done by Tcl_ReadChars.

  Tcl_Obj* pObject = Tcl_NewObj();

  int nRead = Tcl_ReadChars(m_Channel, pObject, m_nReadSize, 0);
  if(nRead <= 0) {
    return EOF;
  }
  // We need to figure out how big this is, allocate a buffer big enough 
  // to hold this and a putback buffer and transfer the string from 
  // the object into the right part of the buffer.

  char* pReadData = Tcl_GetStringFromObj(pObject, &nRead); // This also utf-8 encodes.

  delete []m_pBuffer;
  m_pBuffer = new char[nRead + m_nPutBackSize];
  memcpy(m_pBuffer+m_nPutBackSize, pReadData, nRead);
  Tcl_DecrRefCount(pObject);

  // Set up the pointers appropriately:

  setg(m_pBuffer, m_pBuffer+m_nPutBackSize,
       m_pBuffer+m_nPutBackSize + nRead);

  // And we should now be able to recurse to return the appropriate thingy:

  return underflow();

  
}
