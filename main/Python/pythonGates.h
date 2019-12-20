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

/** @file:  pythonGates.h
 *  @brief: Header for python gate wrapper.
 */
#ifndef PYTHONGATES_H
#define PYTHONGATES_H
#include "Python.h"

#ifdef HAVE_STAT
#undef HAVE_STAT
#endif

#include <config.h>


#include <GateContainer.h>

typedef struct {
    PyObject_HEAD
    CGateContainer* s_pContainer;    
} GateObject, *pGateObject;

extern void setupPythonGates(PyObject* module);
#endif