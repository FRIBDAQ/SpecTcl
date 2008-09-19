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

#include <config.h>
#include "CRingBufferDecoder.h"
#include "DataFormat.h"

#include <Globals.h>
#include <Analyzer.h>
#include <BufferTranslator.h>

#include <string.h>
#include <stdlib.h>

#include <buftypes.h>
#include <map>


using namespace std;

//  Constants:

static const Short_t UNSWAPPED_SHORTSIG(0x0102);
static const Int_t   UNSWAPPED_LONGSIG(0x01020304);

static const Short_t SWAPPED_SHORTSIG(0x0201);
static const Int_t   SWAPPED_LONGSIG(0x04030201);


////////////////////////////////////////////////////////////////////////////////////
// Canonical functions.
//

/*!
  Construction the pointers are mostly what we need to initialize.
  Specifically, m_pBuffer to indicate there is no buffer,and m_pPartialEvent
  to indicate there's no spanning event being constructed.
*/

CRingBufferDecoder::CRingBufferDecoder() :
  m_pBuffer(0),
  m_pPartialEvent(0),
  m_pTranslator(0),
  m_runNumber((UInt_t)-1)
{

}
/*!
   Destructor  IF ther's a buffer assembly in process, its storage should
   be released.
*/
CRingBufferDecoder::~CRingBufferDecoder()
{
  delete m_pPartialEvent;
}

//////////////////////////////////////////////////////////////////////////////////
//
//  Implementations of function overrides.
//

/*!
   The function call operator is called when a buffer is received from the data source.
   Our mission is to pull elements from the buffer and dispatch them to appropriate analyzer
   member functions.
   \param nBytes     - Number of bytes in the buffer.  In our case this is also the number of
                       actual bytes to process.
   \param pBuffer    - Pointer to the buffer recieved from the data source.
   \param rAnalyzer  - Reference to the analyzer whose members we must call.

*/
void 
CRingBufferDecoder::operator()(UInt_t nBytes, Address_t pBuffer, CAnalyzer& rAnalyzer)
{
  m_pAnalyzer     = &rAnalyzer;
  m_pBuffer       = pBuffer;
  m_pBufferCursor = pBuffer;
  m_nBufferSize   = nBytes;
  m_nResidual     = nBytes;

  // Translators can only be created if we are looking at the front of an item.. otherwise
  // they've already been created.
  //
  
  if (!m_pPartialEvent) {
    createTranslator();
  }
  else {
    m_pTranslator->newBuffer(m_pBuffer);
  }

  // Now work on the data we got.

  processBuffer();
}

/*!
   Get a pointer to the current event body.  Note that the call sequences are such that
   there will be a valid m_pBody to give the caller.
   \return const Address_t
   \retval pointer to the body of an item in the ring buffer.
*/
const Address_t
CRingBufferDecoder::getBody()
{
  return m_pBody;
}
/*!
   Return the size of the body of an item.  As with getBody, the call sequences 
   ensure that the body size is valid.
   \return UInt_t 
   \retval Number of bytes in the body that has most recently been dispatched to the analyzer.
*/
UInt_t
CRingBufferDecoder::getBodySize()
{
  return m_nBodySize;
}
/*!
   Return the run number as we know it. Note that having an accurate/meaningful run number
   depends on us having seen at least one state transition item, as they are the only carriers
   of this information.  If we have never seen one, a number with all bits set is returned.

   \return UInt_t
   \retval Current run number if we saw a run state transition item for it.
   \retval Value with all ones if we have never seen a run state transition.

*/
UInt_t
CRingBufferDecoder::getRun()
{
  return m_runNumber;
}

/*!
   Return the entity count for the item.  In  most cases this is 1 as we dispatch
   a single item at a time.  For scaler items, however, it is the number of scalers in the item.

   \return UInt_t
   \retval number of entities in the item just dispatched.
*/
UInt_t
CRingBufferDecoder::getEntityCount()
{
  return m_nEntityCount;
}

/*!
  Get the number of triggers seen by the data source so far. The buffer based NSCL DAQ
  system had buffer sequence numbers, and these were used by many SpecTcl GUI's to compute
  the analysis efficiency.  To support a similar computation, the ring buffer system defines
  a trigger count item type that can be periodically emitted.  

  The return value from this will be an estimate of the number of triggers accepted by the
  system.  When a trigger count item is received, the trigger count stored by this
  object is set to that value. Between trigger counts, it is incremented by one for each physics
  data item so that we don't see artificial efficiency strangeness between them.

  \return UInt_t
  \retval Estimate of the number of triggers that have been received by the system.

*/
UInt_t
CRingBufferDecoder::getSequenceNumber()
{
  return m_nTriggerCount;
}

/*!
   Returns the LAM register count.. returns a hard coded zero as this information
   is not available.. and usually irelevant for a ring buffer based DAQ system.
   \return UInt_t
   \retval 0
*/
UInt_t
CRingBufferDecoder::getLamCount()
{
  return 0;
}
/*!
   Return the bit pattern count.  As with the lam mask, this data are usually not relevant and
   are not supplied in the ring buffer system, so 0 is returned

   \return UInt_t
   \retval 0
*/
UInt_t
CRingBufferDecoder::getPatternCount()
{
  return 0;
}
/*!
   Return the type of the current item.  The call sequences ensure this is well defined
   when called.
   \return UInt_t
   \retval Type of item being dispatched.
*/
UInt_t
CRingBufferDecoder::getBufferType()
{
  return m_nCurrentItemType;
}
/*!
   Return the byte order.  This is returned as a pair of 
   byte order signature elements.  We do this by examining the
   type of translator we have.  If it's a swapping translator
   we returned byte swapped signatures.  If it's a nonswapping 
   translator, we return normal signatures.

   \param[out] signature16  - the 16 bit byte order signature is stored here.
   \param[out] signature32  - The 32 bit byte order signature is stored here.
*/
void
CRingBufferDecoder::getByteOrder(Short_t& signature16,
				 Int_t&   signature32)
{
  if (dynamic_cast<SwappingBufferTranslator*>(m_pTranslator)) {
    signature16 = SWAPPED_SHORTSIG;
    signature32 = SWAPPED_LONGSIG;
  }
  else {
    signature16 = UNSWAPPED_SHORTSIG;
    signature32 = UNSWAPPED_LONGSIG;
  }
}

/*!
   Return the title most recently seen. Run titles are sent in state transition
   items.  If no state transition item has been seen, an empty string is returned
   \return std::string
   \retval The most recently received title.
   \retval empty string if no title has ever been seen.
*/
void
CRingBufferDecoder::getTitle()
{
  return m_title;
}
///////////////////////////////////////////////////////////////////
//
// Non-public utility functions:
//

/*
** Creates the correct translator.  At this point in time, 
** m_pBuffer points to the beginning of an item.
** The buffer translator that currently exists is destroyed.
**
**
*/
void
CRingBufferDecoder::createTranslator()
{
  pRingItem pItem = reinterpret_cast<pRingItem>(m_pBuffer);

  delete m_pTranslator;
  if (*pItem & 0xffff0000) {
    m_pTranslator = new SwappingBufferTranslator(m_pBuffer);
  }
  else {
    m_pTranslator = new NonSwappingBufferTranslator(m_pBuffer);
  }
}

/*
**  Process an input buffer.
**  If partial event processing is in progress, first try to satisfy/dispatch
**  that.  Otherwise, dispatch events from the buffer to the client analyzer
** (m_pAnalyzer).
*/

void
CRingBufferDecoder::processBuffer()
{
  // If a partial event is being assembled across buffer boundaries
  // work on that first.


  if (m_pPartialEvent) {
    UInt_t remaining = m_nPartialEventSize - m_nPartialEventBytes;
    UInt_t append    = m_nBufferSize >= remaining ? remaining : m_nBufferSize;

    m_pPartialEvent = realloc(m_pPartialEvent, append + m_partialEventBytes);
    uint8_t* p      = reinterpret_cast<uint8_t*>(m_pPartialEvent) + m_nPartialEventBytes;
    memcpy(p, m_pBuffer, append);

    m_nPartialEventBytes += append;
    m_nResidual          -= append;
    m_pBufferCursor       = reinterpret_cast<uint32_t*>(reinterpret_cast<uint8_t*> + append);

    if (m_nPartialEventSize <= m_nPartialEventBytes) {
      dispatchPartialEvent();
    }
  }
  // At this point if there's anything left in the buffer (at m_pBufferCursor),
  // we must be starting at the beginning of an item.
  // 
  
  while(m_nResidual) {
    pRingItemHeader pItemHeader = reinterpret_cast<pRingItemHeader>(m_pBufferCursor);
    uint32_t size = m_pTranslator->TranslateLong(pItemHeader->s_size);

    if (size > m_nResidual) {
      // Full event does not fit in the remainder of the buffer..

      createPartialEvent();
      m_nResidual = 0;		// By definition we're done with the buffer.
    }
    else {
      // Full event fits in the remainder of the buffer:

      dispatchEvent(m_pBufferCursor);
      m_pBufferCursor = reinterpret_cast<uint32_t*>(reinterpret_cast<uint8_t*> + size);
      m_nResidual    -= size;
    }
  }
  
}
/*
** Dispatching a partial event is just a matter of using dispatchEvent pointing at the
** assembled event, and then killing off the storage to the assembled event, then
** setting the pointer to zero so everyone knows its actually gone:
*/
void
CRingBufferDecoder::dispatchPartialEvent()
{
  dispatchEvent(m_pPartialEvent);
  free(m_pPartialEvent);
  m_pPartialEvent = reinterpret_cast<uint32_t*>(NULL);

}
/*
  Dispatching an event to the user is a matter of getting the type,
  Filling in the type independent information and then doing type dependent processing
  to invoke the proper analyzer function.
*/
void
CRingBufferDecoder::dispatchEvent(void* pEvent)
{
  // extract the type of the event and the total size:

  pRingItem*       pItem = reinterpret_cast<pRingItemHeader>(pEvent);
  uint32_t         size  = m_pTranslator->TranslateLong(pItem->s_header.s_size);
  uint32_t         type  = m_pTranslator->TranslateLong(pItem->s_header.s_type);
  m_pTranslator->newBuffer(pItem);
  m_pBody                = (pItem->s_body);
  m_nBodySize            = size - sizeof(RingItemHEader);
  m_nCurrentItemType     = mapType(type);

  
  // The remainder of this is item type dependent:


  switch (type) {
  case BEGIN_RUN:
  case END_RUN:
  case PAUSE_RUN:
  case RESUME_RUN:
    {
      pStateChangeItem pState = reinterpret_cast<pStateChangeItem>(pItem);
      m_title        = pState->s_title;
      m_runNumber    = m_pTranslator->TranslateLong(pState->s_runNumber);
      m_nEntityCount = 0;
      m_pAnalyzer->OnStateChange(m_nCurrentItemType, *this);
    }
    break;

  case PACKET_TYPES:
  case MONITORED_VARIABLES:
    {
      pTextItem pText = reinterpret_cast<pTextItem>(pItem);
      m_nEntityCount  = m_pTranslator->TranslateLong(pText->s_stringCOunt);
      m_pAnalyzer->OnOther(m_nCurrentItemType, *this);
    }
    break;
  case INCREMENTAL_SCALERS:
    {
      pScalerItem pScalers = reinterpret_cast<pScalerItem>(pItem);
      m_nEntityCount = m_pTranslator->TranslateLong(pScalers->s_scalerCount);
      m_pAnalyzer->OnScaler(*this);
    }
    break;
  case PHYSICS_EVENT:
    {
      m_nEntityCount = 1;
      m_nTriggerCount++;	// The system has had at least one more trigger.
      m_pAnalyzer->OnPhysics(*this);
    }
    break;
  case PHYSICS_EVENT_COUNT:

    {
      pPhysicsEventCountItem pTriggers = reinterpret_cast<pPhysicsEventCountItem>(pItem);
      m_nTriggerCount = m_pTranslator->TranslateLong(pTriggers->s_eventCount);
      m_pAnalyzer->OnOther(m_nCurrentItemType, *this);
    }
    break;

    // The default is just a call to onOther:
  default:
    m_pAnalyzer->OnOther(m_nCurrentItemType, *this);
    break;
  }
  

}

/*
** This function deals with the fact that ring item types are not necessarily the
** same as the item types spectcl expects.
** The item types that have mapping to old fashioned NSCL buffer types that
** SpecTcl expected are mapped to them here.
*/
UInt_t
CRingBUfferDecoder::mapType(UInt_t type)
{
  static bool                mapSetup(false);
  static map<int, int>  typeMapping;

  if (!mapSetup) {
    typeMapping[BEGIN_RUN]           = BEGRUNBF;
    typeMapping[END_RUN]             = ENDRUNBF;
    typeMapping[PAUSE_RUN]           = PAUSEBF;
    typeMapping[RESUME_RUN]          = RESUMEBF;
    typeMapping[PACKET_TYPES]        = PKTDOCBF;
    typeMapping[MONITORED_VARIABLES] = RUNVARBF;
    typeMapping[INCREMENTAL_SCALERS] = SCALERBF;
    typeMapping[PHYSICS_EVENT]       = DATABF;
      
    mapSetup = true;
  }
  if (typeMapping.find(type) != typeMapping.end()) {
    return typeMapping[type];
  }
  else {
    return type;
  }


}
