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


/////////////////////////////////////////////////////////////

#ifndef SPECTRUMPACKAGE_H  //Required for current class
#define SPECTRUMPACKAGE_H

                               //Required for base classes
#include <TCLObjectPazckage.h>
#include <Histogrammer.h>
#include <histotypes.h>
#include <string>
#include <vector>



// Forward class definitions:
//

class CTCLInterpreter;
class CTCLResult;
class CTCLString;
class CSpectrum;
class CSpectrumFormatter;

class CSpectrumCommand;
class CClearCommand;
class CBindCommand;
class CUnbindCommand;
class ChannelCommand;
class CWriteCommand;
class CReadCommand;
class CDisplayInterface;
class CMPITclPackagedCommand;
                                                               
class CSpectrumPackage  : public CTCLObjectPackage
{
  // Data types
public:
  typedef enum _SpectrumReadFlags {
    fReplace = 1,		// If true replace existing spectrum.
    fLive    = 2,		// If true attempt to make spectrum live.
    fBind    = 4		// If true, bind spectrum to display too.
  } SpectrumReadFlags;
  // Attributes and relations:

protected:
  static UInt_t      m_nNextId;	  // Next spectrum ident.
  CHistogrammer*     m_pHistogrammer; // Histogrammer with spectra.
  CDisplayInterface*   m_pDisplay;

  CMPITclPackagedCommand*  m_pSpectrum; // Executes spectrum cmd.
  CMPITclPackagedCommand*     m_pClear;	// Executes clear command.
  CMPITclPackagedCommand*      m_pBind;	// Executes bind command.
  CMPITclPackagedCommand*    m_pUnbind;	// Executes the unbind command.
  CMPITclPackagedCommand*    m_pChannel; // Executes the channel command.
  CMPITclPackagedCommand*     m_pWrite;	// Executes the swrite command.
  CMPITclPackagedCommand*      m_pRead;
  

public:
			//Constructor with arguments

  CSpectrumPackage (CTCLInterpreter* pInterp, CHistogrammer* pHistogrammer,
                    CDisplayInterface* pDisplay);
  virtual ~ CSpectrumPackage ( );       //Destructor
	
			//Copy constructor [illegal]
private:
  CSpectrumPackage (const CSpectrumPackage& aCSpectrumPackage );
  CSpectrumPackage& operator= (const CSpectrumPackage& aCSpectrumPackage);
  int operator== (const CSpectrumPackage& aCSpectrumPackage);
// Selectors:                    

public:
  static UInt_t getNextId()
  {
    return m_nNextId;
  }
  const CHistogrammer* getHistogrammer() const
  {
    return m_pHistogrammer;
  }
  // Mutators:

  std::string getSignon() const;
protected:               

  void setHistogrammer (CHistogrammer* am_pHistogrammer)
  { 
    m_pHistogrammer = am_pHistogrammer;
  }
  static void setNextId (UInt_t am_nNextId)
  { 
    m_nNextId = am_nNextId;
  }
  static UInt_t AllocateId(); 
  //
  // class operations:
  //
public:
  
  int CreateSpectrum(CTCLInterpreter& rInterp, const char* pName, 
		     const char* pSpecType,
		     std::vector<std::string>& rvParameterNames,
		     std::vector<UInt_t>&      nChannels,
		     std::vector<Float_t>&     fLows,
		     std::vector<Float_t>&     fHighs,
		     const char*               pDataType);

  int CreateSpectrum(CTCLInterpreter& rInterp, const char* pName,
		     const char* pSpecType,
		     std::vector<std::string> xParameterNames,
		     std::vector<std::string> yParameterNames,
		     std::vector<UInt_t>      vChannels,
		     std::vector<Float_t>     fLows,
		     std::vector<Float_t>     fHighs,
		     const char*              pDataType);

  int CreateSpectrum(CTCLInterpreter&           rInterp,
		     const char*                            pName,
		     const char*                            pSpecType,
		     std::vector<std::vector<std::string> > parameterNames,
		     std::vector<UInt_t>                    nChannels,
		     std::vector<Float_t>                   fLows,
		     std::vector<Float_t>                   fHighs,
		     const char*                            pDataType);

  int CreateSpectrum(
    CTCLInterpreter& rInterp, const char* pName, const char* pType,
    const std::vector<std::string>& parameters, bool inX,
    const std::vector<CGateContainer*>& roi,
    const std::vector<UInt_t>&  nchans,
    const std::vector<Float_t>&  lows, const std::vector<Float_t>& highs,
    const char* pDataType
  );
  // List Spectra

  void ListSpectra (std::vector<std::string>& rvProperties, const char* pattern,
		    bool showGates=false)  ;
  //Int_t ListSpectrum (CTCLResult& rResult, const char* pName)  ;
  Int_t ListSpectrum (CTCLInterpreter& rInterp, UInt_t nId,
		      bool showGate=false)  ;

  // Clear Spectra:

  void ClearAll ()  ;
  Int_t ClearSubset (CTCLInterpreter& rInterp, 
		     std::vector<std::string>& rvSpectra) ;
  Int_t ClearSubset (CTCLInterpreter& rInterp, std::vector<UInt_t>& rvIds)  ;

  // Bind spectra to display:

  Int_t BindAll (CTCLInterpreter& rInterp)  ;
  Int_t BindList (CTCLInterpreter& rInterp, std::vector<std::string>& rvNames)  ;
  Int_t BindList (CTCLInterpreter& rInterp, std::vector<UInt_t>& rIds)  ;

  // Remove spectrum bindings:

  Int_t UnbindList (CTCLInterpreter& rInterp, std::vector<std::string>& rvNames)  ;
  Int_t UnbindList (CTCLInterpreter& rInterp, std::vector<UInt_t>& rvIds)  ;
  void UnbindAll ()  ;

  // Spectrum deletion:

  Int_t DeleteList (CTCLInterpreter& rInterp, std::vector<std::string>& rvNames)  ;
  Int_t DeleteList (CTCLInterpreter& rInterp, std::vector<UInt_t>& rvnIds)  ;
  void DeleteAll ()  ;

  // Inquire about spectrum bindings.
  //

  Int_t ListBindings (CTCLInterpreter& rInterp, std::vector<std::string>& rvNames);
  Int_t ListBindings (CTCLInterpreter& rInterp, std::vector<UInt_t>& rvIds)  ;
  void ListAllBindings (CTCLInterpreter& rInterp, const char* pattern="*")  ;

  Bool_t GetChannel (CTCLInterpreter& rInterp, const std::string& rName, 
		     const std::vector<UInt_t>& pIndices)    ;
  Bool_t SetChannel (CTCLInterpreter& rInterp, const std::string& rName, 
		     const std::vector<UInt_t>& rIndices, ULong_t nValue)    ;
  Int_t  Write(std::string& rResult, const std::string& rSpectrum, STD(ostream)& rOut,
	       CSpectrumFormatter* pFormat);
  Int_t  Read(std::string& rResult, STD(istream)& rIn,
	      CSpectrumFormatter* pFormat, UInt_t fFlags);
  // Utility functions:

protected:
  std::string DescribeSpectrum(CSpectrum& rSpectrum, bool showGate=false);
  UInt_t      FindDisplayBinding(const std::string& rName);
  UInt_t      FindDisplayBinding(UInt_t             nId);
  void        FormatBinding(CTCLString& rString, UInt_t nXId, 
			    CSpectrum*  pSpectrum);
  void        makeBinding(CSpectrum& spec, CHistogrammer& hist);
  void        removeBinding(CSpectrum& spec, CHistogrammer& hist);
public:
  Bool_t      GetNumberList(CTCLInterpreter& rInterp, 
			    std::vector<UInt_t>& rvIds,
			    int nArgs, char* pArgs[]);
  static void      GetNameList(std::vector<std::string>& rvNames,
			              char* pattern);
  static void      GetNameList(std::vector<std::string>& rvNames,
			       int nArgs, char* pArgs[]);
  static const char*     SpecTypeToText(SpectrumType_t sType);
  static const char*     DataTypeToText(DataType_t dType);
  static SpectrumType_t  SpectrumType(const char* pType);
  static DataType_t      Datatype(SpectrumType_t st, const char* pType);
  static UInt_t*         ValidateIndices(CSpectrum* pSpec,
					 const std::vector<UInt_t>& rIndices);
  std::string            UniquifyName(std::string originalName); // >>NEW<<
  Bool_t                 AllParamsExist(CSpectrum* pSpectrum); // >>NEW<<

};

#endif
