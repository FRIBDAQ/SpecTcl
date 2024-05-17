/*
    This software is Copyright by the Board of Trustees of Michigan
    State University (c) Copyright 2014.

    You may use this software under the terms of the GNU public license
    (GPL).  The terms of this license are described at:

     http://www.gnu.org/licenses/gpl.txt

     Authors:
             Ron Fox
             Giordano Cerizza
             Simon Giraud
             Aaron Chester
             Jin Hee Chang
	     FRIB
	     Michigan State University
	     East Lansing, MI 48824-1321
*/
#ifndef TCLPUMP_H
#define TCLPUMP_H
#include <vector>
#include <tcl.h>

class CTCLInterpreter;
class CTCLObject;
/**
 * This header contains definitions to support Tcl command pumping in MPI SpecTcl
 * Tcl command pumping refers to the fact that MPISpecTcl consists of a master
 * process (rank 0) that accepts Tcl commands and then re-broadcasts them to all 
 * of the slave processes (rank non-zero).  Those slaves make of that command what they will
 * and send back replies which provide the status of the command and any result string.
 * 
 * This is accomplished by providing a command forwarder and a thread that 
 * receives commands and queues them as events to the Tcl event loop.  This requires
 * that slave processes run an event loop.  The Event loop then executes the command
 * and sends the status and return value back to the master.
 * 
 * When not built with MPI, the broadcaster just executes the command in the local
 * interpreter, and the function to start the event loop is a no-op.
*/
extern void* TclNotifier;
/**
 *   Execute a Tcl command MPI or non MPI environment.  The result is set appropriately either wai.
*/
int ExecCommand(CTCLInterpreter& rInterp, std::vector<CTCLObject>& words);

/**
 * start the command pump.
*/
void startCommandPump(CTCLInterpreter& rInterp);          // Start pumping commands via the event loop.
void stopCommandPump();

/**
 * Am I bult with MPI and running under MPI:
*/
bool isMpiApp();
int  myRank();

#endif