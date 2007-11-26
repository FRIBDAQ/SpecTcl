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
#include "CRWWrite.h"
#include <TCLInterpreter.h>
#include <SpecTcl.h>
#include <Spectrum.h>
#include <string>
#include <string.h>

#include "util.h"

#ifdef HAVE_STD_NAMESPACE
using namespace std;
#endif

/*!
   Instantiate us... our command will just be rwwrite.
*/
CRWWrite::CRWWrite(CTCLInterpreter& interp) :
  CTCLObjectProcessor(interp, string("RDWwrite"))
{}

/*!
  The destructor just establishes the chain to the base class.
*/
CRWWrite::~CRWWrite()
{}

/*!
  processes the command:

\verbatim
   rwwrite spectrumname filename

\endverbatim
    Writes a spectrum to file in radware .spe format.
    \param interp : CTCLInterpreter& 
       Reference to the interpreter running this command.
    \param objv   : std::vector<CTCLObject>& objv
       Reference to a vector of Tcl objects that are our parameters.
       objv[0] is our command name.

    \return int
    \retval TCL_OK spectrum written to file.
    \retval TCL_ERROR spectrum could not be written to file, reason in result.


*/
int
CRWWrite::operator()(CTCLInterpreter& interp,
		     STD(vector)<CTCLObject>& objv)
{
  // Must have 2 parameters (vector size 3).

  if (objv.size() != 3) {
    string   sresult;

    sresult            = "Usage:\n";
    sresult           += "   rwwrite spectrum file\n";
    sresult           += "Where:\n";
    sresult           += "   spectrum   - is the name of a SpecTcl spectrum.\n";
    sresult           += "   file       - is a filename, extension defaults to .spe\n";
    interp.setResult(sresult);

    return TCL_ERROR;
  }

  // The parameters are spectrum name and filename:
  
  string spectrumName = objv[1];
  string filename     = objv[2];
  
  // Locate the spectrum and be sure that this is one we can write.
  
  SpecTcl&   api(*(SpecTcl::getInstance()));
  CSpectrum* pSpectrum = api.FindSpectrum(spectrumName);
  if (!pSpectrum) {
    string sresult = "Spectrum ";
    sresult       += spectrumName;
    sresult       += " does not exist";
    interp.setResult(sresult);


    return TCL_ERROR;
  }
  if (pSpectrum->Dimensionality() != 1) {
    string sresult = "Spectrum ";
    sresult       += spectrumName;
    sresult       += " must be a 1-d spectrum and is not";
    interp.setResult(sresult);

    return TCL_ERROR;
  }

  // We need to turn the spectrum into an array of floats for wspec to work with.

  Size_t   nchan     = pSpectrum->Dimension(0);
  Float_t*  channels  = new Float_t[nchan];
  for (UInt_t i =0; i < nchan; i++) {
    channels[i] = static_cast<Float_t>((*pSpectrum)[&i]);
  }
  // Need to copy the filename since wspec expects char*.

  char filnamcz[80];		// What radware seems to expect.
  memset(filnamcz, 0, sizeof(filnamcz));
  strncpy(filnamcz, filename.c_str(), 79);

  int status = wspec(filnamcz, channels, nchan);
  delete []channels;

  if (status != 0) {
    string result = "Radware's wspec failed to write ";
    result       += filnamcz;
    result       += " check for output on SpecTcl's terminal window";
    interp.setResult(result);

    return TCL_ERROR;
  }

  return TCL_OK;
}
