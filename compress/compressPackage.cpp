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
# @file   compressPackage.cpp
# @brief  Package initialization for zlib compression package.
# @author <fox@nscl.msu.edu>
*/
#include <tcl.h>
#include "inflate.h"
#include "deflate.h"
#include <TCLInterpreter.h>


static const char* pkgVersion="1.0";

/**
 * compress_Init
 *    Provides the initialization function for unsafe-interpreters.
 *    *  Register the package with the interpreter.
 *    *  Encapsulate the interpreter
 *    *  Instantiate the commands (which registers them).l
 *    *  return TCL_OK assuming all goes well.
 * @param pRawInterp -the raw Tcl interpreter.
 * @return int TCL_OK normally.
 */
extern "C" {
    int Compress_Init(Tcl_Interp* pRawInterp)
    {
        int status = Tcl_PkgProvide(pRawInterp, "compress", pkgVersion);
        if (status != TCL_OK) return status;
        
        /*
         *  Must encapsulate this way else leaving scope deletes
         *  the interpreter.  That's ok since Tcl doesn't really provide
         *  a way to unload packages.
         */
        
        CTCLInterpreter& interp(*new CTCLInterpreter(pRawInterp));
        
        /*
         *  Similarly for the commands:
         */
        
        new CInflate(interp);
        new CDeflate(interp);
        
        return TCL_OK;
    }
    
    /**
    * compress_SafeInit
    *   Nothing unsafe about the package so:
    */
    int Compress_SafeInit(Tcl_Interp* interp) {
        return Compress_Init(interp);
    }
void* gpTCLApplication(0);
}

