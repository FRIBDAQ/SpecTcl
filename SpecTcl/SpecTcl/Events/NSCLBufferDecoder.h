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
//  Copyright 1999 NSCL, All Rights Reserved.
//
/////////////////////////////////////////////////////////////

#ifndef __NSCLBUFFERDECODER_H  //Required for current class
#define __NSCLBUFFERDECODER_H
                               //Required for base classes
#ifndef __BUFFERDECODER_H
#include "BufferDecoder.h"
#endif                               
                               
class CNSCLBufferDecoder  : public CBufferDecoder        
{
  string m_sTitle;
public:
			//Default constructor

  CNSCLBufferDecoder () : CBufferDecoder() { } 
  ~ CNSCLBufferDecoder ( ) { }       //Destructor

  // Copy constructor.

  CNSCLBufferDecoder (const CNSCLBufferDecoder& aCNSCLBufferDecoder )   : 
    CBufferDecoder (aCNSCLBufferDecoder) 
  { 
                
  }                                     

			//Operator= Assignment Operator

  CNSCLBufferDecoder& operator= (const CNSCLBufferDecoder& aCNSCLBufferDecoder)
  { 
    if (this == &aCNSCLBufferDecoder) return *this;          
    CBufferDecoder::operator= (aCNSCLBufferDecoder);
    return *this;
  }                                     

			//Operator== Equality Operator

  int operator== (const CNSCLBufferDecoder& aCNSCLBufferDecoder)
  { 
    return ( (CBufferDecoder::operator== (aCNSCLBufferDecoder)));
  }                             
     
public:

  // Pseudo selectors:

  virtual   const Address_t getBody ()  ;
  virtual   UInt_t getBodySize ()   ;
  virtual   UInt_t getRun ()   ;
  virtual   UInt_t getEntityCount ()   ;
  virtual   UInt_t getSequenceNo ()   ;
  virtual   UInt_t getLamCount ()   ;
  virtual   UInt_t getPatternCount ()   ;
  virtual   UInt_t getBufferType ()   ;
  virtual   void getByteOrder (Short_t& Signature16, 
			       Int_t& Signature32)   ;
  virtual   string getTitle();

  virtual   void operator() (UInt_t nBytes, Address_t pBuffer, 
			     CAnalyzer& rAnalyzer)  ;

};

#endif
