///////////////////////////////////////////////////////////
//  CProjectionCommand.cpp
//  Implementation of the Class CProjectionCommand
//  Created on:      20-Jun-2005 10:10:55 AM
//  Original author: Ron Fox
///////////////////////////////////////////////////////////

#include <config.h>


#include "CProjectionCommand.h"
#include <TCLInterpreter.h>
#include <TCLResult.h>
#include <GateContainer.h>
#include <Contour.h>
#include <CGammaContour.h>
#include <OrGate.h>

#include <Spectrum.h>
#include <Spectrum1DL.h>
#include <Gamma1DL.h>
#include <CSpectrum2Dm.h>
#include <SnapshotSpectrum.h>
#include <TrueGate.h>
#include <SpecTcl.h>
#include <histotypes.h>
#include <CAxis.h>
#include "CContourROI.h"
#include "CEverywhereROI.h"
#include <CGammaSpectrum.h>
#include <CFold.h>


#include <vector>
#include <set>
#include <string.h>

#define DESIGN_BY_CONTRACT
#define CHECK_ALL

#include <DesignByContract.h>

#ifdef HAVE_STD_NAMESPACE
using namespace std;
#endif

// Static storage:

static  CTrueGate        truegate;	            // The default projection gate:
static  CGateContainer   truecontainer("-Ungated-", 0, truegate);


/*!
   Construction registers the command.
*/
CProjectionCommand::CProjectionCommand(CTCLInterpreter& rInterp) :
  CTCLProcessor("project", &rInterp)
{
  Register();
}


/*!
   Unregistration/cleanup is done by the base class.
*/
CProjectionCommand::~CProjectionCommand()
{

}


/**
 * Main flow of control of the projection creator.
 * Called whenever the project command is  invoked. This provides the top level
 * control
 * - Whether or not the request is for a snapshot spectrum is determined.
 * - Source spectrum is validated to be a  2-d or a gamma2 spectrum and a
 *    CSpectrum* pointer to it gotten.
 * - Target spectrum is validated not to exist.
 * - The projection direction is obtained.
 * - The projection gate is determined (T if not supplied); the projection gate
 *   must be a 2-d (contour) gate, or a gamma gate.
 * - The target spectrum is created; the  axis specification used is the same as
 *   for the axis of the source spectrum onto which  the projection is being 
 *   made...this is required since that's the resolution at which we have the data.
 * - Either projectx or project y is called to do the actual projection of
 *   the spectrum.
 * - If the spectrum is a snapshot, it's put into a snapshot spectrum.
 * - If not a snapshot, a gate is produced that is the and of the gate on the
 *   source spectrum and the gate  within which the projection was done.  This 
 *   gate is applied to the target spectrum.  The name given to this gate is:
 *
 *       projection_gate_$spectrumgate_and_$projectiongate  <br>
 *   where spectrumgate is the gate on the spectrum and 
 *   $projectiongate is the gate within which the spectrum was proejected.
 *   Note that if either of these is just True,
 *   then the other gate is just applied
 *   to the target spectrum without all this fol-de-rol.  If, additionally, the
 *   source spectrum is a  gamma spectrum with a fold, this fold
 *   is applied to the target spectrum as well (1-dGamma).
 * - The spectrum is entered into the set of known spectra.
 *
 * @param CTCLInterpreter&    
 *        Reference to the interpreter that is running this command.
 * @param CTCLResult&
 *        The result string for the command.
 *        - On success this will be the name of the spectrum created.
 *        - On failure this will be a message descriptive of the error along 
 *          with a usage summary.
 * @param argc
 *       Number of command line words, after Tcl's round of substitution.
 *       Note that this includes the command keyword itself.
 * @param argv
 *       Pointers to the command line words.
 *
 * \return int
 * \retval TCL_ERROR  an error occured in parse or processing.
 * \retval TCL_OK     the projection was successfully accomplished. 
 */
int 
CProjectionCommand::operator()(CTCLInterpreter& rInterp,  CTCLResult& rResult, 
			       int argc, char** argv)
{
  // The first argument is the command name keyword which we don't care about
  // since we trust Tcl's dispatch:

  argc--;
  argv++;

  // For the isSnapshotRequest we must have a command line param.

  if (!argc) {
    rResult  = "Insufficient command line parameters\n";
    rResult += Usage();
    return TCL_ERROR;
  }
  // The first argument could be the -snapshot keyword. If so, the
  // function we call below bumps pointers appropriately.

  bool snapshot = isSnapshotRequest(argc, argv);

  // We must have at least a source spectrum  a target spectrum and a direction of
  // projection..remaining even after the snapshot test then we can extract those:

  if (argc < 3) {
    rResult  = "Insufficient command line parameters\n";
    rResult += Usage();
    return TCL_ERROR;
  }
  const char* pSourceName = argv[0];
  const char* pDestName   = argv[1];
  const char* pDirection  = argv[2];

  // Create the source spectrum, evaluate the direction and get the dest. spectrum
  // too.

  CSpectrum* pSource;
  CSpectrum* pDest;
  direction  Direction;
  try {
    pSource   = getValidatedSourceSpectrum(pSourceName);
    Direction = getValidatedDirection(pDirection);
    pDest     = getValidatedTargetSpectrum(pDestName, pSource, Direction);
  }
  catch (string msg) {
    rResult  = msg;
    rResult += "\n";
    rResult += Usage();
    return TCL_ERROR;
  }
  argc -= 3;			// We have processed 3 parameters.
  argv += 3;
  
  // There can be at most one more parameter, the name of the gate.
  // If it is missing, we will use a True gate for the projection.

  CGateContainer*  pContainer(&truecontainer);
  if (argc) {
    pContainer = getProjectionGate(*argv, pSource);
    argc--;
    argv++;
    if(!pContainer) {
      rResult   = "Invalid gate for projection\n";
      rResult  += Usage();
      return TCL_ERROR;
    }

  }
  // We've gotten everything we need, there had better not be any more parametres:

  if (argc) {
    rResult  = "Too many command line parameters\n";
    rResult += Usage();
    return TCL_ERROR;
  }
  // Now do the projection

  if (Direction == x) {
    projectX(pSource, pDest, pContainer);
  } 
  else {
    projectY(pSource, pDest, pContainer);
  }

  // If desired, wrap the spectrum in a snapshot:
  // otherwise set up the gate appropriately.
  //
  if (snapshot) {
    CSnapshotSpectrum* pSnap = new CSnapshotSpectrum(*pDest);
    pDest = pSnap;
  }
  else {
    GateTarget(pSource, pDest, pContainer);
    if (pSource->getSpectrumType() == keG2D) {
      FoldTarget(pSource, pDest);
    } 
  }

  // Enter the spectrum in spectcl:

  SpecTcl* pApi = SpecTcl::getInstance();
  pApi->AddSpectrum(*pDest);
  return TCL_OK;
  
}


/**
 * if *argv  is "-snapshot"
 * argv is incremented, argc decremented
 * and
 *   true is returned.
 * if *argv is "-nosnapshot"
 *  argv is incremented, argc is
 * decremented and false is returned.
 *
 * otherwise false is returned.
 * Note that the recognition of the -
 * nosnapshot string allows us to
 * explicitly state that nosnapshot is a
 * default that may change as we have
 * experience using the projection command.
 *
 *  @param argc
 *       reference to the number of command words left on the line at the
 *        time this is callled.
 *  @param argv 
 *       Reference to the pointer to the list of word pointers
 *
 * \return bool
 * \retval true   - The user wants to wrap the projection as a snapshot.
 * \retval false  - The user wants the projection to increment. 
 */
bool 
CProjectionCommand::isSnapshotRequest(int& argc, char**& argv)
{
  if (string("-snapshot") == string(*argv)) {
    argv++;
    argc--;
    return true;
  }
  else if(string("-nosnapshot") == string(*argv)) {
    argv++;
    argc--;
    return false;
  }
  else {			// Not snapshot switch.
    return false;
  }
}


/**
 * Returns a pointer to the source spectrum if it exists and is a valid type.
 * To be valid, the source spectrum must be either  a 2-d or a gamma-2d spectrum.
 *
 * At present, we don't project summary spectra.
 *
 *  @param name
 *      The command word that is supposed to contain the name of the
 *      source spectrum.
 * 
 * \return CSpectrum*
 * \retval NULL  - The spectrum either does not exist or is not of a valid type.
 * \retval other - Pointer to the source
 */
CSpectrum* 
CProjectionCommand::getValidatedSourceSpectrum(const char* name)
{
  SpecTcl*    pApi    = SpecTcl::getInstance();
  CSpectrum*  pSource = pApi->FindSpectrum(name);
  
  // Spectrum exists... see if it's the right type:

  if(pSource) {
    SpectrumType_t sType = pSource->getSpectrumType();
    if ( (sType != ke2D)  && (sType != keG2D) && (sType != ke2Dm)) {
      throw string("Invalid source spectrum type must be 2d or gamma 2d");
    }
  }
  else {
    throw string("Source spectrum not found");
  }

  return  pSource;

}


/**
 * If the string is [Xx] return x  if the string is [Yy] return y.  Otherwise:
 * throw string("Invalid direction word [word]");
 * where [word] above is the word that was provided for direction.
 *
 * @param pDirection
 *    A pointer to the command line word that shoulid be the direction.
 * 
 * \return direction
 * \retval x  - to project in x direction.
 * \retval y  - to project in y direction.
 *
 */
CProjectionCommand::direction 
CProjectionCommand::getValidatedDirection(const char* pDirection)
{

  if (strlen(pDirection) != 1) {
    throw string("Invalid direction must be x or y");
  }
  if ((*pDirection == 'x') || (*pDirection == 'X')) { 
    return x;
  }
  if ( (*pDirection == 'y')   || (*pDirection == 'Y')) {
    return y;
  }
  throw string("Invalid direction must be x or y");


}


/**
 * If the spectrum already exists, returns NULL indicating that the name is
 * invalid.
 *
 * If the spectrum does not exist, the axis of the source spectrum described
 * by 'direction'  is used to create a 1-d
 * spectrum (or 1-d gamma if the source spectrum is 2-d gamma) with the axis
 * specification identical to the 'direction' axis of the source spectrum.
 *
 *
 * The parameter specification for the target spectrum is:
 *
 * -  The direction parameter of the source spectrum if the source spectrum
 *    is 2-d 
 * -  All the parameters if the source spectrum is gamma-2d.
 *

 *
 * Note: The new spectrum is not entered into the SpecTcl spectrum list.   This
 * allows it to be generated and possibly wrapped into a Snapshot spectrum.
 *
 * @param name    Pointer to the command line word that should be containing the
 * name of the target spectrum.
 * @param pSource    Pointer to the source spectrum.
 * @param which    Projection direction (used to compute the axis characteristics
 * of the target spectrum).
 * 
 * \return CSpectrum*
 * \retval Pointer to the spectrum that  was created.
 * \retval NULL  if the spectrum already exists.
 *
 */
CSpectrum* 
CProjectionCommand::getValidatedTargetSpectrum(const char* name, 
					       CSpectrum* pSource, 
					       direction which)
{
  // Ensure the spectrum does not yet exist:

  SpecTcl*   api = SpecTcl::getInstance();
  CSpectrum* pDest(0);

  if (api->FindSpectrum(name)) {
    throw string("A spectrum with the target spectrum name already exists");
  }
  //  The source spectrum direction unconditionally determines the 
  //  Axis shape of the target spectrum:

  CSpectrum::Axes axisDescriptions = pSource->getAxisMaps();
  CAxis xAxis = axisDescriptions[0];
  CAxis yAxis = axisDescriptions[1];
  UInt_t    nChannels;
  Float_t   Low;
  Float_t   High;
  CAxis*    pMyAxis;

  if(which == x) {
    pMyAxis = &xAxis;
  }
  else {
    pMyAxis = &yAxis;
  }
  nChannels = pMyAxis->getChannels();
  Low       = pMyAxis->getLow();
  High      = pMyAxis->getHigh();

  // The type of spectrum to create depends on the source spectrum type:
  // We assume the source spectrum has been validated.
  //
  CSpectrum::SpectrumDefinition def = pSource->GetDefinition(); 
  if( def.eType == ke2D) { // regular 2d. -> 1d
    CParameter* pParameter = api->FindParameter( which == x ? 
						    def.vParameters[0]  :
						    def.vParameters[1]);
    REQUIRE(pParameter, "Parameter lookup failed");
    pDest = api->Create1D(string(name), keLong, 
			  *pParameter,
			  nChannels, Low, High);
    
  }
  else  {			            // gamma 2d, or m2-> g1d.
    
    // The parameters we use depend on the spectrum type.
    // for gamma spectra we use all parameters.  For m2 we use
    // every other starting with an index that depends on the projection
    // direction.
    
    int start = 0;            // These are right for gamma spectra.
    int step  = 1;
    if (def.eType = ke2Dm) {
        step = 2;             // Only every other parameter...
        start = which == x ? 0 : 1; // Start depends on projection direction.
    }
    vector<CParameter> parameters;
    for (int i = start; i < def.vParameters.size(); i += step) {
      CParameter* pParam = api->FindParameter(def.vParameters[i]);
      REQUIRE(pParam, "Parameter lookup failed");
      parameters.push_back(*pParam);
    }
    pDest = api->CreateG1D(string(name), keLong, 
			   parameters,
			   nChannels, Low, High);
  }
  if(!pDest) {
    throw string("Could not create target spectrum");
  }
  return pDest;

}


/**
 * If the gate does not exist, or if the gate exists and is neither a
 * contour nor a gamma contour, a null pointer is returned.
 *
 * If, as a contour, the gate is not on the parameters in the source spectrum
 * null is returned.  We require that the gate type match the source spectrum type..
 * that is gamma gates on gamma spectra and ordinary gates on ordinary spectra.
 * This may have some different meanings:
 *  - Ordinary contour on 2-d  spectrum parameters are the gate parameters.
 *  - Gamma contour on Gamma spectrum : All of the spectrum parameters are in
 *    the gamma contour.
 *
 * If all the above conditions got met, the pointer to the gate's container is
 * returned.
 *
 * @param pGateName
 *    Pointer to the name of the gate that is used to select the
 *    region of interest of the projection.
 * @param pSourceSpectrum
 *    Pointer to the source spectrum.
 *
 * \return CGateContainer*
 * \retval NULL  - The gate is incompatible or does not exist.
 * \retval other - The gate is compatible... a pointer to the gate container is
 *                 returned.
 * 
 */
CGateContainer* 
CProjectionCommand::getProjectionGate(const char* pGateName, 
				      CSpectrum* pSourceSpectrum)
{
  
  SpecTcl* api = SpecTcl::getInstance();
  CGateContainer* pGateContainer(0);

  // If the gate does not exist return null right away:

  pGateContainer = api->FindGate(pGateName);
  if(!pGateContainer) {
    return (CGateContainer*)kpNULL;
  }

  // We assume that the spectrum type has been validated already:

  if(pSourceSpectrum->getSpectrumType() == ke2D) {
    // Gate must be a contour:

    if ((*pGateContainer)->Type() == "c") {
      CContour* pGate = dynamic_cast<CContour*>(pGateContainer->getGate());
      if (pSourceSpectrum->UsesParameter(pGate->getxId())   && 
        pSourceSpectrum->UsesParameter(pGate->getyId())) {
        return pGateContainer;
      }
      else {
	return (CGateContainer*)kpNULL;	// wrong parameters.
      }
    }
    else {
      return (CGateContainer*)kpNULL; // 2d gates must be contours.
    }
  }  else if (pSourceSpectrum->getSpectrumType() == keG2D) {			// Gamma -2d.
    if ((*pGateContainer)->Type() == "gc") {
      CGammaContour*  pGate = dynamic_cast<CGammaContour*>(pGateContainer->getGate());
      CGammaSpectrum* pSrc  = (CGammaSpectrum*)pSourceSpectrum;
      vector<UInt_t> parameterIds;
      pSrc->GetParameterIds(parameterIds);
      for(int i =0; i < parameterIds.size(); i++) {
        if (!pGate->UsesParameter(parameterIds[i])) {
            return (CGateContainer*)kpNULL;
          }
      }
      return pGateContainer;
    } else {
      return (CGateContainer*)kpNULL; // Gamma contours only for gamma 2d.
    }
  } else if (pSourceSpectrum->getSpectrumType() == ke2Dm) {
    // 2dm spectra will have an OR gate whose constituents are all contours
    // with the same points.  Validate it and return the compound gate
    // The projection code will need to pull out one of the contours to do the
    // channel value copies in a bit.
    
    return isValid2DmGate(pSourceSpectrum, pGateContainer);
    
  } else {
		return (CGateContainer*)kpNULL;
	}
}


/**
 * For each column in the source spectrum
 * iterates over the rows.  For each
 * channel in the column that's in the
 * region of interest defined by the gate,
 * the values are summed into the
 * corresponding channel of the target
 * spectrum.
 
 * @param sourceSpectrum    Pointer to the description of the source spectrum.
 * @param targetSpectrum    Pointer to the targe spectrum
 * @param gate    Pointer to the gate container for the gate.
 * 
 */
void 
CProjectionCommand::projectX(CSpectrum* sourceSpectrum, 
			     CSpectrum* targetSpectrum, 
			     CGateContainer* gate)
{

  // It is possible the spectrum parameter are flipped relative to the
  // region of interest parameters.  parameterOrder will fill in an array
  // telling us if it's necessary to flip coordinates when checking inclusion
  // in the ROI.
  
  int orderArray [2];
  parameterOrder(orderArray, sourceSpectrum, gate);
  int nXparam = orderArray[0];
  int nYparam = orderArray[1];


  CROI* pRoi = selectROI(sourceSpectrum, gate);

  // Now we can project correctly:

  targetSpectrum->Clear();
  Size_t xSize = sourceSpectrum->Dimension(0) - 2;   // -2 because of root.
  Size_t ySize = sourceSpectrum->Dimension(1) - 2;

  for (UInt_t x = 0; x < xSize ; x++) {  
    ULong_t sum = 0;
    for (UInt_t y = 0; y < ySize; y++) {
      UInt_t indices[2];

      // Inside the gate:

      indices[0]    = x;
      indices[1]    = y;
      ULong_t value = (*sourceSpectrum)[indices];
      if ((*pRoi)(indices[nXparam], indices[nYparam])) {
	sum += value;
      }

    }
    targetSpectrum->set(&x, sum);
  }
  delete pRoi;

}


/**
 *  See projectX above:  This function projects in the y direction.
 * @param sourceSpectrum    The source spectrum.
 * @param targetSpectrum    The target spectrum.
 * @param gate    The region of interest.
 * 
 */
void 
CProjectionCommand::projectY(CSpectrum* sourceSpectrum, CSpectrum* targetSpectrum, 
CGateContainer* gate)
{
  // Figure out which index is which relative to the gate.

  int orderArray[2];
  parameterOrder(orderArray, sourceSpectrum, gate);
  int nXparam = orderArray[0];
  int nYparam = orderArray[1];

  CROI* pROI = selectROI(sourceSpectrum, gate);

  // Now do the projection 
  
  targetSpectrum->Clear();
  Size_t xSize = sourceSpectrum->Dimension(0) - 2;
  Size_t ySize = sourceSpectrum->Dimension(1) - 2;

  for (UInt_t y =0; y < ySize; y++) {
    ULong_t sum = 0;
    for (UInt_t x =0; x < xSize; x++) {
      UInt_t indices[2];
      indices[0] = x;
      indices[1] = y;
      ULong_t value = (*sourceSpectrum)[indices];
      if ((*pROI)(indices[nXparam], indices[nYparam])) {
	sum += value;
      }

    }
    targetSpectrum->set(&y, sum);
  }
  delete pROI;
}

/*!
 * Return a string that contains the usage of the projection command:
 *
 */
string
CProjectionCommand::Usage()
{
  string Result;
  Result  = "Usage: \n";
  Result += "   project ?-[no]snapshot? source target dir ?gate?\n";
  Result += "Where:\n";
  Result += "   -snapshot- Indicates that the target will not increment.\n";
  Result += "   -nosnapshot - (default) requests the target increment\n";
  Result += "   source   - is a source 2-d or gamma 2d spectrum\n";
  Result += "   target   - is the name of a new spectrum to create.\n";
  Result += "   dir      - is x or y, the projection direction\n";
  Result += "   gate     - is an optional contour or gamma contour that \n";
  Result += "              restricts the region of the projection";

  return Result;
}


/*!  
 * select the region of interest object type and create it.
 * - The axis specifications come from the source spectrum.
 * - If the gate type is a contour or gamma contour, a contour ROI
 *   is created, otherwise an Everywhere ROI.
 */
CROI*
CProjectionCommand::selectROI(CSpectrum*      pSource, 
			      CGateContainer* pGate)
{
  
	CSpectrum::Axes maps = pSource->getAxisMaps();
	
	// Ordinary contour type gates....
	
  if ( ((*pGate)->Type() == "c") || ((*pGate)->Type() == "gc")) {
    // Need to figure out the map order as the source spectrum
    // axes may be reversed relative to the gate:

    int order[2];
    parameterOrder(order, pSource, pGate);

    return new CContourROI(pGate, 
			   maps[order[0]], 
			   maps[order[1]]);
  } else if ((*pGate)->Type() == "+") {
		// Or gate of contours for m2:....
		
		int order[2];
		parameterOrder(order, pSource, pGate);     // figure out how to order params.
		CCompoundGate* pActualGate = reinterpret_cast<CCompoundGate*>(pGate->getGate());
		
		// Use the first gate to define the ROI
		
		CGateContainer* pFirstGate = pActualGate->GetConstituents().front();;
		
		return new CContourROI(pFirstGate, maps[order[0]], maps[order[1]]);
		
	} else {
		
		// No ROI (so true everywhere)
		
    return new CEverywhereROI(maps[0], maps[1]);
  }
  return (CROI*)NULL;			// This is actually a bug.
}

/**
 * Utility function to dope out the parameter indices that map 
 * spectrum parameters -> Gate parameters.  In the case of a contour
 * it may be necessary to flip x/y, while gamma spectra are symmetric
 * with respect to their gates and x=y.
 * @param orderArray
 *    Points to an array of 2 integers.  The 0'th indicates which axis is
 *   (0/1) is x in the gate.
 *   the 1'st incidates which is y.
 * @param pSource 
 *    pointer to the source spectrum.
 * @param pGate
 *    Pointer to the gate container.
 */
void
CProjectionCommand::parameterOrder(int* orderArray, CSpectrum* pSource, 
				   CGateContainer* pGate)
{
  orderArray[0] = 0;		// Assume no flip required.
  orderArray[1] = 1;

	// It's all straightforward if not a 2m spectrum:
	
	if (pSource->getSpectrumType() != ke2Dm) {	
		vector <UInt_t> params;
		pSource->GetParameterIds(params);
		if ((*pGate)->Type() == "c") {
			CContour* Gate = dynamic_cast<CContour*>(pGate->getGate());
			int gatex       = Gate->getxId();
			if(params[0] != gatex) {	// The order is reversed.
				orderArray[0] = 1;
				orderArray[1] = 0;
			}
		}
	} else {
		// For m2, we need to look at the first constituent of the gate, and
		// see if its parameters are x/y or y/x of some pair of parameters in the spectrum.
		// at this point we can assume the gate is valid for a m2
		
		std::vector<UInt_t> allparameters;
		std::set<UInt_t>    xparams;
		std::set<UInt_t>     yparams;
		CSpectrum2Dm* pSpec = reinterpret_cast<CSpectrum2Dm*>(pSource);
		pSpec->GetParameterIds(allparameters);
		for (int i =0; i < allparameters.size(); i += 2) {
			xparams.insert(allparameters[i]);
			yparams.insert(allparameters[i+1]);
		}
		CCompoundGate* pActualGate = reinterpret_cast<CCompoundGate*>(pGate->getGate());
		CGateContainer* pFirstGate = pActualGate->GetConstituents().front();
		CPointListGate *pActFirstGate =
			reinterpret_cast<CPointListGate*>(pFirstGate->getGate());
		
		// If the xid is in xparams we guessed the order right, otherwise,
		// reverse:
		
		if (xparams.count(pActFirstGate->getxId()) == 0) {
			orderArray[0] = 1;
			orderArray[1] = 0;             // flip.
		}
		
	}
}
/*!
   Figure out how to gate the target spectum.
*/
void
CProjectionCommand::GateTarget(CSpectrum*      pSource,
			       CSpectrum*      pTarget,
			       CGateContainer* pROI)
{
  CGateContainer* pSourceGate = (CGateContainer*)pSource->getGate();

  // If the region of interest is a T gate, just gate with
  // the source spectrum gate.
  //
  if ((*pROI)->Type() == "T") {
    pTarget->ApplyGate(pSourceGate);
  }
  else if ((*pSourceGate)->Type() == "T") { // Source gate T, use ROI.
    pTarget->ApplyGate(pROI);
  }
  else {	   		         // Must make and of source and ROI
    SpecTcl* pApi = SpecTcl::getInstance();
    vector<string> names;
    names.push_back(pROI->getName());
    names.push_back(pSourceGate->getName());
    CGate* pTargetGate = pApi->CreateAndGate(names);

    string gatename("__PROJECTIONGATE__");
    gatename += pROI->getName();
    gatename += "__AND__";
    gatename += pSourceGate->getName();
    gatename += "__";
    pApi->AddGate(gatename, pTargetGate);
    CGateContainer *pTargetContainer = pApi->FindGate(gatename);
    REQUIRE(pTargetContainer, "Entered gate but could not look it up later");
    pTarget->ApplyGate(pTargetContainer);
  }

}
/*!  Copy the fold from the source to the target spectrum.
 */
void
CProjectionCommand::FoldTarget(CSpectrum* pSource, CSpectrum* pTarget)
{
  CGammaSpectrum* pS = dynamic_cast<CGammaSpectrum*>(pSource);
  CGammaSpectrum* pD = dynamic_cast<CGammaSpectrum*>(pTarget);

  if(pS->haveFold()) {
    SpecTcl* pApi = SpecTcl::getInstance();

    CFold* pFold = pS->getFold();
    string foldGate = pFold->getFoldName();
    if (foldGate != string("")) { // There's a fold.
      CGateContainer* pFoldGate = pApi->FindGate(foldGate);
      REQUIRE(pFoldGate, "Null fold");
      REQUIRE(((*pFoldGate)->Type() == "gc") || ((*pFoldGate)->Type() == "gb") ||
              ((*pFoldGate)->Type() == "gs"), "Incorrect fold type");
      pD->Fold(pFoldGate);
    }
  }
}
/**
 * isValid2DmGate
 *    Determines if the gate in a gate container is a valid gate for
 *    projecting a 2dm spectrum.  The gate must be a contour that was drawn
 *    on the 2dm spectrum.  This means the gate must satisfy the following conditions:
 *    -  The gate must be an OR gate.
 *    -  The constituents must all be contours.
 *    -  The parameters of each contour must be a valid x and y parameter of the
 *       spectrum.
 *    -  All points on the contour must be  the same.
 *
 * @param[in] pSource - spectrum being projected.
 * @param[in] pGate - Gate container of the gate.
 * @return CGateContainer*
 *    @retval pGate - the gate is valid for projection.
 *    @retval nullptr - the gate is not valid for projection.
 */
CGateContainer*
CProjectionCommand::isValid2DmGate(CSpectrum* pSource, CGateContainer* pGate)
{
	CSpectrum2Dm* pSpectrum = reinterpret_cast<CSpectrum2Dm*>(pSource);
	if ((*pGate)->Type() == "+") {
			COrGate* pActualGate = reinterpret_cast<COrGate*>(pGate->getGate());
			std::vector<UInt_t> allparameters;
			std::set<UInt_t>    xparams;           // Only the x parameters.
			std::set<UInt_t>    yparams;           // Only the y parameters.
			
			pSpectrum->GetParameterIds(allparameters);
			for (int i = 0; i < allparameters.size(); i += 2) {
				xparams.insert(allparameters[i]);
				yparams.insert(allparameters[i+1]);
			}
			std::vector<FPoint> points;
			std::list<CGateContainer*>& gates(pActualGate->GetConstituents());
			for (auto p = gates.begin(); p != gates.end(); p++) {
				CGateContainer* pConstituent = *p;
				if ((*pConstituent)->Type() == "c") {
					CPointListGate* pCActual =
						reinterpret_cast<CPointListGate*>(pConstituent->getGate());
					if ( (xparams.count(pCActual->getxId()) + yparams.count(pCActual->getyId())) != 2) {
						break;                         // one or more parameters isn't right
					}
					std::vector<FPoint> gatePoints = pCActual->getPoints();
					if (points.empty()) {
						points = gatePoints;
					} else {
						// need to compre...
						if (gatePoints != points) break;   // not matching points.
					}
				} else {
					break;                             // Not a valid constituent type.
				}
			}
			return pGate;                          // All conditions met.
	}
	
	// If we got here we didn't satisfy one of the criteria above.
	
	return nullptr;
}