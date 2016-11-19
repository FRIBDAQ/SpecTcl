/*
    This software is Copyright by the Board of Trustees of Michigan
    State University (c) Copyright 2014.

    You may use this software under the terms of the GNU public license
    (GPL).  The terms of this license are described at:

     http://www.gnu.org/licenses/gpl.txt

     Authors:
             Ron Fox
             Jeromy Tompkins
             NSCL
             Michigan State University
             East Lansing, MI 48824-1321
*/
#ifndef __TCLSTREAMS_H
#define __TCLSTREAMS_H

#include <tcl.h>

#include <istream>
#include <ostream>
#include <streambuf>


// Need to know what EOF is and to be able to call memmove.. that's in:

#include <stdio.h>
#include <string.h>


// The simplest way to make this work on both g++ 2.x and 3.x is to
// do our own  int_type/char_type definitions since the 2.x I/O library
// is quite a bit simpler with respect to character types:

typedef int  int_type;
typedef char char_type;


/*!
    tcloutbuf represents a C++ output stream buffer that is connected
    to a Tcl_Channel.  We maintain the output buffer as a regular array
    of bytes and do our output via Tcl_WriteChars. This implies that the
    Tcl I/O subsystem will do encoding as defined by the current channel encoding
    value from the assumed output stream of utf-8 characters.

    - This implementation does not support Tcl_Channels in non blocking mode.
    - The assumption is that the underlying stream lib will flush the stream
      on destruction.
    - Destroying the streambuf does not imply a Tcl_Close, or even a Tcl_Flush
      although presumably the I/O subsystem will ensure that the buffer is
      at least written to the channel.


*/

class tcloutbuf  : public std::streambuf {
private:
  Tcl_Channel        m_Channel;	             // Channel we are connected to.
  static const int   m_BufferLength = 1024;  // Size of the output buffer.
  char               m_Buffer[m_BufferLength];
public:
  tcloutbuf(Tcl_Channel Channel);
  virtual ~tcloutbuf();
protected:
  virtual int_type overflow(int_type c = EOF);

};

/*!
    tclinbuf represents a C++ Input stream buffer that is connected
    to a Tcl_Channel.   We maintain an input buffer allocated and filled
    as needed from calls to Tcl_ReadChars().  Note that this function, by default
    decodes characters from the data source to utf-8.

    Note that destruction does not imply closing the Tcl_Channel.
*/
class tclinbuf : public std::streambuf {
private:
  Tcl_Channel    m_Channel;	  // Channel we're connected to.
  int            m_nPutBackSize;  // Size of putback buffer.
  static const int m_nReadSize=1; // Otherwise interleaving Tcl_Read and this will fail.
  char*          m_pBuffer;	  // This will point to the input buffer.
public:
  tclinbuf(Tcl_Channel Channel, 
	   int         nPutbackSize=128);
  virtual ~tclinbuf();
protected:
  virtual int_type underflow();

};


/*
   Streams can be constructed on these now:
*/

/*!
     Output stream based on Tcl_Channel
*/

class tclostream : public std::ostream {
protected:
  tcloutbuf   m_Buffer;
public:
  tclostream(Tcl_Channel chan) : 
    std::ostream(0), 
    m_Buffer(chan) {
    rdbuf(&m_Buffer);
  }
};


/*!
   Input stream based on Tcl_Channel
*/
class tclistream : public std::istream {
protected:
  tclinbuf m_Buffer;
public:
  tclistream(Tcl_Channel chan, int nPutBackSize=128) :
    std::istream(0),
    m_Buffer(chan, nPutBackSize) {
    rdbuf(&m_Buffer);
  }
};

#endif
