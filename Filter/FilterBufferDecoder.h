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

/*
  Change log:
  $Log$
  Revision 5.3  2006/10/10 13:45:16  ron-fox
  BZ217 - Add some additional sanity checking in the filter event decoder
          to attempt to detect when it has been pointed to a data source that
  	is not a filter file.  In that case, the processor will signal an
  	end of file in the run control handler, and print an error message to
  	stderr.
  - Mods to files other than FilterBufferDecoder.cpp are just large->small
    license edits.

  Revision 5.2  2005/06/03 15:19:03  ron-fox
  Part of breaking off /merging branch to start 3.1 development

  Revision 5.1.2.1  2004/12/21 17:51:20  ron-fox
  Port to gcc 3.x compilers.

  Revision 5.1  2004/11/29 16:56:00  ron-fox
  Begin port to 3.x compilers calling this 3.0

  Revision 1.5.4.1  2004/08/30 15:33:04  ron-fox
  Merged with modifications for fixed filter.

  Revision 1.5  2003/10/24 14:42:55  ron-fox
  Get a working filter play back subsystem that
  does break the way events are normally
  processed.

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
  virtual STD(string) getTitle();
  
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
};

#endif
