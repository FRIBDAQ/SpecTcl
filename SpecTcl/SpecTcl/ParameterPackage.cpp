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


#include "ParameterPackage.h"                               
#include "TCLHistogrammer.h"
#include "Exception.h"
#include "DictionaryException.h"
#include "Parameter.h"
#include "ParameterCommand.h"
#include "PseudoCommand.h"
#include "PseudoScript.h"

#include <histotypes.h>
#include <string>
#include <vector>
#include <list>
#include <assert.h>
#include <stdio.h>

static const char* Copyright = 
"CParameterPackage.cpp: Copyright 1999 NSCL, All rights reserved\n";

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
CParameterPackage::AddParameter(CTCLResult& rResult, 
				const char* pName, UInt_t nId, UInt_t nBits) 
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

  try {
    m_pHistogrammer->AddParameter(std::string(pName),
				 nId,
				 nBits);
    rResult = pName;
    return TCL_OK;
  }
  catch (CException& rException) { // Map exceptions to result code, return.
    rResult = rException.ReasonText();
    return TCL_ERROR;
  }
  assert(0);
}
//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    CTCLString CreateTclParameterList ( CTCLInterpreter& rInterp )
//  Operation Type:
//     Inquiry
//
CTCLList 
CParameterPackage::CreateTclParameterList(CTCLInterpreter& rInterp) 
{
// Creates a TCL List whose contents are 
// Descriptions of each parameter.  
// The list is alphabetical order by parameter.
//  Each element of the list is a sublist of the form:
//
//   { name  idnumber bitsofresolution }
//
  CTCLString List;		// List is built up in here.

  // Note that dictionary order should be parameter name order.

  ParameterDictionaryIterator i;
  for(i = m_pHistogrammer->ParameterBegin();  
      i != m_pHistogrammer->ParameterEnd(); i++) {
    List.AppendElement(getParameterInfoListString((*i).second));
    List.Append("\n");
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

  try {
    CParameter* pParam = m_pHistogrammer->FindParameter(std::string(pName));
    if(pParam == (CParameter*)kpNULL) 
      throw CDictionaryException(CDictionaryException::knNoSuchKey,
				 "Locating parameter",
				 pName);
    if(pParam->getName() != std::string(pName))
      throw CDictionaryException(CDictionaryException::knNoSuchKey,
				 "Locating parameter - inexact match only",
				 pName);
    pParam = m_pHistogrammer->RemoveParameter(pName);
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

  char Id[100];
  sprintf(Id, "%d", nId);	// String version of Id string.

  try {
    CParameter* pPar = m_pHistogrammer->FindParameter(nId);
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


  try {
    CParameter* pParam = m_pHistogrammer->FindParameter(std::string(pName));
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

  char Id[100];
  sprintf(Id, "%d", nId);

  try {
    CParameter* pParam = m_pHistogrammer->FindParameter(nId);
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

  List.AppendElement(rParameter.getName()); // Parameter name

  sprintf(Text, "%d", rParameter.getNumber()); // Text version of id.
  List.AppendElement(Text);

  sprintf(Text, "%d", rParameter.getScale()); // Resolution in bits.
  List.AppendElement(Text);


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




































