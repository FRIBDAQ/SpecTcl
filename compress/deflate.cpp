/**

#    This software is Copyright by the Board of Trustees of Michigan
#    State University (c) Copyright 2013.
#
#    You may use this software under the terms of the GNU public license
#    (GPL).  The terms of this license are described at:
#
#     http://www.gnu.org/licenses/gpl.txt
#
#    Author:
#            Ron Fox
#            NSCL
#            Michigan State University
#            East Lansing, MI 48824-1321

##
# @file   deflate.cpp
# @brief  Implement the deflate commnd.
# @author <fox@nscl.msu.edu>
*/
#include "deflate.h"
#include <TCLInterpreter.h>
#include <TCLObject.h>
#include <tcl.h>

#include <zlib.h>

/**
 * constructor
 *    Create the command.  This is actually done by the base class constructor.
 *
 * @param interp - interpreter on which the command is registered.
 * @param command - Command name string (defaults to "deflate").
 */
CDeflate::CDeflate(CTCLInterpreter& interp, const char* command) :
    CTCLObjectProcessor(interp, command, true)
 {}
  
/**
 * destructor -no-op for now.
 */
CDeflate::~CDeflate() {}

/**
 * operator()
 *   Called when the command is executed.
 *
 *  @param interp - the interpreter on which the command is being registered.
 *  @param objv   - Vector of objects that make up the command.
 *  @return int   - TCL_OK for success TCL_ERROR if failed.
 *
 *  The result of the command is the compressed data or an error message
 *  if there was a failure.
*/
int
CDeflate::operator()(CTCLInterpreter& interp, std::vector<CTCLObject>& objv)
{
    bindAll(interp, objv);
    Bytef* pCompressedData(0);
    try {
        requireExactly(objv, 2, "Usage:\n   deflate  data\n");
        
        // Get the data from the object:
        
        Tcl_Obj* dataObj = objv[1].getObject();
        int length;
        
        unsigned char* data = Tcl_GetByteArrayFromObj(dataObj, &length);
        
        // Figure out the size and allocate the output buffer.
        
        uLong requiredLength = compressBound(length);
        pCompressedData = new Byte[requiredLength];
        
        int status = compress(pCompressedData, &requiredLength, data, length);
        switch (status) {
            case Z_OK:
                {
                    Tcl_Obj* pResult = Tcl_NewByteArrayObj(pCompressedData, requiredLength);
                    Tcl_SetObjResult(interp.getInterpreter(), pResult);
                }
                break;
            case Z_MEM_ERROR:
                throw std::string("Compression was not able to allocate sufficient dynamicm emory");
            case Z_BUF_ERROR:
                throw std::string("Result buffer was incorrectly sized");
            default:
                throw std::string("Zlib's compress function failed for some unanticipated reason");
                
        }
        
    }
    catch (std::string msg) {
        delete pCompressedData;
        interp.setResult(msg);
        return TCL_ERROR;
    }
    delete pCompressedData;
    
    return TCL_OK;
}