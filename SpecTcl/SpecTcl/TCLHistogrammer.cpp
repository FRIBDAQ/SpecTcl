// Class: CTCLHistogrammer
// Captures the functionality of a Histogrammer in
// the TCL/TK environment.  What makes this sort of
// histogrammer distinct from CHistogrammer is that
// it contains Pseudo parameters which are defined as
// Tcl/TK scripts.  Rather than rolling CHistogrammer into
// a Tcl/TK dependency, this subclass isolates that 
// dependency from the system.
//
// Author:
//    Ron Fox
//    NSCL
//    Michigan State University
//    East Lansing, MI 48824-1321
//
//
//////////////////////////.cpp file/////////////////////////////////////////////////////
#include "TCLHistogrammer.h" 
#include <NamedItem.h>
#include <DictionaryException.h>
#include <Exception.h>
#include <iostream.h>
#include <assert.h>
#include <algorithm>

static char* pCopyrightNotice = 
"(C) Copyright 1999 NSCL, All rights reserved TCLHistogrammer.cpp \n";

//
// 

// Functions for class CTCLHistogrammer

//////////////////////////////////////////////////////////////////////////////
//
// Function:
//    CTCLHistogrammer(CTCLInterpreterObject* m_pInterpreter,
//	    	       UInt_t nSpecBytes = knDefaultSpectrumSize)
// Operation Type:
//    Constructor
//
CTCLHistogrammer::CTCLHistogrammer (CTCLInterpreter* m_pInterpreter,
				    UInt_t nSpecBytes) :
  CHistogrammer(nSpecBytes),
  m_pIntObject(m_pInterpreter)
{
  assert(m_pInterpreter != (CTCLInterpreter*)kpNULL);
}
/////////////////////////////////////////////////////////////////////////////
//
// Function:
//    ~CTCLHistogrammer()
// Operation Type:
//     Destructor
//
CTCLHistogrammer::~CTCLHistogrammer()
{
}
/////////////////////////////////////////////////////////////////////////////
//
// Function:
//    operator=(const CTCLHistogrammer& aCTCLHistogrammer)
// Operation Type:
//    Assignment
//
CTCLHistogrammer& 
CTCLHistogrammer::operator=(const CTCLHistogrammer& aCTCLHistogrammer)
{
  if(this != &aCTCLHistogrammer) {
    CHistogrammer::operator=(aCTCLHistogrammer);
    m_pIntObject = aCTCLHistogrammer.m_pIntObject;
    m_Pseudos    = aCTCLHistogrammer.m_Pseudos;
  }
  return *this;
}
/////////////////////////////////////////////////////////////////////////////
//
// Function:
//    int operator== (const CTCLHistogrammer& aCTCLHistogrammer) const
// Operation Type:
//    Comparison.
//
int
CTCLHistogrammer::operator== (const CTCLHistogrammer& aCTCLHistogrammer)
{
  return (CHistogrammer::operator==(aCTCLHistogrammer)     &&
	  (m_pIntObject == aCTCLHistogrammer.m_pIntObject) &&
	  (m_Pseudos    == aCTCLHistogrammer.m_Pseudos));
}

//////////////////////////////////////////////////////////////////////////////
//
//  Function:       
//     operator()(const  CEvent&)
//  Operation Type: 
//     Override
void CTCLHistogrammer::operator()(const  CEvent& rE)  
{
  // Overrides the base class operation as follows:
  //  first computes all of the Pseudo parameters
  //  then calls the base class operation.
  //
  // Formal Parameters:
  //    const CEvent& rEvent:
  //        refers to the event to histogram.

  CEvent& rEvent((CEvent&)rE);
  PseudoParameterIterator p=m_Pseudos.begin();
  //
  // First compute the Pseudos...
  //
  while(p != m_Pseudos.end()) {
    try {
      (*p)(rEvent);
    }
    catch (CException& rExcept) {
      cerr << "--------------------------------------------------\n";
      cerr << "Evaluation of script for Pseudo " << (*p).getName() 
	   << " failed\n";
      cerr << "That parameter will be undefined this event\n";
      cerr << "Reason\n";
      cerr << rExcept.ReasonText() << endl;
      cerr << "Script Body:\n";
      cerr << (*p).getRawScriptText().c_str() << endl;
      cerr << "---------------------------------------------------\n";
    }
    p++;
  }
  // Then do the normal histogramming.
  //
  CHistogrammer::operator()(rEvent); // 

}
//////////////////////////////////////////////////////////////////////////////
//
//  Function:       
//     AddPseuudo(CPseudoScript& rScript)
//  Operation Type: 
//     Mutator
void CTCLHistogrammer::AddPseudo(CPseudoScript& rScript)  
{
  // Adds a Pseudo parameter to the end of the evaluation chain:
  //  
  // Formal Parameters:
  //     CPseudoScript&   rScript
  //          Refers to the script.
  // Throws:
  //    CDictionaryException if:
  //     1. The named Pseudo parameter does not exist
  //          in the parameter dictionary.
  //     2. A dependent parameter doesn't exist in the parameter dictionary.
  //     3. A pseudo by that name already has a script attached to it.
  //     

  CParameter* pParam = FindParameter(rScript.getName());
  if(!pParam) {			// No such Pseudo defined.
    throw
      CDictionaryException(CDictionaryException::knNoSuchKey,
           "CTCLHistogrammer::AddPseudo - Parameter not defined in dictionary",
			   rScript.getName());
  }

  list<string>::iterator pIter = rScript.ParNamesBegin();
  while(pIter != rScript.ParNamesEnd()) {
    pParam = FindParameter(*pIter);
    if(!pParam) {		// No such depenent Pseudo parameter.
      throw 
	CDictionaryException(CDictionaryException::knNoSuchKey,
  "CTCLHistogrammer::AddPseudo -dependent Pseudo definition not in parameter dictionary",
			     *pIter);
    }
    pIter++;
  }
  CMatchNamedItem predicate(rScript.getName());
  PseudoParameterIterator Pseudo = find_if(m_Pseudos.begin(), 
					m_Pseudos.end(), 
					predicate);
  if(Pseudo != m_Pseudos.end()) { // Duplicate parameter script.
    throw
      CDictionaryException(CDictionaryException::knDuplicateKey,
        "CTCLHistogrammmer::AddPseudo - Duplicate Pseudo parameter script",
			   rScript.getName());
			   
  }

  m_Pseudos.push_back(rScript);
}
//////////////////////////////////////////////////////////////////////////////
//
//  Function:       
//     RemovePseudo(const string& rPseudoName)
//  Operation Type: 
//     Mutator
void CTCLHistogrammer::RemovePseudo(const string& rPseudoName)  
{
  // Removes a named pseudo parameter from the Pseudo script list.
  // 
  // Formal Paramters:
  //     const string& rPseudoName
  // throws:
  //    CDictionaryException:
  //       If the Pseudo parameter is not in the Pseudo paramter list
  //       Note that it is legal, although probably a bug for the 
  //       parameter to be deleted from the Parameter dictionary
  //      without deleting the Pseudo which computes it.
  //
  
  PseudoParameterIterator pIter = find_if(m_Pseudos.begin(),
				       m_Pseudos.end(),
				       CMatchNamedItem(rPseudoName));
  if(pIter != m_Pseudos.end()) {
    m_Pseudos.erase(pIter);
  }
  else {
    throw CDictionaryException(CDictionaryException::knNoSuchKey,
            "CTCLHistogrammer::RemovePseudo - Pseudo parameter does not exist",
			       rPseudoName);
  }
}
//////////////////////////////////////////////////////////////////////////////
//
//  Function:       
//     FindPseudo(const string& rPseudoName)
//  Operation Type: 
//     Selector.
CPseudoScript& 
CTCLHistogrammer::FindPseudo(const string& rPseudoName)  
{
  // Locates a Pseudo parameter by name and
  // returns a reference to the caller. 
  // 
  // Formal Parameters:
  //    const string & rPseudoName
  // Returns:
  //     Reference to the found parameter.
  // throws:
  //      CDictionaryException if the pseudo doesn't 
  //           exist in the pseudo list.
  //
  
  PseudoParameterIterator pIter = find_if(m_Pseudos.begin(),
				       m_Pseudos.end(),
				       CMatchNamedItem(rPseudoName));
  if(pIter != m_Pseudos.end()) {
    return *pIter;
  }
  else {
    throw CDictionaryException(CDictionaryException::knNoSuchKey,
            "CTCLHistogrammer::RemovePseudo - Pseudo parameter does not exist",
			       rPseudoName);
  }

}
//////////////////////////////////////////////////////////////////////////////
//
//  Function:       
//     PseudoBegin()
//  Operation Type: 
//     Selector
PseudoParameterIterator 
CTCLHistogrammer::PseudoBegin()  
{
  // Returns an iterator which can be used to 
  // flip through the Pseudo script list.
  //
  return m_Pseudos.begin();
}
//////////////////////////////////////////////////////////////////////////////
//
//  Function:       
//     PseudoEnd()
//  Operation Type: 
//     Selector
PseudoParameterIterator 
CTCLHistogrammer::PseudoEnd()  
{
  // Returns an interator which can be used to
  // determine when another iterator is off
  // the end of the Pseudo script list
  //
  return m_Pseudos.end();
}
//////////////////////////////////////////////////////////////////////////////
//
//  Function:       
//     PseudoSize()
//  Operation Type: 
//     Selector
UInt_t CTCLHistogrammer::PseudoSize()  
{
  // Returns the number of elements in the Pseudo script
  // list.
  
  return m_Pseudos.size();
  
}
