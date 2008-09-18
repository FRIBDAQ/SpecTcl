/*
    This software is Copyright by the Board of Trustees of Michigan
    State University (c) Copyright 2008

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
//  CRunControlPackage.cpp
// This class packages run control objects.  The
//  class has a further purpose of isolating the
//  TCl/TK command processors from the actual
//  dirty work of knowning how to start and stop runs
//  and how to interrogate the run state.
//
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
#include "RunControlPackage.h"                               
#include "StartRun.h"
#include "StopRun.h"
#include "Globals.h"
#include "TCLInterpreter.h"
#include "TCLVariable.h"
#include <string>



#ifdef HAVE_STD_NAMESPACE
using namespace std;
#endif



// Local static Storage:
//



// Functions for class CRunControlPackage

//////////////////////////////////////////////////////////////////////////
//
// Function:
//    CRunControlPackage(CTCLInterpreter* pInterp)
// Operation Type:
//    Constructor
//
CRunControlPackage::CRunControlPackage(CTCLInterpreter* pInterp) :
  CTCLCommandPackage(pInterp,
		     std::string(Copyright)),
  m_pStartRun(new CStartRun(pInterp, *this)),
  m_pStopRun(new CStopRun(pInterp, *this)),
  m_pRunState(new CTCLVariable(pInterp,
			       std::string("RunState"), kfFALSE))
{
  // Add commands to the package table:

  AddProcessor(m_pStartRun);
  AddProcessor(m_pStopRun);

  // Initialize the state:
  //

  InitializeRunState();

}

///////////////////////////////////////////////////////////////////////
//
// Function:
//   ~CRunControlPackage()
// Operation Type:
//    Destructor.
//
CRunControlPackage::~CRunControlPackage()
{
  delete m_pStartRun;
  delete m_pStopRun;
  delete m_pRunState;
  
}  

//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    void InitializeRunState (  )
//  Operation Type:
//     Conststency
//
void 
CRunControlPackage::InitializeRunState() 
{
// Locates the current Run control object and
// forces the value of m_RunState to match
// the run state.  in the run control object.  Note
// that if there is no current run control object,
// the run state is set to FALSE since there can be no
// active run without a run control object.
//
// Implicit Inputs:
//    gpRunControl   - Global pointer to current run control
//                              object (kpNULL If no current run
//                              control object).

  if(!gpRunControl) {		// There isn't a run control object...
    m_pRunState->Set("0", TCL_GLOBAL_ONLY |TCL_LEAVE_ERR_MSG);
    return;
  }
  m_pRunState->Set(gpRunControl->getRunning() ? "1" : "0",
		   TCL_GLOBAL_ONLY | TCL_LEAVE_ERR_MSG);

}
//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    Bool_t StartRun (  )
//  Operation Type:
//     Isolator:
//
Bool_t 
CRunControlPackage::StartRun() 
{
// Interacts with the run control object to 
//  start a run.  If no run control object
// exists, then the run is not started.
// If the run is already active, no change in
// state is made.
//
// Returns:
//     kfTRUE    - Run was started or remained started.
//     kfFALSE   - Run was not started because there
//                        is no run control object which can
//                        start the run.
//

  if(!gpRunControl) {
    InitializeRunState();
    return kfFALSE;
  }
  if(!gpRunControl->getRunning()) {
    gpRunControl->Start();
  }
  InitializeRunState();
  return kfTRUE;
  

}
//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    Bool_t StopRun (  )
//  Operation Type:
//     Isolator.
//
Bool_t 
CRunControlPackage::StopRun() 
{
// Interacts with the run control object to 
//  stop a run.  If no run control object
// exists, then the run is marked stopped since
// there can be no active run without a run control
// object.
// If the run is already halted, no change in
// state is made.
//
// Returns:
//     kfTRUE    - Run was stopped or remained halted
//     kfFALSE   - Run was stopped but  there
//                        is no run control object.
//
// Exceptions:  

  if(!gpRunControl) {
    InitializeRunState();
    return kfFALSE;
  }
  if(gpRunControl->getRunning()) {
    gpRunControl->Stop();
  }
  InitializeRunState();
  return kfTRUE;

}
//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    Bool_t isRunning (  )
//  Operation Type:
//     Selector
//
Bool_t 
CRunControlPackage::isRunning() 
{
// Interrogates m_RunState returninng the
// current run state as a boolean.
//

  InitializeRunState();		// Update the run state...
  const char* pState = m_pRunState->Get(TCL_LEAVE_ERR_MSG |
                                        TCL_GLOBAL_ONLY);
  return ((*pState == '1') ? kfTRUE : kfFALSE);

}
