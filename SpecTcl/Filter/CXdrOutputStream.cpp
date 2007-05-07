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
#include "CXdrOutputStream.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <rpc/types.h>		// CYGWIN e.g. does not include in xdr.h!!
#ifdef BROKEN_XDR_H
#include "../Replace/xdr.h"
#else
#include <rpc/xdr.h>
#endif
#include <Iostream.h>

#ifdef HAVE_STD_NAMESPACE
using namespace std;
#endif

/*!
   Construct a closed CXdrOutputStream.  This function does
   not open the file.  That must be done by calling the
   Open member.
   \param nBuffersize (int [in] default=8192)  
      The Size of the outpt buffer.  When a put to the buffer
      woule extend the buffer beyond this number of bytes, the
      buffer is written to the output file.
*/
CXdrOutputStream::CXdrOutputStream(int nBuffersize) :
  m_nBuffersize(nBuffersize),
  m_pBuffer(new char[nBuffersize]),
  m_nFd(-1),
  m_fisOpen(false),
  m_nBlocks(0)
{
  // By default strings construct empty.
}
/*!
  Construct an open CXdrOutuptStream.  This represents an output
  stream that is initially in the open state on a file.
  \param rName (const string& [in])
    Name of the file to create.
  \param nBuffersize (int [in] default=8192)
    Size of the buffer to create.  See above for more information.

  \throw string
     Errors uncovered in construction result in throwing a string.

*/
CXdrOutputStream::CXdrOutputStream(const string& rName, 
				   int nBuffersize) throw (string&):
  m_nBuffersize(nBuffersize),
  m_pBuffer(new char[nBuffersize]),
  m_nFd(-1),
  m_fisOpen(false),
  m_Filename(rName), 
  m_nBlocks(0)
{
  Open(rName);

}
/*!
   Destroy an CXdrOutputStream:
   - delete the buffer.
   - if m_fisOpen is true close the fd.
*/
CXdrOutputStream::~CXdrOutputStream()
{
  try {
    if(m_fisOpen) Close();
  } catch (...) {
    delete []m_pBuffer;
    m_pBuffer = 0;
    throw;
  }
  delete []m_pBuffer;
  m_pBuffer = 0;
}
/*!
   Open an output stream.  The current stream must not be open.
   \param rName (const string& [in]):
     The name of the file to open.

   \throw string
     Exception on error including:
     - Attempt to open an already open stream.
     - Error on the open call.
*/
void
CXdrOutputStream::Open(const string& rName) throw (string&)
{
  // Throw if output stream is open 

  if(m_fisOpen) {
    throw string("CXdrOutputStream::Open stream is already open");
  }


  m_nFd = open(rName.c_str(), O_CREAT | O_TRUNC | O_WRONLY, 0777);
  if(m_nFd < 0) {
    int e = errno;		// in case string perverts it
    string ErrnoText(strerror(e));
    string Exception("CXdrOutputStream::constructor Open of: ");
    Exception += m_Filename;
    Exception += " failed: ";
    Exception += ErrnoText;
    delete []m_pBuffer;		// Clean up prior to throw.
    throw Exception;
  }
  m_Filename = rName; 
  m_fisOpen  = true; 
  

  // Initialize the buffer.

  xdrmem_create(&m_Xdr, m_pBuffer, m_nBuffersize, XDR_ENCODE);
  int LastItem = 0;
  xdr_int(&m_Xdr, &LastItem);	// Reserve space for byte count.
}
/*!
   Close an already open stream.  It is an error to close a stream
   that is already open.

   \throw string&
      - Throws if the stream is already open.
      - Throws if the buffer flush fails.

*/
void 
CXdrOutputStream::Close() throw (string&)
{
  if(!m_fisOpen) {
    throw string("CXdrOutputStream::Close Stream is already closed");
  }



  Flush();

  // Close the file.

  close(m_nFd);
  m_nFd = -1;
  m_fisOpen = false;
  m_Filename = string("");
  

}
/*!
   General purpose output conversion.
   \param object (void* [in]):  
      The object to put to the stream.
   \param converter (xdrproc_t [in]):
      A function pointer to the xdr converter for the type of
      object (e.g. xdr_int if object is a pointer to int).

    \throw  string 
       If the converter reports a failure.
*/
void
CXdrOutputStream::Put(void* object, xdrproc_t converter) 
  throw(string&)
{
  int status = converter(&m_Xdr, object);
  if(!status) {
    throw string("CXdrOutputStream::Put - converter reported a failur");
  }
  Require(1);


}
/*!
   Flush the current buffer to file:
*/
void
CXdrOutputStream::Flush()
{
  int pos = xdr_getpos(&m_Xdr);
  xdr_setpos(&m_Xdr, 0);
  xdr_int(&m_Xdr, &pos);
  
  int nwritten = write(m_nFd, m_pBuffer, m_nBuffersize);
  if(nwritten < 0) {
    int e = errno;
    string Exception("CXdrOutputStream::Close write failed: ");
    Exception += strerror(e);
    throw Exception;
  }
  xdr_destroy(&m_Xdr);
  xdrmem_create(&m_Xdr, m_pBuffer, m_nBuffersize, XDR_ENCODE);
  xdr_int(&m_Xdr, &pos);		// Space for the count.
  
  m_nBlocks++;			// Count the filesize.


}
/*!
   Require a specific amount of free space else flush:
   \param nBytes (int in):
     Number of bytes requried.
*/
void
CXdrOutputStream::Require(int nBytes)
{
  int pos = xdr_getpos(&m_Xdr);
  if (pos + (nBytes*2) >= m_nBuffersize) { // Seems to be a factor of 2.
    Flush();
  }
}
/*!
   Put an int using the stream operators:

   \param n (int in):
     Data to write.
   \return:
     Reference to self to support chaining.
*/
CXdrOutputStream&
CXdrOutputStream::operator<<(int n)
{
  Put(&n, (xdrproc_t)xdr_int);
  return *this;
}
/*!
  Put a float using the stream operators.
   \param f (float f)
     Data to write.
   \return:
     Reference to self to support chaining.
*/
CXdrOutputStream&
CXdrOutputStream::operator<<(float f)
{
  Put(&f, (xdrproc_t)xdr_float);
  return *this;
}

/*!
   Put a double using the stream operators:
*/
CXdrOutputStream&
CXdrOutputStream::operator<<(double f)
{
  Put(&f, (xdrproc_t)xdr_double);

  return *this;
}
/*!
  Put a char using the stream operators.
*/
CXdrOutputStream&
CXdrOutputStream::operator<<(char c)
{
  Put(&c, (xdrproc_t)xdr_char);

  return *this;
}
/*!
  Put a null terminated (c string) to 
  the stream using stream operators
*/
CXdrOutputStream&
CXdrOutputStream::operator<<(const char* c)
{
  Require(strlen(c) + 1);	// Assume no inflation.
  Put(&c, (xdrproc_t)xdr_wrapstring);
  
  return *this;
}
/*!
   Put a bool to the output stream:
*/
CXdrOutputStream&
CXdrOutputStream::operator<<(bool f)
{
  bool_t val = f;		// In case bool/bool_t have different sizes.
  Put(&val, (xdrproc_t)xdr_bool);

  return *this;
}
/*!
   put a short:
*/
CXdrOutputStream&
CXdrOutputStream::operator<<(short n)
{
  Put(&n, (xdrproc_t)xdr_short);
  return *this;
}
/*!
   Put a long:
*/
CXdrOutputStream&
CXdrOutputStream::operator<<(long n)
{
  Put(&n, (xdrproc_t)xdr_long);
  return *this;
}
/*!
   Put an unsigned short:
*/
CXdrOutputStream&
CXdrOutputStream::operator<<(unsigned short n)
{
  Put(&n, (xdrproc_t)xdr_u_short);
  return *this;
}
/*!
   Put an unsigned int:
*/
CXdrOutputStream&
CXdrOutputStream::operator<<(unsigned int n)
{
  Put(&n, (xdrproc_t)xdr_u_int);
  return *this;
}
/*!
   Put an unsigned long:
*/
CXdrOutputStream&
CXdrOutputStream::operator<<(unsigned long n)
{
  Put(&n, (xdrproc_t)xdr_u_long);
  return *this;
}
/*!
   Return the xdr size of an item.  This is not necessarily
   the sizeof size of an item as xdr representation may differ
   radically from internal machine representations.
   \param item (cnst void* (in)):
     A sample item (e.g. a float).
   \param converter (xdrproc_t (in):
     The conversion procedure for item (e.g. xdr_float).

   \return 
      The number of bytes that will be required to convert the 
      item.

   The result is computed by doing a conversion into a temporary
   xdr stream and then looking at the stream position after the
   conversion... Sorry, I don't know any other way to do this.
*/
size_t 
CXdrOutputStream::sizeofItem(const void* item, xdrproc_t converter) const
{
  XDR  x;
  char* buffer = new char[m_nBuffersize];	// can't do bigger than this.
  xdrmem_create(&x, buffer, m_nBuffersize, XDR_ENCODE);
  (*converter)(&x, (void*)item);

  size_t result = xdr_getpos(&x);
  xdr_destroy(&x);
  delete []buffer;
  return result;
}
/*!
   Return the size of an int in xdr representation.
*/
size_t
CXdrOutputStream::sizeofInt() const
{
  int i;
  return sizeofItem(&i, (xdrproc_t)xdr_int);
}
/*!
   Return the size of a float in xdr representation:
*/
size_t
CXdrOutputStream::sizeofFloat() const
{
  float f;
  return sizeofItem(&f, (xdrproc_t)xdr_float);
}
/*!
  Return the size of a long in xdr representation:
*/
size_t
CXdrOutputStream::sizeofLong() const
{
  long l;
  return sizeofItem(&l, (xdrproc_t)xdr_long);
}
/*!
  Return the size of a double in xdr representation.
*/
size_t
CXdrOutputStream::sizeofDouble() const
{
  double d;
  return sizeofItem(&d, (xdrproc_t)xdr_double);
 
}
/*!
   Return the size of a single character in xdr representation.
*/
size_t 
CXdrOutputStream::sizeofChar() const
{
  char c;
  return sizeofItem(&c, (xdrproc_t)xdr_char);
}
/*!
   Return the size of a string in xdr representation.
*/
size_t
CXdrOutputStream::sizeofString(const char* pPrototype) const
{
  return sizeofItem(&pPrototype, (xdrproc_t)xdr_wrapstring);
}
