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
#define PY_SSIZ_T_CLEAN         // from the examples ...
#include <Python.h>
#ifdef HAVE_STAT                // dueling features.
#undef HAVE_STAT
#endif
#include "CPythonCommand.h"

#include <TCLInterpreter.h>
#include <TCLObject.h>
#include <Exception.h>
#include <SpecTcl.h>
#include <TreeParameter.h>

#include <stdexcept>

#include <stdio.h>
#include <iostream>

//
// Python type objects that will be added to spectcl package:
//

#include "pythonParameters.h"            // Parameters/parameter object.
#include "pythonSpectrum.h"             // spectrum object.
#include "pythonVariable.h"             // tree variable objects.

/////////////////////////////////////////////////////////////////////////////
//  The spectcl python module that lets python scripts do SpecTcl stuff.


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

/**
 * spectcl_plist
 *    Creates a python ntuple of parameter name strings. No filtering is done,
 *    No filtering is done.
 */
static PyObject*
spectcl_plist(PyObject* self, PyObject* args)
{
    // there can be no parameters:
    
    if(PyTuple_Size(args) != 0) {
        PyErr_SetString(PyExc_IndexError, "spectcl.plist does not accept any parameters");
        return NULL;
    }
    // Figure out the size of the resulting ntuple:
    
    SpecTcl* pSpecTcl = SpecTcl::getInstance();
    UInt_t nParams    = pSpecTcl->ParameterCount();
    
    // Create the result then iterate over the spectrumdictionary to pull out
    // the parameter names:
    
    PyObject* result = PyTuple_New(nParams);
    int       i      = 0;
    for (
        auto p = pSpecTcl->BeginParameters(); p != pSpecTcl->EndParameters();
        p++, i++
    ) {
        PyTuple_SetItem(result, i, PyUnicode_FromString(p->first.c_str()));
    }
    
    return result;
}
/**
 * spectcl_speclist
 *    @param self - module object.
 *    @param ignored  parameters.
 *    @return PyObject* - tuple of spectrum names.
 */
static PyObject*
spectcl_speclist(PyObject* self, PyObject* Py_UNUSED(ignored))
{
    SpecTcl* api = SpecTcl::getInstance();
    size_t numSpectra = api->SpectrumCount();
    
    PyObject* result = PyTuple_New(numSpectra);
    int i = 0;
    for(auto p = api->SpectrumBegin(); p != api->SpectrumEnd(); p++, i++) {
        PyTuple_SetItem(result, i, PyUnicode_FromString(p->first.c_str()));
        
        
    }
    
    return result;
}
/**
 * spectcl_varlist
 *    @param self - module.
 *    @param ignored -ignroed parameters.
 *    @return PyObject* - A tuple containing the names of all tree vfariables.
 */
static PyObject*
spectcl_varlist(PyObject* self, PyObject* Py_UNUSED(ignored))
{
    PyObject* result = PyTuple_New(CTreeVariable::size());
    int i = 0;
    for (auto p = CTreeVariable::begin(); p != CTreeVariable::end(); p++, i++) {
        std::string name = p->first;
        PyTuple_SetItem(result, i, PyUnicode_FromString(name.c_str()));
    }
    
    return result;
}
// Module symbol table for the spectcl module:

static struct PyMethodDef SpecTclMethods[] = {
    {"tcl", spectcl_tcl, METH_VARARGS, "Run Tcl script in SpecTcl interpreter"},
    {"listparams", spectcl_plist, METH_VARARGS, "List names of SpecTcl parameters"},
    {"listspectra", spectcl_speclist, METH_NOARGS, "List names of spectra"},
    {"listvars", spectcl_varlist, METH_NOARGS, "List tree variable names"},
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
    PyObject* module =  PyModule_Create(&spectclModule);
    
    setupPythonParameterObjects(module);
    setupPythonSpectrumObjects(module);
    setupPythonVariableObjects(module);
    
    return module;
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
    PyRun_SimpleString("import spectcl"); // goddamnit.

    return;

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
