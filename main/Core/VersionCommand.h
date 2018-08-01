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

/**
 * @file VersionCommand.h
 * @brief Define a command to return the SpecTcl version.
 * @author Ron Fox<fox@nscl.msu.edu>
 */
#ifndef VERSIONCOMMAND_H
#define VERSIONCOMMAND_H

#include <TCLObjectProcessor.h>

class CTCLInterpreter;
class CTCLObject;

/**
 * @class CVersionCommand
 *
 *   Define a command processor that returns the Spectcl version in
 *   major.minor-editlevel format.
 *     This is intended to allow scripts to determine if specific featurs
 *   are present in the SpecTcl in which they run.  Note that given the Tcl
 *   octal leading 0 bit scripts should trim leading zeroes from the edit level
 *   prior to interpreting it.
 */
class CVersionCommand : public CTCLObjectProcessor
{
public:
    CVersionCommand(CTCLInterpreter& interp, const char* pCommand="version");
    virtual ~CVersionCommand();
    
    int operator()(CTCLInterpreter& interp, std::vector<CTCLObject>& objv);
};


#endif