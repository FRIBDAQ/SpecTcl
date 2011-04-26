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
#include "CRWRead.h"

#include <TCLInterpreter.h>
#include <SpecTcl.h>
#include <Spectrum.h>

#include "SpectrumFactory.h"
#include <SnapshotSpectrum.h>

#include <string>
#include <string.h>

#include "util.h"

#ifdef HAVE_STD_NAMESPACE
using namespace std;
#endif



static const int defaultMaxChans=8*1024;

// Local static functions:

static string
Usage()
{
  string sresult = "Usage:\n";
  sresult       += "    rwread ?-maxchans n? filename\n";
  sresult       += "Where:\n";
  sresult       += "    filename is the name of the file to read.\n";
  sresult       += "    -maxchans if supplied is followed by an integer\n";
  sresult       += "              that is the largest number of channels\n";
  sresult       += "              to read.  This defaults to 8192";
  return sresult;
}

/*!
   Instantiate us... our command will always be rwread.
*/
CRWRead::CRWRead(CTCLInterpreter& interp) :
  CTCLObjectProcessor(interp, string("rwread"))
{}

/*!
   The destructor just establishes the chain to the base class.
*/
CRWRead::~CRWRead()
{}

/*!
   Process the command:

\verbatim
   rwread ?-maxchans n? filename
\endverbatim

   This command reads a spectrum in radware .spe format from file
into a SpecTcl 1d spectrum encapsulated in a snapshot spectrum.
- -maxchans if present is the maximum number of channels the spectrum can
   occupy.
- filename is the name of the file to read.  If necessary, a .spe is appended
  to give the filename an extension.
*/
int
CRWRead::operator()(CTCLInterpreter& interp,
		     vector<CTCLObject>& objv)
{
  // Need exactly 1  or exactly 3 arguments:
  // (but add 1 for the command keyword).

  if ((objv.size() != 2) && (objv.size() != 4)) {
    string sresult = Usage();
    interp.setResult(sresult);
    return TCL_ERROR;

  }
  // Process the parameters:
  // Filename is always last.
  
  string filename;
  int    maxchans = defaultMaxChans;

  if (objv.size() == 2) {
    filename = (string)(objv[1]);
  }
  else {
    filename = (string)(objv[3]);
    string maxsw = (string)(objv[1]);
    if (maxsw != "-maxchans") {
      string sresult = Usage();
      interp.setResult(sresult);
      return TCL_ERROR;
    }
    try {
      objv[2].Bind(interp);	// Need this for conversiont ->int.
      maxchans = objv[2];	// Fails if not integer.
      if (maxchans < 0) throw "this"; // Force the catch block.
    }
    catch (...) {
      string sresult = "Invalid maxchans parameter: ";
      sresult       += (string)(objv[2]);
      sresult       += "\n";
      sresult       += Usage();
      interp.setResult(sresult);

      return TCL_ERROR;
    }
  }
  // At this point, filename has the name of the file to read.
  // maxchans the maximum no. of channels.  We allocate the array of floats
  // radware uses to hold the spectrum and read it in.

  Float_t* chans = new Float_t[maxchans];
  char     filenamecz[80];
  char     specname[9];
  int      actualSize;
  memset(filenamecz, 0, sizeof(filenamecz));
  memset(specname, 0, sizeof(specname));
  strncpy(filenamecz, filename.c_str(), 79);

  int status = read_spe_file(filenamecz, chans, specname, &actualSize, maxchans);

  if (status != 0) {
    delete []chans;
    string sresult = "read_spe_file failed, see terminal output for more";
    interp.setResult(sresult);

    return TCL_ERROR;

  }

  // Now figure out what the spectrum name will be, and create the
  // spectrum 

  SpecTcl& api(*(SpecTcl::getInstance()));
  int    uniquifier  = 1;
  string spectclName = specname;
  spectclName.erase(spectclName.find_last_not_of(" \n\r\t") + 1);
  while (api.FindSpectrum(spectclName)) {
    char spectclNamecz[100];
    sprintf(spectclNamecz, "%s_%d", spectclName.c_str(), uniquifier);
    spectclName = spectclNamecz;
    uniquifier++;
  }


  vector<UInt_t> dims;
  vector<string> params;


  dims.push_back(actualSize);
  params.push_back("-deleted-");

  CSpectrumFactory fact;
  fact.ExceptionMode(kfFALSE);
  CSpectrum* pSpectrum = fact.CreateSpectrum(spectclName, ke1D, keLong,
					     params, dims);
  
  fact.ExceptionMode(kfTRUE);

  // Load the spectrum with the channel values.

  pSpectrum->Clear();
  for (UInt_t i = 0; i < actualSize; i++) {
    pSpectrum->set(&i, (UInt_t)chans[i]);
  }

  // Wrap in a snapshot spectrum...and add it to the histogrammer.

  pSpectrum = new CSnapshotSpectrum(*pSpectrum, kfTRUE);
  api.AddSpectrum(*pSpectrum);

  // Deallocate dynamic memory and return.

  delete []chans;

  interp.setResult(spectclName);
  
  return TCL_OK;
}
