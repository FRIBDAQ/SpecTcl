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



#ifndef __FILTERBUFFERDECODER_H
#define __FILTERBUFFERDECODER_H

// Headers:


#ifndef __BUFFERDECODER_H
#include <BufferDecoder.h>
#endif

#ifndef __HISTOTYPES_H
#include <histotypes.h>
#endif

#ifndef __BUFFERTRANSLATOR
#include <BufferTranslator.h>
#endif

#ifndef __TRANSLATORPOINTER
#include <TranslatorPointer.h>
#define __TRANSLATORPOINTER
#endif

#ifndef __STL_STRING
#include <string>
#ifndef __STL_STRING
#define __STL_STRING
#endif
#endif

// forward definitions:

class CXdrMemInputStream;
class CXdrInputStream;
// Class definition.

/*!
   This class is a buffer decoder for output data from event filters.
   We process data that consists of output from a CXdrOutputStream.  The
   Buffers contain textually tagged data items.  

   Two tags are currently defined;
   - "header" - Indicating a header event that contains a description of
                the parameters present in the filtered event stream.
		This can only occur as the first event in a buffer.
		It is mapped to buffer type 6000, causing a call to 
		CAnalyzer::OnOther
   - "event"  - Contains a single event and a bitmask indicating which
                parameters were valid for the event.  Runs of these events
		are passed to CAnalyzer::OnPhysics for processing.

   Detailed format follows:

   header:
   \verbatim
   +--------------------------+
   | "header"                 | Xdr string
   +--------------------------+
   | NumParameters            | Xdr Integer
   +--------------------------+
   |  Name(0)                 | Xdr string
          ...
   |  Name(NumParameters-1)   | Xdr String
   +--------------------------+

   \endverbatim

   event:

   \verbatim
   +-----------------------------+
   |   "event"                   | Xdr string
   +-----------------------------+
   | Bitmask(0)                  | Xdr int
         ...                          ...
   | Bitmask((NumParameters+7)/8 | Xdr int
   +-----------------------------+
   |Parameter for lowest set bit | Xdr float
          ...
   |Parameter for highest set bit| Xdr float
   +-----------------------------+

   \endverbatim


*/

class CFilterBufferDecoder : public CBufferDecoder
{
  // Member data:

private:
  char*    m_pTranslated;	//!< XDR gets translated to here.
  Int_t    m_nParamCount;	//!< Number of parameters in an event.
  Int_t    m_nBytes;		//!< Used size of m_pTranslated.
  UShort_t m_nCurrentType;	//!< Current buffer type.
  UShort_t m_nEntities;		//!< How many items in m_pTranslated.
  Bool_t   m_fSeenHeader;	//!< true once we've seen a header.
  BufferTranslator* m_pTranslator; //!< Buffer translator we hand out.
  bool     m_isSingle;          //!< true if buffer is single precision.


  // Constructors and other canonical functions.
public:

  CFilterBufferDecoder();
  CFilterBufferDecoder(const CFilterBufferDecoder& rhs);
  virtual ~CFilterBufferDecoder();

  CFilterBufferDecoder& operator=(const CFilterBufferDecoder& rhs);
  int            operator==(const CFilterBufferDecoder& rhs);
  int            operator!=(const CFilterBufferDecoder& rhs);

  // Functions of the class. These must be implemented by all buffer
  // Decoders.

public:
  virtual const Address_t getBody();
  virtual UInt_t getBodySize();
  virtual UInt_t getRun();
  virtual UInt_t getEntityCount();
  virtual UInt_t getSequenceNo();
  virtual UInt_t getLamCount();
  virtual UInt_t getPatternCount();
  virtual UInt_t getBufferType();
  virtual void   getByteOrder(Short_t& Sig16,
			  Int_t& Sig32);
  virtual std::string getTitle();
  
  virtual void operator()(UInt_t nBytes,
			  Address_t pBuffer,
			  CAnalyzer& rAnalyzer);
  virtual BufferTranslator* getBufferTranslator();
private:
  // Utility functions:
  
  void CopyIn(const CFilterBufferDecoder& rhs);

  void TranslateBuffer(CXdrInputStream& xdr, 
		       UInt_t           nBytes,
		       CAnalyzer&       rAnalyzer);
  void ProcessHeader(CXdrInputStream& xdr,
		     CAnalyzer&       rAnalyzer);
  void ProcessEvents(CXdrInputStream& xdr,
		     CAnalyzer&       rAnalyzer);
  static int CountBits(unsigned mask);
  bool   dataWidth(UInt_t nBytes, void* pBuffer);
};

#endif
