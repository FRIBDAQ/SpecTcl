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
#include "CMASE.h"

#include <Event.h>
#include <TreeParameter.h>
#include <stdint.h>
#include <iostream>
#include <string.h>
#include <SpecTcl.h>
#include <Parameter.h>
#include <TCLInterpreter.h>
#include <TCLVariable.h>
#include <TCLList.h>
#include <stdio.h>

using std::vector;
using std::string;
using std::cerr;

// Assumption: Each CHB is fully populated with 32 channels:

static const int CHB_CHANNELS = 32;

////////////////////////////////////////////////////////////////////////////
// Implemented canonicals.
//
/**
 * Construction is a no-op for now.
 */
CMASE::CMASE() {}
/**
 * Destruction is a chain to the base class:
 */
CMASE::~CMASE() {}

/**
 * Unpack the data from a Mase packet.  The structure of this
 * packet is:
 *  LongwordCount
 *  Channel Designator
 *  Channel Values..
 *
 *  Channel Designator is divided in bytes where, from low to high,
 *  Byte 0 = COB number.
 *  Byte 1 = CHB number.
 *  Byte 2 = Channel number
 *  Byte 3 = 0.
 *  Channel values contain a time in the least significant 16 bits and
 *  energy in the most significant 16 bits.
 *
 * @param rEvent output Parameter 'array'.
 * @param event  Vector of words that make up the event.
 * @param offset Offset to the first 16 bit word of our event.
 * @param pMap   Our mapping data.  note that we will use our extension
 *               creating it if required.
 * @return unsigned int
 * @retval offset to the first unused word.
 */
unsigned int
CMASE::operator()(CEvent&                       rEvent,
				  std::vector<unsigned short>&  event,
				  unsigned int                  offset,
				  CParamMapCommand::AdcMapping* pMap)
{
  COBVector& trees = getInfo(pMap);
  int        words = getLong(event, offset);
  offset++;
  for (int c = 0; c < offset; c++) {
    int chDesignator = getLong(event, offset);
    int chValues     = getLong(event, offset);
    offset          += 2;

    int cob        = chDesignator & 0xff;
    int chb        = (chDesignator >> 8)  & 0xff;
    int chan       = (chDesignator >> 16) & 0xff;

    int t          = chValues & 0xffff; // For when we use the times.
    int e          = (chValues >> 16) & 0xffff;

    // get the tree parameter vector.. or ignore the data if not 
    // defined.

    if (cob < trees.size()) {
      CHBVector* pCHB = trees[cob];
      if(chb < pCHB->size()) {
	CTreeParameterArray* pTree = (*pCHB)[chb];
	
	// Channel numbers are gauranteed to be < 32 and each tree param array
	// is 32 chans long so:

	(*pTree)[chan] = e;
      }
    }

    

  }

  return offset;
}



/////////////////////////////////////////////////////////////////////////////////////////////////////
//   Private utilities
// 
// These are all related to creating/retrieving the mappings between COB/CHB/Channel and
// parameters in SpecTcl.
//


/**
 *  Get the parameter mapping information, or create it and link it to the 
 * parameter mapping structure if it does not yet exist.
 *  @param pMap - parameter mapping structure for this module.
 */

CMASE::COBVector&
CMASE::getInfo(CParamMapCommand::AdcMapping* pMap)
{
  // If the mapping does not yet exist, create it and set it:

  if (pMap->extraData == 0) {
    COBVector* parameters = createCOBVector(pMap->name);
    pMap->extraData = parameters;
  }
  return *reinterpret_cast<COBVector*>(pMap->extraData);
}
/**
 * Create the COB vector.. this requires that we fetch the
 * values of our maseCOBCount and maseCHBCounts Tcl arrays.
 * these will be sensible since otherwise the setup will have failed grossly and we won't
 * get called (I hope).
 * @param name - Base name for the parameters.
 */
CMASE::COBVector* 
CMASE::createCOBVector(string baseName)
{
  COBVector*       pVector = new COBVector;

  // retrieve and decode the  mase arrays:

  SpecTcl*         api     = SpecTcl::getInstance();
  CTCLInterpreter* pInterp = api->getInterpreter();

  // Figure out how many COB's there are.

  CTCLVariable maseCOBCount("maseCOBCount", false);
  maseCOBCount.Bind(*pInterp);
  const char* pCOBCount = maseCOBCount.Get(TCL_GLOBAL_ONLY,
					   const_cast<char*>(baseName.c_str()));
  if (!pCOBCount) {
    cerr << "ERROR - SpecTcl misconfigured. CMASE not able to get maseCOBCount("
	 << baseName << ") from TCL\n";
    exit(-1);
  }
  int cobCount = atoi(pCOBCount);

  // Get an array of CHB counts for each COB:

  CTCLVariable maseCHBCounts("maseCBHCounts", false);
  maseCHBCounts.Bind(*pInterp);
  const char* pCHBCounts = maseCHBCounts.Get(TCL_GLOBAL_ONLY,
					     const_cast<char*>(baseName.c_str()));
  if(!pCHBCounts) {
    cerr << "ERROR - SpecTcl misconfigured CMASE not able to get maseCHBCounts("
	 << baseName << ") from Tcl\n";
    exit(-1);
  }
  CTCLList maseCHBCountList(pInterp, pCHBCounts);
  StringArray maseCHBCountArray;
  maseCHBCountList.Split(maseCHBCountArray);
  

  // Ok now create each CHB vector .we'll just decode the chb count as we go:

  for (int cob = 0; cob < cobCount; cob++) {
    int chbCount = atoi(maseCHBCountArray[cob].c_str());
    pVector->push_back(createCHBVector(baseName, cob, chbCount));
  }

  return pVector;
}
   
/**
 * Create a vectoro of the parameterse for a single COB.  This is a vector
 * of CHB's each with 32 channels.  The names of these parameters 
 * are basename.cobNumber.chbnumber.0..31
 *
 * @param basename  - The base parameter name.
 * @param cobNumber - Number of the COB for which we're creating the CHB
 *                    parameters
 * @param chbCount  - Number of CHB's (each with 32 channels) in this COB.
 * @return CHBVector* 
 * @retval A vector of 32 channel tree vectors.
 *
 */
CMASE::CHBVector*
CMASE::createCHBVector(string basename, int cobNumber, int chbCount)
{
  // Encode the first part of the parameter name:

  char cob[5];			// Should be more than enough.
  sprintf(cob, "%02d", cobNumber);
  string parameterBase = basename;
  parameterBase       += '.';
  parameterBase       += cob;
  parameterBase       += '.';

  // 
  // 
  CHBVector* pResult = new CHBVector;

  for (int chb =0; chb < chbCount; chb++) {
    char chbNumber[5];
    sprintf(chbNumber, "%02d", chb);
    string treeName = parameterBase;
    treeName       += chbNumber;
 
    CTreeParameterArray *pArray = new CTreeParameterArray(treeName,
							  8192,
							  32,
							  0);
    pResult->push_back(pArray);
  }

  return pResult;
}
