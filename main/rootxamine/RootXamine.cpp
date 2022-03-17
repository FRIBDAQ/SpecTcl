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

/** @file:  RootXamine.cpp
 *  @brief: Program entry point for root interface -> Xamine
 */

#include <TApplication.h>
#include <TRint.h>
#include "HistogramManager.h"
#include <SpecTclMirrorClient.h>
#include <MirrorClientInternals.h> 
#include <iostream>
#include <stdlib.h>
#include "cmdline.h"
#include <stdexcept>
/**
 * getPort
 *    Figure out a port number given a string that could be a service
 *    or just a number.
 *
 * @param  host - host that's advertising if it's a service.
 * @param  portString - name of service or ascii-ized port number.
 * @param  username - name of the user advertising the service.
 * @return int - integer port number or throw if can't translate a service.
 */
static short
getPort(const char* host, const char* portString, const char* username)
{
  short result;
  char* end;
  result = strtol(portString, &end, 0);            // Allows all bases.
  if (result > 0) {
    return result;
  }
  if (result < 0) {
    // User gave bad port number
    throw std::range_error("Port numbers must be > 0");
  }
  return LookupPort(host, portString, username);
  
}
/**
 * This program attaches to a specific SpecTcl shared memory,
 * either local or mirrored (starting the mirror if needed).
 * It then:
 * - Creates the root interpreter system.
 * - Starts up a histogram manager which maintains a set of
 *   THxxxx objects that have storage in the SpecTcl shared
 *   memory and
 * - Runs the root interpreter.
 *
 * @note Shared memory access is read only so any TH methods
 * that would modify the contents of shared memory will segfault.
 *
 * 
 */


int main(int argc, char** argv) {
    // Create the root interpreter which may be needed to support making
    // the initial set of histograms - but don't enter the event loop
    // just yet:
    
    gApplication = new TRint("XamineRoot", &argc, argv);
    
    // Parse the parameters that Root left us.
    
    gengetopt_args_info parsedArgs;
    cmdline_parser(argc, argv, &parsedArgs);    // Exit on failure.
    
    // The only optional possibly missing option is the username - needed only if
    // published services are used.
    
    const char* username(nullptr);
    if (parsedArgs.user_given) username = parsedArgs.user_arg;
    
    // Attempt the mirror:
    
    void* pMemory = getSpecTclMemory(
        parsedArgs.host_arg, parsedArgs.rest_arg, parsedArgs.mirror_arg,
        username
    );
    if (!pMemory) {
        std::cerr << "Failed to map to the SpecTcl display shared memory:\n";
        std::cerr << Mirror_errorString(Mirror_errorCode()) << std::endl;
        exit(EXIT_FAILURE);
    }
    
    
    // Set up the initial set of histograms and a repeating timer
    // to make REST requests of

    short port = getPort(parsedArgs.host_arg, parsedArgs.rest_arg, username);
    
    auto pManager = new HistogramManager(
        pMemory,
        parsedArgs.host_arg,
	port
    );
    pManager->start();
    
    gApplication->Run();
}
