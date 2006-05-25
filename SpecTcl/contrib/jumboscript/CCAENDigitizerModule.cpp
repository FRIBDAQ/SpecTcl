#include <config.h>
#include <histotypes.h>
#include "CCAENDigitizerModule.h"    				
#include <TCLInterpreter.h>
#include <TCLResult.h>
// #include <Globals.h>
#include <string.h>
#include <Histogrammer.h>
#include <Parameter.h>
#include <BufferDecoder.h>
#include <CConfigurationParameter.h>
#include <CIntArrayParam.h>
#include <CIntConfigParam.h>
#include <CStringArrayparam.h>
#include <CStringConfigParam.h>
#include <Analyzer.h>
#include <TranslatorPointer.h>

#include <assert.h>
#ifdef HAVE_STD_NAMESPACE
using namespace std;
#endif



// Mask definitions:
    // All data words have these bits:
    
static const unsigned int ALLH_TYPEMASK(0x700);
static const unsigned int ALLH_TYPESHIFT(8);
static const unsigned int ALLH_GEOMASK(0xf800);
static const unsigned int ALLH_GEOSHIFT(11);

    // High part of header.
    
static const unsigned int HDRH_CRATEMASK(0x00ff);
static const unsigned int HDRH_CRATESHIFT(0);

    // Low part of header.
    
static const unsigned int HDRL_COUNTMASK(0X3f00);
static const unsigned int HDRL_COUNTSHIFT(8);

    // High part of data:
    
static const unsigned int DATAH_CHANMASK(0x3f);
static const unsigned int DATAH_CHANSHIFT(0);

    // Low part of data:
    
static const unsigned int DATAL_UNBIT(0x2000);
static const unsigned int DATAL_OVBIT(0x1000);
static const unsigned int DATAL_VBIT(0x40000);
static const unsigned int DATAL_DATAMASK(0x0fff);

    //  High part of trailer:
    
static const unsigned int TRAILH_EVHIMASK(0x00ff);

    // Word types:
    
static const unsigned int HEADER(2);
static const unsigned int DATA(0);
static const unsigned int TRAILER(4);
static const unsigned int INVALID(6);

/*!
    Constructs a digitizer module.    This module is responsible for
    decoding data from a 7xx CAEN digitizer.. They all have a common
    format.  See Unpack for more information about the data format.
    This decoder supports the following parameters:
    - crate   - Integer parameter that is the crate number to be matched
	         against the data.
    - slot     - Integer parameter that is the geographical address to be matched
                   against the data.
    - parameters - String array that defines the set of parameters that
		the data will be unpacked into.  The array has 32 elements,
		one for each channel.  */
CCAENDigitizerModule::CCAENDigitizerModule (CTCLInterpreter& rInterp,
					    const string&      rName)
  :  CModule( rName, rInterp),
     m_nCrate(0),
     m_nSlot(0),
     m_pCrateConfig(0),
     m_pSlotConfig(0),
     m_pParamConfig(0)
  
{  
     

  CreateMap(32);		// Create the parameter map (all undefed now).

    // Register the parameters:

    m_pCrateConfig = (CIntConfigParam*)*AddIntParam(string("crate"));
    m_pCrateConfig->setRange(0, 0xff);
    m_pSlotConfig   = (CIntConfigParam*)*AddIntParam(string("slot"));
    m_pSlotConfig->setRange(0, 0x1f);

    m_pParamConfig= (CStringArrayparam*)*AddStringArrayParam(string("parameters"), 32);

    // The remaining parameters are ignored but registered for compatibility
    // with the readout software (to support a unified readout script).
    //

    AddIntArrayParam(string("threshold"), 32, 0);
    AddBoolParam(string("keepunder"), false);
    AddBoolParam(string("keepoverflow"), false);
    AddBoolParam(string("card"), true);
    AddBoolParam(string("geo"),  true);    // Geographical addressing.
    AddIntArrayParam(string("enable"), 32, 1);
    AddIntParam(string("base"), 0);
    AddBoolParam(string("multievent"), false);
    AddIntParam(string("fastclearwindow"), 0);
    AddIntParam(string("waitloops"), 20); // Readout only.

    //  V775 specific parameters:

    AddIntParam(string("range"), 500);
    AddBoolParam(string("commonstart"), true);

    // V792 specific parameters:

    AddIntParam(string("Iped"), 0);

} 
/*!
    Destructor.  No Action required.
*/
 CCAENDigitizerModule::~CCAENDigitizerModule ( )  //Destructor - Delete dynamic objects
{

}
// Functions for class CCAENDigitizerModule

/*!

   Called prior to processing the first event.  The parameters are decoded as follows:
   - crate - unpacked into m_nCrate.
   - slot   - unpacked into m_nSlot.
   - parameters - Translated into parameter ids and put in m_aParameterMap.
		   Untranslatable parameters map to -1 and the corresponding channels
		    will not be placed in parameter elements.

*/
void
CCAENDigitizerModule::Setup(CAnalyzer& rAnalyzer,
			    CHistogrammer& rHistogrammer)  
{ 
    // The crate and slot are trivial:
    
    m_nCrate = m_pCrateConfig->getOptionValue();
    m_nSlot   = m_pSlotConfig->getOptionValue();
    
    // For the parameter we need to get the histogrammer and 
    // translate names into parameter ids:
    
    for(int i =0; i < 32; i++) {
	string ParamName = (*m_pParamConfig)[i];
	MapElement(i, ParamName);
    }
}  

/*! 

Determines if the next item in the event stream is the
specified digitizer.  If so, the digitizer is unpacked into
the parameters as specified by the parameter map.
CAEN event data looks like a sequence of longwords.

\verbatim
Header | Geo 5bits|0|1|0| Crate 8 bits| 0 | 0| nChannels 6bits| unused 8bits|

Data   | Geo 5bits| mbz 5bits| Channel 6 bits|x|x|Un|Ov| Value: 12 bits|

Trailer| Geo 5bits| 1|0|0| Event number        24 bits                          |

\endverbatim
The header is used to determine if this is data from the right slot.
\note The words within the long word are stored in big endian order while the
    bytes within are in the order of the generating system.  Therefore we treat the
    data as word data.

    \param pEvent (TranslatorPointer<UShort_t> [in]) 
    Pointer to the raw event chunk that 
       could be our event chunk.
    \param rEvent (CEvent& [out])  Reference to the event array we will fill in.
    \param rAnalyzer (CAnalyzer& [in]) Reference to our analyzer.
    \param rDecoder (CBufferDecoder& [in] Reference to our buffer decoder.
  \return - the address of the next section of the event to unpack (unchanged
		if the event segment being pointed to does not match this module.
*/
TranslatorPointer<UShort_t>
CCAENDigitizerModule::Unpack(TranslatorPointer<UShort_t> pEvent, 
			     CEvent& rEvent, 
			     CAnalyzer& rAnalyzer, 
			     CBufferDecoder& rDecoder)  
{ 

    // Make an appropriately translating pointer:

  TranslatorPointer<UShort_t> p(pEvent);

    UShort_t HeaderTop = *p;

    
    // To decode, Header must be a header word and match our crate/slot:
    
    UShort_t nSlot = (HeaderTop & ALLH_GEOMASK) >> ALLH_GEOSHIFT;
    UShort_t nType= (HeaderTop & ALLH_TYPEMASK) >> ALLH_TYPESHIFT;
    UShort_t nCrate=(HeaderTop & HDRH_CRATEMASK) >> HDRH_CRATESHIFT;
    
    if( (nType == HEADER)     && 
        ( nCrate == m_nCrate) &&
        ( nSlot   == m_nSlot)) {                  // match:
	++p;
	UShort_t nChannels = (*p & HDRL_COUNTMASK) >> HDRL_COUNTSHIFT;
	++p;
	// Unpack the channels -> rEvent.
	
	assert(nChannels <= 32);
	for(int i =0; i < nChannels; i++) {
	    UShort_t nChan = (*p & DATAH_CHANMASK) >> DATAH_CHANSHIFT;
	    ++p;
	    int nId = Id(nChan);
	    if( nId >=0 && 
	       ((*p & (DATAL_UNBIT | DATAL_OVBIT)) == 0)) {
		rEvent[nId] = (*p & DATAL_DATAMASK);
	    }
	    ++p;
	}

	p += 2;			// Skip the trailer longword too.
    }
    
    return p;
}
/*!
   Return a string indicating what type of module this is:
*/
string 
CCAENDigitizerModule::getType() const
{
    return string("caen7xx");
}
