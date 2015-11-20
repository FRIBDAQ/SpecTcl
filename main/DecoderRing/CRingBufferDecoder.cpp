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
#include "RingFormatHelper.h"
#include "RingFormatHelper10.h"       // Last chance helper.

#include "RingFormatHelper10Creator.h"
#include "RingFormatHelper11Creator.h"

#include "RingFormatHelperFactory.h"
#include "DataFormat.h"
#include "DataFormatPre11.h"

#include <Analyzer.h>
#include <BufferTranslator.h>

#include <string.h>
#include <stdlib.h>
#include <stdint.h>

#include <buftypes.h>
#include <map>

#include <iostream>
#include <fstream>
#include <iomanip>


using namespace std;

//  Constants:

static const Short_t UNSWAPPED_SHORTSIG(0x0102);
static const Int_t   UNSWAPPED_LONGSIG(0x01020304);

static const Short_t SWAPPED_SHORTSIG(0x0201);
static const Int_t   SWAPPED_LONGSIG(0x04030201);


// local function to dump data to an ostream:
// o - the stream to dump to (reference)
// p - Pointer to the data to dump
// s - Number of bytes to dump.
//
static void
dump(ostream& o, void* p, size_t s)
{

  ios::fmtflags   oldFlags = o.flags();
  streamsize oldWidth = o.width();
  char       oldFill  = o.fill();
  o << hex;
  o << right << setw(4) << setfill('0'); 

  
  

  uint16_t* pout = reinterpret_cast<uint16_t*>(p);

  o << 0 << ' ';
  for (int i =0; i < s; i++) {
    o << *pout++ <<  ' ';
    if (((i+1) % 8) == 0) {
      o << endl;
    }
  }
  o << endl;
  

  o << dec;
  o.flags(oldFlags);
  o.width(oldWidth);
  o.fill(oldFill);
}


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
  m_runNumber((UInt_t)-1),
  m_pGluedBuffer(0),
  m_pCurrentHelper(0),
  m_pDefaultHelper(0),
  m_pCurrentRingItem(0),
  m_pFallbackHelper(new CRingFormatHelper10),
  m_pFactory(new CRingFormatHelperFactory)
{
    // Register the creators we know about:
    // Adding a creator does a copy so this automatic creation is fine.
    
    CRingFormatHelper10Creator create10;
    CRingFormatHelper11Creator create11;
    
    m_pFactory->addCreator(10, 0, create10);
    m_pFactory->addCreator(10, 1, create10);  // 10.x are all the same.
    m_pFactory->addCreator(10, 2, create10);
    
    m_pFactory->addCreator(11, 0, create11);  // 11.x has body headers.
}
/*!
   Destructor  IF ther's a buffer assembly in process, its storage should
   be released.
*/
CRingBufferDecoder::~CRingBufferDecoder()
{
  delete m_pPartialEvent;
  delete m_pCurrentHelper;
  delete m_pDefaultHelper;
  delete m_pFallbackHelper;
  
  delete m_pFactory;

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
  m_pBuffer       = reinterpret_cast<uint32_t*>(pBuffer);
  m_pBufferCursor = m_pBuffer;
  m_nBufferSize   = nBytes;
  m_nResidual     = nBytes;

  // Translators can only be created if we are looking at the front of an item.. otherwise
  // they've already been created.
  //
  
  try {
    if (!m_pPartialEvent && !m_pGluedBuffer) {
      createTranslator();
    }
    else {
      m_pTranslator->newBuffer(m_pBuffer);
    }
    
    // Now work on the data we got.
    
    processBuffer();
  }
  catch (...) {
    cerr << "SpecTcl exiting due to buffer decoder exception\n";
    exit(EXIT_FAILURE);
  }
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
CRingBufferDecoder::getSequenceNo()
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
string
CRingBufferDecoder::getTitle()
{
  return m_title;
}

/*!
  Returns the buffer translator to the caller.
  this allows the caller to construct translator pointers as well.
  \return BufferTranslator*
  \retval Pointer to the most recently constructed buffer translator
*/
BufferTranslator*
CRingBufferDecoder::getBufferTranslator()
{
  return m_pTranslator;
}
/*!
   Returns false idicating that the data source is not going to be
   necessarily providing fixed length data buffers.  This is required to
   ensure that SpecTcl does not block indefinitely on a read when 
   runs end due to them not ending on block boundaries.
*/
bool
CRingBufferDecoder::blockMode()
{
  return false;
}

/**
 * hasBodyHeader
 *
 * Determine if the current item has a body header.
 *
 * @return bool
 * @retval true - a current item is defined and has a body header.
 * @retval false - A current item is not defined or is defined but has no
 *                 body header.
 */
bool
CRingBufferDecoder::hasBodyHeader()
{
    if (m_pCurrentRingItem) {
        CRingFormatHelper* pHelper = getFormatHelper();
        return pHelper->hasBodyHeader(m_pCurrentRingItem);
    } else {
        return false;
    }
}
/**
 * getBodyHeaderPointer
 *
 * Returns a pointer to the current ring items' body header.  See below however.
 *
 * @return void* - Pointer to the item's body pointer.
 * @retval null  - If hasBodyHeader() returns false.
 */
void*
CRingBufferDecoder::getBodyHeaderPointer()
{
    if (!hasBodyHeader()) return reinterpret_cast<void*>(0);
    
    CRingFormatHelper* pHelper  = getFormatHelper();
    return pHelper->getBodyHeaderPointer(m_pCurrentRingItem);
}
/**
 * getItemPointer()
 *    This returns a pointer to the currently dispatched ring item.
 *    If there is no currently dispatched ring item, a null is returned.
 */
void*
CRingBufferDecoder::getItemPointer()
{
    return m_pCurrentRingItem;
}
/**
 * setFormatHelper
 *
 * Sets the currently used format helper.  it is required that the helper have
 * been dynamically allocated.  Note that use of this method is not recommended
 * unless your data is not really coming from NSCL ring buffers...but something
 * close to it.  Better is to use setDefaultFormatHelper This is because when
 * the decoder encounters a RING_FORMAT item, it will replace the format helper
 * with one created from the information in the ring format item.
 *
 * @param pHelper - Pointer to the ring format helper to use.  This must have
 *                  been constructed via new as this will delete it
 */
void
CRingBufferDecoder::setFormatHelper(CRingFormatHelper* pHelper)
{
    m_pCurrentHelper = pHelper;
}
/**
 * setDefaultFormatHelper
 *
 *   Sets the format header to use while we don't know the format of data
 *   from the ring (have not seen a RING_FORMAT item since the last time
 *   we reset the current format helper).
 *
 *   @param pHelper - Pointer to a ring format helper.  This must have been
 *                    constructed with new.
 */
void
CRingBufferDecoder::setDefaultFormatHelper(CRingFormatHelper* pHelper)
{
    m_pDefaultHelper = pHelper;
}
/**
 * getCurrentFormatHelper
 *
 * Returns a pointer to the current format helper.  Note that if there is not
 * one yet, this returns a null.  Receiving a null does not imply that
 * a ring item will cause the decoder to fail as there is also a default helper
 * potentially defined iwth setDefaultFormatHelper and a hardwired fallback
 * helper.  The fallback helper is guaranteed to exist.
 *
 * @return CRingFormatHelper*
 */
CRingFormatHelper*
CRingBufferDecoder::getCurrentFormatHelper()
{
    return m_pCurrentHelper;
}
/**
 * getDefaultHelper
 *
 * Returns a pointer to the default format helper.
 *
 * @return CRingFormatHelper*
 */
CRingFormatHelper*
CRingBufferDecoder::getDefaultFormatHelper()
{
    return m_pDefaultHelper;
}

/**
 * getFormatFactory
 *
 * @return CRingFormatHelperFactory* - pointer to the factory we used and
 *                                     in which all our creators are intalled.
 */
CRingFormatHelperFactory*
CRingBufferDecoder::getFormatFactory()
{
    return m_pFactory;
}

/*----------------------------------------------------------------------------
 * Callbacks during data analysis:
 */

/**
 * OnSourceAttach
 *    Called when a new data source is being attached.
 */
void
CRingBufferDecoder::OnSourceAttach()
{
    // A new data source invalidates the current helper:
    
   invalidateCurrentHelper();
}
/**
 * OnSourceDetach
 *
 * Called when this is being detached from the analyzer.
 */
void
CRingBufferDecoder::OnSourceDetach()
{
    invalidateCurrentHelper();
}
/**
 * OnEndFile
 *
 * Called when we reached the end of an input data source.
 */
void
CRingBufferDecoder::OnEndFile()
{
    invalidateCurrentHelper();
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
  if (pItem->s_header.s_type & 0xffff0000) {
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

   uint8_t* p      = reinterpret_cast<uint8_t*>(m_pPartialEvent) + m_nPartialEventBytes;
    memcpy(p, m_pBufferCursor, append);

    m_nPartialEventBytes += append;
    m_nResidual          -= append;
    m_pBufferCursor       = reinterpret_cast<uint32_t*>(reinterpret_cast<uint8_t*>(m_pBufferCursor) + append);

    if (m_nPartialEventSize <= m_nPartialEventBytes) {
      dispatchPartialEvent();
    }
  }
  // If we have a glued buffer then:
  // - Append the current buffer to it (we can no longer have a partial).
  // - Set the buffer cursor to point into the glue buffer rather than 
  //   the initial buffer:
  // - Add the initial glue size to the buffer size.
  // - Process normally.
  // NOTE: The glue buffer will hang around as used storage until either
  //       we have a partial event for which we have at least the header.
  //       or we have a new need for a glued buffer.
  //
  if (m_pGluedBuffer) {
    m_pGluedBuffer = reinterpret_cast<uint32_t*>(realloc(m_pGluedBuffer, 
							 m_nBufferSize + m_nGlueSize));
    uint8_t* pAppend = reinterpret_cast<uint8_t*>(m_pGluedBuffer);
    pAppend         += m_nGlueSize;
    memcpy(pAppend, m_pBuffer, m_nBufferSize);
    m_nResidual      = m_nGlueSize + m_nBufferSize;
    m_pBufferCursor  = m_pGluedBuffer;
      
  }
   
  // At this point if there's anything left in the buffer (at m_pBufferCursor),
  // we must be starting at the beginning of an item.
  // 
  
  while(m_nResidual) {
    //
    // If the residual is > header size  we can at least make a partial event
    // Otherwise we need to assemble the size using the next buffer too.
    //

    if (m_nResidual > sizeof(RingItemHeader)) {

      pRingItemHeader pItemHeader = reinterpret_cast<pRingItemHeader>(m_pBufferCursor);
      uint32_t size = m_pTranslator->TranslateLong(pItemHeader->s_size);
      if (size == 0) {
	cerr << "For some reason I think the size of an event  is 0\n";
	cerr << "Debugging information in SpecTcl-debug.txt\n";
	ofstream debug("SpecTcl-debugt.txt");
	
	debug << "----- m_pBuffer contents\n";
	dump(debug, m_pBuffer, m_nBufferSize);
	debug << "Residual: "<< m_nResidual <<endl;
	if (m_pPartialEvent) {
	  debug << "--- there's a partial event buffer of size " << m_nPartialEventSize;
	  debug << "    already processed " << m_nPartialEventBytes <<endl;
	  dump(debug, m_pPartialEvent, m_nPartialEventSize);

	}
	debug << "--- Most recent event body delivered was:\n";
	dump(debug, m_pBody, m_nBodySize);
	debug << " Last good item type: " << m_nCurrentItemType << endl;
	debug << " entity count " << m_nEntityCount << endl;
	
	if (m_pGluedBuffer) {
	  debug << "---n Glued buffer exists size " << m_nGlueSize << "\n";
	  dump(debug, m_pGluedBuffer, m_nGlueSize);
	}
	  

	throw "Failed";

      }
      
      if (size > m_nResidual) {
	// Full event does not fit in the remainder of the buffer..
	
	createPartialEvent();
	m_nResidual = 0;		// By definition we're done with the buffer.
	if (m_pGluedBuffer) {	        // Partial events are not compatible with
	  free(m_pGluedBuffer);	        // having glued buffers.
	  m_pGluedBuffer = 0;
	}
      }
      else {
	// Full event fits in the remainder of the buffer:
	
	dispatchEvent(m_pBufferCursor);
	m_pBufferCursor = reinterpret_cast<uint32_t*>(reinterpret_cast<uint8_t*>(m_pBufferCursor) + size);
	m_nResidual    -= size;

	// If we've used up the buffer, kill off any glue buffer:
	// Else hell will break loose on the next buffer.
	if (m_pGluedBuffer && !m_nResidual) {
	  free(m_pGluedBuffer);
	  m_pGluedBuffer = 0;
	}
      }
    }
    else {
      // Allocate space for and put the remainder of the current buffer into it
      // 
      uint32_t* pNewGluedBuffer = reinterpret_cast<uint32_t*>(malloc(m_nResidual));
      memcpy(pNewGluedBuffer, m_pBufferCursor, m_nResidual);

      // If there's an existing glued buffer get rid of it as it must be the one
      // we were originally processing:
      //
      if (m_pGluedBuffer) {
	free(m_pGluedBuffer);
      }
      // Either way, set m_pGluedBuffer:

      m_pGluedBuffer = pNewGluedBuffer;
      m_nGlueSize = m_nResidual;
      m_nResidual = 0;		// Buffer is all consumed.
							      
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

  pRingItem        pItem = reinterpret_cast<pRingItem>(pEvent);
  uint32_t         size  = m_pTranslator->TranslateLong(pItem->s_header.s_size);
  uint32_t         type  = m_pTranslator->TranslateLong(pItem->s_header.s_type);
  m_pTranslator->newBuffer(pItem);


  
  // If we have a ring format item that gives us the current helper:
  
  if (type == RING_FORMAT) {
    invalidateCurrentHelper();
    m_pCurrentHelper = m_pFactory->create(pItem);
  }
  // Now get the correct helper to use:
  
  CRingFormatHelper* pHelper = getFormatHelper();
  
  // From here on in we use pHelper's methods to fish stuff out of the item.
  
  m_pCurrentRingItem = pItem;    // So that callbacks can do stuff.
  m_pBody            = pHelper->getBodyPointer(pItem);
  
  m_nBodySize =
    size
    - (reinterpret_cast<uint8_t*>(m_pBody) - reinterpret_cast<uint8_t*>(pItem));
  
  // The remainder of this is item type dependent:

  m_nCurrentItemType     = mapType(type);
  switch (type) {
  case BEGIN_RUN:
  case END_RUN:
  case PAUSE_RUN:
  case RESUME_RUN:
    {
      
      m_title        = pHelper->getTitle(pItem);
      m_runNumber    = pHelper->getRunNumber(pItem, m_pTranslator);
      m_nEntityCount = 0;
      m_pAnalyzer->OnStateChange(m_nCurrentItemType, *this);
    }
    break;

  case PACKET_TYPES:
  case MONITORED_VARIABLES:
    {
      m_nEntityCount = pHelper->getStringCount(pItem, m_pTranslator);
      m_pAnalyzer->OnOther(m_nCurrentItemType, *this);
    }
    break;
  case PERIODIC_SCALERS:
    {
      
      m_nEntityCount = pHelper->getScalerCount(pItem, m_pTranslator);
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
     
      m_nTriggerCount = pHelper->getTriggerCount(pItem, m_pTranslator);
      m_pAnalyzer->OnOther(m_nCurrentItemType, *this);
    }
    break;

    // The default is just a call to onOther:
  default:
    m_pAnalyzer->OnOther(m_nCurrentItemType, *this);
    break;
  }
  m_pCurrentRingItem = 0;                    // NO longer have a current item.

}

/*
** This function deals with the fact that ring item types are not necessarily the
** same as the item types spectcl expects.
** The item types that have mapping to old fashioned NSCL buffer types that
** SpecTcl expected are mapped to them here.
*/
UInt_t
CRingBufferDecoder::mapType(UInt_t type)
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
    typeMapping[PERIODIC_SCALERS]    = SCALERBF;
    typeMapping[NSCLDAQ10::INCREMENTAL_SCALERS] = SCALERBF;
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
/*
** Create a new partial event that is filled in from the current
** buffer cursor to the end of the buffer.
*/
void
CRingBufferDecoder::createPartialEvent()
{
  // Get the full size of the event and allocate a build buffer for it:

  pRingItemHeader pHeader = reinterpret_cast<pRingItemHeader>(m_pBufferCursor);
  uint32_t    size    = m_pTranslator->TranslateLong(pHeader->s_size);

 

  m_pPartialEvent      = reinterpret_cast<uint32_t*>(malloc(size));
  m_nPartialEventSize = size;

  // Copy in what's left in the current buffer:

  memcpy(m_pPartialEvent, m_pBufferCursor, m_nResidual);
  m_nPartialEventBytes  = m_nResidual;


}
/**
 * getFormatHelper
 *
 *   Figure out which format helper to use
 *   * If m_pCurrentHelper is defined use that.
 *   * If not and m_pDefaultHelper is defined use that.
 *   * If all else fails, use m_pFallbackHelper.
 *
 * @return CRingFormatHelper*
 */
CRingFormatHelper*
CRingBufferDecoder::getFormatHelper()
{
    CRingFormatHelper* pHelper = m_pCurrentHelper;
    if (!pHelper) pHelper = m_pDefaultHelper;
    if (!pHelper) pHelper = m_pFallbackHelper;
    
    return pHelper;
}

/**
 * invalidateCurrentHelper
 *
 * Called when an event has occured that casts doubt on the validity of
 * the current helper.
 */
void
CRingBufferDecoder::invalidateCurrentHelper()
{
    delete m_pCurrentHelper;
    m_pCurrentHelper = reinterpret_cast<CRingFormatHelper*>(0);
}