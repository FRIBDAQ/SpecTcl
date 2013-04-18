/*
    This software is Copyright by the Board of Trustees of Michigan
    State University (c) Copyright 2008

    You may use this software under the terms of the GNU public license
    (GPL).  The terms of this license are described at:

     http://www.gnu.org/licenses/gpl.txt

     Author:
             Ron Fox
	     NSCL
	     Michigan State University
	     East Lansing, MI 48824-1321
*/


#ifndef __CRINGBUFFERDECODER_H
#define __CRINGBUFFERDECODER_H

#ifndef __BUFFERDECODER_H
#include <BufferDecoder.h>
#endif

#ifndef __CRT_STDINT_H
#include <stdint.h>
#ifndef __CRT_STDINT_H
#define __CRT_STDINT_H
#endif
#endif

#ifndef __STL_STRING
#include <string>
#ifndef __STL_STRING
#define __STL_STRING
#endif
#endif

class BufferTranslator;
class CRingFormatHelper;
class CRingFormatHelperFactory;
/*!
   This class defines a buffer decoder that knows how to take data from
   the NSCL DAQ Ring buffer distribution system.  This includes event
   files from the Ring buffer system.  

   The ring buffer system delivers a stream of events, rather than fixed
   length buffers.  Therefore, this code must be able to handle events that
   span buffer boundaries; including events that live in more than two buffers.

   Events are defined in the DataFormat.h header.  Suffice to say the consist
   of a size and a type word both 32 bits long.  The type must live in 16 bits,
   and therefore gives an indication of the endian-ness of the data source relative
   to the processor executing this code (if the bottom 16 bits are 0, we need to swap).

*/

class CRingBufferDecoder : public CBufferDecoder
{
  // Objedt member data:
private:
  uint32_t*    m_pBuffer;	// Buffer being processed.
  uint32_t*    m_pBufferCursor;	// Cursor into buffer being processed.
  uint32_t     m_nBufferSize;   // Bytes in the buffer.
  uint32_t     m_nResidual;     // Bytes remaining to be processed in buffer.
  uint32_t*    m_pPartialEvent; // If non-zero pointer to partial event being assembled.
  uint32_t     m_nPartialEventSize;  // Expected size of the partial event.
  uint32_t     m_nPartialEventBytes; // Bytes we have already gotten in the partial event.
  BufferTranslator* m_pTranslator;
  std::string  m_title;		// Last title gotten from a transition event.
  UInt_t       m_runNumber;     // Last run number   ""    ""
  Address_t    m_pBody;         // Pointer to body of event being delivered to analyzer.
  UInt_t       m_nBodySize;     // Pointer to size of body of event being delivered.
  UInt_t       m_nCurrentItemType; // Item type of event being delivered.
  UInt_t       m_nEntityCount;  // Number of items in event being delivered.
  UInt_t       m_nTriggerCount; // Estimate of the number of triggers so far.
  uint32_t*    m_pGluedBuffer;  // Buffer glued together from last and current buffer.
  uint32_t     m_nGlueSize;     // # bytes in the glued buffer when first made.

  CAnalyzer*         m_pAnalyzer;       // Pointer to the associated analyzer.
  CRingFormatHelper* m_pCurrentHelper;  // Pointer to the current format helper.
  CRingFormatHelper* m_pDefaultHelper;  // Pointer to a default format helper.
  CRingFormatHelper* m_pFallbackHelper;  // 'hard coded' format helper.
  void*              m_pCurrentRingItem; // Item the access methods work on.

  CRingFormatHelperFactory* m_pFactory;

  // Canonical operations:

public:
  CRingBufferDecoder();
  virtual ~CRingBufferDecoder();
  
private:
  CRingBufferDecoder(const CRingBufferDecoder& rhs);
  CRingBufferDecoder& operator=(const CRingBufferDecoder& rhs);
  int operator==(const CRingBufferDecoder& rhs) const;
  int operator!=(const CRingBufferDecoder& rhs) const;
public:
  // Overrides of base class functionality (virtually everything is an override.

  virtual void operator()(UInt_t nBytes, Address_t pBuffer, CAnalyzer& rAnalyzer);

  virtual const Address_t getBody();
  virtual UInt_t getBodySize();
  virtual UInt_t getRun();
  virtual UInt_t getEntityCount();
  virtual UInt_t getSequenceNo();
  virtual UInt_t getLamCount();
  virtual UInt_t getBufferType();
  virtual UInt_t getPatternCount();
  virtual void   getByteOrder(Short_t& signature16,
			      Int_t&   signature32);
  virtual std::string    getTitle();
  virtual BufferTranslator* getBufferTranslator();

  virtual bool blockMode();	// True if data source must deliver fixed sized blocks.
  
  // Format helpers and stuff that gets you at what they know.
  

  bool  hasBodyHeader();
  void* getBodyHeaderPointer();
  void* getItemPointer();

  void  setFormatHelper(CRingFormatHelper* pHelper);
  void  setDefaultFormatHelper(CRingFormatHelper* pHelper);
  CRingFormatHelper* getCurrentFormatHelper();
  CRingFormatHelper* getDefaultFormatHelper();
  
  // Members called that can invalidate the format helper:
  
  virtual void OnSourceAttach();
  virtual void OnSourceDetach();
  virtual void OnEndFile();
  


private:
  void createTranslator();
  void processBuffer();
  void dispatchPartialEvent();
  void dispatchEvent(void* pEvent);
  UInt_t mapType(UInt_t type);
  void createPartialEvent();
  CRingFormatHelper* getFormatHelper();
  void invalidateCurrentHelper();
};


#endif
