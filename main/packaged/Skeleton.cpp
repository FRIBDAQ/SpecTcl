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

/** @file:  Skeleton.cpp
 *  @brief: Provides a user skeleton for the batch SpecTcl user code.
 */

#include <tcl.h>
#include <CPipelineMaker.h>
#include <TCLInterpreter.h>

//  Here you should include your headers for your event processors.

// This file will create a Tcl loadable package, those have versions and
// names... set the version and name here:  Below are sample names.
// If you keep this you'll load your package by
//
// \verbatim
//   lappend auto_path $directory_that_has_what_this_builds
//   package require MyPipeline

static const char* VERSION="1.0";
static const char* PKG_NAME="MyPipeline";

class MyPipelineMaker : public CPipelineMaker
{
public:
    void operator()() {
        // Here instantiate and use RegisterEventProcessor to
        // setup the event processing pipeline. 
    }
};

/**
 * This entry is called when the package is loaded.
 *  The name of this entry is derived from the shared library name as follows:
 *  -  Remove the leading lib kill the trailing .so
 *  -  Set the first letter to uppercase and all others to lower case.
 *  -  Append _Init.
 *  
 *  So, for example, if the library is named libMyPipelineMaker.so
 *  this entry must be called Mypiplinemaker_Init
 *  If you chose a different library name, you must adjust the function
 *  name below.
 */
extern "C" {                              // Tcl n eeds C bindings.
    int Mypipelinemaker_Init(Tcl_Interp* pRawInterpreter)
    {
        Tcl_PkgProvide(pRawInterpreter, PKG_NAME, VERSION);
        CTCLInterpreter* pInterp = new CTCLInterpreter(pRawInterpreter);
        
        // If you have command extensions for SpecTcl you can
        // register them here either on the pRawInterpreter via
        //  Tcl_CreateObjCommand or using Tcl++ by writing
        //  a CTCLObjectProcessor concrete class and instantiating it on
        // pInterp here.
        
        
        
        // Now we setup SpecTcl's event processing pipeline:
        
        CPipelineMaker* pMaker = new MyPipelineMaker;
        (*pMaker)();
        
        // Finally Tcl expeects TCL_OK on success or TCL_ERROR on failure:
        
        
        return TCL_OK;
    }
}