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
#include "CV1729Unpacker.h"
#include <Event.h>
#include <stdint.h>
#include <Spectrum.h>
#include <Parameter.h>
#include <SpecTcl.h>
#include <stdio.h>
#include <string.h>
#include <TCLInterpreter.h>
#include <tcl.h>


#include <string>

using namespace std;

static const int numSamples = 2048; // That's how many we care about..that's the spectrum width.
static const int numCells   = 128*20;
static const int channelOrder[4] = {2,3,0,1};

static       int  freeze;	// if non zero, creating new waveforms is disabled.
static       bool first = true;





// Taken a word at a time, here's the order of the channels
// in the data:
//


/**
 * Static local function binds the freeze variable to the Tcl Variable:
 * freezeFadcSpectra.. and sets it to false.
 */
static void bindVariable()
{
  SpecTcl* pApi = SpecTcl::getInstance();

  // Do this old school because I'm lazy:

  CTCLInterpreter* pInterp = pApi->getInterpreter();
  Tcl_Interp*      pActualInterp = pInterp->getInterpreter();
  Tcl_LinkVar(pActualInterp, "freezeFadcSpectra", reinterpret_cast<char*>(&freeze), TCL_LINK_INT);
  freeze = 0;
}

/////////////////////////////////////////////////////////////////////
// Canonicals..

/*!
  Construction is a no-op.

*/
CV1729Unpacker::CV1729Unpacker() {}

/*!
   Destruction is a no-op.
*/
CV1729Unpacker::~CV1729Unpacker() {}




//////////////////////////////////////////////////////////////////////
//  Virtual function overrides.

/**
 * Perform the unpack.  The data format is as follows:
 *  +------------------------------------+
 *  |   Interrupt register. (32)         |
 *  +------------------------------------+
 *  |   Trigger column number  (32)      |
 *  +------------------------------------+
 *  |  Digitizer data as defined in      |
 *  |  4.7 of the manual.                |
 *  \                                   /
 *  /                                   \
 *  +------------------------------------+
 *
 *  The interrupt register is 1 if there is data worth looking at.
 *  It is not clear to me yet what the digitizer data looks like if there
 *  is not data.
 * 
 * @param rEvent  - The output event (ignored by this unpacker).
 * @param event   - Raw data from the VM-USB
 * @param offset  - Where in event our data starts.
 * @param pMap    - adc mapping structure that describes this device.
 *                  we care about a couple of things.
 *                  - Using the rEvent indices to locate our parameter base name.
 *                  - Using the parameter basenames to find all 2048 spectra associated
 *                    with each used channel.
 *                  - Using the parammap 'virtual slot' element determine which channels will actually
 *                    provide data.
 * @return unsigned int
 * @retval Value of offset for next part of the event.
 *
 */
unsigned int
CV1729Unpacker::operator()(CEvent&                       rEvent,
			    std::vector<unsigned short>&  event,
			    unsigned int                  offset,
			    CParamMapCommand::AdcMapping* pMap)
{
  if (first) {
    bindVariable();
    first = false;
  }

  Info& info = *(findSpectra(*pMap));
  
  // get the two header words:

  uint32_t irqRegister = event[offset++]; offset++;
  uint32_t triggerCol  = event[offset++]; offset++;


  irqRegister &= 1;
  triggerCol  &= 0x7f;
  if(irqRegister && !freeze) {

    int sampleBegin = offset + 3*4; // Skip the header.
    int sampleEnd   = sampleBegin + 128*20*4; // Just off the end of the sample block.
    // Get our spectra:

    next(info);			// advance to next spectrum:
    CSpectrum* pSpectra[4];
    for (int i = 0; i < 4; i++) {
      if (info.s_spectrumIndex < 2048) {
	pSpectra[i] = info.s_Spectra[i][info.s_spectrumIndex];
	if (pSpectra[i]) {
	  pSpectra[i]->Clear();
	}
      }
      else {
	pSpectra[i] = 0;
      }
    }
    // Adapted from V1729.c generously handed to me for examination by CAEN
    // Thanks Massimo.
    // 
    int end_cell = (20*(128 - triggerCol + pMap->vsn +1 ) ) % numCells;
    for (UInt_t i = 0; i < numSamples; i++) {
      UInt_t j = (2560 + i  + end_cell) % numCells;
      for (int d = 0; d < 4; d++) {
	int ch   = channelOrder[d];
	int data = event[sampleBegin + 4*j + d];
	if (pSpectra[ch]) {
	  pSpectra[ch]->set(&i, data);
	}
      }
    }
  }
  if(freeze) {
    info.s_spectrumIndex = 2047; // reset next  spectrum to 0.
  }

  offset += 128*20*4;		// Sample data...
  offset += 3*4;		// Header data...
  offset += 2*2;		// trailer data.
  
  return offset;		// Next chunk of data.
}

///////////////////////////////////////////////////////////////////////////////////////////////////////
// Private utilities:
//

/**
 * locate the pInfo for our mapping element or create it if it does not exist.
 * @param rMap - Reference to the map.
 * @return pInfo
 * @retval pointer to the info.
 */
CV1729Unpacker::pInfo
CV1729Unpacker::findSpectra(CParamMapCommand::AdcMapping& rMap)
{
  SpecTcl* pApi = SpecTcl::getInstance();

  if (! rMap.extraData) {
    pInfo pSpectra = new Info;


    pSpectra->s_spectrumIndex = -1; // start with the first (assuming unpack starts with a next()).
    
    
    // Looping over the channels: 

    for (int chan = 0; chan < 4; chan++) {
      // Null the spectrum pointers:

      for (int spec = 0; spec < 2048; spec++) {
	pSpectra->s_Spectra[chan][spec] = 0;
      } 

      // If the parameter is defined and we can track down its name
      // ..and we can track down the spectra associated with it, put pointers to
      // them into the struct.

      if (rMap.map[chan] != -1) {
	CParameter* pParam = pApi->FindParameter(rMap.map[chan]);
	if (pParam) {
	  string baseName = pParam->getName();
	  for (int i = 0; i < 2048; i++) {
	    char spectrumName[2048];
	    snprintf(spectrumName, sizeof(spectrumName), "%04d.%s",i, baseName.c_str());
	    CSpectrum* pSpec = pApi->FindSpectrum(spectrumName);
	    pSpectra->s_Spectra[chan][i] = pSpec; // Correct thing to do even if there's no match.
	  }

	}
      }
    }

    rMap.extraData = pSpectra;

  }

  return reinterpret_cast<pInfo>(rMap.extraData);
}
/**
 * Manages the spectrumIndex in the info struct; Moves to the next spectrum
 * - Increments s_spectrumIndex
 * - If s_spectrumIndex >= 2048 clears all spectra and sets itself to 0.
 *
 * @param rInfo - reference to the rInfo variable.
 */
void
CV1729Unpacker::next(CV1729Unpacker::Info& rInfo)
{
  rInfo.s_spectrumIndex++;
  rInfo.s_spectrumIndex = rInfo.s_spectrumIndex % 2048;


}
