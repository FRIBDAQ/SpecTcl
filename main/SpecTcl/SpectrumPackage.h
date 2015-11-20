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
//   $Log$
//   Revision 5.4  2007/02/23 20:38:18  ron-fox
//   BZ291 enhancement... add gamma deluxe spectrum type (independent x/y
//   parameter lists).
//
//   Revision 5.3  2006/02/21 19:30:59  ron-fox
//   Add -showgate to spectrum -list command/subcommand
//
//   Revision 5.2  2005/06/03 15:19:28  ron-fox
//   Part of breaking off /merging branch to start 3.1 development
//
//   Revision 5.1.2.3  2005/05/27 11:07:30  thoagland
//   Added support for pseudo, parameter, clear, apply, and bind to take an optional pattern for the -list switch.
//
//   Revision 5.1.2.2  2005/05/24 11:36:48  thoagland
//   Added support for spectrum -list [-byid] [pattern]
//
//   Revision 5.1.2.1  2004/12/15 17:24:06  ron-fox
//   - Port to gcc/g++ 3.x
//   - Recast swrite/sread in terms of tcl[io]stream rather than
//     the kludgy thing I had done of decoding the channel fd.
//     This is both necessary due to g++ 3.x's runtime and
//     nicer too!.
//
//   Revision 5.1  2004/11/29 16:56:12  ron-fox
//   Begin port to 3.x compilers calling this 3.0
//
//   Revision 4.2  2003/04/01 19:55:40  ron-fox
//   Support for Real valued parameters and spectra with arbitrary binnings.
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
		     STD(vector)<STD(string)>& rvParameterNames,
		     STD(vector)<UInt_t>&      nChannels,
		     STD(vector)<Float_t>&     fLows,
		     STD(vector)<Float_t>&     fHighs,
		     const char*               pDataType);

  int CreateSpectrum(CTCLResult& rResult, const char* pName,
		     const char* pSpecType,
		     STD(vector)<STD(string)> xParameterNames,
		     STD(vector)<STD(string)> yParameterNames,
		     STD(vector)<UInt_t>      vChannels,
		     STD(vector)<Float_t>     fLows,
		     STD(vector)<Float_t>     fHighs,
		     const char*              pDataType);
  // List Spectra

  void ListSpectra (STD(vector)<STD(string)>& rvProperties, const char* pattern,
		    bool showGates=false)  ;
  //Int_t ListSpectrum (CTCLResult& rResult, const char* pName)  ;
  Int_t ListSpectrum (CTCLResult& rResult, UInt_t nId,
		      bool showGate=false)  ;

  // Clear Spectra:

  void ClearAll ()  ;
  Int_t ClearSubset (CTCLResult& rResult, 
		     STD(vector)<STD(string)>& rvSpectra) ;
  Int_t ClearSubset (CTCLResult& rResult, STD(vector)<UInt_t>& rvIds)  ;

  // Bind spectra to display:

  Int_t BindAll (CTCLResult& rResult)  ;
  Int_t BindList (CTCLResult& rResult, STD(vector)<STD(string)>& rvNames)  ;
  Int_t BindList (CTCLResult& rResult, STD(vector)<UInt_t>& rIds)  ;

  // Remove spectrum bindings:

  Int_t UnbindList (CTCLResult& rResult, STD(vector)<STD(string)>& rvNames)  ;
  Int_t UnbindList (CTCLResult& rResult, STD(vector)<UInt_t>& rvIds)  ;
  Int_t UnbindXidList (CTCLResult& rResult, STD(vector)<UInt_t>& rvXids)  ;
  void UnbindAll ()  ;

  // Spectrum deletion:

  Int_t DeleteList (CTCLResult& rResult, STD(vector)<STD(string)>& rvNames)  ;
  Int_t DeleteList (CTCLResult& rResult, STD(vector)<UInt_t>& rvnIds)  ;
  void DeleteAll ()  ;

  // Inquire about spectrum bindings.
  //

  Int_t ListBindings (CTCLResult& rResult, 
		      STD(vector)<STD(string)>& rvNames)  ;
  Int_t ListBindings (CTCLResult& rResult, STD(vector)<UInt_t>& rvIds)  ;
  Int_t ListXidBindings (CTCLResult& rResult, STD(vector)<UInt_t>& rvXIds)  ;
  void ListAllBindings (CTCLResult& rResult, const char* pattern)  ;
  Bool_t GetChannel (CTCLResult& rResult, const STD(string)& rName, 
		     const STD(vector)<UInt_t>& pIndices)    ;
  Bool_t SetChannel (CTCLResult& rResult, const STD(string)& rName, 
		     const STD(vector)<UInt_t>& rIndices, ULong_t nValue)    ;
  Int_t  Write(STD(string)& rResult, const STD(string)& rSpectrum, STD(ostream)& rOut,
	       CSpectrumFormatter* pFormat);
  Int_t  Read(STD(string)& rResult, STD(istream)& rIn,
	      CSpectrumFormatter* pFormat, UInt_t fFlags);
  // Utility functions:

protected:
  STD(string) DescribeSpectrum(CSpectrum& rSpectrum, bool showGate=false);
  UInt_t      FindDisplayBinding(const STD(string)& rName);
  UInt_t      FindDisplayBinding(UInt_t             nId);
  void        FormatBinding(CTCLString& rString, UInt_t nXId, 
			    CSpectrum*  pSpectrum);
public:
  Bool_t      GetNumberList(CTCLResult& rResult, 
			    STD(vector)<UInt_t>& rvIds,
			    int nArgs, char* pArgs[]);
  static void      GetNameList(STD(vector)<STD(string)>& rvNames,
			       CTCLResult& rResult,
			       char* pattern);
  static void      GetNameList(STD(vector)<STD(string)>& rvNames,
			       int nArgs, char* pArgs[]);
  static const char*     SpecTypeToText(SpectrumType_t sType);
  static const char*     DataTypeToText(DataType_t dType);
  static SpectrumType_t  SpectrumType(const char* pType);
  static DataType_t      Datatype(SpectrumType_t st, const char* pType);
  static UInt_t*         ValidateIndices(CSpectrum* pSpec,
					 const STD(vector)<UInt_t>& rIndices);
  void                   UniquifyName(CSpectrum* pSpectrum); // >>NEW<<
  Bool_t                 AllParamsExist(CSpectrum* pSpectrum); // >>NEW<<

};

#endif
