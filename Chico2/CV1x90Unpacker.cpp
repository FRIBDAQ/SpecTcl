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
#include "CV1x90Unpacker.h"

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

/// ASSUMPTION:
///   There are at most 128 channels. Note that if this is wrong, the
///   parameter map is also going to break;

static const int   MAX_CHANNELS = 128;

////////////////////////////////////////////////////////////////////
//
// Constants that define the fields we need to see in the
// TDC longwords:
//

// Item type field and possible values:

static const uint32_t  ITEM_TYPE    = 0xf8000000; // Item type field.
static const uint32_t  TYPE_GBLHEAD = 0x40000000; // global header type
static const uint32_t  TYPE_TDCHEAD = 0x08000000; // TDC chip header data.
static const uint32_t  TYPE_DATA    = 0x00000000; // Tdc data.
static const uint32_t  TYPE_TDCTRAIL= 0x18000000; // TDC Trailer.
static const uint32_t  TYPE_ERROR   = 0x20000000; // TDC error word.
static const uint32_t  TYPE_TRIGTIME= 0x88000000; // Extended trigger time.
static const uint32_t  TYPE_GBLTRAIL= 0x80000000; // Global trailer (end of TDC data).

// What we care about in the gbl header:

static const uint32_t GBLHEAD_VSN   = 0x0000001f; // Mask for virtual slot number.

// What we care about in the ERROR words:

static const uint32_t ERROR_TDCMASK   = 0x03000000; // Mask of TDC chip number.
static const uint32_t ERROR_TDCSHIFT  = 24;
static const uint32_t ERROR_BITS      = 0x00007fff; // Mask of error bits.

static const char* ERROR_STRINGS[] = {
  "Hit lost in group 0 from read-out FIFO overflow.",
  "Hit lost in group 0 from L1 overflow.",
  "Hit error has been detected in group 0.",
  "Hit lost in group 1 from read-out FIFO overflow,",
  "Hit lost in group 1 from L1 overflow",
  "Hit error has been detected in group 1",
  "Hit lost in group 2 from read-out FIFO overflow,",
  "Hit lost in group 2 from L1 overflow",
  "Hit error has been detected in group 2",
  "Hit lost in group 3 from read-out FIFO overflow,",
  "Hit lost in group 3 from L1 overflow",
  "Hit error has been detected in group 3",
  "Hits rejected because of programmed event size limit",
  "Event lost (trigger FIFO overflow",
  "Internal Fatal Chip error has been detected"
};

////////////////////////////////////////////////////////////////////
// Canonicals

/*!
    Constructs the beast... 
*/
CV1x90Unpacker::CV1x90Unpacker() {}

/*!   
  Destroys the beast../
*/
CV1x90Unpacker::~CV1x90Unpacker()
{

}

////////////////////////////////////////////////////////////////////////
//
// The public interface:

/*!
  The function call operator is expected to
  # Determine if it is being pointed at TDC datat relevant to it.
  # If so unravel said data using the reference channel number
  # and maximum number of hits to retain for each spectrum.
  # as well as the tree parameter arrays for each channel of the
  # device.
  
  \param rEvent   - Reference to the unpacked event array. We're going to ignore
                    this in favor of using Tree parameters.
  \param event    - This is the raw event unpacked into a vector of shorts.
  \param offset   - This is the offset into the event parameter at which
                    we must start unpacking.
  \param pMap     - This is the ADC mapping element that corresponds to our module.

  \return int
  \retval Offset to the next chunk of the event.

*/
unsigned int
CV1x90Unpacker::operator() (CEvent&                      rEvent,
			    vector<unsigned short>&      event,
			    unsigned int                 offset,
			    CParamMapCommand::AdcMapping* pMap)
{

  vector<int32_t> rawTimes[128]; //  Raw times are stored here pending ref time subtraction.


  // If this chunk of the event is for us, there should be a TDC global header,
  // and it should have a geo field that matches the vsn in our pMap element.

  uint32_t header = getLong(event, offset);
  if (header == 0xffffffff) {
    return offset+2;
  }
  if ((header & ITEM_TYPE) != TYPE_GBLHEAD) return offset; // not TDC data.
  
  if ((header & GBLHEAD_VSN ) != pMap->vsn) return offset;

  // We've established this is our module.  We need to get the information
  // associated with this TDC.

  TdcInfo&  info = getInfo(pMap); // Get our TDC specific junk.

  // Now unpack the data:

  offset += 2;			// 2 words / long
  size_t   maxoffset = event.size(); // protection against malformed data.

  // We're really only interested in the following data types:
  // TDC data,
  // 

  bool done = false;
  uint32_t    referenceTime = 0;
  while((offset < maxoffset) && !done) {
    uint32_t datum = getLong(event, offset);
    if (datum == 0xffffffff) break; // premature end of event.
    offset += 2;
    switch (datum & ITEM_TYPE) {
      // Ignored types:
    case TYPE_TDCHEAD:
    case TYPE_TDCTRAIL:
    case TYPE_TRIGTIME:
      break;
      // The GBLTRAIL type  bounces us out of the loop:

    case TYPE_GBLTRAIL:
      done  = true;
      break;

      // The error type prints out an error message:

    case TYPE_ERROR:
      reportError(datum, pMap->vsn);
      break;

      // TDC data must be extracted and histogrammed.

    case TYPE_DATA:
      uint32_t channel = (datum & info.s_chanmask) >> info.s_chanshift;
      uint32_t time    = datum & info.s_datamask;
      rawTimes[channel].push_back(time);
      break;
      
    }
  }
  // If the next longword is a 0xffffffff that's due to the BERR
  // at the end of our readout:

  if(getLong(event, offset) == 0xffffffff) offset += 2;

  // 
  // Two cases to consider.  If the reference channel number is -1
  // there's no reference channel..otherwised there is:
  //
  int32_t reftime = 0;		// Default to no reference chhanel:
  if (info.s_refchannel >= 0) {	//  Reference channel used:


    if (rawTimes[info.s_refchannel].size() > 0) {
      reftime = rawTimes[info.s_refchannel][0];
    }
    else {
      std::cerr << "-- TDC data with no hits in reference time discarded from vsn: ";
      std::cerr << pMap->vsn << std::endl;
      return offset;
    }
  }

  // The reftime defaults to zero which essentially does not adjust the times
  // if no reference channel is specified.

  for (int i = 0; i < info.s_channelCount; i++) {
    int hits = rawTimes[i].size();
    if (hits > info.s_depth) hits = info.s_depth;
    CTreeParameterArray* pArray = info.s_parameters[i];
    if (pArray) {		// No parameter defined.
      CTreeParameterArray&  Array(*pArray);
      for (int hit =0; hit < hits; hit++) {
	double triggerRelative = static_cast<double>(rawTimes[i][hit] - reftime);
	triggerRelative        = triggerRelative*info.s_chansToNs;
	
	Array[hit] = triggerRelative;	// common stop assumption.
	  }
    }
  }



  return offset;
  
} 




/////////////////////////////////////////////////////////////////////////
// Utility functions:
//


/*
** Get the information about a TDC.
** If necessary we'll create new information and 
** link it to the mapping array first.
*/
CV1x90Unpacker::TdcInfo&
CV1x90Unpacker::getInfo(CParamMapCommand::AdcMapping* pMap)
{
  if (!pMap->extraData) {
    pMap->extraData =  newTdc(pMap->name);
  }
  return *reinterpret_cast<TdcInfo*>(pMap->extraData);
}

/*
** Report error information from a TDC (to stderr).
**
** Parameters: 
**   errorWord A TDC error word.  THe bottom 14 bits are the error info.
**             The TDC Number is also encoded in this word.
**             see ERROR_TDCxxxx
**   slot      The virtual slot number.
*/

void
CV1x90Unpacker::reportError(uint32_t errorWord, int slot)
{
  uint32_t chip   = (errorWord & ERROR_TDCMASK) >> ERROR_TDCSHIFT;
  uint32_t errors = errorWord & ERROR_BITS;

  std::cerr << "V1x90: An error word was produced by chip number " << chip
	    << " in vsn " << slot << std::endl;

  std::cerr << "The following error bits were set:\n";
  uint32_t bitnum = 0;
  while (errors) {
    uint32_t bit = 1 << bitnum;
    if (errors & bit) {
      std::cerr << ERROR_STRINGS[bitnum] << std::endl;
      errors &= ~bit;
    }
    bitnum++;
  }
  std::cerr << "-----------------------------------\n";
}
/*
**  
** Creates a new TDC info structure for this TDC given its name.
** The information for this data structure comes from the
** ::CAENV1x90 global array element indexed by the TDC module name.
** This element is a 3 element TCL list that contains
** the reference channel
** the maximum number of hits retained per channel.
** the number of TDC channels in the module.
**
** Parameters:
**   Name of the module.
** Returns
**   Pointer to a dynamically allocated filled in TdcInfo struct.
*/

CV1x90Unpacker::TdcInfo*
CV1x90Unpacker::newTdc(string name)
{
  // Locate the interpreter and get the value of
  // ::CAENV1x90(name)

  SpecTcl*         api     = SpecTcl::getInstance();
  CTCLInterpreter* pInterp = api->getInterpreter();
  CTCLVariable     infoStringVar("CAENV1x90", false);
  infoStringVar.Bind(*pInterp);

  const char* pInfoString = infoStringVar.Get(TCL_GLOBAL_ONLY,
					      const_cast<char*>(name.c_str()));
  if (!pInfoString) {
    std::cerr << "ERROR - SpecTcl mis configured. Cannot locate the Tcl variable: "
	      << "CAENV1x90(" << name << ")\n";
    std::cerr << "Be sure to set -refchannel, -depth, and -channelcount configs\n";
    exit(-1);
  }

  // This should be three numbers.  For now assume that's the case in fact:

  int refchan, depth, channels;
  int nints = sscanf(pInfoString, "%d %d %d", &refchan, &depth, &channels);
  if (nints != 3) {
    std::cerr << "ERROR - SpecTcl mis configured. CAENV11x90(" << name.c_str() << ")\n";
    std::cerr << "        must have 3 integers but was : " << pInfoString << std::endl;
    std::cerr << " check the -refchannel, -depth and -channnelcount configs\n";
    exit(-1);
  }

  TdcInfo* result = new TdcInfo;
  result->s_refchannel   = refchan;
  result->s_depth        = depth;
  result->s_channelCount = channels;

  // Channels must be 16, 32, 64, or 128... and these imply different values
  // for the channel mask/shift/datamask members of the resulting struct:

  switch (channels) {
    // V1190: 18 bits of data then 7 bits of channel number:
  case 128:
  case 64:
    result->s_chanmask  = 0x03f80000;
    result->s_chanshift = 19;
    result->s_datamask  = 0x0007ffff;
    break;
    // V1290: 20 bits of data then 5 bits of channel number:
  case 32:
  case 16:
    result->s_chanmask   = 0x03e00000;
    result->s_chanshift  = 21;
    result->s_datamask   = 0x001fffff;
    break;
    // Illegal values
  default:		
    std::cerr << "ERROR Spectcl mis configured. CAENV1x90(" << name.c_str() << ")\n";
    std::cerr << "  -channelcount must be one of 16,32,64 or 128 but was: " << channels
	      << std::endl;
    exit(-1);
  }

  makeTreeParams(*pInterp,
		 name,
		 *result);

  
  
  return result;

}

/*
** Create the tree parameters for a tdc.
**
** Parameters:
**   interp  - Reference to the TCL interpreter that's running SpecTcl.
**             This is used to fetch the adcChannels element for our TDC
**             That element provides the base name for the tree parameter
**             arrays made for each channel.
**   name    - Name of the TDC module... used to index into the 
**             adcChannels element to get the array of channel base names.
**   info    - Reference to the info struct.  This is an in/out parameter.
**             On the one hand we use info.s_channelCount and info.s_depth
**             to figure out how many tree parameter arrays must be built
**             and how many elements each has, on the other hand,
**             the whole point of this exercise is to build the
**             s_parameters element and fill it in.
*/
void
CV1x90Unpacker::makeTreeParams(CTCLInterpreter&           interp,
			       string                    name,
			       CV1x90Unpacker::TdcInfo&  info)
{
  // Inputs from info:

  int nParams = info.s_channelCount;
  int depth   = info.s_depth;

  // Create the pointer array:

  info.s_parameters = new CTreeParameterArray*[nParams];
  for (int i =0; i < nParams; i++) {
    info.s_parameters[i] = NULL; // Start out with no TP arrays defined.
  }

  CTCLVariable adcChannels(string("adcChannels"), false);
  adcChannels.Bind(interp);

  // Get the channel names and break them up into a list of channels.
  // These serve as the base names for the TDC.
  //

  const char* channelString = adcChannels.Get(TCL_GLOBAL_ONLY, \
					      const_cast<char*>(name.c_str()));
  if (!channelString) {
    std::cerr << "WARNING SpecTcl misconfigured, adcChannels(" << name.c_str() << ")\n";
    std::cerr << "      is undefined. Set that up in you daqconfig file\n";
    std::cerr << "      this TDC will not be unpacked.\n";
    exit(-1);
  }

 
  // Split the string up into a list.
  
  CTCLList adcBaseList(&interp, channelString);
  StringArray baseNames;
  adcBaseList.Split(baseNames);

  // The V1x90Windows parameter contains the window width,
  // the window offset, and the resolution in floating point ns.
  // this is used to determine the limits and channel count.
  //

  CTCLVariable window(string("V1x90Windows"), false);
  window.Bind(interp);
  const char* windowInfo = window.Get(TCL_GLOBAL_ONLY,
				      const_cast<char*>(name.c_str()));
  if (!windowInfo) {
    std::cerr << "WARNING:: SpecTcl misconfigured, V1x90Windows(" 
	      << name << ") is undefined.\n";
    exit(-1);
  }

  double junk, resolution;
  sscanf(windowInfo, "%lf %lf %lf", &junk, &junk, &(info.s_chansToNs));


  // Let spectcl do the mapping at histogram time, rather than having the
  // tree parameter do it for us.

  double low = 0.0;
  double high = static_cast<double>(1 << 20);

  // For now it's not an error to have too many adcBaseList elements.
  // nor too few for that matter.
  // note that elements that contain "" are not defined.

  for (int i = 0; (i < baseNames.size()) && (i < nParams); i++) {
    if (baseNames[i] != string("")) {
      info.s_parameters[i] = new CTreeParameterArray(baseNames[i],
						     low, high, 
						     "time",
						     depth, 0);
    }
  }
  CTreeParameter::BindParameters(); // Ensure these new ones are bound.
}
