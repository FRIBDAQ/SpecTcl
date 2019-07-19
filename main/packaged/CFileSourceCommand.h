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

/** @file:  CFileSourceCommand.h
 *  @brief: Command to hook a file data source (getter) into the anlyzecommand.
 */
#ifndef CFILESOURCECOMMAND_H
#define CFILESOURCECOMMAND_H
#include <TCLObjectProcessor.h>

/**
 * @class CFileSourceCommand
 *    Class that executes the "filesource" command.  The filesource command
 *    establishes a getter with the CAnalyzeCommand class that connects
 *    to an event file. The form of this command is:
 *
 * \verbatim
 *     filesource path ?blocksize?
 * \endverbatim
 *
 *  *  path is the path to the event file to process an
 *  *  blocksize is an optional parameter that specifies the size of the block
 *               for each read from file.  This defaults to 8192 which may not
 *               be optimal.
 *
 *   @note - Data getters are assumed to be dynamically allocated so any prior
 *           data getter is deleted.
 */
class CFileSourceCommand : public CTCLObjectProcessor
{
public:
    CFileSourceCommand(CTCLInterpreter& interp);  // fixed "filesource" command.
    
    int operator()(CTCLInterpreter& interp, std::vector<CTCLObject>& objv);
    
};


#endif