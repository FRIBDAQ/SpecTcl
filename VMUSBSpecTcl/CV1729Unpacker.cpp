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

#include <string>

using namespace std;

static const int numSamples = 2048; // That's how many we care about.
static const int channelOrder[4] = {2,3,0,1};

// Taken a word at a time, here's the order of the channels
// in the data:
//


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
  Info& info = *(findSpectra(*pMap));
  
  // get the two header words:

  uint32_t irqRegister = event[offset++]; offset++;
  uint32_t triggerCol  = event[offset++]; offset++;


  irqRegister &= 1;
  triggerCol  &= 0x7f;
  if(irqRegister) {

    int sampleBegin = offset + 3*4; // Skip the header.
    int sampleEnd   = sampleBegin + 128*20*4; // Just off the end of the sample block.

    // We need to locate the first sample in time in accordance with 2.4.2 of the manual.
    // since I never udnerstood the values of the verniers, and  since only signal
    // shape is important we'll only go for column resolution
    // for the start.  The trigger col is relative to the last column.  The post trigger must
    // be added back in as well:

    /*
    triggerCol =  128-triggerCol; // start column numbered from 0.
    int startCol   = triggerCol +  pMap->vsn; // we stored the post there.
    if (startCol >= 128) {
      startCol -= 128;
    }

    int startSample= startCol*20;	    // 20 samples/column.
    */
    int startCol = (pMap->vsn - triggerCol) % 128;
    int startSample = startCol*20;

    // Compute the0 start positions for the 4 channels.  the channels are stored
    // last to first:

    int dataOffsets[4];
    int o = 0;
    for (int i = 0; i < 4; i++) {
      dataOffsets[i] = sampleBegin + startSample + channelOrder[i];
      o++;
    }
    // Get our spectra:

    next(info);			// advance to next spectrum:
    CSpectrum* pSpectra[4];
    for (int i = 0; i < 4; i++) {
      pSpectra[i] = info.s_Spectra[i][info.s_spectrumIndex];
      if (pSpectra[i]) {
	pSpectra[i]->Clear();
      }
    }
    // Now start untangling the data:

    for (unsigned int s = 0; s < numSamples; s++) {
      for (int c =0; c < 4; c++) {
	int sample = event[dataOffsets[c]];
	dataOffsets[c] += 4;
	if (dataOffsets[c] >= sampleEnd) {
	  dataOffsets[c] = channelOrder[c] + sampleBegin;
	}
	if (pSpectra[c] && (s < pSpectra[c]->Dimension(0))) {
	  pSpectra[c]->set(&s, sample);
	}
      }
    }

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
	    snprintf(spectrumName, sizeof(spectrumName), "%s.%04d", baseName.c_str(), i);
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
  if (rInfo.s_spectrumIndex >= 2048) { // Need to reset/clear.
    rInfo.s_spectrumIndex = 0;
    for (int c = 0; c < 4; c++) {
      for (int s = 0; s < 2048; s++) {
	if (rInfo.s_Spectra[c][s]) {
	  rInfo.s_Spectra[c][s]->Clear(); // For now don't clear..
	}

      }
    }

  }
}
