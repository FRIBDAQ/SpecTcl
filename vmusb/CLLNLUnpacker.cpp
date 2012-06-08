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

#include <config.h>
#include "CLLNLUnpacker.h"
#include "CParamMapCommand.h"
#include <Event.h>
#include <Analyzer.h>
#include <TCLAnalyzer.h>
#include <BufferDecoder.h>
#include <EventFormatError.h>

// Constants:

static const uint16_t LengthMask(0x0fff);
static const uint16_t ContinuedMask(0x1000);
// CAEN definitions.

    // All data words have these bits:
    
static const uint32_t ALLH_TYPEMASK(0x7000000);
static const uint32_t ALLH_TYPESHIFT(24);
static const uint32_t ALLH_GEOMASK(0xf8000000);
static const uint32_t ALLH_GEOSHIFT(27);

    // High part of header.
    
static const uint32_t HDRH_CRATEMASK(0x00ff0000);
static const uint32_t HDRH_CRATESHIFT(16);

    // Low part of header.
    
static const uint32_t HDRL_COUNTMASK(0X3f00);
static const uint32_t HDRL_COUNTSHIFT(8);

    // High part of data:
    
static const uint32_t DATAH_CHANMASK(0x3f0000);
static const uint32_t DATAH_CHANSHIFT(16);

    // Low part of data
    
static const uint32_t DATAL_UNBIT(0x2000);
static const uint32_t DATAL_OVBIT(0x1000);
static const uint32_t DATAL_VBIT(0x40000);
static const uint32_t DATAL_DATAMASK(0x0fff);

    //  High part of trailer:
    
static const uint32_t TRAILH_EVHIMASK(0x00ff0000);

    // Word types:
    
static const uint32_t HEADER(2);
static const uint32_t DATA(0);
static const uint32_t TRAILER(4);
static const uint32_t INVALID(6);


/*!
  Construct the unpacker. The initial state is...well Initial...
*/
CLLNLUnpacker::CLLNLUnpacker() :
  m_state(Initial),
  m_size(0)
{
}
/*!  
  Dont' need anything for the destructor:
*/
CLLNLUnpacker::~CLLNLUnpacker()
{
}
/*!
   Copy constructor... just copy the member data.
*/
CLLNLUnpacker::CLLNLUnpacker(const CLLNLUnpacker& rhs) :
  CEventProcessor(rhs),
  m_state(rhs.m_state),
  m_event(rhs.m_event),
  m_size(rhs.m_size)
{
}

/*! 
    Assignment.. not much different than copy construction.
*/
CLLNLUnpacker&
CLLNLUnpacker::operator=(const CLLNLUnpacker& rhs)
{
  if (this != &rhs) {
    CEventProcessor::operator=(rhs);
    m_state            = rhs.m_state;
    m_event            = rhs.m_event;
    m_size             = rhs.m_size;
  }
  return *this;
}
/*!
   Equality if all members are identical:
*/
int
CLLNLUnpacker::operator==(const CLLNLUnpacker& rhs) const
{
  return  (CEventProcessor::operator==(rhs)                       &&
	   (m_state            == rhs.m_state)                    &&
	   (m_event            == rhs.m_event)                    &&
	   (m_size             == rhs.m_size));

}
int
CLLNLUnpacker::operator!=(const CLLNLUnpacker& rhs) const
{
  return !(*this == rhs);
}

/*!
   The unpacker.  Most of the detail about this is described in the class
   header.
*/
Bool_t
CLLNLUnpacker::operator()(const Address_t pEvent,
			    CEvent&         rEvent,
			    CAnalyzer&      rAnalyzer,
			    CBufferDecoder& rDecoder)
{
  UInt_t                      nWords = 0; // This will be tallied as we go.
  CTclAnalyzer&               analyzer(dynamic_cast<CTclAnalyzer&>(rAnalyzer));
  TranslatorPointer<UShort_t> p(*(rDecoder.getBufferTranslator()), pEvent);
  CTclAnalyzer &              rAna((CTclAnalyzer&)rAnalyzer);

  // What we do next depends on the state:

  switch (m_state) {
  case Initial:
    fetchSuperEvent(p);
    m_state = Internal;		// Want it to fall through to unpack.
  case Internal:
    unpackModule(rEvent);
    break;			// Don't want to fall through for exception.
  default:
    throw CEventFormatError(static_cast<int>(CEventFormatError::knBadPacketContents),
			    "Invalid state in CLLNLUnpacker::operator()");
  }

  // If the super event is empty we've finished the super event.
  // otherwise we still have a way to go:

  if (m_event.empty()) {
    rAna.SetEventSize(m_size*sizeof(uint16_t));
    m_state = Initial;		// Need an event header next.
  }
  else {
    rAna.entityNotDone();	// Don't let analyze decrement entity count.
    rAna.SetEventSize(0);	// Don't advance pointers until the last chunk.
  }
  return kfTRUE;

}

///////////////////////////////////////////////////////////////
//////////////////// Utility functions; ///////////////////////
///////////////////////////////////////////////////////////////


/*
  Fetch a super event from the buffer.  A super event is defined
  as all the event fragments in a full event... a VMUSB event.
  this can come in muliple 2Kword chunks.
*/
void
CLLNLUnpacker::fetchSuperEvent(TranslatorPointer<UShort_t>& p)
{
  // We're going to eat up the delimiters so the first thing should
  // be the event header.
  //
  m_size = 0;
  int fragments = 0;

  // Fetch the first header:
  bool     more         = true;	// force the first loop pass.
  uint16_t header;
  uint16_t fragmentSize;

  
  while (more) {
    header       = *p++;
    fragmentSize = header & LengthMask;
    more         = (header & ContinuedMask) != 0;
    m_size += fragmentSize + 1;;
    fragments++;

    // Copy out a single event fragment.
    // Since Jan has decided to put 0xffff after each BERR
    // terminating block we need to copy out a 16 bit word at a time.
    //

    while (fragmentSize) {
      m_event.push_back(*p++);
      fragmentSize--;
    }
  }
}

// Utility functions for longs in CAEN data:

uint32_t getType(uint32_t value) {
  return (value & ALLH_TYPEMASK) >> ALLH_TYPESHIFT;
}
bool isHeader(uint32_t value)
{
  return getType(value) == HEADER;
}
/*
   We just consume longwords in the m_event until we have
   either emptied it or analyzed a module.
   It is possible for there to be no data to analyze (all invalids).
*/

void
CLLNLUnpacker::unpackModule(CEvent& rEvent)
{

  // We're going to try to do this by only paying attention
  // to the data words:
  
  // We're going to get data until we have a non datum
  // because I don't trust the CAEN module channel count. It has let me
  // down before.
  //
  
  
  CParamMapCommand::ParameterMap& theMap(CParamMapCommand::getMap());
  uint32_t datum = getGoodl();
  uint32_t type = getType(datum);
  int      slot = (datum & ALLH_GEOMASK) >> ALLH_GEOSHIFT;
  //
  // Lowest geo is 3 hard coded to get around Jtec issues
  //
  if (slot < 3) {
#ifdef REPORT_BAD_EVENTS
    cerr << "slot num too small " << dec << slot << endl;
#endif
    m_event.clear();
    return;
  }
  while ((type == DATA) && (!m_event.empty())) {
    
    
    int channel = (datum & DATAH_CHANMASK) >> DATAH_CHANSHIFT;
    int value   = (datum & DATAL_DATAMASK);
    bool overflow = (datum & DATAL_OVBIT) != 0;
    bool underflow= (datum & DATAL_UNBIT) != 0;
    bool valid    = (datum& DATAL_VBIT)  != 0;

    if (channel > 31) {
#ifdef REPORT_BAD_EVENTS
      cerr << dec << "Bad channel number: " << hex  << datum 
	   << dec << " " << channel << " " << value << endl;
#endif
      // When we see this what appears to be happening is that
      // the top 16 bits got dropped...in that case,
      // The top 16 bits are likley the bottom 16  bit of
      // the next datum...so push that into the front and
      // see if we can keep analyzing:

      if (!m_event.empty()) {
	uint16_t nextlow = (datum >> 16) & 0xffff;
	m_event.push_front(nextlow);
      }
      return;
    }
    if (theMap.size() > slot) { // Map must have the slot.
      int paramno = theMap[slot][channel];
      if ((paramno >= 0)    &&
	  (!overflow)       && (!underflow)   && valid)
	  rEvent[paramno] = value;
    }
    if (!m_event.empty()) {
      datum = getGoodl();		// This is ok since there should be a trailer.
      type  = getType(datum);
      // If the slot changed... then we put the data back and
      // break ...
      int newslot = (datum & ALLH_GEOMASK) >> ALLH_GEOSHIFT;
      if (newslot != slot) {
	m_event.push_front(datum);
	break;
      }
    }
  }  
}

/*
** Utilties to get at the super event
**   peekw  - gets the next word without removing it.
**   getw   - gets the next word destructively
**   getl   - gets the next longword destructively.
*/
uint16_t
CLLNLUnpacker::peekw() {
  return m_event.front();
}
uint16_t
CLLNLUnpacker::getw()
{
  if (m_event.empty()) {
    return 0xffff;
  }
  uint16_t result = m_event.front();
  m_event.pop_front();
  return result;
}
uint32_t
CLLNLUnpacker::getl()
{
  uint32_t result;
  result  = getw() | ((uint32_t)getw() << 16); // Litle endian data.
  return result;
}
uint32_t
CLLNLUnpacker::getGoodl()
{
  union {
    uint16_t words[2];
    uint32_t Long;
  } result;
  result.Long =0;
  int      goodwords = 0;

  while (!m_event.empty() && (goodwords < 2)) {
    uint16_t item = getw();
    if (item != 0xffff) {
      result.words[goodwords]  = item;
      goodwords++;
    }
  }
  if (goodwords < 2) {
    return 0xffffffff;
  } 
  else {
    return result.Long;
  }
}
