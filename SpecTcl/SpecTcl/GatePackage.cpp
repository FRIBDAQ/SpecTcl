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

#include <config.h>
#include "GatePackage.h"
#include "GateCommand.h"
#include "ApplyCommand.h"
#include "UngateCommand.h"
#include "SpecTcl.h"

#include <TCLInterpreter.h>    				
#include <TCLResult.h>
#include <Histogrammer.h>
#include <Gate.h>
#include <Cut.h>
#include <PointlistGate.h>
#include <CGammaCut.h>
#include <CGammaBand.h>
#include <CGammaContour.h>
#include <MaskEqualGate.h>
#include <MaskAndGate.h>
#include <MaskNotGate.h>

#include <GateFactory.h>

#include <Spectrum.h>
#include <Parameter.h>
#include <Exception.h>

#include <string>
#include <algorithm>
#include <stdio.h>
#include <Iostream.h>

#ifdef HAVE_STD_NAMESPACE
using namespace std;
#endif

static const  char* pCopyrightNotice = 
"(C) Copyright 1999 NSCL, All rights reserved GatePackage.cpp \n";

UInt_t CGatePackage::m_nNextId(1);


// Helper classes and functions:

Bool_t IdCompare(CGateContainer* e1,
		 CGateContainer* e2)
{
  CGateContainer& p1 = *e1;
  CGateContainer& p2 = *e2;

  return (p1.getNumber() < p2.getNumber());
}

// Functions for class CGatePackage

//////////////////////////////////////////////////////////////////////////////
//
// Function:
//   CGatePackage(CTCLInterpeter* pInterp, CHistogrammer* pHistogrammer)
//
// Operation Type:
//    Constructor.
//
CGatePackage::CGatePackage(CTCLInterpreter* pInterp, 
			   CHistogrammer* pHistogrammer) :
  CTCLCommandPackage(pInterp, pCopyrightNotice),
  m_pHistogrammer(pHistogrammer),
  m_pGateCommand(new CGateCommand(pInterp, *this)),
  m_pApplyCommand(new CApplyCommand(pInterp, *this)),
  m_pUngateCommand(new CUngateCommand(pInterp, *this))
  
{
  // Add the commands to the list which are registered:

  AddProcessor(m_pGateCommand);
  AddProcessor(m_pApplyCommand);
  AddProcessor(m_pUngateCommand);
  
}
//////////////////////////////////////////////////////////////////////////////
//
// Function:
//   ~ CGatePackage ( )
// Operation type:
//    Destructor.
//
CGatePackage::~CGatePackage( )
{
  delete m_pGateCommand;
  delete m_pApplyCommand;
  delete m_pUngateCommand;
}
//////////////////////////////////////////////////////////////////////////////
//
//  Function:       AddGate(CTCLResult& rResult, 
//                          const std::string& rGateName, 
//                          const CGate* pGate)
//  Operation Type: Mutator.

Bool_t 
CGatePackage::AddGate(CTCLResult& rResult, const std::string& rGateName, 
		      const CGate* pGate)  
{
  // Adds a gate to a spectrum.
  //
  // Formal Parameters:
  //     CTCLResult& rResult:
  //         Pointer to the result text which is
  //         >appended< to.
  //         On success, nothing is appended,
  //         On failure,  the reason is appended.
  //     const std::string& rGateName:
  //         Name of the gate to add.
  //     const CGate*      pGate
  //         Pointer to the gate which is added to the dictionary.
  // Returns:
  //    Bool_t
  //                    kfTRUE   - Success.
  //                    kfFALSE  - Failure.
 
  // First try to replace the gate as an existing one:
  //
  SpecTcl& api(*(SpecTcl::getInstance()));

  try {
    api.ReplaceGate(rGateName, *(CGate*)pGate);
    return kfTRUE;
  }
  catch(...) {
    //  If that fails, then add the gate as new:
    // 
    try {			// Exceptions get turned into return strings:
      api.AddGate(rGateName,  (CGate*)pGate);
      return kfTRUE;		// Success.
    }
    catch (CException& rException) {
      rResult += rException.ReasonText();
      return kfFALSE;
    }
  }
}
//////////////////////////////////////////////////////////////////////////////
//
//  Function:       ListGates()
//  Operation Type: Selector.
//

/*!
  \para Functionality
   Returns a TCL formatted list which contains 
   the gate definitions sorted by alphabetical
   order.

      Each gate definition is a sublist containing the following elements:
      #   Gate name.
      #   Gate Id
      #   Gate Type
      #   Gate contents which is a list that is gate type dependent:
                  - Cut:  { parameter { low hi }}
                  - Band,
                  - Contour { {xparam yparam} { {x1 y1} {x2 y2} ... }
                  - GammaCut { low hi {speclist}}
                  - GammaBand  { {x1 y1} {x2 y2} ... {speclist}}
                  - GammaContour  { {x1 y1} {x2 y2}...{speclist}}
                  - Not        { Gatename}
                  - And,
                  - Or          { Gate1 Gate2 ... }
                  - True,
                  - False,
                  - Deleted     {}
  
 */
CTCLString CGatePackage::ListGates(const char* pattern)  
{
  SpecTcl& api(*(SpecTcl::getInstance()));
  CTCLString Gates;
  CGateDictionaryIterator gi = api.GateBegin();
  while(gi != api.GateEnd()) {
    const char* name = (&((*gi).second))->getName().c_str();
    if ( Tcl_StringMatch(name, pattern))
    {
      Gates.AppendElement(GateToString(&((*gi).second)));
      Gates.Append("\n");
    }
    gi++;
  }
  return Gates;
}
///////////////////////////////////////////////////////////////////////////
//
//  Function:       ListGatesById()
//  Operation Type: Selector.

CTCLString CGatePackage::ListGatesById(const char* pattern)  
{
  // Lists the gates in the gate dictionary
  // sorted by Id rather than name.
  //  The output is identical to that of ListGates()
  //
  vector<CGateContainer*> vGates;
  // 
  // We do this in three stages:
  //
  // 1. Collect  the gate information in vGates:
  //
  SpecTcl& api(*(SpecTcl::getInstance()));
  CGateDictionaryIterator gi = api.GateBegin();
  while(gi != api.GateEnd()) {
    const char* name = (&((*gi).second))->getName().c_str();
    if ( Tcl_StringMatch(name, pattern))
      {
      vGates.push_back(&((*gi).second));
      }
    gi++;
  }
  //  2. Sort the gates by ID:
  //
  SortGateListById(vGates);
  //
  //  3. Textuallize the gates into the CTCLString:
  //
  CTCLString sGates;
  for(UInt_t i = 0; i < vGates.size(); i++) {
    sGates.AppendElement(GateToString(vGates[i]));
    sGates.Append("\n");
  }
  return sGates;
  
}
//////////////////////////////////////////////////////////////////////////////
//
//  Function:       DeleteGates(CTCLResult& rResult, 
//                              const vector<string>& rGateNames)
//  Operation Type: Mutator
//
/*!
  \para Functionality:
   Deletes gates given a set of gate names.

   The result string has either nothing appended
   to it (all the gates were deleted) or alternatively
   a list of failure lists as follows:
  <BR>
        {gate_name failure_reason} ...<BR>
  
    \para Formal Parameters:
          \param <TT>rResult (CTCLResult& [out]):</TT>
            The result string as described above.
          \param <TT>rNames (const vector<string>& [in]):</TT>
               Set of names of gates to delete.
    \para Returns:
    \retval Bool_t
          - kfTRUE - All gates deleted.
          - kfFALSE - Some gates could not be deleted.
   \note <CENTER><B>NOTE:</B></CENTER>
       The deleted gates are actually replaced by a 
        CFalseGate.
*/
Bool_t 
CGatePackage::DeleteGates(CTCLResult& rResult, 
			  const vector<string>& rGateNames)  
{
  SpecTcl& api(*(SpecTcl::getInstance()));
  
  CTCLString ResultString;
  UInt_t nFailed = 0;
  for(UInt_t nGate = 0; nGate < rGateNames.size(); nGate++) {
    try {			// All failures are thrown...
      api.DeleteGate(rGateNames[nGate]);
    }
    catch(CException& rFailed) { // Failures are caught and appended to rResult
      CTCLString rFailure;
      rFailure.StartSublist();
      rFailure.AppendElement(rGateNames[nGate]);
      rFailure.AppendElement(rFailed.ReasonText());
      rFailure.EndSublist();
      ResultString.AppendElement(rFailure);
      nFailed++;
    }
  }
  rResult += (const char*)ResultString;
  return (nFailed == 0);
}
///////////////////////////////////////////////////////////////////////////////
//
//  Function:       DeleteGates(CTCLResult& rResult, 
//                              const vector<string>& rIds)
//  Operation Type: Mutator.
Bool_t CGatePackage::DeleteGates(CTCLResult& rResult, 
				 const vector<UInt_t>& rIds)  
{
  // Deletes a set of gates given their Ids.
  // The return result string is either empty or
  // alternatively contains a list of failing gates where
  //  each element is a sublist containing:
  //
  //    Id - Id of the gate which could not be
  //                deleted.
  //    Reason - Why the gate could not be deleted.
  //
  // Formal Parameters:
  //     CTCLResult&                            rResult
  ///       Result string as described above. Note that the result is appended
  //        to any existing text in the result.
  //     const std::vector<std::string>&   rIds
  //         Ids of gates which will be deleted.
  // Returns:
  //     Bool_t:
  //               kfTRUE   - All gates were deleted.
  //               kfFALSE  - At least one gate could not be deleted.
  
  vector<string> Names;
  CTCLString     LookupResult;
  UInt_t         nFailed = 0;

  // The gates are looked up.  Those which exist are then passed
  // to the delete by name function.
  //

  for(UInt_t nGate = 0; nGate < rIds.size(); nGate++) {
    CGateContainer* pG = m_pHistogrammer->FindGate(rIds[nGate]);
    if(pG) {
      Names.push_back(pG->getName());
    }
    else {
      CTCLString Failure;
      char       Id[100];

      sprintf(Id, "%d", rIds[nGate]);
      Failure.StartSublist();
      Failure.AppendElement(Id);
      Failure.AppendElement(" No gate with this Id exists");
      Failure.EndSublist();
      rResult += (const char*)Failure;
      nFailed++;
    }
  }
  // Now Delete the gates we got:


  Bool_t AllDeleted = DeleteGates(rResult, Names);
  return ((AllDeleted) && (nFailed == 0));
  
  
}
///////////////////////////////////////////////////////////////////////////
//
//  Function:       ApplyGate(CTCLString& rResult, const string& rGateName, 
//                            const string& rSpectrumName)
//  Operation Type: Mutator
//
Bool_t CGatePackage::ApplyGate(CTCLString& rResult, const string& rGateName, 
			       const string& rSpectrumName)  
{
  // Applies the selected gate to a spectrum.
  //    The result string returned is empty on success
  //    and has a list shown below on failure:
  //
  //    {Gatename SpectrumName Failure_reason}
  //
  // Formal Parameters:
  //    CTCLString&   rResult:
  //        Result string.
  //     const std::string& rGateName
  //         Name of gate to apply.
  //     const std::string& rSpectrumName:
  //         Name of spectrum to apply it on.
  // Returns:
  //      kfTRUE - Success.
  //      kfFALSE - failure.
  //
  SpecTcl& api(*(SpecTcl::getInstance()));
  try {
    api.ApplyGate(rGateName, rSpectrumName);
    return kfTRUE;
  }
  catch(CException& rExcept) {
    rResult = rExcept.ReasonText();
    return kfFALSE;
  }
}
/////////////////////////////////////////////////////////////////////////////
//
//  Function:       ListAppliedGate(CTCLString& rApplication, 
//                                  const string& rName)
//  Operation Type: Selector
//
Bool_t CGatePackage::ListAppliedGate(CTCLString& rApplication, 
				     const string& rName)  
{
  // Describes the gate which is applied to a given 
  // spectrum (specified by name).  The
  // information is returned in the result string
  //  and is the representation of the gate as
  //  would have been returned by ListGates
  //  if only that gate were in the database.
  //
  //  If this fails, then the reason for failure is returned
  //  in the result string.
  // Formal Parameters:
  //      CTCLString& rResult:
  //          Result string.  
  //       const string& rSpectrum:
  //          Name of the spectrum.
  // Returns:
  //     kfTRUE  - Able to get gate information.
  //    kfFALSE   - Unable to get gate information.
  
  CSpectrum* pSpec = m_pHistogrammer->FindSpectrum(rName);
  if(!pSpec) {
    rApplication += "Spectrum not in dictionary";
    return kfFALSE;
  }
  // Now given the spectrum, get the gate container, format the
  // gate as a string and tack it on to the result string.

  CGateContainer* pGate = (CGateContainer*)(pSpec->getGate());
  rApplication.Append(GateToString(pGate));
  return kfTRUE;
}
//////////////////////////////////////////////////////////////////////////
//
// Function:
//     Bool_t Ungate(CTCLString& rResult, const string& rName)
// Operation Type:
//     Mutator
//
Bool_t
CGatePackage::Ungate(CTCLString& rResult, const string& rName)
{
  // Removes the gate on a spectrum.
  // 
  // Formal parameters:
  //     CTCLString& rResult:
  //        String which will carry failure reason if there was a
  //        problem.
  //     const string& rName:
  //         The name of the spectrum to ungate.
  // Returns:
  //   Bool_t kfTRUE on success, kfFALSE on failure.
  //

  try {
    m_pHistogrammer->UnGate(rName);
    return kfTRUE;
  }
  catch(CException& rExcept) {
    rResult = rExcept.ReasonText();
    return kfFALSE;
  }
  assert(0);
} 
//////////////////////////////////////////////////////////////////////////
//
//  Function:       SortGateListById(vector<CGateContainer*>& rpGates)
//  Operation Type: Utility
void CGatePackage::SortGateListById(vector<CGateContainer*>& rpGates)  
{
  // Takes a vector of pointers to CGateContainer and sorts them so that
  // the pointers are in gate id order.
  //
  // Formal parameter:
  //     vector<CGateContainer*>& rpGates
  //           Refers to the vector of pointers.
  // Note that the vector is sorted in place.
  //

  sort(rpGates.begin(), rpGates.end(), IdCompare);
  
}

///////////////////////////////////////////////////////////////////////
//  Function:       GateToString(CGateContainer* pGate)
//  Operation Type: Utility
//

/*!
  \para Functionality:
   Converts a gate to its string 
   representation.
  
   \para Formal Parameters:
          \param <TT>pGate ( pGateCGateContainer* [in]):</TT>
                      Pointer to the container of the
                      gate to convert.
  
   \para Returns:
       \retval std::string:
           String representation of the gate as defined
           in the definition of ListGates()
  
*/
std::string CGatePackage::GateToString(CGateContainer* pGate)  
{

  CTCLString Result;
  char       Id[100];

  Result.AppendElement(pGate->getName());
  sprintf(Id, "%d", pGate->getNumber());
  Result.AppendElement(Id);

  CGateContainer& rGate(*pGate);
  Result.AppendElement(rGate->Type());

  // The rest of the result depends on the gate type:

  Result.StartSublist();

  string type = rGate->Type();
 
  if(type == "s" ) {		// Cut.
    CCut& rCut((CCut&)*rGate);
    UInt_t nPid = rCut.getId();
    CParameter* Param = m_pHistogrammer->FindParameter(nPid);
    if(Param) {
      Result.AppendElement(Param->getName());
    }
    else {
      Result.AppendElement("-Deleted Parameter-");
    }
  }
  else if( (type == "b") ||
	   (type == "c")) {	// Band or contour, C2Band
    CPointListGate& rPlist((CPointListGate&)(*rGate));
    Result.StartSublist();
    CParameter* Param = m_pHistogrammer->FindParameter(rPlist.getxId());
    if(Param) {
      Result.AppendElement(Param->getName());
    }
    else {
      Result.AppendElement("-Deleted Parameter-");
    }
    Param = m_pHistogrammer->FindParameter(rPlist.getyId());
    if(Param) {
      Result.AppendElement(Param->getName());
    }
    else {
      Result.AppendElement("-Deleted Parameter-");
    }
    Result.EndSublist();
  }
  
  else if (type == "em") {
    CMaskEqualGate& rMask((CMaskEqualGate&)*rGate);  
    UInt_t nPid = rMask.getId();
    CParameter* Param = m_pHistogrammer->FindParameter(nPid);
    if(Param) {
      Result.AppendElement(Param->getName());
    }
    else {
      Result.AppendElement("-Deleted Parameter-");
    }    
    long value = rMask.getCompare();
    Result.AppendElement(value ,"%#X");
    Result.EndSublist();
    return Result;
  }
 else if (type == "am") {
    CMaskAndGate& rMask((CMaskAndGate&)*rGate);  
    UInt_t nPid = rMask.getId();
    CParameter* Param = m_pHistogrammer->FindParameter(nPid);
    if(Param) {
      Result.AppendElement(Param->getName());
    }
    else {
      Result.AppendElement("-Deleted Parameter-");
    }    
    long value = rMask.getCompare();
    Result.AppendElement(value ,"%#X");
    Result.EndSublist();
    return Result;
  }
 else if (type == "nm") {
    CMaskNotGate& rMask((CMaskNotGate&)*rGate);  
    UInt_t nPid = rMask.getId();
    CParameter* Param = m_pHistogrammer->FindParameter(nPid);
    if(Param) {
      Result.AppendElement(Param->getName());
    }
    else {
      Result.AppendElement("-Deleted Parameter-");
    }    
    long value = rMask.getCompare();
    Result.AppendElement(value ,"%#X");
    Result.EndSublist();
    return Result;
  }

  else if ((type == "gs") ||
	   (type == "gb") ||
	   (type == "gc")) {
    ;
  }
  else if (type == "-" ) {	// Not.
    ;				// No special action required.
  }
  else if ((type == "*") ||
	   (type == "+")) {	// And, or
    ;				// No special action required.
  }
  else if ((type == "-deleted-") ||
	   (type == "F")         ||
	   (type == "T")) {	// True, false, deleted.
    ;				// No special action required.
  }
  else {			// Unknown type...
    Result.AppendElement("-Unknown gate type-");
  }
  //  For all gates, we list the constituents as elements:

  if(type == "gb" || type == "gc") {
    Result.StartSublist();
    CConstituentIterator Constituent = rGate->Begin();
    CConstituentIterator end = rGate->End();
    while(Constituent != end) {
      Result.AppendElement(rGate->GetConstituent(Constituent));
      Constituent++;
    }
    Result.EndSublist();
  }
  else if( (type == "s") || (type == "gs")) {
    CConstituentIterator rIter = rGate->Begin();
    string GateInfo = rGate->GetConstituent(rIter);
    UInt_t id;
    Float_t low, hi;		// because constituents have 'too much data'.
    char param[100];
    sscanf(GateInfo.c_str(), "%d %f %f", &id, &low, &hi);
    sprintf(param,"%f %f", low, hi);
    Result.AppendElement(param);

  }
  else {			// Special case for slice

    CConstituentIterator Constituent = rGate->Begin();
    CConstituentIterator End = rGate->End();
    while(Constituent != End) {
      Result.AppendElement(rGate->GetConstituent(Constituent));
      Constituent++;
    }

  }

  // 
  // Special case code to output parameters for
  // gamma gates.  
  //
  vector<UInt_t>           paramIds;
  vector<UInt_t>::iterator pIds;
  Bool_t                   isGammaGate = kfFALSE;

  if (type == "gs") {
    CGammaCut& rCut = ((CGammaCut&)*rGate);
    paramIds = rCut.getParameters();
    isGammaGate = kfTRUE;
  }
  if (type == "gc") {
    CGammaContour& rContour = ((CGammaContour&)*rGate);
    paramIds = rContour.getParameters();
    isGammaGate = kfTRUE;
  }
  if (type == "gb") {
    CGammaBand& rBand = ((CGammaBand&)*rGate);
    paramIds = rBand.getParameters();
    isGammaGate =kfTRUE;
  }
  if(isGammaGate) {
    Result.StartSublist();
    for(pIds = paramIds.begin(); pIds != paramIds.end(); pIds++) {
      UInt_t id;
      id =*pIds;
      CParameter* pParam = m_pHistogrammer->FindParameter(id);
      if(pParam) {
	Result.AppendElement(pParam->getName());
      } 
      else {
	Result.AppendElement("-Deleted Parameter-");
      }

    }
    Result.EndSublist();
  }

  Result.EndSublist();
  return Result;
}

///////////////////////////////////////////////////////////////////////////
//
// Function:
//   UInt_t AssignId()
// Operation type:
//    Static, protected utility.
//
UInt_t
CGatePackage::AssignId()
{
  // Returns a unique gate identifier.
  
  return CGateFactory::AssignId();
}
