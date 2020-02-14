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

/** @file:  pythonParameters.h
 *  @brief: Defines externally visible parts of the
 *          spectcl.parameters and spectcl.parameter types.
 */
#ifndef PYTHONPARAMETERS_H
#define PYTHONPARAMETERS_h

#define PY_SSIZE_T_CLEAN         // from the examples ...
#include <Python.h>
#ifdef HAVE_STAT                // dueling features.
#undef HAVE_STAT
#endif

extern void setupPythonParameterObjects(PyObject* module);

// typedefs that describe the extensions that are added to type:

// Nothing is added to the parameters object.

// The parametr object includes a pointer to its CTreeParameter.

class CTreeParameter;
typedef struct _ParameterData {
    CTreeParameter* s_pParameter;
} ParameterData, *pParameterData;

#endif