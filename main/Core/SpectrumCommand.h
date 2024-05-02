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
#include "TCLPackagedObjectProcessor.h"
#include <TCLObject.h>
#include <histotypes.h>
#include <vector>
#include <string>


class CTCLInterpreter;
class CTCLResult;
class CTCLCommandPackage;   
class SpectrumTraceObserver;      
class CSpectrum;
class CGateContainer;
class CTCLObject;
                               
class CSpectrumCommand  : public CTCLPackagedObjectProcessor       
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
  CSpectrumCommand (CTCLInterpreter* pInterp);
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
  virtual   int operator() (CTCLInterpreter& rInterpreter, 
			    std::vector<CTCLObject>& objv)  ;
  Int_t New (CTCLInterpreter& rInterpreter, 
	     int nArgs, const char* pArgs[])  ;
  Int_t List (CTCLInterpreter& rInterp, 
	      int nArgs, const char* Args[]);
  Int_t Delete (CTCLInterpreter& rInterp,
		int nArgs, const char* pArgs[])  ;
  Int_t Trace  (CTCLInterpreter& rInterp, 
		int nArgs, const char* pArgs[]);

protected:
  static void   Usage(CTCLInterpreter& rInterp, const char* prefix = nullptr);
  static Switch MatchSwitch(const char* pSwitch);

  void          SortSpectraById(std::vector<std::string>& rvProperties);
  void          SortSpectraByName(std::vector<std::string>& rvProperties);

  static void   VectorToResult(CTCLInterpreter& rInterp, 
			       std::vector<std::string>& rvStrings);
public:
  UInt_t      ExtractId(const std::string& rProperties);
  std::string ExtractName(const std::string& rProperties);
  void traceAdd(const std::string& name,
		const CSpectrum* pSpectrum);
  void traceRemove(const std::string& name,
		   const CSpectrum* pSpectrum);
  std::vector<CGateContainer*> getConstituents(CTCLInterpreter& rInterp, std::string gateName);
  std::vector<CGateContainer*> getGates(CTCLInterpreter& rInterp, CTCLObject& gates);

};

#endif
