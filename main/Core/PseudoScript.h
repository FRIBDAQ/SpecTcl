/*
    This software is Copyright by the Board of Trustees of Michigan
    State University (c) Copyright 2014.

    You may use this software under the terms of the GNU public license
    (GPL).  The terms of this license are described at:

     http://www.gnu.org/licenses/gpl.txt

     Authors:
             Ron Fox
             Jeromy Tompkins 
	     NSCL
	     Michigan State University
	     East Lansing, MI 48824-1321
*/


// Class: CPseudoScript                     //ANSI C++
//
// Encapsulates a Pseudo parameter script.
// This class contains several TCL Objects which
// are worth some explanation.
//
//  A Pseudo parameter script is constructed from
//  a tcl procedure fragment called the RawScriptText
//  and a list of dependent parameters (vParameterNames)
//  The generated script is a procedure called:
//       $PseudoName$_procedure
//  it has a parameter list with formal parameters
//  identical to the names of the dependent parameters.
//  The body is the RawScriptText.   
//   This is converted in to a CTCLObjedt called a
//    ScriptProc which is then evaluated in order to
//    a) Check for syntax errors and 
//    b) compile it as byte code for efficient multiple
//    executions.
//    Another set of objects is also created in order
//    to be able to speedily invoke the procedure at
//    run time:
//      Call Stub is an object which will be copied and
//    turned into the actual call tothe procedure and
//    evaluated.  It will consist of the name of the 
//    procedure defined by ScriptProc
//       m_vParameterObjects is an array of
//    objects which will be filled in with the
//    integer values of the depedent parameters
//    and appended to the copy of the call stub
//    generating the script which is actually executed
//    to evaluate the pseudo.
//      The Pseudo is a procedure which returns a result
//     which is stuffed into the target id.
//
//
// Author:
//     Ron Fox
//     NSCL
//     Michigan State University
//     East Lansing, MI 48824-1321
//     mailto: fox@nscl.msu.edu
// 
// (c) Copyright NSCL 1999, All rights reserved PseudoScript.h
//

#ifndef PSEUDOSCRIPT_H  //Required for current class
#define PSEUDOSCRIPT_H


#include "NamedItem.h"
#include <Event.h>
#include "TCLInterpreterObject.h"
#include <Histogrammer.h>
#include "TCLObject.h"
#include <list>
#include <string>
#include <vector>


class CPseudoScript  : public CNamedItem        
{                         
  std::list<std::string> m_vParameterNames; //Names of input parameters.
  std::list<UInt_t> m_vParameterIds;  //Paramter Ids which are input to the script.
  std::string       m_sRawScriptText; //Raw text of script.
  CTCLObject   m_ScriptProc;	//1:1 aggregation part data member
  CTCLObject   m_CallStub;	//1:1 aggregation part data member        
  CTCLInterpreter* m_pInterpreter; //association object data member      
  Bool_t       m_fEnabled;     // True if should eval pseudo.
public:

   // Constructors and other cannonical operations:
  
  CPseudoScript(const std::string& rName, 
		CTCLInterpreter* pInterp,
		CHistogrammer& rHistogrammer) ;
  CPseudoScript(const std::string& rName,
		STD(vector)<std::string>& rDependentParameters,
		CHistogrammer& rHistogrammer,
		CTCLInterpreter* pInterp, 
		const std::string& ScriptText);
  ~CPseudoScript ( )  
  { }  
  
  
  //Copy constructor 
  
  CPseudoScript (const CPseudoScript& aCPseudoScript )   : 
    CNamedItem (aCPseudoScript) 
  { 
    m_vParameterNames   = aCPseudoScript.m_vParameterNames;
    m_vParameterIds     = aCPseudoScript.m_vParameterIds;
    m_sRawScriptText    = aCPseudoScript.m_sRawScriptText;
    m_ScriptProc        = aCPseudoScript.m_ScriptProc;
    m_CallStub          = aCPseudoScript.m_CallStub;  
    m_pInterpreter      = aCPseudoScript.m_pInterpreter;
    m_fEnabled          = aCPseudoScript.m_fEnabled;
   
  }                                     

   // Operator= Assignment Operator 

  CPseudoScript& operator= (const CPseudoScript& aCPseudoScript) {
    if(this != &aCPseudoScript) {
      CNamedItem::operator=(aCPseudoScript);
      m_vParameterNames   = aCPseudoScript.m_vParameterNames;
      m_vParameterIds     = aCPseudoScript.m_vParameterIds;
      m_sRawScriptText    = aCPseudoScript.m_sRawScriptText;
      m_ScriptProc        = aCPseudoScript.m_ScriptProc;
      m_CallStub          = aCPseudoScript.m_CallStub;  
      m_pInterpreter      = aCPseudoScript.m_pInterpreter;
      m_fEnabled          = aCPseudoScript.m_fEnabled;
    }
    return *this;
  }
 
   //Operator== Equality Operator 

  int operator== (const CPseudoScript& aCPseudoScript) const {
    return ( CNamedItem::operator==(aCPseudoScript)  &&
	     (m_vParameterNames   == aCPseudoScript.m_vParameterNames) &&
	     (m_vParameterIds     == aCPseudoScript.m_vParameterIds)   &&
	     (m_sRawScriptText    == aCPseudoScript.m_sRawScriptText)  &&
	     (m_ScriptProc        == aCPseudoScript.m_ScriptProc)      &&
	     (m_CallStub          == aCPseudoScript.m_CallStub)        &&
	     (m_pInterpreter      == aCPseudoScript.m_pInterpreter)    &&
	     (m_fEnabled          == aCPseudoScript.m_fEnabled) );
     
  }
  int operator!=(const CPseudoScript& aCPseudoScript) const {
    return !(operator==(aCPseudoScript));
  }
	
// Selectors:

public:

  std::list<std::string> getParameterNames() const
  { return m_vParameterNames;
  }
  std::list<UInt_t> getParameterIds() const
  { return m_vParameterIds;
  }
  std::string getRawScriptText() const
  { return m_sRawScriptText;
  }
  std::string getOutputParameter() const
  { return getName();;
  }
  UInt_t getOutputId() const
  { return getNumber();
  }
                       
  CTCLObject& getScriptObject() 
  { return m_ScriptProc;
  }
  CTCLObject& getCallStubObject() 
  { return m_CallStub;
  }
                       
  CTCLInterpreter* getInterpreter() const
  { return m_pInterpreter;
  }
  Bool_t isEnabled() const {
    return m_fEnabled;
  }
                       
// Mutators:

protected:

  void setParameterNames (const std::list<std::string> am_vParameterNames)
  { 
    m_vParameterNames = am_vParameterNames;
    RebuildState();
  }
  void setParameterIds (const std::list<UInt_t> am_vParameterIds)
  { 
    m_vParameterIds = am_vParameterIds;
    RebuildState();
  }
  void setOutputParameter (const std::string am_sOutputParameter)
  { 
    setName(am_sOutputParameter);
  }
  void setOutputId (const UInt_t am_nOutputId)
  { 
    setNumber(am_nOutputId);
  }
  
  void setInterpreter (CTCLInterpreter* am_pInterpreter)
  { 
    m_pInterpreter = am_pInterpreter;
  }
  void setEnabled(Bool_t newvalue) {
    m_fEnabled = newvalue;
  }
  
  // Public mutators.
public:
  void AddDependentParameter(const std::string& rName, CHistogrammer& rHisto);
  void AddDependentParameter(UInt_t        nId,   CHistogrammer& rHisto);
  void setRawScriptText (const std::string am_sRawScriptText)
  { 
    m_sRawScriptText = am_sRawScriptText;
    RebuildState();
  }


  // Operations on the class.
public:

  virtual   void operator() (CEvent& rEvent)    ;
  std::list<std::string>::iterator ParNamesBegin ()    ;
  UInt_t ParNamesSize ()    ;
  std::list<std::string>::iterator ParNamesEnd ()    ;
  std::list<UInt_t>::iterator ParIdsBegin ()    ;
  UInt_t ParIdsSize ()    ;
  std::list<UInt_t>::iterator ParIdsEnd ()    ;
  
protected:
  // Protected utilities.

  void RebuildState();		// Rebuild script and parameter object std::list.
				// from raw text and parameter std::lists.
  void BindOutputParameter(const std::string& rName, 
			   CHistogrammer& rHistogrammer);
};

#endif







