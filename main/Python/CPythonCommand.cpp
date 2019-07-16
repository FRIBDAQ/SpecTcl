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

/** @file:  CPythonCommand.cpp
 *  @brief: Implement command processor that executes Python Scripts.
 */

#include "CPythonCommand.h"
#include <TCLInterpreter.h>
#include <TCLObject.h>
#include <Exception.h>
#include <stdexcept>

#include <stdio.h>
#include <Python.h>

static const wchar_t* pName = L"SpecTcl";

/**
 * constructor
 *    Constructs the command - registers with Tcl and
 *    initializes the captive Python intepreter.  Note that the
 *    program name will just be "SpecTcl".
 *
 *  @param interp - references the interpreter on which we're running.
 *  @param name   - Name of the command registered.
 */
CPythonCommand::CPythonCommand(CTCLInterpreter& interp, const char* name) :
    CTCLObjectProcessor(interp, name, true),
    m_Interp(interp)
{
    Py_SetProgramName(
        const_cast<wchar_t*>(pName)
    );
    Py_Initialize();
}
/**
 * destructor
 *    Finalize the interpreter:
 */
CPythonCommand::~CPythonCommand()
{
    Py_Finalize();            // Not much we can do if it fails.
}

/**
 * operator()
 *    Executes the command.  Just require that we have a subcommand,
 *    dispatch based on it.  Note we use exception processing
 *    for errors.
 *
 *  @param interp interpreter running the command.
 *  @param objv   command words.
 *  @return int - TCL_OK if all is ok else TCL_ERROR, 
 */
int
CPythonCommand::operator()(CTCLInterpreter& interp, std::vector<CTCLObject>& objv)
{
    bindAll(interp, objv);
    try {
        requireAtLeast(objv, 2);
        
        std::string sc = objv[1];              // Subcommand.
        
        if(sc == "exec" ) {
            execute(interp, objv);
        } else if (sc == "source") {
            source(interp, objv);
        } else {
            throw std::string("Invalid subcommand");
        }
    } catch (std::string msg) {
        interp.setResult(msg);
        return TCL_ERROR;
    } catch (CException& e) {
        interp.setResult(e.ReasonText());
        return TCL_ERROR;
    } catch (std::exception& e) {
        interp.setResult(e.what());
        return TCL_ERROR;
    }
    
    
    return TCL_OK;
}


/////////////////////////////////////////////////////////////////////////////
// protected methods.

/**
 * execute
 *    Execute a script string in objv[2].
 */
void
CPythonCommand::execute(CTCLInterpreter& interp, std::vector<CTCLObject>& objv)
{
    requireExactly(objv, 3);
    std::string script = objv[2];
    
    PyRun_SimpleString(script.c_str());
}
/**
 * source
 *    Run a python file.
 */
void
CPythonCommand::source(CTCLInterpreter& interp, std::vector<CTCLObject>& objv)
{
    requireExactly(objv, 3);
    
    std::string file = objv[2];
    
    FILE* fp = fopen(file.c_str(), "r");
    if (!fp) {
        std::string error = file;
        error += " could not be opened";
        throw error;
    }
    PyRun_AnyFile(fp, file.c_str());
    fclose(fp);
}