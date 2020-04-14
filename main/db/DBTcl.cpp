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

/** @file:  DBTcl.cpp
 *  @brief: Provides the DBTcl and its instance command ensembles
 */
#include "DBTcl.h"

#include "SpecTclDatabase.h"
#include "SaveSet.h"

#include <TCLInterpreter.h>
#include <TCLObject.h>

#include <stdexcept>
#include <sstream>

namespace SpecTcl {
///////////////////////////////////////////////////////////
// DBTcl class:

int DBTcl::m_serialNumber(0);

/**
 * constructor:
 *   Constructs the DBTcl command.  This is the base
 *   command of a command ensembel with the following
 *   subcommands:
 *
 *   -  create - creates a new database file.
 *   -  connect- Creates a unique command ensemble
 *               that allows the user to manipulate an
 *               existing database.  See the DBInstance
 *               class for more information about that
 *               command, which will be of the form
 *               DBTclInstancen where n is a unique number.
 *  @param interp - interpreter on which the DBTcl command is
 *               registered.
 */
DBTcl::DBTcl(CTCLInterpreter& interp) :
    CTCLObjectProcessor(interp, "DBTcl", true)
{}

/**
 * operator()
 *    Command processor for the DBTcl command
 *    - Ensure there's a subcommand.
 *    - Tease the subcommand out of command parameters.
 *    - Dispatch to the subcommand processor, returning an
 *      error indication if the subcommand does not match.
 * @param interp - interpreter executing the command.
 * @param objv   - vector command paranmeters - including
 *                 the command name.
 * @return int  - TCL_OK on success else TCL_ERROR
 * @note excepion procesing is used to make error processing
 *       simpler.
 */
int
DBTcl::operator()(CTCLInterpreter& interp, std::vector<CTCLObject>& objv)
{
    int status = TCL_OK;
    
    
    return status;
}




}                          // SpecTcl namespace.