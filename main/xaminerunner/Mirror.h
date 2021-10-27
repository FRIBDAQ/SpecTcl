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

/** @file:  Mirror.h
 *  @brief: Set up mirror with SpecTcl using Tcl command and the Mirror API.
 */
#ifndef MIRROR_H
#define MIRROR_H

#include <TCLObjectProcessor.h>

/**
 * @class CTCLMirrorSetup
 *     Bridge between Tcl for the mirror API.
 *     Sets up a mirror for us, if necessary and
 *     returns a pointer to the mapped memory.
 *
 *  Syntax:  Xamine::startMirrorIfNeeded host rest mirror user
 *    * host - the host running SpecTcl.
 *    * rest - The REST service or port.
 *    * mirror - The MIRROR servic eor port.
 *    * user  - If either rest or mirror are NSCLDAQ advertised service names,
 *             the username that advertised those services.
 */
class CTCLMirrorSetup : public CTCLObjectProcessor
{
public:
    CTCLMirrorSetup(CTCLInterpreter& interp);
    virtual ~CTCLMirrorSetup();
    
    virtual int operator()(CTCLInterpreter& interp, std::vector<CTCLObject>& objv);
};

#endif