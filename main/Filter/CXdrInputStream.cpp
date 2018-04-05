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
#include "CXdrInputStream.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>

#ifdef HAVE_STD_NAMESPACE
using namespace std;
#endif

/*!
   Construct a default xdr input stream.  This input stream
   is unconnected to any source.  All we do is:
   - Set the buffersize as per the parameter.
   - Create a buffer for the data.
   - Set the isOpen flag to false.

   \param nBuffersize (int in default = 8192)
      The buffer size for the stream.
*/
CXdrInputStream::CXdrInputStream(int nBuffersize) :
  m_nBuffersize(nBuffersize),
  m_pBuffer(new char[nBuffersize]),
  m_fIsOpen(false),
  m_nFid(-1)
{
  
}
/*!
   Construct an Xdr data source with a connection identifier.
   Note that the source must later be opened via a call to the
   Open() member function.  Open cannot be called now because
   it is virtual and virtuality does not work in constructors.

   Therefore, the only difference between this call and the
   other constructor is that m_sConnectionId is set.

 */
CXdrInputStream::CXdrInputStream(const string& connection,
				 int nBuffersize) :
  m_nBuffersize(nBuffersize),
  m_pBuffer(new char[nBuffersize]),
  m_sConnectionId(connection),
  m_fIsOpen(false),
  m_nFid(-1)
{
}
/*!
   Destructor for input stream:
   - Delete the buffer.
*/
CXdrInputStream::~CXdrInputStream()
{
  delete []m_pBuffer;
}

/*!
   Connect the stream to a connection id.
   The default connection is to a file named by the
   connection id.  This function:
   - opens the file for read.
   - Sets m_nFid from the open.
   - Sets m_fIsOpen true.
   - Reads the first buffer from the connection.

   \throw string 
      Description of any errors that were discovered.
      In the event of the throw, none of the state variables
      will have been changed..

    \note
      It is an error to open a file with an empty name.
*/
void
CXdrInputStream::Connect() 
  throw (string)
{
  if(m_sConnectionId == string("")) {
    throw string("Empty filename in CXdrInputStream::Connect");
  }
  int fd = open(m_sConnectionId.c_str(), O_RDONLY);
  if(fd < 0) {
    string msg(strerror(errno));
    throw msg;
  }

  // At this time, everything worked. We set up the member
  // data:

  m_fIsOpen = true;
  m_nFid    = fd;

  // Read the first buffer;

  DoRead();
  
}
/*!
   Fetch an item from the stream.
   \param item (void* [out]):
     Pointer to the item to fetch.  Note that for a string,
     you will need to pass a pointer to a pointer.
   \param cvt (xdrproc_t [in]):
     Pointer to the converstion function e.g. xdr_int

   If the last item is read, then the Read member is called to
   read the next buffer.

   Reading from a disconnected stream results in a string
   exception.
*/
void
CXdrInputStream::Get(void* item, xdrproc_t cvt) throw (string)
{
  if(!m_fIsOpen) {
    throw string("Stream is not open at CXdrInputStream::Get");
  }
  (*cvt)(&m_Xdr, item);

  // See if we need to read another buffer:

  ConditionalRead();



}
/*!
   Read the next buffer from the file.
   \param pBuffer (void* [out]):
     points to the buffer to read.  m_nBuffersize bytes are read.
   \return int
    - Number of bytes read if success
    - 0 if end of file.
    - -1 if error with errno containing the reason.
*/
int
CXdrInputStream::Read(void* pBuffer) 
{
  return read(m_nFid, pBuffer, m_nBuffersize);
}
/*!
   Disconnect a stream from the input file.
*/
void
CXdrInputStream::Disconnect() throw (string)
{
  if(!m_fIsOpen) {
    throw 
      string("File already closed in CXdrInputStream::Disconnect");
  }
  int stat = close(m_nFid);
  m_fIsOpen = false;
  if(stat < 0) {
    string msg(" Close failed in CXdrInputStream::Disconnect ");
    msg += strerror(errno);
    throw msg;
  }
}
/*!
   Do a read and all the associated book-keeping and error checking:
   # The Read member function is called
   # The following conditional action is taken:
   -  If the read completed in error, an string exception is thrown.
   -  If the read completed in end of file, Disconnect is called.
   -  If the read completed normally, the Xdr translator is created,
      and the number of used words is read into m_nSize.
   
   \throw string
     In the event the Read member returns an error.
*/
void
CXdrInputStream::DoRead() throw (string)
{
  int nRead = Read(m_pBuffer);
  if(nRead < 0) {
    string msg(strerror(errno));
    throw msg;
  }
  if(nRead == 0) {
    Disconnect();
  }
  xdrmem_create(&m_Xdr, m_pBuffer, m_nBuffersize, XDR_DECODE);
  xdr_int(&m_Xdr, &m_nSize);
} 
/*!
   Checks the current position within the data buffer.
   If we are at the end of the buffer:
   - The m_Xdr object is destroyed,
   - DoRead is called to setup the next buffer for decoding.

   \throw string
      The DoRead function can throw a string that passes right
      through us/
*/
void
CXdrInputStream::ConditionalRead() throw (string)
{
  int pos = xdr_getpos(&m_Xdr);
  if(pos >= m_nSize) {
    xdr_destroy(&m_Xdr);
    DoRead();
  } 
}
/*!
   Stream operator to extract an integer from the xdr input stream.
   \param n (int& [out])
      The integer will be extracted into this value.
   \return CXdrOutputStream&
   *this to support get chaining.

*/
CXdrInputStream&
CXdrInputStream::operator>>(int& n)
{
  Get(&n, (xdrproc_t)xdr_int);
  return *this;
}
/*!
   Stream operator to extract a float from the xdr input stream.
   \param n (float& [out]):
      The float to extract.
   \return CXdrInputStream&
   *this to support get chaining.

*/
CXdrInputStream&
CXdrInputStream::operator>>(float& n)
{
  Get(&n, (xdrproc_t)xdr_float);
  return *this;
}
/*!
   Stream operator to extract a double from the xdr input stream.
   \param n (double& [out])
      The double to extract.
   \return CXdrInputStream&
   *this to supprot get chaining.
*/
CXdrInputStream&
CXdrInputStream::operator>>(double& n)
{
  Get(&n, (xdrproc_t)xdr_double);
  return *this;
}
/*!
   Stream operator to extract a character from the xdr input
   stream. Note that successive characters are not packed so 
   You may not use this to extract a character at a time from a 
   string.  Either extract into an array (char*) or a string.

   \param c (char& [out])
      The character to extract.
   \return CXdrInputStream&
   *this to supprot get chaining.
*/
CXdrInputStream&
CXdrInputStream::operator>>(char& c)
{
  Get(&c, (xdrproc_t)xdr_char);
  return *this;
}
/*!
   Get character string into a character array.

   \param c (char* [out]):]
     Pointer to the storage for the string.  It is the
     caller's responsibility to allocate a large enough buffer
     for this string.
   \return CXdrInputStream&
   *this to supprot get chaining.

*/
CXdrInputStream&
CXdrInputStream::operator>>(char* c)
{
  xdr_string(&m_Xdr, &c, m_nBuffersize);
  ConditionalRead();
  return *this;
}
/*!
  Get a character string into an stl string.
  As with all items, the string is assumed not to 
  span buffer boundaries.. fixing it's maximum length
  at m_nBuffersize or so.

  \param s (string& [out]):
    String to get into.
   \return CXdrInputStream&
   *this to support get chaining.

*/
CXdrInputStream&
CXdrInputStream::operator>>(string& s) 
{
  char* strbuf =  new char[m_nBuffersize];
  strbuf[0] = 0;
  (*this) >> strbuf;
  s = strbuf;
  delete []strbuf;
  return *this;

}  
/*!
   Get a bool from the xdr input stream buffer.
   \param b (bool & [out]):
     Bool to get into.
   \return CXdrInputStream&
   *this to support get chaining
*/
CXdrInputStream&
CXdrInputStream::operator>>(bool& b)
{
  Get(&b, (xdrproc_t)xdr_bool);
  return *this;
}
