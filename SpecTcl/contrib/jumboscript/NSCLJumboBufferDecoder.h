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

//  CNSCLJumboBufferDecoder.h:
//
//    This file defines the CNSCLJumboBufferDecoder class.
//
// Author:
//    Ron Fox
//    NSCL
//    Michigan State University
//    East Lansing, MI 48824-1321
//    mailto:fox@nscl.msu.edu
//
//  Copyright 1999 NSCL, All Rights Reserved.
//
/////////////////////////////////////////////////////////////

#ifndef __NSCLJUMBOBUFFERDECODER_H  //Required for current class
#define __NSCLJUMBOBUFFERDECODER_H
//Required for base classes
#ifndef __JUMBOBUFFERDECODER_H
#include "BufferDecoder.h"
#endif                               
                               
#ifndef __STL_STRING
#include <string>
#ifndef __STL_STRING
#define __STL_STRING
#endif
#endif
//
// This is a test class for decoding buffers that
// have version  6 or higher.  Those buffers have
// an additional high 16 bits  of word count located
// in the 14'th word of the buffer.
//
class CNSCLJumboBufferDecoder : public CBufferDecoder {
  STD(string) m_sTitle;
 public:
  // Constructors:
  CNSCLJumboBufferDecoder() : CBufferDecoder() {} // Default Constructor.
  virtual ~CNSCLJumboBufferDecoder() {} // Destructor.
private:
  CNSCLJumboBufferDecoder(const CNSCLJumboBufferDecoder& aCNSCLJumboBufferDecoder);
  CNSCLJumboBufferDecoder& operator=(const CNSCLJumboBufferDecoder& aCNSCLJumboBufferDecoder);
public:

  int operator==(const CNSCLJumboBufferDecoder& aCNSCLJumboBufferDecoder) { // Equality.
    return ((CBufferDecoder::operator== (aCNSCLJumboBufferDecoder)));
  }

public:
  // Pseudo selectors:
  virtual const Address_t getBody();
  virtual UInt_t getBodySize();
  virtual UInt_t getRun();
  virtual UInt_t getEntityCount();
  virtual UInt_t getSequenceNo();
  virtual UInt_t getLamCount();
  virtual UInt_t getPatternCount();
  virtual UInt_t getBufferType();
  virtual void getByteOrder(Short_t& Signature16, Int_t& Signature32);
  virtual STD(string) getTitle();


  virtual void operator()(UInt_t nBytes, Address_t pBuffer, CAnalyzer& rAnalyzer);

  // Specific to jumbo buffers:
public:
  bool size32();		// True if sizes are 32 bits big.
};

#endif
