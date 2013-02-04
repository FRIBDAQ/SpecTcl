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
#include "CRateEventProcessor.h"
#include <Event.h>
#include <SpecTcl.h>
#include <TCLInterpreter.h>
#include <TCLVariable.h>
#include <iostream>
#include <stdlib.h>

using namespace std;

/*  These are all trivial methods:
 */

/*!
  The constructor must initialize the tree parameter for the calibrated time.
  for now we initialize it so that it's called 'cTime', has a preferred range of about
  an hour and has units of Milliseconds.  We also initialize the timestamp
  parameter so that we can get it's value for our computation.

*/
CRateEventProcessor::CRateEventProcessor() :
  m_milliseconds(string("cTime"), 3600000, (double)0.0, (double)3600000.0, 
		 string("Ms"), true),
  m_timestamp(string("timestamp"), 10000000, (double)0.0, (double)10000000.0, 
	      string("Ticks"), true),
   m_timeBase(0),
   m_needTimeBase(true) 
{

}
//! The destructor just needs to chain to the base class destructor by being virtual.
CRateEventProcessor::~CRateEventProcessor() {}


/*!
  At the begin of each run we need to set m_calibrationFactor
  to the value of the timeCalibration global Tcl variable.
  If this variable is not defined, we'll use the value 1.0, defaulting
  to an uncalibrated time...we'll also emit a warning to cerr and
  to whatever the tcl stderr channel points to at the time (tkcon?).
*/
Bool_t
CRateEventProcessor::OnBegin(CAnalyzer& rAnalyzer,
			     CBufferDecoder& rDecoder)
{
  SpecTcl*         api      = SpecTcl::getInstance();
  CTCLInterpreter* pInterp  = api->getInterpreter();

  CTCLVariable     calibration(pInterp, string("timeCalibration"), false);
  const char*      pValue   = calibration.Get();

  // If the variable has not been defined, use the value 1.0:
  // warn:

  if (!pValue) {
    m_calibrationFactor = 1.0;
    cerr << "Warning: no global variable timeCalibration defined, using 1.0\n";
    pInterp->GlobalEval(
	     "puts stderr {Warning: no global variable timeCalibration defined, using 1.0}\n");

  }
  else {
    // Convert the string to a double.. If it does not convert, then again
    // default to 1.0 

    char*  endptr;
    double calibration = strtod(pValue, &endptr);
    
    if (endptr == pValue) {

      // conversion failed.

      cerr << "Warning: timeCalibration value was not a valid floating number using 1.0\n";
      pInterp->GlobalEval(
	     "puts stderr {Warning: timeCalibration value was not a float, using 1.0}\n");

    }
    else {
      m_calibrationFactor = calibration;
    }
 
  }
  m_needTimeBase = true;
  return kfTRUE;
}


/*!
   Compute the parameters:
*/
Bool_t
CRateEventProcessor:: operator()(const Address_t pEvent,
				 CEvent&         rEvent,
				 CAnalyzer&      rAnalyzer,
				 CBufferDecoder& rDecoder)
{
  if (m_needTimeBase) {
    m_timeBase = m_timestamp;
    m_needTimeBase = false;
  }


  m_timestamp = m_timestamp - m_timeBase;
  m_milliseconds = (m_timestamp)/m_calibrationFactor;

  return kfTRUE;
}
