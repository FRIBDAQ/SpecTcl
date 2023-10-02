/*
    This software is Copyright by the Board of Trustees of Michigan
    State University (c) Copyright 2017.

    You may use this software under the terms of the GNU public license
    (GPL).  The terms of this license are described at:

     http://www.gnu.org/licenses/gpl.txt

     Authors:
             Ron Fox
             Giordano Cerriza
	     NSCL
	     Michigan State University
	     East Lansing, MI 48824-1321
*/

/** @file:  TclXamine.cpp
 *
 *  @brief: Implement the TclXamine.cpp class.  See header for more.
 */
#include "TclXamine.h"
#include <TCLInterpreter.h>
#include <TCLObject.h>
#include <Exception.h>
#include <ErrnoException.h>
#include <string>
#include <client.h>
#include <stdexcept>
#include <iostream>
/**
 * TclXamine constructor
 *
 * @param interp - interpreter on which the command is being registered.
 */
TclXamine::TclXamine(CTCLInterpreter& interp) :
  CTCLObjectProcessor(interp, "Xamine::Xamine", TCLPLUS::kfTRUE)
{}

/**
 * TclXamine destructor.
 */
TclXamine::~TclXamine()
{}

/**
 * operator()
 *    -  Ensure there's at least a subcommand then dispatch to the appropriate
 *       handler.
 *    - ALl errors are encapsulated in the try/catch block.
 *  @param interp - interpreter running the command.
 *  @param objv   - Command words.
 */
int
TclXamine::operator()(CTCLInterpreter& interp, std::vector<CTCLObject>& objv)
{
    bindAll(interp, objv);
    try {
        requireAtLeast(objv, 2, "Requires at least a subcommand");
        std::string sub = objv[1];
        if (sub == "genenv") {
            genenv(interp, objv);
        } else if (sub == "checkmem") {
          checkmem(interp, objv);
        } else if (sub == "start") {
                start(interp, objv);
        } else {
            std::string msg = "Unrecognized subcommand: " ;
            msg += sub;
            throw std::string(msg);
        }
    }
    catch (std::string msg) {
        interp.setResult(msg);
        return TCL_ERROR;
    }
    catch (CException& e) {
        interp.setResult(e.ReasonText());
        return TCL_ERROR;
    }
    catch (std::exception& e) {
        interp.setResult(e.what());
        return TCL_ERROR;
    }
    catch (...) {
        interp.setResult("Unexpected exception type");
        return TCL_ERROR;
    }
    return TCL_OK;
}
///////////////////////////////////////////////////////////////////////////////
// Subcommand execution methods.  These all report errors via exceptions.

/**
 * genenv
 *   Sets up the Xamine environment variables.  Parameters are the shared memory
 *   key and size.  Note that on failure, Xamine_genenv is assumed to leave
 *   an errno with the proper values.
 *  @param interp - interpreter running the command.
 *  @param objv   - Command words.
 */
void
TclXamine::genenv(CTCLInterpreter& interp, std::vector<CTCLObject>& objv)
{
    requireExactly(objv, 4, "Xamine::Xamine genenv shmemname spectrumbytes");
    std::string name = objv[2];
    int         size = objv[3];
    
    if (!Xamine_genenv(name.c_str(), size)) {
        throw CErrnoException("Unable to set up Xamine environment variables");
    }
}
/**
 * checkmem
 *    See if we can actually map to a specific Xamine memory.
 *    We could get the info about al ocal memory but still not be able
 *    to map if SpecTcl is running in a persistent container.
 * @param interp - interpreter running the command.
 * @param objv  - command words.
 *
 *  @note beginning with Rustogramer, the key can take several forms:
 */
void
TclXamine::checkmem(CTCLInterpreter& interp, std::vector<CTCLObject>& objv)
{
  requireExactly(objv, 4, "Xamine::Xamine checkmem key bytes");
  std::string name = objv[2];
  int         size = objv[3];

  std::cerr << "Trying to map " << name << std::endl;
  volatile Xamine_shared* p;
  int status = Xamine_MapMemory(const_cast<char*>(name.c_str()), size, &p);
  std::cerr << "Back with status: " << status << std::endl;
  
  // Success is status true and p != -1 as a pointer.
  
  volatile Xamine_shared* badptr =
    const_cast<volatile Xamine_shared*>(reinterpret_cast<Xamine_shared*>(-1));
  bool result;
  if (status && (p != badptr) ) {
    std::cerr << "returning true\n";
    result = true;
  } else {
    std::cerr << "Returning false\n";
    result = false;
  }
  CTCLObject r;
  r.Bind(interp);
  r = result;
  interp.setResult(r);
}
/**
 * start
 *    Start Xamine - no more parameters.
 *  @param interp - interpreter running the command.
 *  @param objv   - Command words.
 */
void
TclXamine::start(CTCLInterpreter& interp, std::vector<CTCLObject>& objv)
{
    requireExactly(objv, 2, "Xamkine::Xamine start");
    if (!Xamine_Start()) {
        throw CErrnoException("Unable to start Xamine");
    }
}
