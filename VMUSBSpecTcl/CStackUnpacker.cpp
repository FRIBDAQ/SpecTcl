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
#include "CHINP.h"
#include "CPSD.h"

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
static CHINP              unpackHINP;
static CPSD               unpackPSD;

// table of unpackers for each type of module.

CModuleUnpacker* CStackUnpacker::m_unpackers[] = 
  {
    &unpack785, 
    &unpackNADC2530,
    &unpackMADC32,
    &unpackV1x90,
    &unpackV977,
    &unpackMase,
    &unpack785,			// CAEN Dual range placeholder
    &unpackHINP,
    &unpackPSD
};

//////////////////////////////////////////////////////////////////////////////////
//  Canonicals:

// The ones we have ar no-ops.

CStackUnpacker::CStackUnpacker() {}
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
  vector<uint16_t> event;
  StackInfo        info;

  info        = assembleEvent(p, event);
  int stackId = info.s_stackNumber;
  analyzer.SetEventSize((info.s_stackSize)*sizeof(uint16_t)); // +1 for the header.


  // Get our stack map:

  const CStackMapCommand::stackMap& myMap(CStackMapCommand::getMap(stackId));

  // Unpack each module in the stack:

  unsigned int offset = 0;



  for (int i = 0; i < myMap.size(); i++) {
    CParamMapCommand::AdcMapping* pMap = myMap[i];
    int moduleType = pMap->type;
    CModuleUnpacker* pUnpacker = m_unpackers[moduleType];
    offset = (*pUnpacker)(rEvent, event, offset, pMap);
  }


  // Burn up any remaining 0xfff's at the end of the event:

  while(offset < event.size()) {
    if (event[offset] == 0xffff) {
      offset++;
    }
    else {
      break;
    }
  }

  // Something went wrong if we didn't burn up the entire event:

  if (offset != event.size()) {
    cerr << "**WARNING** Event not entirely decoded by unpackers\n";
    cerr << "            Event will not be histogrammed, proceeding with next event\n";
    return kfFALSE;
  }
 

  
  return kfTRUE;

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
      uint16_t datum = *p; ++p;
      event.push_back(datum);
    }
    totalSize += fragmentSize;	// Words in the fragment...
  }
  
  result.s_stackNumber = stackId;
  result.s_stackSize   = totalSize;
  return result;
}
