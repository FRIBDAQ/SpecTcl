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

/** @file:  TclXamine.h
 *  @brief: Wrapper for stuff that directly calls the Xamine interface.
 */
#ifndef TCLXAMINE_H
#define TCLXAMINE_H

#include <TCLObjectProcessor.h>
class CTCLInterpreter;
class CTCLObject;

/**
 * @class TclXamine
 *         command ensemble Xamine::Xamine that provides the following subcommands:
 *         -  genenv  - Create the environment variables needed to run SpecTcl
 *         -  start   - Start Xamine
 *         -  stop    - Stop Xamine
 *         -  setGateHandler - Set a handler for gates from Xamine that runs in the
 *                            main thread.
 *         -  pollGate - Poll for a gate. - not needed if setGateHandler used.
 *         -  startGatePollThread
 *         -  enterGate - Add a new gate.
 *         -  removeGate - Remove a gate.
 *         -  enumerateGates - lists all gates on a spectrum.
 */
class TclXamine : public CTCLObjectProcessor
{
public:
    TclXamine(CTCLInterpreter& interp);
    virtual ~TclXamine();
    int operator()(CTCLInterpreter& interp, std::vector<CTCLObject>& objv);
    
    // Sub command executors.
private:
    void genenv(CTCLInterpreter& interp, std::vector<CTCLObject>& objv);
    void checkmem(CTCLInterpreter& interp, std::vector<CTCLObject>& objv);
    void start(CTCLInterpreter& interp, std::vector<CTCLObject>& objv);
    
    // Utility methods:
    
private:
    
    
};
#endif