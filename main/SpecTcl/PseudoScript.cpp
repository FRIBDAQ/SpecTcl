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

// Class: CPseudoScript
// Encapsulates a pseudo parameter script.
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
//      The pseudo is a procedure which returns a result
//     which is stuffed into the target id.
//
//
// Author:
//    Ron Fox
//    NSCL
//    Michigan State University
//    East Lansing, MI 48824-1321
//
//
//////////////////////////.cpp file/////////////////////////////////////////////////////
#include <config.h>
#include "PseudoScript.h"    				
#include <TCLInterpreter.h>
#include <DictionaryException.h>
#include <TCLString.h>
#include <Exception.h>
#include <TCLException.h>

#include <stdlib.h>
#include <list>

#include <Iostream.h>
#include <stdio.h>

#ifdef HAVE_STD_NAMESPACE
using namespace std;
#endif


static const char* pCopyrightNotice = 
"(C) Copyright 1999 NSCL, All rights reserved PseudoScript.cpp \n";


// Functions for class CPseudoScript

//////////////////////////////////////////////////////////////////////////////
//
// Function:
//   CPseudoScript(const string rName, 
//		CTCLInterpreter* pInterp,
//		CHistogrammer& rHistogrammer)
// Operation Type:
//    Constructor.
//
CPseudoScript::CPseudoScript(const string& rName, 
			     CTCLInterpreter* pInterp,
			     CHistogrammer& rHistogrammer) :
  CNamedItem(rName, 0),
  m_pInterpreter(pInterp),
  m_fEnabled(kfTRUE)
{
  //  Generate an empty script:
  //  Formal Parameters:
  //     const string& rName:
  //        Name of the new pseudo (must already exist in rHistogrammer's
  //        parameter dictionary.
  //     CTCLInterpreter* pInterp:
  //        Pointer to the interpreter within which the script's objects
  //        will be evaluated.
  //     CHistogrammer& rHistogrammer:
  //        Refers to a histogrammer for which the pseudo is defined.
  //

  BindOutputParameter(rName, rHistogrammer);
}

//////////////////////////////////////////////////////////////////////////////
//
// Function:
//   CPseudoScript(const string& rName, UInt_t nId,
//		const vector<string>& rDependentParameters,
//		CHistogrammer& rHistogrammer,
//	        CTCLInterpreter* pInterp, string ScriptText)
//
// Operation Type:
//    Constructor.
//
CPseudoScript::CPseudoScript(const string& rName,
			     vector<string>& rDependentParameters,
			     CHistogrammer& rHistogrammer,
			     CTCLInterpreter* pInterp, 
			     const string& ScriptText) :
  CNamedItem(rName, 0),
  m_sRawScriptText(ScriptText),
  m_pInterpreter(pInterp),
  m_fEnabled(kfTRUE)
{
  // Constructs a fully developed Pseudo parameter script, given all
  // the needed data.
  // Throws a CDictionary exception for any parameter name not found in 
  // rHistogrammer's parameter dictionary.
  //
  // Formal Parameters:
  //   const string& rName: 
  //      Name of the pseudo as entered in the parameter dictionary.
  //   const vector<string>& rDependentParameters:
  //      Array of names of parameters this pseudo needs.
  //   CHistogrammer& rHistogrammer:
  //      Refers to the histogrammer in which this pseudo will be used.
  //   CTCLInterpreter* pInterp:
  //      Refers to the interpreter in which all the objects will be evaluated.
  //   string
  // The following exceptions will be thrown:
  //    CDictionaryException:  If there is no parameter id matching the
  //                           a dependent parameter name.
  //    CDictionaryException:  If there is no parameter with the same name
  //                           as the pseudo.
  //  NOTE:
  //     The bindings between parameter name and parameter ids is static.
  //     that is if a parameter is deleted and then rebuilt with a
  //     different Id, the old id is used.
  //

  BindOutputParameter(rName, rHistogrammer);

  // Create the list of dependent parameters and their ids:

  vector<string>::iterator pPName = rDependentParameters.begin();
  while(pPName != rDependentParameters.end()) {
    AddDependentParameter(*pPName, rHistogrammer);
    pPName++;
  }
  // Prepare the pseudo script for execution:

  RebuildState();
}

//////////////////////////////////////////////////////////////////////////////
//
//  Function:       
//     operator()(CEvent& rEvent)
//  Operation Type: 
//     Evaluation.
void CPseudoScript::operator()(CEvent& rEvent)  
{
  // Evaluates the script, given an input event array.
  // The input event array is modified to incorporate the
  // pseudo parameter.
  //
  //  Formal Parameters:
  //        CEvent& rEvent:
  //            Refers to the input event.
  //  Returns 
  //    void
  //  NOTE:
  //     Script evaluation failures are noted at stdout and
  //     the result parameter is set to undefined.  This 
  //     involves catching CTCLExceptions from the script object.
  //

  // Update the event parameter objects with the actual parameter values.

  if(!m_fEnabled) return;	// Return if disabled.
  list<UInt_t>::iterator     pId         = m_vParameterIds.begin();
  string Pseudo(m_CallStub);
  //
  // We must now build up the actual parameter list.
  // This will consist of pairs of paramter value and
  // true/false flags:
  //     The flags are true if the parameter id is in the range of the
  //     Event vector and that rEvent[nid].isValid() and false otherwise:
  //
  
  while(pId != m_vParameterIds.end()) {

    char  Value[100];
    Float_t nValue;
    bool  isValid;
    if((*pId) < rEvent.size()) {
      if(rEvent[*pId].isValid()) {
		nValue = rEvent[*pId];
		isValid= true;
      } 
      else {
	nValue =-1;
	isValid = false;
      }
    }
    else {
      nValue =  -1;	    // This means undefined.
      isValid= false;   // Which is unconditionally not set.
    }
    sprintf(Value, " %f %d", nValue,
    		isValid ? 1 : 0);
    Pseudo += Value;
    pId++;

  }
  // The Pseudo is ready to be evaluated:

  try {
    CTCLObject Result;
    Result = m_pInterpreter->GlobalEval(Pseudo).c_str();
    Result.Bind(m_pInterpreter);
    rEvent[getNumber()] = (double)(Result);	// Set the result.
  }
  catch (CTCLException& rException) {
    string Result;
    Result = rException.ReasonText();

    cerr << "Evaluation of script: " << getName() << " failed\n";
    cerr << "Result was: " << Result << endl;
    cerr << rException.WasDoing() << endl;
    cerr << "Disabling pseudo - fix it please" << endl;
    setEnabled(kfFALSE);
    cerr.flush();
  }
  catch (CException& rException) { // Throw on script execution error.
    cerr << "Evaulation of script: " << getName() << " failed\n";
    cerr << rException.WasDoing() << endl;
    cerr << "Disabling pseudo - fix it please" << endl;
    setEnabled(kfFALSE);
    cerr.flush();

  }
  catch (...) {
    cerr << " Evaluation of script: " << getName() << " faild\n";
    cerr << " Unanticipated exception thrown \n";
    cerr << "Disabling pseudo - fix it please" << endl;
    setEnabled(kfFALSE);
    cerr.flush();
  }
  
  
}
//////////////////////////////////////////////////////////////////////////////
//
//  Function:       
//     ParNamesBegin()
//  Operation Type: 
//     selector
list<string>::iterator CPseudoScript::ParNamesBegin()  
{
  // Lets the caller start iterating through the set of 
  // parameter names on which the pseudo depends.
  //
  
  return m_vParameterNames.begin();
  
}
//////////////////////////////////////////////////////////////////////////////
//
//  Function:       
//     ParNamesSize()
//  Operation Type: 
//     Selector
UInt_t CPseudoScript::ParNamesSize()  
{
  // Gives the number of parameters the
  // pseudo depends on.
  
  return m_vParameterNames.size();
}
//////////////////////////////////////////////////////////////////////////////
//
//  Function:       
//     ParNamesEnd()
//  Operation Type: 
//     selector
list<string>::iterator CPseudoScript::ParNamesEnd()  
{
  // Retrieves the off-the-end iterator
  // for the dependent name set.
  
  return m_vParameterNames.end();
}
//////////////////////////////////////////////////////////////////////////////
//
//  Function:       
//     ParIdsBegin()
//  Operation Type: 
//     selector
list<UInt_t>::iterator CPseudoScript::ParIdsBegin()  
{
  // Returns a starting point iterator
  // for the set of parameter ids on which 
  // this pseudo depends.
  
  return m_vParameterIds.begin();
}
//////////////////////////////////////////////////////////////////////////////
//
//  Function:       
//     ParIdsSize()
//  Operation Type: 
//     Selector
UInt_t CPseudoScript::ParIdsSize()  
{
  // Returns number of dependent parameters (from ID list).
  
  return m_vParameterIds.size();
}
//////////////////////////////////////////////////////////////////////////////
//
//  Function:       
//     ParIdsEnd()
//  Operation Type: 
//     selector
list<UInt_t>::iterator CPseudoScript::ParIdsEnd()  
{
  // Returns the off the end iterator
  // for the parameter id list.

  return m_vParameterIds.end();

 
}
////////////////////////////////////////////////////////////////////////////
//
// Function:
//    void AddDependentParameter(const string& rName, CHistogrammer& rHisto)
// Operation Type:
//    Mutator
//
void
CPseudoScript::AddDependentParameter(const string& rName, 
					  CHistogrammer& rHisto)
{
  // Adds a dependent parameter to the pseudo.
  //
  // Formal Parameters:
  //     const string & rName:
  //       Name of the new dependent parameter.
  //     CHistogrammer& rHisto:
  //       Histogrammer in which the parameter is relevant.
  //
  //  Throws a dictionary exception if the parameter does not exist.

  CParameter* pParam = rHisto.FindParameter(rName);
  if(pParam) {
    m_vParameterNames.push_back(pParam->getName());
    m_vParameterIds.push_back(pParam->getNumber());
  }
  else {
    throw CDictionaryException(CDictionaryException::knNoSuchKey,
	  "CPseudoScript::AddDependentParameter - Failed to locate parameter",
			       rName);
  }
}
/////////////////////////////////////////////////////////////////////////////
//
// Function:
//   void BindOutputParameter(const string& rName, 
//			      CHistogrammer& rHistogrammer)
// Operation type:
//   Protected utility.
// 
void
CPseudoScript::BindOutputParameter(const string& rName, 
				   CHistogrammer& rHistogrammer)
{
  // Takes a parameter name and binds our output parameter to the 
  // current definition of that name in the parameter dictionary
  // given in the histogrammer.  This binding is static.
  //
  //  Formal Parameters:
  //    const string& rName:
  //      The name of the output parameter.
  //    CHistogrammer& rHistogrammer:
  //       The histogrammer in which the parameter is to be bound.
  //
  // Throws:
  //    CDictionaryException in the event the parameter is not defined to
  //    the histogrammer.
  //
  CParameter* pParam = rHistogrammer.FindParameter(rName);
  if(pParam) {
    setNumber(pParam->getNumber());
  }
   else {
    throw CDictionaryException(CDictionaryException::knNoSuchKey,
	  "CPseudoScript::BindOutputParameter - Failed to locate parameter",
			       rName);
  }

}
////////////////////////////////////////////////////////////////////////////
//
// Function:
//   void RebuildState()
//
// Operation Type:
//   Protected Utility.
//
void
CPseudoScript::RebuildState()
{
  // Under the assumption that the following are set up:
  //   m_vParameterNames
  //   m_vParameterIds
  //   m_sRawScriptText
  //   m_pInterpreter,
  //
  // The following members are computed, and the script is then ready
  // for operation:
  //
  //  m_ScriptProc
  //  m_CallStub
  //

  // Create the script procedure and execute it.  Note that this just defines
  // and compiles the evaluation procedure into byte code.   If a prior def
  // exists, then it will be overidden.
  // 
  string ProcName("proc ");
  string name(getName());	// Construct the name of the procedure.
  name   += "_Procedure ";
  CTCLString ProcedureDef(ProcName.c_str());
  ProcedureDef.AppendElement(name); // So the name could have spaces etc. in it.

  ProcedureDef.StartSublist();	// Construct the parameter definition.
  list<string>::iterator pn = m_vParameterNames.begin();
  //
  // The procedure is parameterized by:
  //   parameter parameter.isValid pairs 
  // where
  //    parameter is the name of a parameter known to the histogramming
  //    kernel.  For each event, parameter will be substituted for with
  //    the value of that parameter while parameter.isValid will be the
  //    result of isValid() on that parameter.
  //
  while(pn != m_vParameterNames.end()) {
    ProcedureDef.AppendElement((*pn));
    string isValid(*pn);
    isValid+=  "isValid";
    ProcedureDef.AppendElement(isValid);
    pn++;
  }
  ProcedureDef.EndSublist();

  ProcedureDef.AppendElement(m_sRawScriptText);	// Script body.
  m_ScriptProc = (const char*)ProcedureDef;

  // Note that any exception thrown from executing the proc will
  // be passed back to the client.
  //
  m_ScriptProc.Bind(m_pInterpreter);
  try {
    m_ScriptProc();
  }
  catch(CException& rExcept) {
    cerr << "Unable to create a psuedo script: "<<rExcept.ReasonText()<< endl;
    cerr << "Most likely the psedo invocation will fail at invocation\n";
  }
  //
  // Finally a stub object is created.  The stub object is a template
  // for calling the procedure.

  CTCLString Stub;
  Stub.AppendElement(name);
  m_CallStub.Bind(m_pInterpreter);
  m_CallStub = (const char*)Stub;
  m_fEnabled = kfTRUE;		// The proc is enabled.
  
}
