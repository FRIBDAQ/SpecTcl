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

#ifndef __SPECTRUMPACKAGE_H  //Required for current class
#define __SPECTRUMPACKAGE_H

                               //Required for base classes
#ifndef __TCLCOMMANDPACKAGE_H
#include <TCLCommandPackage.h>
#endif                               
                               
                               //Required for 1:1 associated classes
#ifndef __HISTOGRAMMER_H
#include <Histogrammer.h>
#endif                                                               

#ifndef __HISTOTYPES_H
#include <histotypes.h>
#endif


#ifndef __STL_STRING
#include <string>
#ifndef __STL_STRING
#define __STL_STRING
#endif
#endif

#ifndef __STL_VECTOR
#include <vector>
#ifndef __STL_VECTOR
#define __STL_VECTOR
#endif
#endif



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
                                                               
class CSpectrumPackage  : public CTCLCommandPackage        
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
  static UInt_t     m_nNextId;	  // Next spectrum ident.
  CHistogrammer*    m_pHistogrammer; // Histogrammer with spectra. 
  CSpectrumCommand* m_pSpectrum; // Executes spectrum cmd.
  CClearCommand*    m_pClear;	// Executes clear command.
  CBindCommand*     m_pBind;	// Executes bind command.
  CUnbindCommand*   m_pUnbind;	// Executes the unbind command.
  ChannelCommand*   m_pChannel; // Executes the channel command.
  CWriteCommand*    m_pWrite;	// Executes the swrite command.
  CReadCommand*     m_pRead;

public:
			//Constructor with arguments

  CSpectrumPackage (CTCLInterpreter* pInterp, CHistogrammer* pHistogrammer);
  virtual ~ CSpectrumPackage ( );       //Destructor
	
			//Copy constructor [illegal]
private:
  CSpectrumPackage (const CSpectrumPackage& aCSpectrumPackage );
public:

			//Operator= Assignment Operator [illegal]
private:
  CSpectrumPackage& operator= (const CSpectrumPackage& aCSpectrumPackage);
public:

			//Operator== Equality Operator [allowed but...]

  int operator== (const CSpectrumPackage& aCSpectrumPackage)
  { return (
	    (CTCLCommandPackage::operator== (aCSpectrumPackage)) &&
	    (m_pHistogrammer == m_pHistogrammer)
	    );
  }         
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
  
  int CreateSpectrum(CTCLResult& rResult, const char* pName, 
		     const char* pSpecType,
		     std::vector<std::string>& rvParameterNames,
		     std::vector<UInt_t>&      nChannels,
		     std::vector<Float_t>&     fLows,
		     std::vector<Float_t>&     fHighs,
		     const char*               pDataType);

  int CreateSpectrum(CTCLResult& rResult, const char* pName,
		     const char* pSpecType,
		     std::vector<std::string> xParameterNames,
		     std::vector<std::string> yParameterNames,
		     std::vector<UInt_t>      vChannels,
		     std::vector<Float_t>     fLows,
		     std::vector<Float_t>     fHighs,
		     const char*              pDataType);

  int CreateSpectrum(CTCLResult&                            rResult, 
		     const char*                            pName,
		     const char*                            pSpecType,
		     std::vector<std::vector<std::string> > parameterNames,
		     std::vector<UInt_t>                    nChannels,
		     std::vector<Float_t>                   fLows,
		     std::vector<Float_t>                   fHighs,
		     const char*                            pDataType);

  // List Spectra

  void ListSpectra (std::vector<std::string>& rvProperties, const char* pattern,
		    bool showGates=false)  ;
  //Int_t ListSpectrum (CTCLResult& rResult, const char* pName)  ;
  Int_t ListSpectrum (CTCLResult& rResult, UInt_t nId,
		      bool showGate=false)  ;

  // Clear Spectra:

  void ClearAll ()  ;
  Int_t ClearSubset (CTCLResult& rResult, 
		     std::vector<std::string>& rvSpectra) ;
  Int_t ClearSubset (CTCLResult& rResult, std::vector<UInt_t>& rvIds)  ;

  // Bind spectra to display:

  Int_t BindAll (CTCLResult& rResult)  ;
  Int_t BindList (CTCLResult& rResult, std::vector<std::string>& rvNames)  ;
  Int_t BindList (CTCLResult& rResult, std::vector<UInt_t>& rIds)  ;

  // Remove spectrum bindings:

  Int_t UnbindList (CTCLResult& rResult, std::vector<std::string>& rvNames)  ;
  Int_t UnbindList (CTCLResult& rResult, std::vector<UInt_t>& rvIds)  ;
  Int_t UnbindXidList (CTCLResult& rResult, std::vector<UInt_t>& rvXids)  ;
  void UnbindAll ()  ;

  // Spectrum deletion:

  Int_t DeleteList (CTCLResult& rResult, std::vector<std::string>& rvNames)  ;
  Int_t DeleteList (CTCLResult& rResult, std::vector<UInt_t>& rvnIds)  ;
  void DeleteAll ()  ;

  // Inquire about spectrum bindings.
  //

  Int_t ListBindings (CTCLResult& rResult, 
		      std::vector<std::string>& rvNames)  ;
  Int_t ListBindings (CTCLResult& rResult, std::vector<UInt_t>& rvIds)  ;
  Int_t ListXidBindings (CTCLResult& rResult, std::vector<UInt_t>& rvXIds)  ;
  void ListAllBindings (CTCLResult& rResult, const char* pattern)  ;
  Bool_t GetChannel (CTCLResult& rResult, const std::string& rName, 
		     const std::vector<UInt_t>& pIndices)    ;
  Bool_t SetChannel (CTCLResult& rResult, const std::string& rName, 
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
public:
  Bool_t      GetNumberList(CTCLResult& rResult, 
			    std::vector<UInt_t>& rvIds,
			    int nArgs, char* pArgs[]);
  static void      GetNameList(std::vector<std::string>& rvNames,
			       CTCLResult& rResult,
			       char* pattern);
  static void      GetNameList(std::vector<std::string>& rvNames,
			       int nArgs, char* pArgs[]);
  static const char*     SpecTypeToText(SpectrumType_t sType);
  static const char*     DataTypeToText(DataType_t dType);
  static SpectrumType_t  SpectrumType(const char* pType);
  static DataType_t      Datatype(SpectrumType_t st, const char* pType);
  static UInt_t*         ValidateIndices(CSpectrum* pSpec,
					 const std::vector<UInt_t>& rIndices);
  void                   UniquifyName(CSpectrum* pSpectrum); // >>NEW<<
  Bool_t                 AllParamsExist(CSpectrum* pSpectrum); // >>NEW<<

};

#endif
