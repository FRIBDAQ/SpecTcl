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

/** @file:  CFileSourcCommand.cpp
 *  @brief: Implement the file data soure setter.
 */
#include "CFileSourceCommand.h"
#include "CAnalyzeCommand.h"
#include "CFileDataGetter.h"
#include <Exception.h>
#include <TCLInterpreter.h>
#include <TCLObject.h>

#include <stdexcept>
#include <string>

static const size_t DEFAULT_BLOCKSIZE=8192;

/**
 * constructor
 *   @param interp - references the interpreter.
 */
CFileSourceCommand::CFileSourceCommand(CTCLInterpreter& interp) :
    CTCLObjectProcessor(interp, "filesource",  true)
{}

/**
 * operator()
 *    Execute the filesource command.
 *
 *   @param interp - interpreter running the command.
 *   @param objv   - command line objects.
 *   @return int - Tcl status.
 *   @note exception processing is used to simplify error handling.
 */
int
CFileSourceCommand::operator()(CTCLInterpreter& interp, std::vector<CTCLObject>& objv)
{
    try {
        bindAll(interp, objv);
        requireAtLeast(objv, 2);
        requireAtMost(objv, 3);
        
        size_t blocksize = DEFAULT_BLOCKSIZE;
        std::string file = objv[1];
        if (objv.size() == 3) {
            blocksize = (int)(objv[2]);
        }
        CFileDataGetter* pGetter = new CFileDataGetter(file.c_str(), blocksize);
        CAnalyzeCommand::setDataGetter(pGetter);
        
    } catch (CException& e) {
        interp.setResult(e.ReasonText());
        return TCL_ERROR;
    } catch(std::exception& e) {
        interp.setResult(e.what());
        return TCL_ERROR;
    } catch (std::string msg) {
        interp.setResult(msg);
        return TCL_ERROR;
    } catch (char* msg) {
        interp.setResult(msg);
        return TCL_ERROR;
    }
    return TCL_OK;
}