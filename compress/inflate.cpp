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
# @file   inflate.cpp
# @brief  Implement the inflate command
# @author <fox@nscl.msu.edu>
*/
#include "inflate.h"
#include <TCLInterpreter.h>
#include <TCLObject.h>
#include <zlib.h>
#include <stdlib.h>

/**
 * constructor
 *    Use the base class constructor to register the command.
 *
 *  @param interp - Reference to the interpreter that will get this command.
 *  @param command - Command keyword string defaults to inflate
 */
CInflate::CInflate(CTCLInterpreter& interp, const char* command) :
    CTCLObjectProcessor(interp, command, true)
{}

/**
 * destructor
 */
CInflate::~CInflate() {}

/**
 * operator()
 *    Executes the inflate command.  Note that the implementation of this command
 *    is a bit problematic given that we're just using the uncompress function
 *    rather than the more complex versions.  This is because, from the
 *    zlib docs: " (The size of the uncompressed data must have been saved
 *    previously by the compressor and transmitted to the decompressor by
 *    some mechanism outside the scope of this compression library.)"
 *    Which means we don't have an a-priori way to size the output buffer.
 *    What we are going to do therefore is something that is a bit
 *    ugly:
 *    -  An initial output buffer of 4* the input buffer is
 *       allocated.
 *    -  If a decompression fails with Z_BUF_ERROR, indicating we
 *       did not choose a sufficiently large buffer we'll add
 *       more space and try again (increment is .5 the size of the
 *       original input buffer).
 *
 *    This process continues until either we fail in our realloc
 *    or we get a status other than Z_BUF_ERROR at which point we're
 *    as done as we can be.
 *
 * @param interp - references the interpreter running the command.
 * @param objv   - Vector of encapsulated objects that represent the
 *                 command words.
 * @return int   - TCL_OK - if success and TCL_ERROR on error.
 *                 On success the command result is the decompressed data
 *                 while on failure the result is an error message.
 */
int
CInflate::operator()(CTCLInterpreter& interp, std::vector<CTCLObject>& objv)
{
    bindAll(interp, objv);
    void*   pUncompressed(0);
    try {
        requireExactly(objv, 2, "Usage:\n  inflate compressed-data");
        
        // Get pointer and length of compressed data:
        
        int length;
        unsigned char* pCompressed = Tcl_GetByteArrayFromObj(
            objv[1].getObject(), &length);
        
        // Give the allocationa try and loop until we get a
        // status other than Z_BUF_ERROR:
        
        int status = Z_BUF_ERROR;
        unsigned uclength = 4*length;
        uLongf actualLen;
        unsigned char* pUncompressed(0);
        pUncompressed = reinterpret_cast<unsigned char*>(malloc(uclength));
        while(pUncompressed && (status == Z_BUF_ERROR)) {
            
            actualLen = uclength;
            status = uncompress(pUncompressed, &actualLen, pCompressed, length);
            if (status == Z_OK) break;
            
            uclength += length/4;            // In case we need to enlarge
            pUncompressed =
                reinterpret_cast<unsigned char*>(realloc(pUncompressed, uclength));
        }
        // If pUncompressed == 0 we failed to realloc:
        
        if (!pUncompressed) {
            throw std::string("Could not allocate the output buffer");
        }
        switch (status) {
            case Z_OK:
                {
                    Tcl_Obj* result = Tcl_NewByteArrayObj(
                        reinterpret_cast<unsigned char*>(pUncompressed),
                        actualLen
                    );
                    Tcl_SetObjResult(interp.getInterpreter(), result);
                }
                break;
            case Z_DATA_ERROR:
                throw std::string("Input was not compressed with 'deflate'");
            case Z_MEM_ERROR:
                throw std::string("Zlib internal memory allocation failed");
            default:
                throw std::string("Unanticipated error from zlib uncompress function");
        }
        
    }
    catch (std::string msg) {
        free(pUncompressed);
        interp.setResult(msg);
        return TCL_ERROR;
    }
    
    free(pUncompressed);
    return TCL_OK;
}