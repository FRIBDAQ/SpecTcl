//  CSpectrumPackage.h:
//
//    This file defines the CSpectrumPackage class.
//
// Author:
//    Ron Fox
//    NSCL
//    Michigan State University
//    East Lansing, MI 48824-1321
//    mailto:fox@nscl.msu.edu
//
//  Copyright 1999 NSCL, All Rights Reserved.
//
//  Change Log:
//      July 3, 1999 - Changed the way spectra are created to take
//                     advantage of spectrum factories:
//                     Removed New1d, New2d replaced with CreateSpectrum.
//
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


#ifndef __STL_STRING_H
#include <string>
#define __STL_STRING_H
#endif

#ifndef __STL_VECTOR_H
#include <vector>
#define __STL_VECTOR_H
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
		     std::vector<UInt_t>&      rvResolutions,
		     const char*                     pDataType);

  // List Spectra

  void ListSpectra (std::vector<std::string>& rvProperties)  ;
  Int_t ListSpectrum (CTCLResult& rResult, const char* pName)  ;
  Int_t ListSpectrum (CTCLResult& rResult, UInt_t nId)  ;

  // Clear Spectra:

  void ClearAll ()  ;
  Int_t ClearSubset (CTCLResult& rResult, 
		     std::vector<std::string>& rvSpectra) ;
  Int_t ClearSubset (CTCLResult& rResult, std::vector<UInt_t>& rvIds)  ;

  // Bind spectra to display:

  Int_t BindAll (CTCLResult& rResult)  ;
  Int_t BindList (CTCLResult& rResult, std::vector<std::string>& rvNames)  ;
  Int_t BindList (CTCLResult& rResult, std::vector<UInt_t>& rIds)  ;

  // Remoe spectrum bindings:

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
  void ListAllBindings (CTCLResult& rResult)  ;
  Bool_t GetChannel (CTCLResult& rResult, const string& rName, 
		     const vector<UInt_t>& pIndices)    ;
  Bool_t SetChannel (CTCLResult& rResult, const string& rName, 
		     const vector<UInt_t>& rIndices, ULong_t nValue)    ;
  Int_t  Write(string& rResult, const string& rSpectrum, ostream& rOut,
	       CSpectrumFormatter* pFormat);
  Int_t  Read(string& rResult, istream& rIn,
	      CSpectrumFormatter* pFormat, UInt_t fFlags);
  // Utility functions:

protected:
  std::string DescribeSpectrum(CSpectrum& rSpectrum);
  UInt_t      FindDisplayBinding(const std::string& rName);
  UInt_t      FindDisplayBinding(UInt_t             nId);
  void        FormatBinding(CTCLString& rString, UInt_t nXId, 
			    CSpectrum*  pSpectrum);
public:
  Bool_t      GetNumberList(CTCLResult& rResult, 
			    std::vector<UInt_t>& rvIds,
			    int nArgs, char* pArgs[]);
  static void      GetNameList(std::vector<std::string>& rvNames,
			       int nArgs, char* pArgs[]);
  static const char*     SpecTypeToText(SpectrumType_t sType);
  static const char*     DataTypeToText(DataType_t dType);
  static SpectrumType_t  SpectrumType(const char* pType);
  static DataType_t      Datatype(SpectrumType_t st, const char* pType);
  static UInt_t*         ValidateIndices(CSpectrum* pSpec,
					 const vector<UInt_t>& rIndices);
  void                   UniquifyName(CSpectrum* pSpectrum); // >>NEW<<
  Bool_t                 AllParamsExist(CSpectrum* pSpectrum); // >>NEW<<

};

#endif
