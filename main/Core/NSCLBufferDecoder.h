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

//  CNSCLBufferDecoder.h:
//
//    This file defines the CNSCLBufferDecoder class.
//
// Author:
//    Ron Fox
//    NSCL
//    Michigan State University
//    East Lansing, MI 48824-1321
//    mailto:fox@nscl.msu.edu
//
//
/////////////////////////////////////////////////////////////

#ifndef NSCLBUFFERDECODER_H  //Required for current class
#define NSCLBUFFERDECODER_H

#include "BufferDecoder.h"
#include <string>

class CNSCLBufferDecoder : public CBufferDecoder {
  STD(string) m_sTitle;
 public:
  // Constructors:
  CNSCLBufferDecoder() : CBufferDecoder() {} // Default Constructor.
  virtual ~CNSCLBufferDecoder() {} // Destructor.
private:
  CNSCLBufferDecoder(const CNSCLBufferDecoder& aCNSCLBufferDecoder);
  CNSCLBufferDecoder& operator=(const CNSCLBufferDecoder& aCNSCLBufferDecoder);
public:

  int operator==(const CNSCLBufferDecoder& aCNSCLBufferDecoder) { // Equality.
    return ((CBufferDecoder::operator== (aCNSCLBufferDecoder)));
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
};

#endif
