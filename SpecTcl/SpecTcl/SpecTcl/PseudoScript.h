
 
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

#ifndef __PSEUDOSCRIPT_H  //Required for current class
#define __PSEUDOSCRIPT_H


#ifndef __NAMEDITEM_H
#include "NamedItem.h"
#endif

#ifndef __EVENT_H
#include <Event.h>
#endif                               

#ifndef __TCLINTERPRETEROBJECT_H
#include "TCLInterpreterObject.h"
#endif

#ifndef __HISTOGRAMMER_H
#include <Histogrammer.h>
#endif

#ifndef __TCLOBJECT_H
#include "TCLObject.h"
#endif

#ifndef __STL_LIST                               
#include <list>
#define __STL_LIST
#endif

#ifndef __STL_STRING
#include <string>
#define __STL_STRING
#endif

#ifndef __STL_VECTOR
#include <vector>
#define __STL_VECTOR
#endif

class CPseudoScript  : public CNamedItem        
{                         
  list<string> m_vParameterNames; //Names of input parameters.
  list<UInt_t> m_vParameterIds;  //Paramter Ids which are input to the script.
  string       m_sRawScriptText; //Raw text of script.
  CTCLObject   m_ScriptProc;	//1:1 aggregation part data member
  CTCLObject   m_CallStub;	//1:1 aggregation part data member        
  CTCLInterpreter* m_pInterpreter; //association object data member      
  Bool_t       m_fEnabled;     // True if should eval pseudo.
public:

   // Constructors and other cannonical operations:
  
  CPseudoScript(const string& rName, 
		CTCLInterpreter* pInterp,
		CHistogrammer& rHistogrammer) ;
  CPseudoScript(const string& rName,
		vector<string>& rDependentParameters,
		CHistogrammer& rHistogrammer,
		CTCLInterpreter* pInterp, 
		const string& ScriptText);
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

  list<string> getParameterNames() const
  { return m_vParameterNames;
  }
  list<UInt_t> getParameterIds() const
  { return m_vParameterIds;
  }
  string getRawScriptText() const
  { return m_sRawScriptText;
  }
  string getOutputParameter() const
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

  void setParameterNames (const list<string> am_vParameterNames)
  { 
    m_vParameterNames = am_vParameterNames;
    RebuildState();
  }
  void setParameterIds (const list<UInt_t> am_vParameterIds)
  { 
    m_vParameterIds = am_vParameterIds;
    RebuildState();
  }
  void setOutputParameter (const string am_sOutputParameter)
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
  void AddDependentParameter(const string& rName, CHistogrammer& rHisto);
  void AddDependentParameter(UInt_t        nId,   CHistogrammer& rHisto);
  void setRawScriptText (const string am_sRawScriptText)
  { 
    m_sRawScriptText = am_sRawScriptText;
    RebuildState();
  }


  // Operations on the class.
public:

  virtual   void operator() (CEvent& rEvent)    ;
  list<string>::iterator ParNamesBegin ()    ;
  UInt_t ParNamesSize ()    ;
  list<string>::iterator ParNamesEnd ()    ;
  list<UInt_t>::iterator ParIdsBegin ()    ;
  UInt_t ParIdsSize ()    ;
  list<UInt_t>::iterator ParIdsEnd ()    ;
  
protected:
  // Protected utilities.

  void RebuildState();		// Rebuild script and parameter object list.
				// from raw text and parameter lists.
  void BindOutputParameter(const string& rName, 
			   CHistogrammer& rHistogrammer);
};

#endif







