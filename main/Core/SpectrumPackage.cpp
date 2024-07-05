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

static const char* Copyright = "(C) Copyright Michigan State University 2008, All rights reserved";
//  CSpectrumPackage.cpp
// Interfaces the TCL Command processing objects to
// spectrum manipulation functions.  This insulates the
// commands from changes in the underlying system 
//     
//
//   Author:
//      Ron Fox
//      NSCL
//      Michigan State University
//      East Lansing, MI 48824-1321
//      mailto:fox@nscl.msu.edu
//

//////////////////////////.cpp file/////////////////////////////////////////////////////

//
// Header Files:
//

#include <config.h>
#include "SpectrumPackage.h"                               
#include "ClearCommand.h"
#include "BindCommand.h"
#include "UnbindCommand.h"
#include "SpectrumCommand.h"
#include "ChanCommand.h"
#include "Globals.h"
#include "SpectrumFactory.h"
#include "SpectrumFactoryException.h"
#include "WriteCommand.h"
#include "ReadCommand.h"
#include "CM2Projection.h"

#include <Exception.h>
#include <RangeError.h>
#include <ErrnoException.h>
#include <DictionaryException.h>
#include <SnapshotSpectrum.h>
#include <Parameter.h>
#include <TCLInterpreter.h>
#include <TCLResult.h>
#include <SpectrumFormatter.h>
#include <SpecTcl.h>
#include <DisplayInterface.h>
#include <Display.h>

#include <MPITclPackagedCommand.h>
#include <TclPump.h>

#include <iostream>

#include <tcl.h>
#include <stdio.h>
#include <assert.h>
#include <math.h>
#include <string.h>
#include <errno.h>
#include <limits.h>
#include <sstream>

#include <string>
#include <stdexcept>


using namespace std;

#include "BindTraceSingleton.h"


// Static member attributes.

UInt_t CSpectrumPackage::m_nNextId = 0;

// Static unbound members:

struct SpecTypes {
  const char           *pName;
  SpectrumType_t eType;
};

static const SpecTypes aSpecTypes[] = {
  {"1",    ke1D},
  {"1d",   ke1D},
  {"2",    ke2D},
  {"2d",   ke2D},
  {"g1",   keG1D},
  {"g1d",  keG1D},
  {"g2",   keG2D},
  {"g2d",  keG2D},
  {"s",    keSummary},
  {"S",    keStrip},
  {"summary", keSummary},
  {"b",    keBitmask},
  {"bitmask", keBitmask},
  {"m2",   ke2Dm},
  {"gd",   keG2DD},
  {"gs",   keGSummary},
  {"2dmproj", ke2DmProj }

};
static const UInt_t nSpecTypes = sizeof(aSpecTypes)/sizeof(SpecTypes);

struct DataTypes {
  const char*      pName;
  DataType_t eType;
};

static const DataTypes aDataTypes[] = {
  {"byte",   keByte },
  {"word",   keWord },
  {"long",   keLong },
  {"float",  keFloat},
  {"double", keDouble}
};
static const UInt_t nDataTypes = sizeof(aDataTypes)/sizeof(DataTypes);



//////////////////////////////////////////////////////////////////////////
//
// Function:
//   CSpectrumPackage (CTCLInterpreter* pInterp, CHistogrammer *pHistogrammer)
// Operation Type:
//   Constructor.
//
CSpectrumPackage::CSpectrumPackage (CTCLInterpreter* pInterp,
                    CHistogrammer*   pHistogrammer,
                    CDisplayInterface *pDisplay) :
  m_pHistogrammer(pHistogrammer),
  m_pSpectrum(0),
  m_pClear(0),
  m_pBind(0),
  m_pUnbind(0),
  m_pChannel(0),
  m_pWrite(0),
  m_pRead(0),
  m_pDisplay(pDisplay)
{
  auto spectrumInner = new CSpectrumCommand(pInterp, "::spectcl::serial::spectrum");
  addCommand(spectrumInner);
  m_pSpectrum = new CMPITclPackagedCommand(*pInterp, "spectrum", spectrumInner);
  addCommand(m_pSpectrum);

  auto clearInner = new CClearCommand(pInterp, "::spectcl::serial::clear");
  addCommand(clearInner);
  m_pClear = new CMPITclPackagedCommand(*pInterp, "clear", clearInner);
  addCommand(m_pClear);

  auto sbindInner = new CBindCommand(pInterp, "::spectcl::serial::sbind");
  addCommand(sbindInner);
  m_pBind = new CMPITclPackagedCommand(*pInterp, "sbind", sbindInner);
  addCommand(m_pBind);

  auto unbindInner = new CUnbindCommand(pInterp, "::spectcl::serial::unbind");
  addCommand(unbindInner);
  m_pUnbind = new CMPITclPackagedCommand(*pInterp, "unbind", unbindInner);
  addCommand(m_pUnbind);

  auto chanInner = new ChannelCommand(pInterp, "::spectcl::serial::channel");
  addCommand(chanInner);
  m_pChannel = new CMPITclPackagedCommand(*pInterp, "channel", chanInner);
  addCommand(m_pChannel);

  auto writeInner = new CWriteCommand(pInterp, "::spectcl::serial::swrite");
  addCommand(writeInner);
  m_pWrite = new CMPITclPackagedCommand(*pInterp, "swrite", writeInner);
  addCommand(m_pWrite);

  auto readInner = new CReadCommand(pInterp, "::spectcl::serial::sread");
  addCommand(readInner);
  m_pRead = new CMPITclPackagedCommand(*pInterp, "sread", readInner);
  addCommand(m_pRead);                 // In Event sink pipeline.
}
//////////////////////////////////////////////////////////////////////////
//
//  Function:
//     ~CSpectrumPackage()
// Operation Type:
//     Destructor
//
CSpectrumPackage::~CSpectrumPackage ( )
{
  delete m_pSpectrum;
  delete m_pClear;
  delete m_pBind;
  delete m_pUnbind;
  delete m_pChannel;
  delete m_pWrite;
  delete m_pRead;
}

std::string 
CSpectrumPackage::getSignon() const {
  return Copyright;
}
/*!
   Creates a spectrum and enters it in the Sorter's spectrum dictionary.
   The Spectrum factory is used to validate parameters and create the 
   spectrum.  We:
   -  translate the spectype and data type parameters into 
       SpecType_t and DataType_t's
   -  Use the factory to create the spectrum and 
   - install the spectrum in the sorter. Translating exceptions into
      appropriate rResult return strings.

      \param <tt> rInterp  (CTCLInterpreter&)</tt>
          Interpreter running the command.  setResult is used to set the result
          string
	  the text that is returned from the command substitution.
      \param <tt> pName   (const char* [in])            </tt>
          The name of the spectrum to create.
      \param <tt> pSpectype (const char* [in])           </tt>
          The type of spectrum being created.  This will be 
	  translated to a SpectrumType_t enumerator by the
	  SpectrumType helper function.
      \param <tt> rvParameterNames (vector<string>& [in]) </tt>
          A vector of the names of parameters required by
	  a spectrum.  The parameter names are passed uninterpreted
	  to the spectrum factory which will use them to lookup
	  the associated parameter description objects.
      \param <tt> nChannels (vector<UInt_t>& [in])       </tt>
          Vector of number of channels for each axis.  The number
	  of elements in this vector will depend on the spectrum
	  type.
      \param <tt> fLows (vector<Uint_t>& [in])          </tt>
          Vector of low limits represented by channel 0's of the
	  axes.
      \param <tt> fHighs  (vector<UInt_t>& [in])          </tt>
         Vector of high limits represented by the last channel
	 of the axes.
      \param <tt> pDataType (const char* [in])        </tt>
         The type of each channel.  This will be translated to
	 a DatatType_t by the Datatype helper function.

    \return int
       A status value that is one of:
       - TCL_OK if the spectrum was created.  In this case,
         rResult will contain the name of the spectrum.
       - TCL_ERROR if the spectrum could not be created.  In this
         case, rResult will contain a textual error message
	 describing why the spectrum could not be created.

*/
int
CSpectrumPackage::CreateSpectrum(CTCLInterpreter& rInterp,
				 const char* pName, 
				 const char* pSpecType,
				 std::vector<std::string>& rvParameterNames,
				 std::vector<UInt_t>&      nChannels,
				 std::vector<Float_t>&     fLows,
				 std::vector<Float_t>&     fHighs,
				 const char*               pDataType)
{
  SpecTcl& api(*(SpecTcl::getInstance()));
  std::string rResult;    // Makes porting a bit easier.
  CSpectrum* pSpec = 0;
  try {
    SpectrumType_t sType = SpectrumType(pSpecType);
    DataType_t dType= Datatype(sType, pDataType);


    pSpec = api.CreateSpectrum(string(pName), sType, dType, 
			       rvParameterNames,
		                nChannels, &fLows, &fHighs);

    api.AddSpectrum(*pSpec);

  }
  catch (CException& rExcept) {
    delete pSpec;		// In case it was the add that did it.
    rResult = rExcept.ReasonText();
    rInterp.setResult(rResult);
    return TCL_ERROR;
  }
  // pre-compute the description string and set it:
  
  pSpec->setTextDescription(DescribeSpectrum(*pSpec, false));
  
  // Return the name as the result.
  
  rResult = pSpec->getName();
  rInterp.setResult(rResult);
  return TCL_OK;
}

/*!
  Create a spectrum that requires separate axis specs,and enter it into the
  histogrammer's spectrum dictionarry.

  \param rInterp   - The interpreter running the command that invoked us.
                     setResult is used to set the command result
  \param pName     - The name of the spectrum to create.
  \param pSpecType - Points to the textual spectrum type. 
  \param xParmeterNames - Names of the parameters on the x axis.
  \param yParameterNames- Names of the parameters on the y axis.
  \param vChannels - Vector of number of channels per axis.
  \param fLows     - Vector of low axis limits.
  \param fHighs    - Vector of high axis limits.
  \param pDataType - Type of data for each channel
 */
int
CSpectrumPackage::CreateSpectrum(CTCLInterpreter& rInterp, const char* pName,
		     const char* pSpecType,
		     std::vector<std::string> xParameterNames,
		     std::vector<std::string> yParameterNames,
		     vector<UInt_t>           vChannels,
		     std::vector<Float_t>     fLows,
		     std::vector<Float_t>     fHighs,
		     const char*              pDataType)
{

  // Wrap the spectrum creation/histogrammer entry in a try block so that we
  // can convert any exceptions into a failed return status:

  CSpectrum* pSpec(0);
  SpecTcl *pApi = SpecTcl::getInstance();
  std::string rResult;
  try {
    SpectrumType_t sType = SpectrumType(pSpecType);
    DataType_t     dType = Datatype(sType, pDataType);

    pSpec = pApi->CreateSpectrum(string(pName),
				 sType, dType,
				 xParameterNames, yParameterNames,
				 vChannels,
				 &fLows, &fHighs);
    pApi->AddSpectrum(*pSpec);
    rResult = pSpec->getName();
    rInterp.setResult(rResult);
    return TCL_OK;
  }
  catch (CException& except) {
    rResult = except.ReasonText();
    rInterp.setResult(rResult);
    return TCL_ERROR;
  }
  catch (string& msg) {
    rResult = msg;
    rInterp.setResult(rResult);
    return TCL_ERROR;
  }
  catch (const char* msg) {
    rResult = string(msg);
    rInterp.setResult(rResult);
    return TCL_ERROR;
  }
  catch (...) {
    rResult = string("Unanticipated exception while creating spectrum");
    rInterp.setResult(rResult);
    return TCL_ERROR;
  }
}

/*!
   Create a spectrum that requires a vector of a vector of parameters.
   At present, the only example of this is a gamma summary spectrum.

   \param rInterp    - A Tcl interpreter running the command. setResult is used to set the result string.
   \param pName       - Name of the new spectrum.
   \param pSpecType   - Pointer to the text string spectrum type.
   \param parameterNames - Vector of parameter name vectors.
   \param nChannels   - Vector of # channels in each axis (only one element).
   \param fLows       - Vector of low values that describe the real coordinates of the axis (only 1 element)
   \param fHighs      - Vector of high values that describe the real coordinates of the axis (only 1 element).
   \param  pDataType   - Pointer to text string channel type (e.g. "long").
   
   \return int
   \retval TCL_OK  - Spectrum was created and entered in the spectrum dictionary.
   \retval TCL_ERROR - a problem was detected either in creating the spectrum or entering it into
                       the dictionary.

   \note SIDE EFFECTS:
   - New spectrum is made.
   - Spectrum dictionary observers will be invoked with creation.
   - Various exceptions will be caught here and turned into result strings and TCL_ERROR returns.

*/
int 
CSpectrumPackage::CreateSpectrum(CTCLInterpreter& rInterp,
				 const char*                            pName,
				 const char*                            pSpecType,
				 std::vector<std::vector<std::string> > parameterNames,
				 std::vector<UInt_t>                    nChannels,
				 std::vector<Float_t>                   fLows,
				 std::vector<Float_t>                   fHighs,
				 const char*                            pDataType)
{
  CSpectrum* pSpec(0);
  SpecTcl*   pApi = SpecTcl::getInstance();
  std::string rResult;                   // Eases the port to not having a CTCLResult param.

  // Wrap the 'business logic' in a try/catch block so that the
  // exceptions can be converted to result strings and TCL_ERROR returns:

  try {
    SpectrumType_t sType = SpectrumType(pSpecType);
    DataType_t     dType = Datatype(sType, pDataType);
    
    pSpec                = pApi->CreateSpectrum(string(pName), sType, dType,
						parameterNames,
						nChannels, &fLows, &fHighs);
    pApi->AddSpectrum(*pSpec);
    rResult = pSpec->getName();
    rInterp.setResult(rResult);

    return TCL_OK;
  }
  catch (CException& except) {
    rResult = except.ReasonText();
    rInterp.setResult(rResult);
    return TCL_ERROR;
  }
  catch (string& msg) {
    rResult = msg;
    rInterp.setResult(rResult);
    return TCL_ERROR;
  }
  catch (const char* msg) {
    rResult = string(msg);
    rInterp.setResult(rResult);
    return TCL_ERROR;
  }
  catch (...) {
    rResult = string("Unanticipated exception while creating spectrum");
    rInterp.setResult(rResult);
    return TCL_ERROR;
  }
}
/**
 * CreateSpectrum
 *     Creates an m2dprojection.
 *
 * @param[out] rInterp - Interpreter running the command. setResult is used to set the result string.
 * @param[in]  pName  - Name of the new spectrum.
 * @param[in]  pType  - Spectrum type.   Must be ke2DmProj
 * @param[in]  parameters - Names of the parameters (x1 y1 x2 y2...).
 * @param[in]  inX        - True if projection is in X or false otherwise.
 * @param[in]  roi        - Vector of gate containers that define the ROI.
 * @param[in]  nchans     - only one element - number of channels on the x axis.
 * @param[in]  lows,highs - only one element each - the low/high limit of the axis.
 * @param[in]  Data Type  - Data type selector.
 * @return  int - TCL_OK or TCL_ERROR
 */
int
CSpectrumPackage::CreateSpectrum(
    CTCLInterpreter& rInterp, const char* pName, const char* pType,
    const std::vector<std::string>& parameters, bool inX,
    const std::vector<CGateContainer*>& roi,
    const std::vector<UInt_t>&  nchans,
    const std::vector<Float_t>&  lows, const std::vector<Float_t>& highs,
    const char* pDataType
)
{
    SpecTcl& api(*SpecTcl::getInstance());
    std::string result;
    try {
        // Convert spectrum and channel types to their enums.
        
        SpectrumType_t type;
        DataType_t     dType;
        
        std::stringstream typeString(pType);
        typeString >> type;
        
        std::stringstream dTypeString(pDataType);
        dTypeString >> dType;
    
        if (type != ke2DmProj) {
            throw std::logic_error(
                "2dm projection spectrum creator invoked with wrong spectrum type"
            );
        }
        // Look up all the parameters and make a vector of parameter defs:
        
        std::vector<CParameter> params;
        for (int i = 0; i < parameters.size(); i++) {
            CParameter* pParam = api.FindParameter(parameters[i]);
            if (!pParam) {
                throw CDictionaryException(
                    CDictionaryException::knNoSuchKey, "Looking up parameter ",
                    parameters[i].c_str()
                );
            }
            params.push_back(*pParam);
        }
        // Pull out the channels, low, high specs.. throwing an error
        // if there are not the right number of them.
        
        int chans;
        float low;
        float high;
        
        if ((nchans.size() != 1) || (lows.size() != 1) || (highs.size() != 1)) {
            throw std::string("Should only be one axis for 2dmproj");
        }
        chans = nchans[0];
        low   = lows[0];
        high  = highs[0];
        
        CSpectrum* pSpec = api.CreateM2Projection(
            pName, dType, params, roi, inX, chans, low, high
        );
        api.AddSpectrum(*pSpec);
        
        result = pName;
        rInterp.setResult(result);
        return TCL_OK;

    } 
    catch (const char* msg) {
        result = std::string(msg);
        rInterp.setResult(result);
    }
    catch (std::string msg) {
        result = msg;
        rInterp.setResult(result);
    }
    catch (CException& re) {
        result = re.ReasonText();
        rInterp.setResult(result);
    }
    catch (std::exception& e) {
        result = e.what();
        rInterp.setResult(result);
    }
    catch (...) {
        result = std::string(
            "Unanticipated exception type caught in CSpectrumPackage::CreateSpectrum for m2dproj"
        );    
        rInterp.setResult(result);
    }
    // TCL_OK returns are done inside the try block:
    
    return TCL_ERROR;
}

/*!
 Returns the set of spectrum definitions.  Each
 Spectrum's properties is stored in a string formatted as
 a TCL List in the following format:

   id name dimensions { parameters...} {resolutions...} ?gatename?


    \param vector<std::string>& rvProperties
        Vector which will be cleared and filled with the
        set of definitions strings.
    \param pattern (const char*)
       A pattern to match.. only spectra with glob matching against the
      pattern are listed.
    \param showGates : bool [false]
       If true, the gate name is also added to the listing.
       This is actually just passed to DescribSpectrum.
*/
void 
CSpectrumPackage::ListSpectra(std::vector<std::string>& rvProperties, 
			      const char* pattern, bool showGates) 
{

  SpecTcl& api(*(SpecTcl::getInstance()));
  rvProperties.erase(rvProperties.begin(), rvProperties.end());
  SpectrumDictionaryIterator p = api.SpectrumBegin();

  for(; p != api.SpectrumEnd(); p++) {

    std::string name = ((p->second)->getName());

    if (Tcl_StringMatch(name.c_str(), pattern) )
      {
	// Spectra have pre-computed definition strings that we use if they
	// are available else we compute/cache one.
	// The showGates flag gets handled here regardless:
    
	
	CSpectrum* rSpec((*p).second);
	std::string d =  rSpec->getTextDescription();
	if (d == "") {
	  //  Need to compute/cache.
	   
	   d = DescribeSpectrum(*rSpec, false);
	   rSpec->setTextDescription(d);
	}
	// If necessary fold in the gate to the definition:
	
	if (showGates) {
	  CTCLString Description(d);
	  const CGateContainer&  g(*(rSpec->getGate()));
	  Description.AppendElement(g.getName());
	  d = std::string((const char*)(Description));
	}
	
	rvProperties.push_back(d);
      }
  }
}


/*!
 Produces a string describing the selected
 spectrum.  The spectrum properties are given in
 the form:
       id   name  dimensions {parameterlist} {resolutionlist} ?gatename?

    \param   rInterp CTCLInterpreter&
          Interpreter running the command. setResult is used to set the result string.
    \param  nId UInt_t
         Identifier of the spectrum to describe.
    \param showGate : bool [false]
         This is passeed on to DescribeSpectrum if true, the
         name of the gate on each spectrum is also provided.
    \return int
       \retval TCL_OK    If the spectrum properties
                                       could be retrieved, then rResult
                                       contains the property list.
       \retval TCL_ERROR If there was a problem and then
                                         rResult provides the details.

*/
Int_t 
CSpectrumPackage::ListSpectrum(CTCLInterpreter& rInterp, UInt_t nId,
			       bool showGates) 
{


  SpecTcl& api(*(SpecTcl::getInstance()));
  std::string rResult;
  CSpectrum* pSpec(0);
  SpectrumDictionaryIterator i = api.SpectrumBegin();
  while(i != api.SpectrumEnd()) {
    if(i->second->getNumber() == nId) {
      pSpec = i->second;
      break;
    }
    i++;
  }


  if(pSpec) {
    rResult += DescribeSpectrum(*pSpec, showGates);
    rInterp.setResult(rResult);
    return TCL_OK;
  }
  else {
    rResult += "Spectrum does not exist";
    rInterp.setResult(rResult);
    return TCL_ERROR;
  }

}
//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    void ClearAll (  )
//  Operation Type:
//     Interface
//
void 
CSpectrumPackage::ClearAll() 
{
// Clears all of the spectra known to the
// current histogrammer.
// 

  SpecTcl& api(*(SpecTcl::getInstance()));
  api.ClearAllSpectra();

}
//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    Int_t ClearSubset ( CTCLInterpreter& rInterp, 
//                        std::vector<std::string>& rvSpectra )
//  Operation Type:
//     Interface.
//
Int_t 
CSpectrumPackage::ClearSubset(CTCLInterpreter& rInterp,
			      std::vector<std::string>& rvSpectra) 
{
// Clears a set of histograms.
//  
// Formal Parameters:
//     CTCLInterpreter&    rInterp:
//           Interpreter running the command.  setResult is used to set the result string.
//      std::vector<std::string>& rvSpectra:
//            Vector of names of spectra to clear.
// Returns:
//      TCL_OK            - All spectra were cleared.
//      TCL_ERROR    - Some spectra could not be cleared.
//                                  rResult is a list of the set of spectra
//                                  which could not be cleared.  The only
//                                  reason a spectrum can't be cleared is that
//                                  it doesn't exist.
  SpecTcl& api(*(SpecTcl::getInstance()));

  Bool_t     Failed = kfFALSE;
  std::string rResult;
  CTCLString ResultString;
  std::vector<std::string>::iterator p = rvSpectra.begin();
  for(; p != rvSpectra.end(); p++) {
    try {
      api.ClearSpectrum(*p);
    }
    catch (CDictionaryException& rDict) {
      ResultString.AppendElement(*p);
      Failed = kfTRUE;
    }
  }

  rResult = (const char*)ResultString;
  rInterp.setResult(rResult);
  return (Failed ? TCL_ERROR : TCL_OK);
}
//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    Int_t ClearSubset ( CTCLInterpreter& rInterp, std::vector<UInt_t>& rvIds )
//  Operation Type:
//     Interface
//
Int_t 
CSpectrumPackage::ClearSubset(CTCLInterpreter& rInterp, std::vector<UInt_t>& rvIds) 
{
// Clears a subset of the spectra.
// 
// Formal Parameters:
//       CTCLResult& rInterp
//           Interpreter running the command.  setResult sets the result string
//       std::vector<UInt_t>& rvIds:
//            Vector of Idents of spectra to clear.
//  Returns:
//      TCL_OK          - All spectra were cleared.
//       TCL_ERROR - Some spectra could not be cleared.
//                                rResult contains a list of spectra which
//                                could not be cleared.  The only reason a 
//                                spectrum can't be cleared is that it doesn't 
//                                exist.
//
  SpecTcl& api(*(SpecTcl::getInstance()));
  std::string rResult;
  Bool_t      Failed = kfFALSE;
  CTCLString  Result;
  std::vector<UInt_t>::iterator p = rvIds.begin();
  for(; p != rvIds.end(); p++) {
    CSpectrum* pSpec = api.FindSpectrum(*p);
    if(pSpec) {
      pSpec->Clear();
    }
    else {
      char sID[100];
      sprintf(sID, "%d", *p);
      Result.AppendElement(sID);
      Failed = kfTRUE;
    }
  }
  rResult = (const char*)(Result);
  rInterp.setResult(rResult);
  return  (Failed ? TCL_ERROR : TCL_OK);
}
//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    Int_t BindAll ( CTCLInterpreter& rInterp)
//  Operation Type:
//     Interface.
//
Int_t
CSpectrumPackage::BindAll(CTCLInterpreter& rInterp)
{
// The current set of spectra bound to the
//  histogrammer memory is unbound.  The
// full set of spectra is bound to the display
//  memory.
// 
// Formal Parameters:
//     CTCLInterpreter& rInterp:
//         References the TCL Interpreter running the command that called us.
// Returns:
//      TCL_OK         - All spectra could be bound.
//      TCL_ERROR - Some spectra could not be bound.
//                               rResult contains a list.  Each list
//                               entry contains two elements.
//                               the first element is the name of the
//                               spectrum which could not be bound and
//                              the second  is the reason it couldn't be bound.

  // This all requires a histotgramer:

  
  SpecTcl& api(*(SpecTcl::getInstance()));
 
  if (m_pHistogrammer) {
    CTCLString Result;

    Bool_t     Failed = kfFALSE;
    SpectrumDictionaryIterator p = api.SpectrumBegin();

    for(; p != api.SpectrumEnd(); p++) {
      CSpectrum* pSpec = (*p).second;
      try {
        CDisplay* pDisplay = api.GetDisplayInterface()->getCurrentDisplay();
        if (pDisplay) {
          makeBinding(*pSpec, *m_pHistogrammer);
        }
      }
      catch (CException& rExcept) {
        Result.StartSublist();
        Result.AppendElement(pSpec->getName());
        Result.AppendElement(rExcept.ReasonText());
        Result.EndSublist();
        Failed = kfTRUE;
      }
    }
    rInterp.setResult((const char*)(Result));
    return (Failed ? TCL_ERROR : TCL_OK);

  } else {
    return TCL_OK;
  }
}
//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    Int_t BindList ( CTCLInterpreter& rInterp, std::vector<std::string>& rvNames )
//  Operation Type:
//     Interface.
//
Int_t 
CSpectrumPackage::BindList(CTCLInterpreter& rInterp,
			   std::vector<std::string>& rvNames) 
{
// Binds a subset of the spectra to the display memory.
// 
// Formal Parameters:
//      CTCLInterpreter& rInterp:
//           References the TCL Interpreter running this command.
//      std::vector<std::string>& rvNames:
//           vector of spectrum names which will be
//           bound.
//  Returns:
//      TCL_OK            - All spectra bound.
//      TCL_ERROR    - Some spectra could not be bound.
//                                  The rResult string contains a list,
//                                   each list element is a two element list
//                                   containing the name of the spectrum which
//                                   could not be bound and the reason it
//                                   couldn't be bound.
//  
  // This only operates if the m_pHistogrammer is not null.  It will be null 
  // in parallel mode for ranks not equal to MPI_EVENT_SINK_RANK (Globals.h)
  // ALl other ranks will return TCL_OK with  an empty result string

  if (m_pHistogrammer) {
    CTCLString Result;
    Bool_t     Failed = kfFALSE;
    CDisplay*  pDisplay = m_pDisplay->getCurrentDisplay();
    SpecTcl& api = *(SpecTcl::getInstance());

    std::vector<std::string>::iterator p = rvNames.begin();
    for(; p != rvNames.end(); p++) {
        try {
            CSpectrum* pSpec = m_pHistogrammer->FindSpectrum(*p);
            if (pSpec) {
              makeBinding(*pSpec, *(api.GetHistogrammer()));
            } else {
                throw CDictionaryException(CDictionaryException::knNoSuchKey,
                                          "binding spectrum by name", *p);
            }
        }
      catch (CException& rExcept) {
        Failed = kfTRUE;
        Result.StartSublist();
        Result.AppendElement(*p);
        Result.AppendElement(rExcept.ReasonText());
        Result.EndSublist();
      }
      catch (...) {
        // Any other exception...

        Failed = kfTRUE;
        Result = "BindToDisplay threw an error of some sort";
      }
    }

    rInterp.setResult((const char*)Result);
    return (Failed ? TCL_ERROR : TCL_OK);
  } else {
    return TCL_OK;
  }

}
//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    Int_t BindList ( CTCLInterpreter rInterp, std::vector<UInt_t>& rIds )
//  Operation Type:
//     Interface.
//
Int_t
CSpectrumPackage::BindList(CTCLInterpreter& rInterp, std::vector<UInt_t>& rIds) 
{
// Binds a list of spectra to the displayer 
// given their ids.
// 
// Formal Parameters
//     CTCLInterpreter& rInterp:
//        The TCL Interpreter running the calling command..
//     std:vector<UInt_t>&
//         Vector containing the IDs  to bind.
// Returns:
//     TCL_OK         - If the spectra were all bound.
//     TCL_ERROR - If any spectra could not be bound.
//                              The result string contains a list
//                              each list element is a 2 element sublist.
//                              of the form { id reason}  where id is the
//                              Id of a spectrum which could not be bound and
//                              reason is the reason it could not be bound.
//

  // This method can only execute if there's a histogramer.  There will not be one
  // in parallel mode for ranks other than MPI_EVENT_SINK_RANK (Globals.h).
  // 
  if (m_pHistogrammer) {
    Bool_t      Failed = kfFALSE;
    CTCLString  Result;

    CDisplay*    pDisplay = m_pDisplay->getCurrentDisplay();
    SpecTcl& api          = *(SpecTcl::getInstance());

    for(auto p=rIds.begin(), end=rIds.end(); p != end; p++) {
        try {
            CSpectrum* pSpec = m_pHistogrammer->FindSpectrum(*p);
            if(pSpec) {
                makeBinding(*pSpec, *(api.GetHistogrammer()));
            }
            else {
                char TextId[100];
                sprintf(TextId, "id=%d", *p);
                throw CDictionaryException(CDictionaryException::knNoSuchId,
                                          "Looking up spectrum to bind",
                                          TextId);
            }
        }
        catch (CException& rExcept) {
            char TextId[100];
            sprintf(TextId, "%d", *p);
            Result.StartSublist();
            Result.AppendElement(TextId);
            Result.AppendElement(rExcept.ReasonText());
            Result.EndSublist();
            Failed = kfTRUE;
        }
    }

    rInterp.setResult((const char*)(Result));
    return (Failed ? TCL_ERROR : TCL_OK);
  } else {
    // If we are not the event sink pipeline just return TCL_OK with no result

    return TCL_OK;
  }
}
//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    Int_t UnbindList ( CTCLInterpreter& rInterp, 
//                       std::vector<std::string> rvNames )
//  Operation Type:
//     Interface
//
Int_t 
CSpectrumPackage::UnbindList(CTCLInterpreter& rInterp, 
			     std::vector<std::string>& rvNames) 
{
// Unbinds a set of spectra from display memory.
// The contents of each unbound spectrum are copied back
//  into local memory and the display program forgets
//  about the spectrum. 
//
// Formal Parameters:
//      CTCLInterpreter& rInterp
//            References the TCL interpreter running this command.
//       std::vector<std::string>& rvNames:
//             Names of the spectra to unbind.
//   Returns:
//        TCL_OK           - All spectra unbound.
//         TCL_ERROR  - Some spectra not unbound.  See
//                                   The various Bind operations for
//                                    the structure of the result string.
//
//
  
  Bool_t                             Failed = kfFALSE;
  CTCLString                         MyResults;
  std::vector<UInt_t>                vXids;

  SpecTcl* pApi                     = SpecTcl::getInstance();
  CHistogrammer* pSorter            = pApi->GetHistogrammer();
  std::string rResult;
  #ifdef WITH_MPI
  if (gMPIParallel && (myRank() != MPI_EVENT_SINK_RANK)) {
    return TCL_OK;
  }
  #endif

  for(auto p=rvNames.begin(), end=rvNames.end(); p != end; p++) {
    try {
        CSpectrum* pSpectrum = pSorter->FindSpectrum(*p);
        if (pSpectrum) {
            removeBinding(*pSpectrum, *pSorter);
        } else {
            throw CDictionaryException(CDictionaryException::knNoSuchKey,
                                       "unbinding spectrum by name", *p);
        }
      }
      catch (CException& rException) {
          Failed = kfTRUE;
          MyResults.StartSublist();
          MyResults.AppendElement(*p);
          MyResults.AppendElement(rException.ReasonText());
          MyResults.EndSublist();
      }
  }
  // Append our results to the reason text, and return the right value
  // depending on Failed.

  if (Failed) {
      rResult += " ";
      rResult += (const char*)(MyResults);
      rInterp.setResult(rResult);
  }

  return (Failed ? TCL_ERROR : TCL_OK);

}
//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    Int_t UnbindList (CTCLInterpreter& rInterp, std::vector<UInt_t>& rvIds )
//  Operation Type:
//     Interface
//
Int_t
CSpectrumPackage::UnbindList(CTCLInterpreter& rInterp, std::vector<UInt_t>& rvIds) 
{
// Unbinds a list of spectra from the display
//
// Formal Parameters:
//     CTCLInterpreter& rInterp:
//          TCL Interpreter runnin gthe command.
//      std::vector<UInt_t>& rvIds
//           Vector of spectrum ids to unbind.
//   Returns:
//       TCL_OK        - All spectra in rvIds are unbound.
//       TCL_ERROR     - some spectra in rvIds are not unbound.
//                       see BindList for the form of the result list.
//

  // The strategy is to create a bindings list and then call the
  // version of UnbindList which operates directly on display bindings.
  // This is done to:
  //  a. Share maximum code.
  //  b. Because the histogrammer unbind function operates directly on
  //     those display bindings.
  //

  Bool_t                        Failed = kfFALSE;
  CTCLString                    MyResults;
  std::vector<UInt_t>           vXids;
  std::vector<UInt_t>::iterator p = rvIds.begin();

  SpecTcl& api = *(SpecTcl::getInstance());

  CDisplay* pDisplay = m_pDisplay->getCurrentDisplay();
  CHistogrammer* pSorter = api.GetHistogrammer();
  std::string rResult;

#ifdef WITH_MPI
  if (gMPIParallel && (myRank() != MPI_EVENT_SINK_RANK)) {
    return TCL_OK;
  }
#endif
  // Build the xid list.. Any failures go into the MyResults string.
  //
  for(auto p=rvIds.begin(), end=rvIds.end(); p != end; p++) {
    try {
          CSpectrum* pSpectrum = pSorter->FindSpectrum(*p);
          if (pSpectrum) {
              removeBinding(*pSpectrum, *pSorter);
          } else {
              throw CDictionaryException(CDictionaryException::knNoSuchId,
                                         "unbinding spectrum by id", *p);
          }
    }
    catch (CException& rExcept) {
      char TextId[100];
      sprintf(TextId, "%d", *p);
      Failed = kfTRUE;

      MyResults.StartSublist();
      MyResults.AppendElement(TextId);
      MyResults.AppendElement(rExcept.ReasonText());
      MyResults.EndSublist();
    }
  }

  if (Failed) {
      rResult += " ";
      rResult += (const char*)(MyResults);
      rInterp.setResult(rResult);
  }

  return (Failed ? TCL_ERROR : TCL_OK);
}


//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    void UnbindAll (  )
//  Operation Type:
//     Interface
//
void 
CSpectrumPackage::UnbindAll() 
{
// Unbinds all spectra from the display  
//
  // Can only be done  if we have a display or a histogtramer...and we won't have either
  // if we're parallel but not MPI_EVENT_SINK_RANK.
#ifdef WITH_MPI
  if (gMPIParallel && (myRank() != MPI_EVENT_SINK_RANK)) {
    return;
  }
#endif
  if (m_pHistogrammer && m_pDisplay) {
    SpecTcl& api = *(SpecTcl::getInstance());

    CDisplay* pDisplay = m_pDisplay->getCurrentDisplay();
    CHistogrammer* pSorter = api.GetHistogrammer();

    for(auto p = m_pHistogrammer->SpectrumBegin();
            p != m_pHistogrammer->SpectrumEnd(); p++) {
        try {
            removeBinding(*(p->second), *pSorter);
            pDisplay->removeSpectrum(*(p->second), *pSorter);
        }
        catch(CException& rException) { } // Some spectra will not be bound.
    }
  }

}
//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    Int_t DeleteList ( CTCLResult& rResult,
//                        std::vector<std::string>& rvNames )
//  Operation Type:
//     Interface.
//
Int_t 
CSpectrumPackage::DeleteList(CTCLInterpreter& rInterp,
			     std::vector<std::string>& rvNames) 
{
//  Deletes a list of spectra.  Note that if the
//  spectrum is bound to the displayer, it is
//  first unbound before being deleted.
//
// Formal parameters:
//      CTCLInterpreter& rInterp:
//         References the interpreter running the command that called us
//      std::vector<std::string>& rNames:
//          vector containing the names of the spectra to
//          delete.
//  Returns:
//        TCL_OK          - Everything worked.
//        TCL_ERROR  - One or more spectra could not
//                                  be deleted.  See BindList for the
//                                  result string fromat.
//
  SpecTcl& api(*(SpecTcl::getInstance()));
  CTCLString  MyResult;
  Bool_t      fFailed = kfFALSE;

  // For each spectrum in the list, we delete it.  We'll return a list of
  // the set of spectra which could not be deleted.  The only reason a
  // spectrum cannot be deleted is that it doesn't exist.  
  // The spectra are unbound if necessary, but the user does not receive
  // the results of the unbind.
  //
  
  UnbindList(rInterp, rvNames);


  std::vector<std::string>::iterator p = rvNames.begin();
  for(auto p = rvNames.begin(), end = rvNames.end(); p != end; p++) {
    CSpectrum* pSpec = api.RemoveSpectrum(*p);
    if(pSpec) {			// Spectrum existed..
      delete pSpec;		// Destroy it.
    }
    else {
      MyResult.StartSublist();
      MyResult.AppendElement(*p);
      MyResult.AppendElement("No such Spectrum");
      MyResult.EndSublist();
      fFailed = kfTRUE;
    }
  }
  // Set result string from the failure list.
  //
  rInterp.setResult((const char*)(MyResult));
  return (fFailed ? TCL_ERROR : TCL_OK);
}
//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    Int_t DeleteList ( CTCLInterpreter& rInterp, std::vector<UInt_t>& rvnIds )
//  Operation Type:
//     Interface
//
Int_t 
CSpectrumPackage::DeleteList(CTCLInterpreter& rInterp, std::vector<UInt_t>& rvnIds) 
{
// Deletes a set of spectra given their Ids.
//
// Formal Parameters:
//      CTCLInterpreter& rInterp
//            The interpreter running the command.
//      std::vector<UInt_t>& rvnIds:
//             Vector of ids of spectra to delete.
// Returns:
//       TCL_OK                - Deletes were all done.
//       TCL_ERROR             - Some deletes not done.  The
//                               result string is formatted as in BindList()
//
  if (m_pHistogrammer) {
    CTCLString  MyResult;
    Bool_t     fFailed = kfFALSE;
    std::vector<std::string> vNameList;
    std::string rResult;

    // Our strategy is to just convert the ids into a vector of names
    // and call the DeleteList which operates on names.

    std::vector<UInt_t>::iterator p = rvnIds.begin();
    for(auto p=rvnIds.begin(), end=rvnIds.end(); p != end; p++) {
      CSpectrum* pSpec = m_pHistogrammer->FindSpectrum(*p);
      if(pSpec) {			// Spectrum exists..
        vNameList.push_back(pSpec->getName());
      }
      else {			// Spectrum does not exist.
        char txtid[100];
        MyResult.StartSublist();
        sprintf(txtid," %d ", *p);
        MyResult.AppendElement(txtid);
        MyResult.AppendElement("No Such Spectrum ID");
        MyResult.EndSublist();
        fFailed = kfFALSE;
      }
    }
    //  The vNameList is a spectrum list which is suitable for
    //  the name drive version of delete list:
    //  Once that's done, we just merge the failure lists and
    //  status codes...

    Int_t tclStat = DeleteList(rInterp, vNameList);
    rResult =  rInterp.GetResultString();
    rResult += (const char*)MyResult;
    rInterp.setResult(rResult);

    if(tclStat != TCL_OK) fFailed = kfTRUE;

    return (fFailed ? TCL_ERROR : TCL_OK);
  } else {
    return TCL_OK;
  }

}
//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    void DeleteAll (  )
//  Operation Type:
//     Interface
//
void 
CSpectrumPackage::DeleteAll() 
{
  if (m_pHistogrammer) {
      SpecTcl& api(*(SpecTcl::getInstance()));

      // Deletes all spectra known to the
      // histogrammer.
      //

      // For each spectrum in the list, we unbind it and then delete it.
      // Note that since deletion invalidates iterators, we repeatedly 
      // delete the front element.
      // 
      CDisplay* pDisplay = m_pDisplay->getCurrentDisplay();
      CHistogrammer* pSorter = api.GetHistogrammer();

      SpectrumDictionaryIterator p;
      while(m_pHistogrammer->SpectrumCount()) {
          p = m_pHistogrammer->SpectrumBegin();
          CSpectrum* pSpec = (*p).second;
          try {
              if ( pDisplay->spectrumBound(pSpec) ) {
                  pDisplay->removeSpectrum(*pSpec, *pSorter);
              }
          }
          catch (CException& rExcept) { // Exceptions in the find are ignored.
          }
          CSpectrum* pSpectrum = api.RemoveSpectrum(pSpec->getName());
          delete pSpectrum;		// Destroy spectrum storage.
    }

  }
}
//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    Int_t ListBindings (CTCLInterpreter& rInterp, std::vector<std::string> rvNames )
//  Operation Type:
//     Interface
//
Int_t 
CSpectrumPackage::ListBindings(CTCLInterpreter& rInterp, 
			       std::vector<std::string>& rvNames) 
{
// Produces a list of the display bindings.
//
// Formal Parameters:
//     CTCLInterpreter& rInterp:
//        interpreter running the command.  
//    std::vector<std::string>&   rvNames:
//        Names of spectra about which to give bindings.
// Returns:
//     TCL_OK            - Successful list.
//     TCL_ERROR         - Failed list and contains the reasons for failures.
//
  if (m_pHistogrammer) {
    std::vector<std::string>::iterator p = rvNames.begin();
    CTCLString                         GoodResults;
    CTCLString                         BadResults;
    Bool_t                             fFailed = kfFALSE;

    // For each spectrum in the list, locate a binding.   Good bindings are
    // put in the GoodResults string while any exception adds entries
    // to the BadResults string and sets fFailed -> kfTRUE
  
    for(; p != rvNames.end(); p++) {
      try {
        UInt_t xid = FindDisplayBinding(*p);
        CSpectrum* pSpec  = m_pHistogrammer->FindSpectrum(*p);
        if(pSpec) {
    FormatBinding(GoodResults, xid, pSpec);
    
        }
        else {
    throw CDictionaryException(CDictionaryException::knNoSuchKey,
            "Looking up bound spectrum",
            *p);
        }
      }
      catch (CException& rExcept) {
        BadResults.StartSublist();
        BadResults.AppendElement(*p);
        BadResults.AppendElement(rExcept.ReasonText());
        BadResults.EndSublist();
        fFailed = kfTRUE;
      }
    }

    // Return the appropriate error code and results string:

    if(fFailed) {
      rInterp.setResult((const char*)BadResults);
      return TCL_ERROR;
    }
    else {
      rInterp.setResult((const char*)GoodResults);
      return TCL_OK;
    }
  } else {
    return TCL_OK;              // Parallel but not in event sink pipeline.
  }
}
//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    Int_t ListBindings (CTCLInterpreter& rInterp, std::vector<UInt_t>& rvIds )
//  Operation Type:
//     interface
//
Int_t 
CSpectrumPackage::ListBindings(CTCLInterpreter& rInterp, 
			       std::vector<UInt_t>& rvIds) 
{
// lists bindings given spectrum ids.
//  The action of this function is identical to
//   the previous one, however spectra are
//   specified by ids.
//
// Formal Parameters:
//     CTCLInterpreter& rInterp:
//         TCL Interpreter running the command that alled us.
//    std::vector<UInt_t>&   rvIds:
//         set of spectrum ids to list bindings for.
// Returns:
//    See previous function.
//
  if (m_pHistogrammer) {
    CTCLString                     GoodList;
    CTCLString                     BadList;
    SpectrumDictionaryIterator     pS;
    std::vector<UInt_t>::iterator  p = rvIds.begin();
    Bool_t                         fFailed = kfFALSE;

    // Each binding is looked up by id and added to the output list.
    // The name from the bindings list is used to lookup the spectrum and
    // get the information on it.

    for(; p != rvIds.end(); p++) {
      try {
        UInt_t     xid   = FindDisplayBinding(*p);
        CSpectrum* pSpec = m_pHistogrammer->FindSpectrum(*p);
        if(!pSpec) {
          char txtid[100];
          sprintf(txtid, "%d", *p);
          throw CDictionaryException(CDictionaryException::knNoSuchId,
                  "Looking up bound spectrum",
                  txtid);
        }
        FormatBinding(GoodList, xid, pSpec);
      }
      catch (CException& rException) {
        char txtid[100];
        sprintf(txtid, "%d", *p);
        BadList.StartSublist();
        BadList.AppendElement(txtid);
        BadList.AppendElement(rException.ReasonText());
        BadList.EndSublist();
      }
    }
    // Fill in result from the appropriate list and return the status.

    if(fFailed) {
      rInterp.setResult((const char*)BadList);
      return TCL_ERROR;
        
    }
    else {
      rInterp.setResult((const char*)GoodList);
      return TCL_OK;
    }
  } else {
    return TCL_OK;   // MPI Parallel but not in MPI_EVENT_SINK_RANK
  }
}
//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    void ListAllBindings (CTCLInterpreter& rInterp)
//  Operation Type:
//     Interface
//
void 
CSpectrumPackage::ListAllBindings(CTCLInterpreter& rInterp, const char* pattern) 
{
// Creates a complete list of the display bindings.
// 
// Formal Parameters:
//      CTCLInterpreter& rInterp:
//               TCL Interpreter running the command.
//      const char* pattern
//             If not null only spectra that match this glob pattern are included in the list.
//             defaults to "*"
  if (m_pHistogrammer) {
    CTCLString ResultList;

    // The strategy is to find the binding for each spectrum.
    // if it exists, then add it to the list.  If not, return the error.
    //
    SpectrumDictionaryIterator p = m_pHistogrammer->SpectrumBegin();
    for(; p != m_pHistogrammer->SpectrumEnd(); p++) {
      try {
        char textId[100];
        CSpectrum *pSpec = (*p).second;
        UInt_t xid = FindDisplayBinding(pSpec->getName());
        const char* name = (pSpec->getName()).c_str();
        if (Tcl_StringMatch(name, pattern))
    {  
      FormatBinding(ResultList,  xid, pSpec);
    }
      }
      catch(CException& rExcept) { // No match .. ignore.
      }
    }
    rInterp.setResult((const char*)ResultList); // Put output list in the result string.
  } 
}
//////////////////////////////////////////////////////////////////////////////
//
//  Function:       
//     GetChannel(CTCLInterpreter& rInterp, const string& rName,
//                const UInt_t* pIndices)
//  Operation Type: 
//     selector.
Bool_t 
CSpectrumPackage::GetChannel(CTCLInterpreter& rInterp, const string& rName,
			     const vector<UInt_t>& rIndices)  
{
  // Retrieves a channel value from a spectrum.
  // 
  // Formal Parameters:
  //    CTCLInterpreter& rInterp:
  //       Interpreter that's running this command.
  //   const string& rName:
  //       Name of spectrum.
  //   const vector<UInt_t>& pIndices:
  //       list of indexes to pass to the operator [].
  //  Returns:
  //      Bool_t   kfTRUE - It worked.
  //      Bool_t   kfFALSE - it failed.

  if (m_pHistogrammer) {
    std::string rResult;
    try {
      // Get a pointer to the spectrum object.
      //
      CSpectrum* pSpec = m_pHistogrammer->FindSpectrum(rName);
      if(pSpec == (CSpectrum*)kpNULL) {
        throw CDictionaryException(CDictionaryException::knNoSuchKey,
          "Looking up spectrum for channel get",
          rName);
      }

      UInt_t* pIds = ValidateIndices(pSpec, rIndices);
      ULong_t nChan = (*pSpec)[pIds];
      delete []pIds;

      // Now the nChan must be turned into a string which is placed in the
      // reason text:
      //

      char sChan[20];
      sprintf(sChan, "%lu", nChan);
      rResult += sChan;
      rInterp.setResult(rResult);

    }
    catch (CException & rExcept) {
      rResult += rExcept.ReasonText();
      rInterp.setResult(rResult);
      return TCL_ERROR;
    }
    return kfTRUE;
  } else {
    return kfTRUE;     // MPI Parallel but not MPI_EVENT_SINK_RANK
  }  
}
//////////////////////////////////////////////////////////////////////////////
//
//  Function:       
//     SetChannel(CTCLInterpreter& rInterp, const string& rName, const UInt_t* 
//                pIndices, ULong_t nValue)
//  Operation Type: 
//     Mutator
Bool_t 
CSpectrumPackage::SetChannel(CTCLInterpreter& rInterp, const string& rName, 
			     const vector<UInt_t>& rIndices, ULong_t nValue)  
{
  // Sets the value of a spectrum channel to a
  // particular value.
  //
  // Formal Parameters:
  //     CTCLInterpreter& rInterp
  //        Interpreter running the command that called us.
  //     const string& rName
  //        Name of spectrum to modify.
  //     const UInt_t* pIndices:
  //       list of spectrum indices.
  //     ULong_t nValue:
  //       New value for the channel.
  // Returns:
  //    kfTRUE  - Success, with new vailue in the result string.
  //    kfFALSE - Failure with reason in the Result string.   
  //
  if (m_pHistogrammer) {
    std::string rResult;
    try {
      // Get a pointer to the spectrum object.
      //
      CSpectrum* pSpec = m_pHistogrammer->FindSpectrum(rName);
      if(pSpec == (CSpectrum*)kpNULL) {
        throw CDictionaryException(CDictionaryException::knNoSuchKey,
          "Looking up spectrum for channel get",
          rName);
      }

      UInt_t* pIds = ValidateIndices(pSpec, rIndices);
      
      ULong_t nOldValue = (*pSpec)[pIds];
      pSpec->set(pIds, nValue);
      delete []pIds;                      // Was a memory leak!!
      
      // Result code will be the old value.
      
      char sValue[20];
      sprintf(sValue, "%ld", nOldValue);
      rResult += sValue;
      rInterp.setResult(rResult);
    }
    catch (CException & rExcept) {
      rResult += rExcept.ReasonText();
      rInterp.setResult(rResult);
      return kfFALSE;
    }
    return kfTRUE;
  } else {
    return kfTRUE;  // MPI Parellel but  not MPI_EVENT_SINK_RANK
  }

}
/////////////////////////////////////////////////////////////////////////
//
// Function:
//    Int_t Write(string& rResult, const string& rSpectrum,
//                ostream& rOut, CSpectrumFormatter* pFormat)
//
// Operation type:
//    Package service.
//
Int_t
CSpectrumPackage::Write(string& rResult, const string& rSpectrum,
			ostream& rOut, CSpectrumFormatter* pFormat)
{
  //  Writes a spectrum to a file using a formatter supplied by the caller.
  //
  // Formal Parameters:
  //    string& rResult:
  //       Result string will contain any errors reported from the write.
  //    const string& rSpectrum:
  //       Identifies the spectrum to write.
  //    ostream& rOut:
  //       Identifies the I/O stream to which to write data.
  //    CSpectrumFormatter* pFormat:
  //       Points to a formatter which will actually to the write.
  //

  if (m_pHistogrammer) {
    try {
      CSpectrum* pSpectrum =  m_pHistogrammer->FindSpectrum(rSpectrum);
      if(!pSpectrum) {
        throw CDictionaryException(CDictionaryException::knNoSuchKey,
          "CSpectrumPackage::Write() spectrum lookup",
          rSpectrum);
      }
      ParameterDictionary& rDict((ParameterDictionary&)
              m_pHistogrammer->getParameterDictionary());
      pFormat->Write(rOut, *pSpectrum, rDict);
    }
    catch (CException& rException) {
      rResult = rException.ReasonText();
      return TCL_ERROR;
    }
    catch (...) {
      rResult = "Unrecognized exception caught at CSpectrumPackage::WRite() ";
      rResult += " -- Continuing.";
      return TCL_ERROR;
    }
    return TCL_OK;
  } else {
    return TCL_OK;           // MPI Parallel but not MPI_EVENT_SINK_RANK
  }
}
/////////////////////////////////////////////////////////////////////////
//
// Function:
//   Int_t  Read(string& rResult, istream& rIn,
//	         CSpectrumFormatter* pFormat, SpectrumReadFlags fFlags)
// Operation Type:
//    Package service.
//
Int_t
CSpectrumPackage::Read(string& rResult, istream& rIn,
		       CSpectrumFormatter*       pFormat, 
		       UInt_t                    fFlags)
{
  // Reads a spectrum from file.  Once the spectrum is read, 
  // the fFlags parameter determines what is done to it, as described below.
  // Regardless, a new spectrum is entered into the spectrum dictionary.
  //
  // Formal Parameters:
  //   string& rResult   [out]:
  //      Returns the result string which is the name of the 
  //      generated spectrum  if the operation was
  //      successful or contains an error message if the operation failed.
  //   istream& rIn      [modified]:
  //      Stream from which the spectrum data is read.
  //   CSpectrumFormatter* pFormat   [in]:
  //      Pointer to an object which knows how to extract a spectrum from
  //      the stream.
  //   UInt_t   fflags    [in]:
  //      Is a bit encoded set of flags which determines what is done to 
  //      the spectrum once it has been read in.  The bits are as follows:
  //         Bit             Meaning
  //         ================================================================
  //         fReplace        If a spectrum named the same as the one read from
  //                         file is already defined in the histogrammer's
  //                         spectrum dictionary, then it is replaced by the
  //                         newly read spectrum.
  //
  //         fLive           If all of the parameters which make up the
  //                         spectrum are defined, the spectrum is
  //                         made into a live spectrum.  Otherwise it
  //                         is made static.  A static spectrum is a snapshot
  //                         which will not be incremented.
  //  
  //         fBind           Attempts to bind the spectrum into the displayer's
  //                         namespace.
  // Returns:
  //    TCL_OK      - Success.
  //    TCL_ERROR   - On failure.

  if (m_pHistogrammer && m_pDisplay) {
    SpecTcl& api(*(SpecTcl::getInstance()));

    ParameterDictionary& rDict((ParameterDictionary&)
                m_pHistogrammer->getParameterDictionary());
    CSpectrum*           pSpectrum(0);
    std::string          originalName;
    CSpectrum*           pOld(0);              // Will hold old spectrum.

    CDisplay* pDisplay = m_pDisplay->getCurrentDisplay();
    std::pair<std::string, CSpectrum*> specread;
    
    // First thing to try to do is to get the spectrum formatter to read the
    // spectrum from file.  We'll get a  pointer to a newly allocated 
    // filled in spectrum.  The flags will determine if we are to wrap
    // a snapshot spectrum around this.

    try {
      specread = pFormat->Read(rIn, rDict);
      originalName = specread.first;
      pSpectrum    = specread.second;
      
      
      //
      // Process the Replace flag, this determines if we are allowed to
      // overwrite an existing spectrum or if we must uniquify the name.
      //
      if((fFlags & fReplace) == 0) { // Must uniquify name:
          pSpectrum->renameSpectrum(UniquifyName(originalName).c_str());
      }
      else {			// Replace existing spectrum if there's one.
          
          pOld = api.FindSpectrum(originalName);
          if (pOld) {        
            // Apply any old gate to the new spectrum - doesn't matter if this is
            // a snapshot or not:
            CGateContainer* pGate = (CGateContainer*)pOld->getGate();
            if(pGate) {
              pSpectrum->ApplyGate(pGate);
            }
            // If needed unbind the old spectrum.
            
            pDisplay->removeSpectrum(*pOld, *m_pHistogrammer);
            api.RemoveSpectrum(originalName);
            
            delete pOld;             // Should also unbind.
          }
          
          pSpectrum->renameSpectrum(originalName.c_str());   // Now that the old one is gone.
      }
      //  Process the Live flag: This determines if we need to wrap the
      //  spectrum around a snapshot spectrum container:
      //
      if( !((fFlags & fLive) && AllParamsExist(pSpectrum))) {  // Must wrap
          pSpectrum = new CSnapshotSpectrum(*pSpectrum, kfTRUE);
      }
      //  We now have a viable spectrum.  The spectrum is entered into the
      //  dictionary.
      //
      m_pHistogrammer->AddSpectrum(*pSpectrum);

      if(fFlags & fBind) {	// Bind it if requested.
          pDisplay->addSpectrum(*pSpectrum, *(api.GetHistogrammer()));
      }
    }
    catch (CException& rExcept) {	// All exceptions drop here.
      if(pSpectrum) {		// It may have been entered in the hgrammer.
        api.RemoveSpectrum(pSpectrum->getName());
        delete pSpectrum;
      }
      string Reason(rExcept.ReasonText()); // Haul out the reason code.
      rResult = Reason + string(" ") +
        string(rExcept.WasDoing()); // return it to the caller 
      return TCL_ERROR;		// along with an error completion status.
    }
    catch (...) {
      rResult = string( " some unexpected exception type was caught reading the spectrum");
      if(pSpectrum) {		// It may have been entered in the hgrammer.
        api.RemoveSpectrum(pSpectrum->getName());
        delete pSpectrum;
      }
      return TCL_ERROR;
    }
    //
    // Control passes here only if the spectrum was read in with no errors.
    // The name is returned as the result string and a success indication
    // is returned for the function value.
    //
    rResult = pSpectrum->getName();
    pSpectrum->createStatArrays(pSpectrum->Dimensionality());
    return TCL_OK;

  } else {
    return TCL_OK;                 // MPI  Parallel but not MPI_EVENT_SINK_RANK
  }
}
/*!
    Given a spectrum, this function describes it in 'standard' form.
    Standard form is a Tcl formatted list.  The list elements are in
    order:
    - id    The id number of the spectrum.
    - name  The name of the spectrum (used by the user to refer to
            the spectrum.
    - type  A string that describes the spectrum type.
    - Parms A list of parameters that make up the spectrum.
    - Axes  A list of axis definitions required by the spectrum.
            The axis defintions are themselves unconditionally
	    a list of the form {low hi channels}   The number
	    of axis definitions will depend on the spectrum type,
	    but the spectrum base class provides member functions
	    that allow us to mechanically unravel how many there
	    should be, and individually fetch them regardless of
	    spectrum type.
    - datatype - the type of each channel of the spectrum (e.g. word).
    
    @note:  For the ke2DmProj spectrum parms consists of a three element list.
            The first element of the list is the list of X/Y parameters
            x/y interleaved.  The second element is either x or y indicating
            the projection direction.  The last element is the list of gates
            that make up the projection ROI.  Yep  it's wierd but so is this
            spectrum.
    
    Note from the above, that even if a spectrum was defined with 
   one or more axis definitions in the simplified nbits format,
   the axis will be described in the full low, hi, channels format.

   \param <tt> rSpectrum (CSpectrum& [in]): </tt>
      The spectrum to describe.
   \param <tt>showGate (bool [in] default = false): </tt>
      If true, an additional element is added to the spectrum
      information that lists the name of the gate applied to the
      spectrum.

   \retval std::string
       A description of the spectrum (TCL list rendered as a string)
*/
std::string
CSpectrumPackage::DescribeSpectrum(CSpectrum& rSpectrum, bool showGate)
{
  // Given a spectrum, describes it in standard form.

  CTCLString Description;
  char       txtNum[100];
  CSpectrum::SpectrumDefinition Def = rSpectrum.GetDefinition();

  sprintf(txtNum, "%d", Def.nId);
  Description.AppendElement(txtNum);

  Description.AppendElement(Def.sName);

  Description.AppendElement(SpecTypeToText(Def.eType));


  //
  // List the parameters in the spectrum;
  // If the spectrum type is keG2DD then it is a 2d Gamma Deluxe and 
  // must have two parameter lists, X and Y parameters otherwise
  // a single parameter list.
  //

  auto api = SpecTcl::getInstance();
  Description.StartSublist();	// Regardless there's an outer sublist:

  if(Def.eType == keG2DD) {
    Description.StartSublist();	// X parameters:
    std::vector<UInt_t>::iterator p;
    for (p = Def.vParameters.begin(); p != Def.vParameters.end(); p++) {
      CParameter* pPar = api->FindParameter(*p);
      Description.AppendElement(pPar ? pPar->getName() : 
				std::string("--Deleted Parameter--"));
    }
    Description.EndSublist();   // X parameters.
    Description.StartSublist();	// Y parameters:
    for (p = Def.vyParameters.begin(); p != Def.vyParameters.end(); p++) {
      CParameter* pPar = api->FindParameter(*p);
      Description.AppendElement(pPar ? pPar->getName() :
				std::string("--Deleted Parameter--"));
    }
    Description.EndSublist();
  }
  else if (Def.eType == keGSummary) {
    std::vector<UInt_t>::iterator p;
    bool newSublist = true;
    for (p = Def.vParameters.begin(); p != Def.vParameters.end(); p++) {
      if(newSublist) {
        newSublist = false;
        Description.StartSublist();
      }
      UInt_t id = *p;
      if (id == UINT_MAX) {
        Description.EndSublist();
        newSublist = true;
      } else {
        CParameter* pPar = api->FindParameter(*p);
        Description.AppendElement(pPar ? pPar->getName() :
				  std::string("--Deleted Parameter--"));
      }
    }
  }
  else if (Def.eType == ke2DmProj) {
    bool x;
    std::vector<CGateContainer*> roiGates;
    if (Def.eDataType == keLong) {
        CM2ProjectionL *pSpec = reinterpret_cast<CM2ProjectionL*>(&rSpectrum);
        x = pSpec->isXprojection();
        roiGates = pSpec->getRoiGates();
    } else {
        CM2ProjectionW* pSpec = reinterpret_cast<CM2ProjectionW*>(&rSpectrum);
        x = pSpec->isXprojection();
        roiGates = pSpec->getRoiGates();
    }
    
    // List of parameters.
    
    Description.StartSublist();
    for (int i = 0; i < Def.vParameters.size(); i++) {
        CParameter* xpar = api->FindParameter(Def.vParameters[i]);
        CParameter* ypar = api->FindParameter(Def.vyParameters[i]);
        if (x) {
          Description.AppendElement(
              xpar ? xpar->getName() : std::string("--Deleted Parameter--")
          );
        } else {
          Description.AppendElement(
              ypar ? ypar->getName() : std::string("--Deleted Parameter--")
          );
        }
    }
    Description.EndSublist();
    
    // X/Y direction.
    
    Description.AppendElement(x ? "x" : "y");
    
    // List of gates in the ROI.
    
    Description.StartSublist();
    for (int i = 0; i < roiGates.size(); i++) {
        Description.AppendElement(roiGates[i]->getName());
    }
    Description.EndSublist();
  }
  else {
    std::vector<UInt_t>::iterator p = Def.vParameters.begin();
    for(; p != Def.vParameters.end(); p++) {
      CParameter* pPar = api->FindParameter(*p);
      Description.AppendElement(pPar ? pPar->getName() :
				std::string("--Deleted Parameter--"));
    }
  }
  Description.EndSublist();

  //
  // List the axis dimensions.
  //   The axis sublist is a list of axis definitions.  Each axis definition
  //   is a triplet list of low high nchannels:
  Description.StartSublist();	// List of axes.

  // Once more gamma summaries are strange... just use the second element of the description:
  //
  int start =0;
  if (Def.eType == keGSummary) {
    start = 1;
  }
  for(int i = start; i < Def.nChannels.size(); i++) {
    Description.StartSublist();	// Axis definition

    sprintf(txtNum, "%f", Def.fLows[i]);
    Description.AppendElement(txtNum); // Low...

    sprintf(txtNum, "%f", Def.fHighs[i]);
    Description.AppendElement(txtNum); // High...

    sprintf(txtNum, "%d", Def.nChannels[i]);    // Because of Root TH.
    Description.AppendElement(txtNum);   // Channel count.

    Description.EndSublist();	// End axis definition.
  }
  Description.EndSublist();	// end list of axes.
  
  // List the data type:

  Description.AppendElement(DataTypeToText(Def.eDataType));

  // If requested, show the gate applied:

  if (showGate) {
    const CGateContainer&  g(*(rSpectrum.getGate()));
    Description.AppendElement(g.getName());
  }

  // Return the description string:

  return std::string((const char*)Description);

}
/////////////////////////////////////////////////////////////////////////
//
// Function:
//   UInt_t      FindDisplayBinding(const std::string& rName)
// Operation:
//    Protected Utility
// 
UInt_t
CSpectrumPackage::FindDisplayBinding(const string& rName)
{
  // Locate a display binding associated with a spectrum name.
  // 
  // Formal Paramters:
  //     const std:string& rName:
  //          Name of the spectrum whose binding we'll locate.
  // Returns:
  //     The Xid (Display id number) of the binding.
  // Throws:
  //    CDictionary Exception if the spectrum is not bound.
  // 
  
  if (m_pHistogrammer) {
    CSpectrum *pSpec = m_pHistogrammer->FindSpectrum(rName);
    if(!pSpec) {			// the spectrum must exist in fact..
      throw CDictionaryException(CDictionaryException::knNoSuchId,
              "Looking up spectrum from name",
              rName);
    }

    CDisplay* pDisplay = m_pDisplay->getCurrentDisplay();
    SpectrumContainer spectra = pDisplay->getBoundSpectra();
    size_t nSpectra = spectra.size();
    for(size_t i = 0; i < nSpectra; i++) {
        CSpectrum* pBoundSpec = spectra[i];
        if(pBoundSpec) {
            if(rName == pBoundSpec->getName())
                return i;
        }
    }
    throw CDictionaryException(CDictionaryException::knNoSuchKey,
            "Spectrum with this name is not bound",
            rName);
  } else {
    return 0;               // MPI but not MPI_EVENT_SINK_RANK
  }
}
////////////////////////////////////////////////////////////////////////
//
// Function:
//   UInt_t FindDisplayBinding(UInt_t nId)
// Operation Type:
//   Protected Utility:
//
UInt_t
CSpectrumPackage::FindDisplayBinding(UInt_t nId)
{
  // Locate a display binding associated with a spectrum id.
  // We locate the spectrum, get it's name and call the previous
  // version of this member function.
  //
  // Formal Parameters:
  //    UInt_t  nId:
  //       Number of the spectrum to check.
  // Returns:
  //    The display binding id (Xid).
  // Throws:
  //    CDictionaryException:
  //     a. From the other FindDisplayBinding
  //     b. If nId does not correspond to a spectrum known to the histogrammer
  //

  if (m_pHistogrammer) {
    CSpectrum* pSpec = m_pHistogrammer->FindSpectrum(nId);
    if(pSpec) {
      return FindDisplayBinding(pSpec->getName());
    }
    else {
      char txtId[100];
      sprintf(txtId, "%d", nId);
      throw CDictionaryException(CDictionaryException::knNoSuchId,
              "Looking up spectrum from id",
              txtId);
    }
  } else {
    return 0;             // MPI but not MPI_EVENT_SINK_RANK
  }
}
/////////////////////////////////////////////////////////////////////////
//
// Function:
//   void FormatBinding(CTCLString& rString, UInt_t nXId, 
// 		        CSpectrum*  pSpectrum)
// Operation Type:
//    Protected utility:
//
void
CSpectrumPackage::FormatBinding(CTCLString& rString, UInt_t nXId, 
 		                CSpectrum*  pSpectrum)
{
  // Format a spectrum bindings sublist on the end rstring.
  //
  // Formal Parameters:
  //    CTCLString&   rString:
  //          Desination of the format.
  //    UInt_t nXid:
  //          Spectrum binding
  //    CSpectrum* pSpectrum:
  //          Spectrum.
  //

  char strid[100];
  rString.StartSublist();

  sprintf(strid, "%d", pSpectrum->getNumber());
  rString.AppendElement(strid);
  rString.AppendElement(pSpectrum->getName());
  sprintf(strid, "%d", nXId);
  rString.AppendElement(strid);

  rString.EndSublist();
}
///////////////////////////////////////////////////////////////////////////
//
//  Function:
//    Bool_t    GetNumberList(CTCLInterpreter& rInterp, 
//			      std::vector<UInt_t>& rvIds,
//			      int nArgs, char* pArgs[]);
//  Operation Type:
//    Protected parsing utility.
Bool_t
CSpectrumPackage::GetNumberList(CTCLInterpreter& rInterp, 
			    std::vector<UInt_t>& rvIds,
			    int nArgs, const char* pArgs[])
{
  // Parses the remainder of the parameters as an unsigned
  // integer list (e.g. set of spectrum ids).
  // Returns the ids in rvIds.
  // success returns kfFALSE, failure kfTRUE
  // with rResult containing a two element list:
  //  first element the text: 
  //   "The following parameters must be unsigned integers"
  //  second element a sublist of the parameters which failed to parse.
  //
  CTCLString MyResult;
  Bool_t fFailed = kfFALSE;
  std::string rResult;

  for(Int_t i = 0; i < nArgs; i++) {
    Int_t value;
    Int_t tclStatus = Tcl_GetInt(rInterp.getInterpreter(), pArgs[i], &value);

    if((tclStatus != TCL_OK) || (value < 0) ) {
      if(!fFailed) {		// First failure needs to add string:
        fFailed = kfTRUE;
        MyResult.AppendElement
          ("The following parameters must be Unsigned integers");
        MyResult.StartSublist();
      }
      MyResult.AppendElement(pArgs[i]);
    }

    rvIds.push_back((UInt_t)value);
  }

  if(fFailed) {
    MyResult.EndSublist();
    rResult += (const char*)MyResult;
  }
  rInterp.setResult(rResult);
  return fFailed;
}
///////////////////////////////////////////////////////////////////////////
//
// Function:
//   void      GetNameList(std::vector<std::string>& rvNames,
//			       int nArgs, char* pArgs[])
// Operation Type:
//    Protected parsing utility.
//
void
CSpectrumPackage::GetNameList(std::vector<std::string>& rvNames,
			       char* pattern)
{
  //  produces a vector of names from the nArgs, pArgs parameters.
  //
  SpecTcl& api(*(SpecTcl::getInstance()));
  rvNames.erase(rvNames.begin(), rvNames.end());
  SpectrumDictionaryIterator p = api.SpectrumBegin();

  for(; p != api.SpectrumEnd(); p++) {
    const char* name = ((p->second)->getName()).c_str();
    if (Tcl_StringMatch(name, pattern) )
      {
        CSpectrum* rSpec((*p).second);
        rvNames.push_back(((p->second)->getName()).c_str());
      }
  }
  return;
}

///////////////////////////////////////////////////////////////////////////
//
// Function:
//   void      GetNameList(std::vector<std::string>& rvNames,
//			       int nArgs, char* pArgs[])
// Operation Type:
//    Protected parsing utility.
//
void
CSpectrumPackage::GetNameList(std::vector<std::string>& rvNames,
			       int nArgs, const char* pArgs[])
{
  //  produces a vector of names from the nArgs, pArgs parameters.
  //
  for(Int_t i  = 0; i < nArgs; i++) 
    rvNames.push_back(pArgs[i]);
}

//////////////////////////////////////////////////////////////////////////
//
// Function:
//   AllocateId()
// Operation type:
//   Unique assignment.
//
UInt_t
CSpectrumPackage::AllocateId()
{
  UInt_t nId = getNextId();
  setNextId(nId+1);
  return nId;
}
//////////////////////////////////////////////////////////////////////////
//
// Function:
//    char* SpecTypeToText(SpecType_t sType)
// Operation Type:
//    Mapping.
//
const char*
CSpectrumPackage::SpecTypeToText(SpectrumType_t sType)
{
  for(UInt_t i = 0; i < nSpecTypes; i++) {
    if(sType == aSpecTypes[i].eType) 
      return aSpecTypes[i].pName;
  }
  return "*unknown*";
}
//////////////////////////////////////////////////////////////////////////////
//
// Function:
//   const char*     DataTypeToText(DataType_t dType)
// Operation Type:
//    Mapping.
//
const char*
CSpectrumPackage::DataTypeToText(DataType_t dType)
{
  for(UInt_t i = 0; i < nDataTypes; i++) {
    if(dType == aDataTypes[i].eType) 
      return aDataTypes[i].pName;
  }
  return "*unknown*";
}
////////////////////////////////////////////////////////////////////////
//
// Function:
//     SpectrumType_t  SpectrumType(const char* pType)
// Operation Type:
//     Mapping.
//
SpectrumType_t
CSpectrumPackage::SpectrumType(const char* pType)
{
  for(UInt_t i = 0; i < nSpecTypes; i++) {
    if(strcmp(pType, aSpecTypes[i].pName) == 0) 
      return aSpecTypes[i].eType;
  }
  errno = EINVAL;
  throw CErrnoException("Translating Spectrum type string");
}
//////////////////////////////////////////////////////////////////////////
//
//  Function:
//      DataType_t      Datatype(Spectrumtype_t st, const char* pType)
//
//  Operation Type:
//      Mapping:
//
DataType_t
CSpectrumPackage::Datatype(SpectrumType_t st, const char* pType)
{
  //  If pType is NULL, then we default  to longword channels:

  if(!pType) {
    return keLong;
  }

  // Look up pType:
  //
  for(UInt_t i = 0; i < nDataTypes; i++) {
    if(strcmp(pType, aDataTypes[i].pName) == 0) 
      return aDataTypes[i].eType;
  }
  errno = EINVAL;
  throw CErrnoException("Translating data type string");
}
////////////////////////////////////////////////////////////////////////
//
// Function:
//   UInt_t ValidateIndices(CSpectrum* pSpec, vector<UInt_t>& rIndices)
// Operation Type:
//   Protected utility.
//
UInt_t*
CSpectrumPackage::ValidateIndices(CSpectrum* pSpec, 
				  const vector<UInt_t>& rIndices)
{
  // Validates the indices of a spectrum.
  //
  // Formal Parameters:
  //    CSpectrum* pSpec:
  //      Pointer to the spectrum.
  //    vector<UInt_t>& rIndices:
  //      Array of spectrum indices.
  // Returns:
  //    Pointer to an array of valid indices.
  // Throws:
  //    CRangeError on errors.
  //
  // For this to work:
  //     rIndices must have as many elements as pSpec's dimensionality.
  //     rIndices elements must all be within the spectrum's channel 
  //     limits.
  //       Either violation results in a CRangeError exception.
  //
  if(rIndices.size() != pSpec->Dimensionality()) {
    throw CRangeError(pSpec->Dimensionality(), pSpec->Dimensionality(),
		      rIndices.size(),
		      "Range checking indices in channel get");
  }
  for(UInt_t i = 0; i < rIndices.size(); i++) {
    int size =  pSpec->Dimension(i) - 2;
    if(rIndices[i] >= size) { // -2 for root under/overflow.
      throw CRangeError(0, size, rIndices[i],
			"Range Checking indices in channel get");
    }    
  }
  // The indices have to be put in a C array from the vector in order to 
  // match the call signature of the spectrum's operator[]:
  
  UInt_t* pIs = new UInt_t[rIndices.size()];
  for(UInt_t i = 0; i < rIndices.size(); i++) {
    pIs[i] = rIndices[i];
  }
  return pIs;
}
//////////////////////////////////////////////////////////////////////////
//
// Function:
//   void  UniquifyName(CSpectrum* pSpectrum)
// Operation Type:
//   Utility.
//     Given a candidate spectrum name, returns a new spectrum name based on it
//     that is unique
// @param basename - the base spectrum name.
// @return std::string - a spectrum name similar to basename but this spectrum
//                       is not know to SpecTcl.
//
std::string
CSpectrumPackage::UniquifyName(std::string basename)
{

  UInt_t nSuffix(0);
  std::string result(basename);
  if (m_pHistogrammer) {    // Null if MPI and not MPI_EVENT_SINK_RANK
    while(m_pHistogrammer->FindSpectrum(result)) {
      char Suffix[100];
      sprintf(Suffix,"_%u", nSuffix);
      result = basename + Suffix;
      nSuffix++;
    }
  }
  return result;
}
///////////////////////////////////////////////////////////////////////////
//
// Function:
//   Bool_t  AllParamsExist(CSpectrum* pSpectrum)
// Operation Type:
//   Utility
//
Bool_t
CSpectrumPackage::AllParamsExist(CSpectrum* pSpectrum)
{
  // This function determines if all of the parameters in a spectrum really
  // are defined.  A parameter is not defined if its id is UINT_MAX
  //
  vector<UInt_t> ParamIds;
  pSpectrum->GetParameterIds(ParamIds);
  for(UInt_t i = 0;  i < ParamIds.size(); i++) {
    if(ParamIds[i] == UINT_MAX) return kfFALSE;
  }
  return kfTRUE;
}


/**
 * makeBinding
 *   Given a histogram object and the histogrammer bind the spectrum
 *   to the displayer.  If the spectrum is already bound to the displayer,
 *   It is not bound again.
 *
 * @param spec - reference to the CHistogram object that defines the spectrum.
 * @param hist - reference to the CHistogrammer object that is the histogramming kernel.
 *
 */
void
CSpectrumPackage::makeBinding(CSpectrum& spec, CHistogrammer& hist)
{
  std::string name = spec.getName();

  // Get the current bindings and see if this one is in it.
  
  CDisplay* pDisplay = m_pDisplay->getCurrentDisplay();
  SpectrumContainer spectra = pDisplay->getBoundSpectra();
  size_t nSpectra = spectra.size();
  for(size_t i = 0; i < nSpectra; i++) {
      CSpectrum* pBoundSpec = spectra[i];
      if(pBoundSpec) {
          if(name == pBoundSpec->getName())
            return;                     // Duplicate binding.
      }
  }
  // This isn't bound yet so add it:

  UInt_t binding = pDisplay->addSpectrum(spec, hist);
  BindTraceSingleton& traceContainer(BindTraceSingleton::getInstance());
  CTCLObject objName;
  CTCLObject objId;
  CTCLInterpreter* pInterp = m_pClear->getInterpreter();  // Need to get it from one of my commands.
  objName.Bind(*pInterp);
  objId.Bind(*pInterp);
  objName = name;
  objId   = static_cast<int>(binding);
  try {
    traceContainer.invokeSbind(*pInterp, objName, objId);
  }
  catch (CException & e) {
    std::string msg("Error in firing a trace for sbind: ");
    msg += e.ReasonText();
    throw std::runtime_error(msg);
  }

}
/**
 * removeBinding
 *     Remove a binding from the displayer and fire the traces associated
 *     with unbind.
 *  @param spec - spectrum to unbind.
 *  @param hist - Histogrammer.
 */
void
CSpectrumPackage::removeBinding(CSpectrum& spec, CHistogrammer& hist)
{
  std::string name = spec.getName();
  SpecTcl* pApi    = SpecTcl::getInstance();
  CDisplayInterface* pDispInterface = pApi->GetDisplayInterface();
  CDisplay* pDisplay                = pDispInterface->getCurrentDisplay();
  
  int id = pDisplay->removeSpectrum(spec, hist);
  

  
  
  // Note that if asked to unbind a spectrum that's not sbound,
  // as can happen in unbind -all, id is -1 so the condition below
  // prevents a spurious trace in that case.
  
  if (id >= 0) {
    CTCLInterpreter* pInterp = m_pClear->getInterpreter();
    CTCLObject      objName;
    CTCLObject      objId;
    objName.Bind(*pInterp);
    objId.Bind(*pInterp);
    objName = name;
    objId  = id;
    BindTraceSingleton& traceContainer(BindTraceSingleton::getInstance());
    traceContainer.invokeUnbind(*pInterp, objName, objId);
  }
}