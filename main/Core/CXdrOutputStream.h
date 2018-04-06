#ifndef __CXDROUTPUTSTREAM_H
#define __CXDROUTPUTSTREAM_H
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


#ifndef __STL_STRING
#include <string>
#define __STL_STRING
#endif

#ifndef __RPC_XDR_H
#include <rpc/types.h>		/* Cygwin e.g. needs this. */
#ifdef BROKEN_XDR_H
#include "../Replace/xdr.h"
#else
#include <rpc/xdr.h>
#endif
#ifndef __RPC_XDR_H
#define __RPC_XDR_H
#endif
#endif


/*!
   This class produces a buffered binary output stream in XDR format.
   The class will take care of buffering, but it is up to the user to
   understand the structure of the data written to the file.
   See CXdrInputStream for a class that knows how to read this sort of data.
   Each buffer has as a first word, the offset at which it ends.  This is
   automatically maintained.
*/
class CXdrOutputStream {
  int   m_nBuffersize;		//!< Bytes in the buffer.
  char* m_pBuffer;		//!< Data are xdr'd into this buffer.
  int   m_nFd;			//!< File descriptor to output file.
  bool  m_fisOpen;		//!< True if file is currently open.
  std::string m_Filename;		//!< Filename if one has been supplied.
  XDR   m_Xdr;			//!< XDR stream to buffer.
  size_t m_nBlocks;             //!< Size of output file so far.

public:
  CXdrOutputStream(int nBuffersize=8192);
  CXdrOutputStream(const std::string& rName, int nBuffersize=8192)
    throw (std::string&);
  ~CXdrOutputStream();
private:
  CXdrOutputStream(const CXdrOutputStream& rhs);
  CXdrOutputStream& operator=(const CXdrOutputStream& rhs);
  int operator==(const CXdrOutputStream& rhs);
  int operator!=(const CXdrOutputStream& rhs);
public:
  
  // Selectors

public:

  bool isOpen() const {		//!< true if stream is open.
    return m_fisOpen;
  }
  int getFd() const {		//!< fd or -1 if stream closed.
    return m_nFd;
  }
  const char* getBuffer() const { //!< Pointer to output buffer.
    return m_pBuffer;
  }
  int getBuffersize() const {	//!< # bytes of buffering.
    return m_nBuffersize;
  }
  std::string getFilename() const {	//!< filename or empty if stream closed.
    return m_Filename;
  }
  size_t getBlocks() const {
    return m_nBlocks;
  }

  // Mutators

  // Class operations.

  void Open(const std::string& rName) throw (std::string&);
  void Close() throw (std::string&);
  void Put(void* object, xdrproc_t converter) throw (std::string&);
  bool Require(int nBytes);
  bool Test(int nBytes);

  // Sizers (sizeof is no good in xdr as it does not have to match
  // the native size need not match the xdr size:

  size_t sizeofItem(const void* item, xdrproc_t converter) const;
  size_t sizeofInt() const;
  size_t sizeofFloat() const;
  size_t sizeofLong() const;
  size_t sizeofDouble() const;
  size_t sizeofChar() const;
  size_t sizeofString(const char* pPrototype) const;


  // Stream operators:

  CXdrOutputStream& operator<<(short n);
  CXdrOutputStream& operator<<(int n);
  CXdrOutputStream& operator<<(long n);

  CXdrOutputStream& operator<<(float n);
  CXdrOutputStream& operator<<(double n);

  CXdrOutputStream& operator<<(char c);
  CXdrOutputStream& operator<<(const char* c);
  CXdrOutputStream& operator<<(const std::string& s) 
    {return ((*this) << s.c_str());}

  CXdrOutputStream& operator<<(bool b);

  CXdrOutputStream& operator<<(unsigned short n);
  CXdrOutputStream& operator<<(unsigned int n);
  CXdrOutputStream& operator<<(unsigned long n);



  void Flush();


};
#endif

