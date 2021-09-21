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

/** @file:  TclDict.h
 *  @brief: TCLDictionary utils.
 */
#ifndef TCLDICT_H
#define TCLDICT_H

#include <tcl.h>
class CTCLObject;
class CTCLInterpreter;

namespace Tcl {
    int DictPut(
        CTCLInterpreter& pInterp, CTCLObject& dict, const char* key,
        Tcl_Obj* value
    );
    int DictPut(
        CTCLInterpreter& pInterp, CTCLObject& dict, const char* key,
        CTCLObject& value
    );
    int DictPut(
        CTCLInterpreter& pInterp, CTCLObject& dict, const char* key, int value
    ) ;
    int DictPut(
        CTCLInterpreter& pInterp, CTCLObject& dict, const char* key,
        const char* value
    );
}

#endif