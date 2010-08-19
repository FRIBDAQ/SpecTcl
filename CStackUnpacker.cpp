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
#include "CStackUnpacker.h"
#include "CModuleUnpacker.h"
#include "C785Unpacker.h"
#include "CNADC2530Unpacker.h"
#include "CMADC32Unpacker.h"
#include "CV1x90Unpacker.h"
#include "CV977Unpacker.h"
#include "CParamMapCommand.h"
#include "CStackMapCommand.h"
#include "CMASE.h"

#include <Event.h>
#include <TCLAnalyzer.h>
#include <Analyzer.h>
#include <BufferDecoder.h>

#include <iostream>

using namespace std;

// Constants

static const UShort_t VMUSB_STACKID_MASK(0xe000);
static const UShort_t VMUSB_STACKID_SHIFT(13);
static const UShort_t VMUSB_CONTINUE(0x1000);
static const UShort_t VMUSB_LENGTH(0x0fff);


// Local and class static data:


static C785Unpacker       unpack785;
static CNADC2530Unpacker  unpackNADC2530;
static CMADC32Unpacker    unpackMADC32;
static CV1x90Unpacker     unpackV1x90;
static CV977Unpacker      unpackV977;
static CMASE              unpackMase;

// table of unpackers for each type of module.

CModuleUnpacker* CStackUnpacker::m_unpackers[] = 
  {
    &unpack785, 
    &unpackNADC2530,
    &unpackMADC32,
    &unpackV1x90,
    &unpackV977,
    &unpackMase
};

//////////////////////////////////////////////////////////////////////////////////
//  Canonicals:

// The ones we have ar no-ops.

CStackUnpacker::CStackUnpacker()  :
  m_state(CStackUnpacker::Initial),
  m_size(0),
  m_offset(0)
{
}
CStackUnpacker::~CStackUnpacker() {}


////////////////////////////////////////////////////////////////////////////////////
// The unpacker itself.
//

/*!
   Unpacks the data from a VM-USB event
   - The event is assembled into a fector of unsigned 16 bit words.. along the way
     extracting the stack id.
   - The stack map is fetched and the parameter maps in order are used to determine
     which module unpackers to invoke for each chunk of data in the event.

   \param pEvent    - Pointer to the raw data.
   \param rEvent    - The 'array' of parameters in which to unpack the data.
   \param rAnalyzer - Reference to the analyzer.  This is really a Tcl Analyzer and is
                      used to set the size of the event (in bytes).
   \param rDecoder  - Reference to the buffer decoder which is not used at all atually.

   \return Bool_t
   \retval kfTRUE - The unpacker succeeded.  Remaining event processors can be called.
   \retval kfFALSE - The unpacker failed.  The remaining event processors should not be called.

*/

Bool_t
CStackUnpacker::operator()(const Address_t pEvent,
			   CEvent&         rEvent,
			   CAnalyzer&      rAnalyzer,
			   CBufferDecoder& rDecoder)
{
  CTclAnalyzer&    analyzer(dynamic_cast<CTclAnalyzer&>(rAnalyzer));
  TranslatorPointer<UShort_t> p(*(rDecoder.getBufferTranslator()), pEvent);
  StackInfo        info;


  switch (m_state) {
  case Initial:			// Need to fetch a new event from the data...
    info        = assembleEvent(p,  m_event);
    m_offset    = 0;
    m_size      = info.s_stackSize;
    m_state     = Internal;
    m_stack     = info.s_stackNumber;
  case Internal:
    unpackEvent(rEvent);
    break;
  default:
    throw string("Invalid state in CStackUnpacker::operator()");
  }


  // What we do now depends on whether or not there is data left in the megaevent:
  // note that m_size includes the header which is not in the megaevent:

  if (m_offset < (m_size-1)) {
    // More data in superevent; stay in the internal state..

    analyzer.entityNotDone();
    analyzer.SetEventSize(0);
  }
  else {
    // No data left in the superevent, transition to the Initial state:

    analyzer.SetEventSize(m_size*sizeof(uint16_t));
    m_state  = Initial;
  }

  return kfTRUE;


}
/**
 ** Unpack a single module:
 ** Parameters:
 **   rEvent - Reference to the event 'vector' we are filling with parameters.
 ** Implicit Inputs:
 **  m_event - Current superevent (vector<uint16_t>).
 **  m_offset - offset into the current superevent
 **  m_stack  - Our stack number.
 ** The stack maps.
 **
 ** We are actually going to iterate over all modules unless we run out of data first.
 */
void
CStackUnpacker::unpackEvent(CEvent& rEvent)
{
  // The stackmap command contains data structures that map the event parameters to
  // parameter ids in the rEvent:

  try {
    const CStackMapCommand::stackMap& myMap(CStackMapCommand::getMap(m_stack));
    
    // Give each module a chance:
    
    size_t offset = m_offset;
    for (int i =0; i < myMap.size(); i++) {
      CParamMapCommand::AdcMapping* pMap = myMap[i];
      int moduleType = pMap->type;
      CModuleUnpacker* pUnpacker = m_unpackers[moduleType];
      m_offset = (*pUnpacker)(rEvent, m_event, m_offset, pMap);
      
      // Done if we finished with the available data too:
      
      
      
      if(m_offset >= (m_size - 1)) break;
    }
    if (offset == m_offset) {
      cerr << "Processing the event did not result in progress through the buffer\n";
      cerr << "skipping on to the next superevent\n";
      m_offset = m_size;
    }
  }
  catch(string msg) {
    cerr << "Event unpacker found an unrecoverable problem: "
	 << msg << endl;
    cerr << "skipping on to the next superevent\n";
    m_offset = m_size;
  }

}



//////////////////////////////////////////////////////////////////////////////////
// Utilties

/*
   Assemble an event from the VMUSB event fragments.  While we're at it,
   extract the stack id and return it to the caller.

*/
CStackUnpacker::StackInfo
CStackUnpacker::assembleEvent(TranslatorPointer<UShort_t>&p, 
			      vector<uint16_t>& event)
{
  StackInfo result;
  bool done    = false;
  int  stackId = -1;
  size_t totalSize = 0;
  uint16_t datum;
  event.clear();

  while(!done) {
    // Decode the header:

    uint16_t header = *p; ++p;
    done            = (header & VMUSB_CONTINUE) == 0;
    int fragmentSize = header & VMUSB_LENGTH;
    totalSize++;		// headers are a word of size...

    // only pull the stackid out of the first header (just in case).

    if (stackId < 0) {
      stackId = (header & VMUSB_STACKID_MASK) >> VMUSB_STACKID_SHIFT;
    }
    // Append the fragment to the event vector

    for (int i=0; i < fragmentSize; i++) {
      datum = *p; ++p;
      event.push_back(datum);
    }
    totalSize += fragmentSize;	// Words in the fragment...
  }

  // The event is terminated with the 0xffffffff of the BERR.. 
  // remove that from the assembled event:

  if (datum == 0xffff) {
    event.pop_back();
    event.pop_back();
  }
  result.s_stackNumber = stackId;
  result.s_stackSize   = totalSize;
  return result;
}
