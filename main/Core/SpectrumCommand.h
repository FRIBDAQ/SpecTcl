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

//  CSpectrumCommand.h:
//
//    This file defines the CSpectrumCommand class.
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
/////////////////////////////////////////////////////////////

#ifndef SPECTRUMCOMMAND_H  //Required for current class
#define SPECTRUMCOMMAND_H
                               //Required for base classes
#ifndef __TCLPACKAGEDCOMMAND_H
#include "TCLPackagedCommand.h"
#endif

#ifndef __TCLOBJECT_H
#include <TCLObject.h>
#endif

#ifndef __HISTOTYPES_H
#include <histotypes.h>
#endif
              
#ifndef __STL_VECTOR
#include <vector>
#ifndef __STL_VECTOR
#define __STL_VECTOR
#endif
#endif

#ifndef __STL_STRING
#include <string>
#ifndef __STL_STRING
#define __STL_STRING
#endif
#endif

class CTCLInterpreter;
class CTCLResult;
class CTCLCommandPackage;   
class SpectrumTraceObserver;      
class CSpectrum;
class CGateContainer;
class CTCLObject;
                               
class CSpectrumCommand  : public CTCLPackagedCommand        
{
public:
  enum Switch {
    keNew,
    keList,
    keId,
    keDelete,
    keAll,
    keById,
    keShowGate,
    keTrace,
    keNotSwitch
  };
private:
  CTCLObject     m_createTrace;	// Trace script fired on a spectrum creation.
  CTCLObject     m_removeTrace; // Trace script fired on a spectrum deletion.
  bool           m_fTracing;    // True when traces are being called.

  SpectrumTraceObserver* m_pObserver; 

public:
			//Constructor with arguments
  CSpectrumCommand (CTCLInterpreter* pInterp, CTCLCommandPackage& rPackage );
  virtual ~CSpectrumCommand ( );

			//Copy constructor - Illegal
private:
  CSpectrumCommand (const CSpectrumCommand& aCSpectrumCommand );
public:

			//Operator= Assignment Operator - Illegal;
private:
  CSpectrumCommand& operator= (const CSpectrumCommand& aCSpectrumCommand);
  int operator==(const CSpectrumCommand& rhs);
  int operator!=(const CSpectrumCommand& rhs);
public:

  // Operations:

public:  
  virtual   int operator() (CTCLInterpreter& rInterpreter, CTCLResult& rResult,
			    int  nArgs, char* pArgs[])  ;
  Int_t New (CTCLInterpreter& rInterpreter, CTCLResult& rResult,
	     int nArgs, char* pArgs[])  ;
  Int_t List (CTCLInterpreter& rInterp, CTCLResult& rResult, 
	      int nArgs, char* Args[]);
  Int_t Delete (CTCLInterpreter& rInterp, CTCLResult& rResult, 
		int nArgs, char* pArgs[])  ;
  Int_t Trace  (CTCLInterpreter& rInterp, CTCLResult& rResult,
		int nArgs, char* pArgs[]);

protected:
  static void   Usage(CTCLResult& rResult);
  static Switch MatchSwitch(const char* pSwitch);

  void          SortSpectraById(std::vector<std::string>& rvProperties);
  void          SortSpectraByName(std::vector<std::string>& rvProperties);

  static void   VectorToResult(CTCLResult& rResult, 
			       std::vector<std::string>& rvStrings);
public:
  UInt_t      ExtractId(const std::string& rProperties);
  std::string ExtractName(const std::string& rProperties);
  void traceAdd(const std::string& name,
		const CSpectrum* pSpectrum);
  void traceRemove(const std::string& name,
		   const CSpectrum* pSpectrum);
  std::vector<CGateContainer*> getConstituents(CTCLResult& res, std::string gateName);
  std::vector<CGateContainer*> getGates(CTCLResult& res, CTCLObject& gates);

};

#endif
