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


#ifndef CXDRMEMINPUTSTREAM_H
#define CXDRMEMINPUTSTREAM_H
#include "CXdrInputStream.h"

#include <string>

/*!
   This class derives from CXdrInputStream to do input
   from a single in memory buffer. The method is quite simple,
   Open is a no-op and Read transfers the single in memory buffer
   the first time and indicates an eof on subsequent calls.
*/
class CXdrMemInputStream : public CXdrInputStream
{
private:
  void* m_pBuffer;

  bool  m_fHaveRead;
public:
  // Constructors an other canonicals.

  CXdrMemInputStream(int nBuffersize,
		  void* pBuffer);
  virtual ~CXdrMemInputStream();

  // Copy etc. are not supported.

private:
  CXdrMemInputStream(const CXdrMemInputStream& rhs);
  CXdrMemInputStream& operator=(const CXdrMemInputStream& rhs);
  int operator==(const CXdrMemInputStream& rhs);
  int operator!=(const CXdrMemInputStream& rhs);
public:

  // Selectors

public:

  void* getBuffer() {
    return m_pBuffer;
  }
  bool AlreadyRead() const {
    return m_fHaveRead;
  }



  // Overrides of the base class.

  virtual void Connect();
  virtual void Disconnect();
protected:
  virtual int Read(void* pBuffer);

};


#endif
