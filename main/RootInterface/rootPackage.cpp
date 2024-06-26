/*
    This software is Copyright by the Board of Trustees of Michigan
    State University (c) Copyright 2017.

    You may use this software under the terms of the GNU public license
    (GPL).  The terms of this license are described at:

     http://www.gnu.org/licenses/gpl.txt

     Authors:
             Ron Fox
             Jeromy Tompkins 
	     NSCL
	     Michigan State University
	     East Lansing, MI 48824-1321
*/

/** @file:  rootPackage.cpp
 *  @brief: provides package initialization code for root interface package.
 */

/**
 *  A bit about what's going on here.  Bits and pieces of SpecTcl (histograms)
 *  are now cast in terms of root objects.  SpecTcl's initialization and
 *  shutdown are Root aware and furthermore the Tcl event loop time slices
 *  with the root event loop.
 *
 *  SpecTcl, however also provides some Tcl commands that further integrate
 *  SpecTcl and Root (root exec and roottree for example).
 *  These are extracted into a dynamically loadable root package.
 *
 *   This code is the initialization code for that package.
 */

#include <config.h>
#include <tcl.h>
#include <SpecTcl.h>
#include <TCLInterpreter.h>
#include <TCLObject.h>
#include <TCLVariable.h>
#include "TreeCommand.h"
#include "CRootExec.h"
#include "CRootExitCommand.h"
#include <TCLTimer.h>
#include <TApplication.h>
#include <TRint.h>
#include <TSystem.h>
#include <iostream>
#include <TclPump.h>
#include <MPITclCommand.h>
#include <Globals.h>
#include "RootEventLoop.h"
#include <iostream>
#include <sstream>




extern int SpecTclArgc;
extern char** SpecTclArgv;

// Local classes:

/**
 * CRootEventLoop
 *    Is a timer class that executes whenever the Tcl event loop is idle,
 *    asking Root to run any pending events it has.
 */

CRootEventLoop::CRootEventLoop(CTCLInterpreter* pInterp) :
  CTCLTimer(pInterp, 100), m_exit(false) {
    Set();
  }
CRootEventLoop::~CRootEventLoop() {}
  
void 
CRootEventLoop::operator()() {
    extern TSystem* gSystem;
    if (m_exit) return;               // Also don't reschedule if asked to exit.
    if (!gSystem) return;             // Getting killed off.
    gSystem->ProcessEvents();         // Process root events.
    Set();                            // Reschedule
}
void 
CRootEventLoop::stop() {
  m_exit = true;
}


CRootEventLoop* SpecTclRootEventLoop(0);

static const char* version("1.0");


 /**
  * loadInOtherRanks
  *   This is called by the root process in the MPI environment.
  *   It loads the Root interface library and package in all of the other
  *   ranks.
  */
static void loadInOtherRanks(CTCLInterpreter* pInterp) {
#ifdef WITH_MPI
      std::cerr << "Pushing root interface to the other ranks\n";
    // load $SpecTclHome/lib/libRootInterface.so:

    // Find SpecTclHome:

    CTCLVariable home(pInterp, "SpecTclHome", false);
    const char* pHome = home.Get();
    if (!pHome) {
      std::cerr << "Could not translate SpecTclHome variable\n";
      return;
    } else {
      std::cerr << "SpecTclHome is " << pHome << std::endl;
    }

    {
      std::stringstream s;
      s << pHome << "/lib/libRootInterface.so";
      std::string rootlib(s.str());
      std::vector<CTCLObject> words;
      CTCLObject loadCommand; loadCommand.Bind(pInterp);
      loadCommand = "load";
      words.push_back(loadCommand);
      CTCLObject filename; filename.Bind(pInterp);
      filename = rootlib;
      words.push_back(filename);

      ExecCommand(*pInterp, words);

      std::cerr << "Sprayed the root library load command ok\n";
    }
    {
      // The packasge require:

      CTCLObject package; package.Bind(pInterp); package = "package";

      CTCLObject require; require.Bind(pInterp); require="require";

      CTCLObject pkgName; pkgName.Bind(pInterp); pkgName = "rootinterface";
      
      std::vector<CTCLObject> words = {package, require, pkgName};

      ExecCommand(*pInterp, words);

      std::cerr << "Sprayed package require for root.\n";
    }
#endif
}

extern "C" {
    int Rootinterface_Init(Tcl_Interp* interp)
    {
        
        Tcl_PkgProvide(interp, "rootinterface", version);
        
        SpecTcl* api = SpecTcl::getInstance();
        CTCLInterpreter* pInterp = api->getInterpreter();
        
        // Set up the Root event loop to be multiplexed with Tcl's event
        // loop off the timer:
        
  
        gApplication = new TRint("SpecTcl", &SpecTclArgc, SpecTclArgv );

        // In MPI SpecTcl, we run the event loop in the event sink pipeline.

        if (!isMpiApp() || (myRank() == MPI_EVENT_SINK_RANK)) {
          SpecTclRootEventLoop = new CRootEventLoop(pInterp);
        }
        
        // Set up root interface commands:
        
        // Use the standard exit command everywhere but rank 2 if mpi
        // This deserves explanation:
        // exit will get done in the MPI_ROOT_RANK.  As it exits,
        // the MPI exit handler will be called and it will send an exit command
        // to the command pumps but not expect a response (wrapping this in a
        // MPITclCOmmand will result in a hang on the responses).
        // the event sink is where all of the root stuff runs and so that's the only 
        // place we need a special exit command.
        //
        if (!isMpiApp() || (myRank() == MPI_EVENT_SINK_RANK)) {
          new CRootExitCommand(*pInterp, "exit"); 
        }
        // new TreeCommand(*pInterp, "roottree");
        new CMPITclCommand(*pInterp, "rootexec", new CRootExec(*pInterp, "rootexec"));
        
        std::cerr << " SpecTcl Root interface loaded: \n";
        std::cerr << "    - Root is a product of CERN (http://root.cern.ch)\n";

        // Ok here's the cool thing in order to work, we need to 
        // require the package in all the other ranks since SpecTclRC only gets
        // run in the root rank and that's where we recommend including it.

        if (isMpiApp() && (myRank() == MPI_ROOT_RANK)) {
          loadInOtherRanks(pInterp);
        }

        return TCL_OK;
    }
}
