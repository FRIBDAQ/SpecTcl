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

//  CParameterPackage.cpp
//
//   Author:
//      Ron Fox
//      NSCL
//      Michigan State University
//      East Lansing, MI 48824-1321
//      mailto:fox@nscl.msu.edu
//
//////////////////////////.cpp file/////////////////////////////////////////////////////

//
// Header Files:
//

#include <config.h>
#include "ParameterPackage.h"                               
#include "TCLHistogrammer.h"
#include "Exception.h"
#include "DictionaryException.h"
#include "Parameter.h"
#include "ParameterCommand.h"
#include "PseudoCommand.h"
#include "PseudoScript.h"
#include "SpecTcl.h"
#include "TCLResult.h"

#include <histotypes.h>
#include <string>
#include <vector>
#include <list>
#include <assert.h>
#include <stdio.h>


#ifdef HAVE_STD_NAMESPACE
using namespace std;
#endif

// Functions for class CParameterPackage

//////////////////////////////////////////////////////////////////////////
//
// Function:
//    CParameterPackage (CTCLInterpreter* pInterp, 
//                       CTCLHistogrammer* pHistogrammer)
// OPeration Type:
//    Constructor
//

CParameterPackage::CParameterPackage (CTCLInterpreter* pInterp, 
				      CTCLHistogrammer* pHistogrammer) :
  CTCLCommandPackage(pInterp, Copyright),
  m_pHistogrammer(pHistogrammer),
  m_pParameter(new CParameterCommand(pInterp, *this)),
  m_pPseudo(new CPseudoCommand(pInterp, *this))
{
  AddProcessor(m_pParameter);
  AddProcessor(m_pPseudo);
}
//////////////////////////////////////////////////////////////////////////
// 
// Function:
//   ~CParameterPackage()
// Operation Type:
//   Destructor
//
CParameterPackage::~CParameterPackage()
{
  delete m_pParameter;
  delete m_pPseudo;
}
//////////////////////////////////////////////////////////////////////////
//
// Function:
//    CParameterPackage (const CParameterPackage& aCParameterPackage )
//  Operation Type:
//     Copy Constructor
//

CParameterPackage::CParameterPackage 
                         (const CParameterPackage& aCParameterPackage ) :
  CTCLCommandPackage(aCParameterPackage),
  m_pHistogrammer(aCParameterPackage.m_pHistogrammer),
  m_pParameter(new CParameterCommand(aCParameterPackage.getInterpreter(),
				   *this))
{

}

//////////////////////////////////////////////////////////////////////////
//
// Function:
//    CParameterPackage& operator=(const CParameterPackage& aCParameterPackage)
// Operation Type:
//    Assignment operator.
//

CParameterPackage&
CParameterPackage::operator=(const CParameterPackage& aCParameterPackage)
{
  if(this != &aCParameterPackage) {
    CTCLCommandPackage::operator=(aCParameterPackage);
    m_pHistogrammer = aCParameterPackage.m_pHistogrammer; 
    m_pParameter    = new CParameterCommand(getInterpreter(),
					    *this);
  }
  return *this;
}

//////////////////////////////////////////////////////////////////////////
// 
// Function:
//     int operator== (const CParameterPackage& aCParameterPackage)
// Operation Type:
//      Comparison.
//
int
CParameterPackage::operator==(const CParameterPackage& aCParameterPackage)
{
  return (
	  CTCLCommandPackage::operator==(aCParameterPackage) &&
	  (m_pHistogrammer == aCParameterPackage.m_pHistogrammer)
	  );
}

//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    Int_t AddParameter ( CTCLResult& rResult, const char* pName, UInt_t nId, UInt_t nBits )
//  Operation Type:
//     Interface
//
Int_t 
CParameterPackage::AddParameter(CTCLResult& rResult, const char* pName, 
				UInt_t nId, UInt_t nBits,
				Float_t nLow, Float_t nHi, 
				const char* pUnits) 
{
// Interacts with the Histogrammer to create a new
// parameter.  Catches common exceptions and
// converts them into errors.
//
// Formal Parameters
//
//    CTCLResult&     result:
//         Where the result is placed.  The result is one of the
//         following:
//             If successful, the name of the new parameter.
//             If failed a string describing why failed.
//    const char*  pName:
//             Requested name of the new parameter.
//     UInt_t          nID:
//              Identifier of the parameter.
//     UInt_t          nBits:
//              Ln2 of the parameter resolution.
// Returns
//     TCL_OK           - If the parameter was entered successfullly
//     TCL_ERROR   - If the parameter could not be entered.
// NOTE:
//     The reason string is placed in the result. object passed in.
//
  SpecTcl& api(*(SpecTcl::getInstance()));

  try {
    if(nHi == nLow) {
      api.AddParameter(pName, nId, nBits);
      rResult = pName;
      return TCL_OK;
    }
    else {
      if(pUnits != (char*)kpNULL) {
	api.AddParameter(pName, nId, nBits, nLow, nHi, pUnits);
      }
      else {
	api.AddParameter(pName, nId, nBits, nLow, nHi, "");
      }
      rResult = pName;
      return TCL_OK;
    }
  }
  catch (CException& rException) { // Map exceptions to result code, return.
    rResult = rException.ReasonText();
    return TCL_ERROR;
  }
  assert(0);
}
/*!
   Add a real parameter.  Real parameters are never scaled.  Spectra select
   from a range within the real parameter. 
   \param rResult (CTCLResult& [out]) Result string of the creation.  If
                 the parameter was created, the result string contains the
		 new parameter's name. Otherwise and error message.
   \param pName   (const char* [in]) Name to assign the new parameter.
                 Note the histogrammer's parameter creationals will throw
		 an exception if this is a duplicate.  We catch the exception
		 and map it to a TCL_ERROR return with the exception reason
		 text in rResult.
   \param nId     (int [in]) The parameter id (slot in the event array).
                 This must be unique or else the histogrammer's creational
		 function will throw an exception.  We catch the exception
		 and map it to a TCL_ERROR return with the exception reason
		 text in rResult.
   \param pUnits (const char* [in] defaults to kpNULL) : If not null units
                 to assign to the real parameter. If null the parameter will
		 not be considered unit-less.
   \return Any of:
   - TCL_OK   - The parameter was successfully added to the histogrammer's 
                dictionary.
   - TCL_ERROR- The parameter was not successfully added to the histogrammer's
                dictionary.
*/
Int_t
CParameterPackage::AddParameter(CTCLResult& rResult, const char* pName,
				UInt_t nId, const char* pUnits)
{
  SpecTcl& api(*(SpecTcl::getInstance()));
  string Units;
  if(pUnits) {
    Units = pUnits;
  }

  try {
    api.AddParameter(pName, nId, Units);

  }
  catch (CException& rException) {
    rResult = rException.ReasonText();
    return TCL_ERROR;
  }
  rResult = pName;
  return TCL_OK;
}
//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    CTCLString CreateTclParameterList ( CTCLInterpreter& rInterp )
//  Operation Type:
//     Inquiry
//
CTCLList 
CParameterPackage::CreateTclParameterList(CTCLInterpreter& rInterp, const char* pattern) 
{
// Creates a TCL List whose contents are 
// Descriptions of each parameter.  
// The list is alphabetical order by parameter.
//  Each element of the list is a sublist of the form:
//
//   { name  idnumber bitsofresolution }
//
  SpecTcl& api(*(SpecTcl::getInstance()));
  CTCLString List;		// List is built up in here.

  // Note that dictionary order should be parameter name order.

  ParameterDictionaryIterator i;
  for(i = api.BeginParameters();  
      i != api.EndParameters(); i++) {
      const char* name = (((*i).second).getName()).c_str();
      if (Tcl_StringMatch(name, pattern))
	  {
	    List.AppendElement(getParameterInfoListString((*i).second));
	    List.Append("\n");
	  }
  }


  return CTCLList(&rInterp, (const char*)List);


}
//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    Int_t DeleteParameter ( CTCLResult& rResult, const char* pName )
//  Operation Type:
//     mutator
//
Int_t 
CParameterPackage::DeleteParameter(CTCLResult& rResult, const char* pName) 
{
// Requests that a parameter be deleted.
//
// Formal Parameters:
//    CTCLResult:
//        Holds either:
//              Name of the parameter deleted or
//              reason parameter could not be deleted
//              from caught exceptions
//    const char* pName:
//        Name of the parameter string
//
// Returns:
//      TCL_OK         - success
//      TCL_ERROR - Failed with reason in the error string
// NOTE:
//   Unlike the histogramming library, we require an exact match of parameter
//   name

  SpecTcl& api(*(SpecTcl::getInstance()));
  try {
    CParameter* pParam = api.FindParameter(std::string(pName));
    if(pParam == (CParameter*)kpNULL) 
      throw CDictionaryException(CDictionaryException::knNoSuchKey,
				 "Locating parameter",
				 pName);
    if(pParam->getName() != std::string(pName))
      throw CDictionaryException(CDictionaryException::knNoSuchKey,
				 "Locating parameter - inexact match only",
				 pName);
    pParam = api.RemoveParameter(pName);
    delete pParam;
    rResult = pName;
    return TCL_OK;
  }
  catch (CException& rExcept) {
    rResult = rExcept.ReasonText();
    return TCL_ERROR;
  }
  assert(0);

}
//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    Int_t DeleteParameter ( CTCLResult& rResult, UInt_t nId )
//  Operation Type:
//     mutator
//
Int_t 
CParameterPackage::DeleteParameter(CTCLResult& rResult, UInt_t nId) 
{
// Deletes a requested parameter.
// 
// Formal Parameters:
//      CTCLResult& rResult:
//         The result string which will contain either:
//          The name of the parameter deleted or
//          The reason (snagged from the exception)
//           why the deletion failed.
//      UInt_t id:
//         Number of the parameter to delete.
// NOTE:
//     This is done a bit inefficiently in order to re-use the delete by name
//     code

  SpecTcl& api(*(SpecTcl::getInstance()));

  char Id[100];
  sprintf(Id, "%d", nId);	// String version of Id string.

  try {
    CParameter* pPar = api.FindParameter(nId);
    if(pPar == (CParameter*)kpNULL)
      throw CDictionaryException(CDictionaryException::knNoSuchId,
				 "Looking up parameter",
				 Id);
    DeleteParameter(rResult, pPar->getName().c_str());
    return TCL_OK;
  }
  catch (CException& rException) {
    rResult = rException.ReasonText();
    return TCL_ERROR;
  }
  assert(0);

}
//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    Int_t ListParameter ( CTCLResult& rResult, const char*  pName )
//  Operation Type:
//     Inquiry
//
Int_t 
CParameterPackage::ListParameter(CTCLResult& rResult, const char*  pName) 
{
// Creates a TCL list which describes a single parameter.
// 
// Formal Parameters:
//      CTCLResult&   rResult:
//             Contains the result string which ies either a list of the form:
//                      { name  id resolutionbits }
//             or is a description of why this list could not have been made.
//       const char* pName:
//           Pointer to the name of the parameter to list.
// Returns:
//     TCL_OK       - List returned.
//     TCL_ERROR- Error string returned.
//  Note:
//     We allow inexact matches.

  SpecTcl& api(*(SpecTcl::getInstance()));
  try {
    CParameter* pParam = api.FindParameter(std::string(pName));
    if(pParam == (CParameter*)kpNULL)
      throw CDictionaryException(CDictionaryException::knNoSuchKey,
				 "Looking up parameter",
				 pName);
    rResult = (const char*)getParameterInfoListString(*pParam);
  }
  catch(CException& rExcept) {
    rResult = rExcept.ReasonText();
    return TCL_ERROR;
  }
  return TCL_OK;

}
//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    Int_t ListParameter ( CTCLResult& rResult, UInt_t  nId )
//  Operation Type:
//     Inquiry
//
Int_t 
CParameterPackage::ListParameter(CTCLResult& rResult, UInt_t  nId) 
{
// Creates a Tcl list which represents the information
// describing a parameter. 
//
// Formal Parameters:
//    CTCLResult& rResult
//       Either contains a list of the form:
//            { name id resolutionbits }
//       Or the reason such a list could not be made.
//   UInt_t nId:
//       ID of the parameter which is requested.
// Returns:
//     TCL_OK         - List was created.
//      TCL_ERROR- List could not be created.
// Exceptions:  

  SpecTcl& api(*(SpecTcl::getInstance()));

  char Id[100];
  sprintf(Id, "%d", nId);

  try {
    CParameter* pParam = api.FindParameter(nId);
    if(pParam == (CParameter*)kpNULL)
      throw CDictionaryException(CDictionaryException::knNoSuchKey,
				 "Looking up parameter",
				 Id);
    rResult = (const char*)getParameterInfoListString(*pParam);
  }
  catch(CException& rExcept) {
    rResult = rExcept.ReasonText();
    return TCL_ERROR;
  }
  return TCL_OK;
}
//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    CTCLString getParameterInfoListString ( CParameter& rParameter )
//  Operation Type:
//     Utility (public)
//
CTCLString 
CParameterPackage::getParameterInfoListString(CParameter& rParameter) 
{
// Given a parameter, returns a TCL list which describes
// the parameter.
//
//  Formal Parameters:
//       CParameter&   rParameter:
//              Refers to the parameter to decode.
//  Returns:
//      String containing list of the form { name id resolutionbits }

  char       Text[100];
  CTCLString List;
  string     Empty("");	// Tcl empty string.

  List.AppendElement(rParameter.getName()); // Parameter name

  sprintf(Text, "%d", rParameter.getNumber()); // Text version of id.
  List.AppendElement(Text);

  //  We produce the following elements:
  //     nBits     - only defined if hasScale().
  //     Lowlimit  - Only defined if hasScale() && getLow() != getHigh()
  //     Highlimit - Only defined if hasScale() && getLow() != getHigh()
  //     Units     - Only defined if nonempty string.
  //
  //  Undefined values have their places held by {}  (an empty string).

  if(rParameter.hasScale()) {	// Parameter has used bits info:
    sprintf(Text, "%d", rParameter.getScale());
    List.AppendElement(Text);
  } else {
    List.AppendElement(Empty);
  }
  
  if(rParameter.getLow() != rParameter.getHigh()) { // Parameter has range:
    List.StartSublist();

    List.AppendElement(rParameter.getLow());
    List.AppendElement(rParameter.getHigh());

    List.AppendElement(rParameter.getUnits());

    List.EndSublist();

  }
  else {			// No scaling or range info:
    string RangeUnits("{} {} {");	// No low/high
    RangeUnits += rParameter.getUnits(); // append units to that.
    RangeUnits += '}';		// as the guts of a list.
    List.AppendElement(RangeUnits);	// range/units 
  }


  return List;

}
///////////////////////////////////////////////////////////////////////////////
//
//  Function:
//    Int_t AddPseudo(CTCLResult& rResult, const char* pPseudoName,
// 		      vector<string>& rDependents, const char* pBody)
//
// Operation Type:
//   Mutator.
//
Int_t
CParameterPackage::AddPseudo(CTCLResult& rResult, const char* pPseudoName,
			     vector<string>& rDependents, const char* pBody)
{
  // Adds a Pseudo parameter definition to the current histogrammer.
  // 
  // Formal Parameters:
  //  CTCLResult& rResult:
  //    Result string which, on success is the name of the procedure
  //    created.
  //  const char* pPseudoName:
  //    Name of the pseudo to add.
  //  vector<string>& rDependents:
  //    The parameters on which this pseudo depends.
  //  const char* pBody:
  //    The text of the body of the script.
  //

  CTCLInterpreter* Interp = m_pHistogrammer->getInterpreter();
  try {
    string name(pPseudoName);
    string body(pBody);
    CPseudoScript pseudo(name, rDependents,
			 *m_pHistogrammer, Interp, body);
    m_pHistogrammer->AddPseudo(pseudo);
    rResult += pseudo.getName();
    rResult += "_Procedure";
  }
  catch(CException& rException) {
    rResult += rException.ReasonText();
    return TCL_ERROR;
  }

  return TCL_OK;  
}
/////////////////////////////////////////////////////////////////////////////
//
// Function:
//      Int_t DescribePseudo(const string& rName, string& rDescription)
// Operation Type:
//    selector.
//
Int_t
CParameterPackage::DescribePseudo(const string& rName, string& rDescription)
{
  // produces a string which describes a pseudo parameter.
  // The string is a TCL formatted list containing:
  //   Pseudo name.
  //   A sublist containing the dependent parameters.
  //   The script body.
  // Formal Parameters:
  //    const string& rName:
  //      Name of the pseudo.
  //    string& rDescription:
  //      Will contain the description if found or reason it could not be.
  // Returns:
  //    TCL_OK    - Found and described.
  //    TCL_ERROR - Not found.

  try {
    // Script name:

    CPseudoScript& rScript = m_pHistogrammer->FindPseudo(rName);
    CTCLString Descrip;
    Descrip.AppendElement(rScript.getOutputParameter());
    
    // Dependent parameters.

    Descrip.StartSublist();
    list<string>::iterator pPars = rScript.ParNamesBegin();
    while(pPars != rScript.ParNamesEnd()) {
      Descrip.AppendElement(*pPars);
      pPars++;
    }
    Descrip.EndSublist();

    // Script body.

    Descrip.AppendElement(rScript.getRawScriptText());
   
    rDescription = string((const char*)Descrip);
    return TCL_OK;
  }
  catch ( CException& rExcept ) {
    rDescription = rExcept.ReasonText();
    return TCL_ERROR;
  }
}     
/////////////////////////////////////////////////////////////////////////////
//
// Function:
//   Int_t DeletePseudo(const string& rName, string& rResult)
// Operation Type:
//   Mutator.
//
Int_t
CParameterPackage::DeletePseudo(const string& rName, string& rResult)
{
  // Deletes a pseudo parameter, note that the associated parameter
  // definition remains.
  //
  // Formal parameters:
  //   const string & rName:
  //      Name of the parameter to delete.
  //   string& result:
  //      If an error occurs the reason is here.
  // Returns:
  //   TCL_OK on success
  //   TCL_ERROR on failure.
  //

  try {
    m_pHistogrammer->RemovePseudo(rName);
    rResult = "";
    return TCL_OK;
  }
  catch (CException& rExcept) {
    rResult = rExcept.ReasonText();
    return TCL_ERROR;
  }

}
///////////////////////////////////////////////////////////////////////////
//
//  Function:
//     void  GetPseudoNames(list<string>& rNames)
//  Operation Type:
//     Selector.
//
void
CParameterPackage::GetPseudoNames(list<string>& rNames)
{
  // Returns the names of all of the pseudo parameters.
  //
  // Formal Parameters:
  //    list<string>& rNames:
  //       Set of names returned.
  //
  PseudoParameterIterator p = m_pHistogrammer->PseudoBegin();
  while(p != m_pHistogrammer->PseudoEnd()) {
    rNames.push_back((*p).getOutputParameter());
    p++;
  }
}
