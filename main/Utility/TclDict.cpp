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

#include <stdexcept>
#include <sstream>

/**
 * This section of the file provides overloads for DictPut.  Regardless of parameter types:
 * - pInterp - is the interpreter on which the dictionary object is defined.
 * - dict    - is the dictionary affected.
 * - key     - Is a dictionary key.
 * - value   - Is a value to be added to the dict.
 * 
 * Return value is Tcl_Ok on success and Tcl_Error on faliure.  Failure happens only if the
 * input dict is not a valid dictionary (e.g. {a b c} is not a valid dict.)
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
/**
 * This section of the file cotains overloads for dictGet
 * - rInterp is a reference to an interpreter object.
 * - dict is a CTCLObject referencde containing a dictionary.,
 * - key is the key whose value we will fetch.
 * 
 * The functions return the value in various forms.  Note that
 * input CTCLObject parameters will wind up bound to the rInterp.
 * Output CTCLObjects will also be bound to the rInterp.
 * A few exceptions are possible:
 * 
 * -  std::out_of_range - if the dictionary does not have the specified key.
 * -  std::invalid_argument -if the dictionary parameter is not a valid dict (e.g. 
 *    Tcl_DictObjGet) return Tcl_Error.
 */

 // This is the one on which all the others can be based:
 // note CTCLObject copy construct ....
 CTCLObject
 Tcl::DictGet(
        CTCLInterpreter& rInterp, CTCLObject& dict, CTCLObject& key
    ) {
        
        Tcl_Obj* oResult;
        
        int status = Tcl_DictObjGet(
            rInterp.getInterpreter(), dict.getObject(), key.getObject(), &oResult
        );
        if (status != TCL_OK) {
            throw std::invalid_argument("Tcl::dictGet - invalid dictionary passed in");
        }
        if (!oResult) {
            std::stringstream smsg;
            smsg << "Dictionary does not have key" << std::string(key);
            std::string msg(smsg.str());
            throw std::out_of_range(msg);
        }

        // All ok, so wrap the oResult:

        CTCLObject result(oResult);
        result.Bind(rInterp);
        return result;
    }
    // For this one, we just need to wrap the key in  a CTCLObject then 
    // call the preveous overload:

    CTCLObject
    Tcl::DictGet(CTCLInterpreter& rInterp, CTCLObject& dict, const char* key) {
        CTCLObject okey;
        okey.Bind(rInterp);
        okey = std::string(key);

        return Tcl::DictGet(rInterp, dict, okey);
    }
    // For this one we just need to unwrap the object returned by the previous one:

    std::string 
    Tcl::DictGetAsStr(CTCLInterpreter& rInterp, CTCLObject& dict, const char* key) {
        CTCLObject objResult = Tcl::DictGet(rInterp, dict, key);
        return std::string(objResult);
    }

