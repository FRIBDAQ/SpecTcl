//  Cparameterpackage.h:
//
//    This file defines the CParameterPackage class.
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

#ifndef __PARAMETERPACKAGE_H  //Required for current class
#define __PARAMETERPACKAGE_H

#ifndef __TCLCOMMANDPACKAGE_H
#include "TCLCommandPackage.h"
#endif

#ifndef __TCLLIST_H
#include "TCLList.h"
#endif

#ifndef __STL_LIST
#include <list>
#define __STL_LIST
#endif

// Forward class references:

class CTCLInterpreter;                               
class CTCLHistogrammer;
class CParameter;
class CParameterCommand;
class CPseudoCommand;
class CParameterPackage : public CTCLCommandPackage
{
  CTCLHistogrammer*  m_pHistogrammer;	// Refers to the Histogrammer in which
				// parameters will be defined.

  CParameterCommand* m_pParameter; // Command Processor.
  CPseudoCommand*    m_pPseudo;
public:
			// Normal constructor.

  CParameterPackage (CTCLInterpreter* pInterp, 
		     CTCLHistogrammer* rHistogrammer);
  virtual ~ CParameterPackage ( ); // Destructor



                        // Copy Constructor.

  CParameterPackage (const CParameterPackage& aCParameterPackage );

			//Operator= Assignment Operator

  CParameterPackage& operator= (const CParameterPackage& aCParameterPackage);

			//Operator== Equality Operator

  int operator== (const CParameterPackage& aCParameterPackage);

                        // Selectors:

public:
  CTCLHistogrammer& getHistogrammer() const {
    return *m_pHistogrammer;
  }
                       // Mutators:

protected:
  void setHistogrammer(CTCLHistogrammer& rHistogrammer) {
    m_pHistogrammer = &rHistogrammer;
  }
  //   Operations on the class:
  //

public:                    
  Int_t AddParameter (CTCLResult& rResult, const char* pName, 
		      UInt_t nId, UInt_t nBits)  ;
  CTCLList CreateTclParameterList (CTCLInterpreter& rInterp)  ;
  Int_t DeleteParameter (CTCLResult& rResult, const char* pName)  ;
  Int_t DeleteParameter (CTCLResult& rResult, UInt_t nId)  ;
  Int_t ListParameter (CTCLResult& rResult, const char*  pName)  ;
  Int_t ListParameter (CTCLResult& rResult, UInt_t  nId)  ;
  CTCLString getParameterInfoListString (CParameter& rParameter)  ;

  Int_t AddPseudo(CTCLResult& rResult, const char* pPseudoName,
		  vector<string>& rDependents, const char* pBody);
  Int_t DescribePseudo(const string& rName, string& rDescription);
  Int_t DeletePseudo(const string& rName, string& rResult);
  void  GetPseudoNames(list<string>& rNames);
  
};

#endif
