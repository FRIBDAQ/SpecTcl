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

/** @file:  pythonVariable.h
 *  @brief: spectcl.variable type - encapsulates CTreeVariables and their properties
 */
#ifndef PYTHONVARIABLE_H
#define PYTHONVARIABLE_H

#define PY_SSIZE_T_CLEAN         // from the examples ...
#include <Python.h>
#ifdef HAVE_STAT                // dueling features.
#undef HAVE_STAT
#endif


class CTreeVariableProperties;

extern void setupPythonVariableObjects(PyObject* module);   // intialization method.

/**
 * @note This defines the extension data for the object.  In our case,
 *       we maintain a CTreeVariableProperties object.  That's the underlying
 *       data associated with a tree variable anyway.
 */

typedef struct _VariableData {
    CTreeVariableProperties* s_pProperties;
} VariableData, *pVariableData;


#endif