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
#include <Python.h>
#ifdef HAVE_STAT                // dueling features.
#undef HAVE_STAT
#endif
#include "CPythonCommand.h"

#include <TCLInterpreter.h>
#include <TCLObject.h>
#include <Exception.h>
#include <SpecTcl.h>

#include <stdexcept>

#include <stdio.h>


/////////////////////////////////////////////////////////////////////////////
//  The spectcl python mocule that lets python scripts do SpecTcl stuff.


/**
 * spectcl_tcl
 *    Run a tcl script from python in SpecTcl's interpreter.
 *
 * @param self - pointer to the module object we're in.
 * @param args - Parameters to the command - should be a single string.
 * @return - string that's the interpreter result of the Tcl script execution
 * @note raises a RuntimeError if the script fails.
 * @note the script scope is the current tcl scope thus e.g.:
 * \verbatim
 *   proc a {stuff} {
 *      set value [expr $stuff*2]
 *      python exec {spectcl.tcl("puts $value")}
 *   }
 * \endverbatim
 *   Should work just fine... and print $stuff*2 when a was called.
 *   @note - naturally  spectcl.tcl("source filename") can be used as well
 *   as package requires to load packages in the SpecTcl interpreter.
 */
static PyObject*
spectcl_tcl(PyObject* self, PyObject* args)
{
    const char* script;
    if (!PyArg_ParseTuple(args, "s", &script)) {
        return NULL;                  // parse tuple raises exceptions on error.
    }
    
    SpecTcl* api = SpecTcl::getInstance();
    CTCLInterpreter* pInterp = api->getInterpreter();       // Tcl interpreter.
    std::string result;
    try {
        result = pInterp->Eval(script);
    }
    catch (CException& e) {
        std::string msg = e.ReasonText();
        PyErr_SetString(PyExc_RuntimeError, msg.c_str());
        return NULL;
    }
    catch (...) {
        PyErr_SetString(PyExc_RuntimeError, "Unanticipated exception from Tcl script");
        return NULL;
    }
    
    // Create the string result and pass it back.
    
    return  PyUnicode_FromString(result.c_str());
    
}

// Module symbol table for the spectcl module:

static struct PyMethodDef SpecTclMethods[] = {
    {"tcl", spectcl_tcl, METH_VARARGS, "Run Tcl script in SpecTcl interpreter"},
    {NULL, NULL, 0, NULL}
};

// Module definition struct:

static struct PyModuleDef spectclModule = {
    PyModuleDef_HEAD_INIT,
    "spectcl", NULL, -1,
    SpecTclMethods
};

// Module initialization function:

PyMODINIT_FUNC
PyInit_SpecTcl(void)
{
    return PyModule_Create(&spectclModule);
}


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
    // Add the SpecTcl module  to the set of preloaded/built in modules:
    
    PyImport_AppendInittab("spectcl", &PyInit_SpecTcl);
    
    Py_Initialize();
    PyImport_ImportModule("spectcl");
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