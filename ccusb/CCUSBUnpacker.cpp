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
#include "CCUSBUnpacker.h"
#include <Event.h>
#include <Analyzer.h>
#include <BufferDecoder.h>
#include <TCLAnalyzer.h>
#include "ParamMapCommand.h"
#include "CCCUSBPacket.h"
#include "CPh7xxUnpacker.h"
#include "CFixedSizedUnpacker.h"
#include "CC1205Unpacker.h"


#include <string>
#include <iostream>
#include <stdio.h>

using namespace std;




static CPh7xxxUnpacker         ph7xxx;
static CFixedSizedUnpacker     ad811(8);
static CFixedSizedUnpacker     lrs2249w(12);
static CFixedSizedUnpacker     lrs2228(8);
static CC1205Unpacker          c1205;

static CCCUSBPacket* unpackers[] = {
  &ph7xxx, 			// Type 0 => phillips 7xxx
  &ad811,			// Type 1 => Ortec AD811
  &lrs2249w,                    // Type 2 => LRS 2249 QDC
  &lrs2228,                     // Type 3 => LRS 2228 TDC.
  &c1205			// Type 4 => CAEN C1205 QDC.
};				// Indices must match types in spectclsetup.tcl

///////////////////////////////////////////////////////////////////////////////////
/*!
    Analyze an event.
    \param pEvent    - Pointer to the raw event.  We assume that buffer byte order
                        signatures are present that will allow us to determine the
			byte ordering.
    \param rEvent    - The parameter 'array' into which this data will be unpacked.
    \param rAnalyzer - The analyzer that's in charge of this buffer.  This is
                       a CTCLAnalyzer to which we need to give the size of the event
    \param rDecoder  - The buffer decoder that understands the outer structure of the
                       event buffer.  We're going to use it to get byte ordering to
                       construct a translating pointer so we can reference the
                       data correctly even if there is a byte ordering mis-match
                       between this system and the one that created the data.
*/
Bool_t
CCUSBUnpacker::operator()(const Address_t pEvent,
			    CEvent&         rEvent,
			    CAnalyzer&      rAnalyzer,
			    CBufferDecoder& rDecoder)
{

  // Get all the book-keeping out of the way and create a pointer
  // p that points to the event body, and nWords, a remaining word count.
  //

  TranslatorPointer<UShort_t> p(*(rDecoder.getBufferTranslator()), pEvent);
  CTclAnalyzer&            rAna((CTclAnalyzer&)rAnalyzer);
  UShort_t               nWords = *p++;   	// Word count and pointer to body.
  rAna.SetEventSize((nWords+1)*sizeof(UShort_t));

  
  // Note in the CCUSB the word count is not self inclusive!


  CParamMapCommand* pMap   = CParamMapCommand::getInstance();
  int               module = 0;


  // Get the data
  //  The CCUSB puts in an extra word after the end of a QStop transfer.
  //  (the data when Q was not set).   I've arranged that the 
  //  max transfer count be 17 not 16 so that there's always
  //  that extra word, and therefore we know to unconditionally
  //  skip it.

  while (nWords > 0) {

    const CParamMapCommand::ParameterMap* moduleInfo= pMap->getModuleMap(module);
    int type    = moduleInfo->s_moduleType;
    int id      = moduleInfo->s_id;

    try {

      // Do some sanity checking here specifically:
      // - The type must be valid.
      // - The id must match the id of the next unpacker because 
      //   all readers must at least put their id in the buffer.
      //
      if ((type < 0) || (type >= sizeof(unpackers)/sizeof(CCCUSBPacket*))) {
	char message[100];
	sprintf(message, "Module type %d is out of range", type);
	throw string(message);
      }
      CCCUSBPacket* pUnpacker = unpackers[type];
      if (id != *p) {
	char message[100];
	sprintf(message, "ID in buffer: %d does not match that of expected unpacker (%d)",
		id, *p);
	throw message;
      }
      
      int wordsConsumed = pUnpacker->unpack(p, moduleInfo, rEvent);
      
      p      += wordsConsumed;
      nWords -= wordsConsumed;
      module++;
      
    }
    catch (string msg) {
      cerr << "Error unpacking data: " << msg << " Event will be ignored " << endl;
      return kfFALSE;
    }
    catch (const char* msg) {
      cerr << "Error unpacking data: " << msg << " event will be ignored " << endl;
      return kfFALSE;
    }
    catch (...) {
      cerr << "Error unpacking data .. unable to determine cause event will be ignored\n";
      return kfFALSE;
    }
    
  }
  
  // Must return true to histogram.
  
  return kfTRUE;
}


