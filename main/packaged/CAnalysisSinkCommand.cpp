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

/** @file:  CAnalysisSinkCommand.cpp
 *  @brief: Implements the analysissink command processor.
 */


#include "CAnalysisSinkCommand.h"
#include "CAnalyzeCommand.h"
#include "CDistributeToAnalyzer.h"

#include <TCLInterpreter.h>
#include <Exception.h>
#include <stdexcept>

/**
 * constructor.
 *   @param interp - references the interpreter.
 */
CAnalysisSinkCommand::CAnalysisSinkCommand(CTCLInterpreter& interp) :
    CTCLObjectProcessor(interp, "analysissink", true)
{}


/**
 * operator()
 *     Process the command, which takes no parameters.
 *  @param interp - interpreter
 *  @param objv   - command line parameters.
 *  @return int   - Tcl command status.
 */
int
CAnalysisSinkCommand::operator()(CTCLInterpreter& interp, std::vector<CTCLObject>& objv)
{
    try {
        requireExactly(objv, 1);             // No command parameters.
        
        CDataDistributor *pDist = new CDistributeToAnalyzer;
        CAnalyzeCommand::setDistributor(pDist);
        
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

    
    return TCL_OK;
}

