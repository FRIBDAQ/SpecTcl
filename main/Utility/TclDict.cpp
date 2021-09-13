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

/** @file:  TclDict.cpp
 *  @brief: TCL Dictionary utilities.
 */

#include "TclDict.h"
#include "TCLInterpreter.h"
#include "TCLObject.h"

/**
 * This file provides overloads for DictPut.  Regardless of parameter types:
 * - pInterp - is the interpreter on which the dictionary object is defined.
 * - dict    - is the dictionary affected.
 * - key     - Is a dictionary key.
 * - value   - Is a value to be added to the dict.
 */

int
Tcl::DictPut(
    CTCLInterpreter& interp, CTCLObject& dict, const char* key, Tcl_Obj* value
)
{
    return Tcl_DictObjPut(
        interp.getInterpreter(), dict.getObject(),
        Tcl_NewStringObj(key, -1), value
    );
}

int Tcl::DictPut(
    CTCLInterpreter& interp, CTCLObject& dict, const char* key, CTCLObject& value
)
{
    return DictPut(interp, dict, key, value.getObject());
}

int
Tcl::DictPut(
    CTCLInterpreter& interp, CTCLObject& dict, const char* key, int value
)
{
    Tcl_Obj* pValue = Tcl_NewIntObj(value);
    return DictPut(interp, dict, key, pValue);
}

int
Tcl::DictPut(
    CTCLInterpreter& interp, CTCLObject& dict, const char* key, const char* value
)
{
    Tcl_Obj* pValue = Tcl_NewStringObj(value, -1);
    return DictPut(interp, dict, key, pValue);
}