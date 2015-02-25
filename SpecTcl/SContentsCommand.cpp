/*
    This software is Copyright by the Board of Trustees of Michigan
    State University (c) Copyright 2005.

    You may use this software under the terms of the GNU public license
    (GPL).  The terms of this license are described at:

     http://www.gnu.org/licenses/gpl.txt

     Author:
             Ron Fox
	     NSCL
	     Michigan State University
	     East Lansing, MI 48824-1321
*/

/**
 * @file SContentsCommand.cpp
 * @brief Implements the scontents command.
 * @author Ron Fox <fox@nscl.msu.edu>
 */
#include <config.h>
#include "SContentsCommand.h"
#include <Spectrum.h>
#include <SpecTcl.h>

/**
 * constructor
 *    Use the base class constructor to register the command
 * @param interp - references the interpreter on which we are registering.
 * @param command - Pointer to the string that defines the command keyword.
 *                  this defaults to "scontents"
 */
CSContentsCommand::CSContentsCommand(CTCLInterpreter& interp, const char* command) :
    CTCLObjectProcessor(interp, command, true)
{}

/**
 * destructor
 */
CSContentsCommand::~CSContentsCommand() {}

/**
 * operator()
 *   Command processor.  We get here if we must:
 *   *   Figure out the dimensionality of the spectrum
 *   *   Figure out the size of each axis.
 *   *   Fetch the data from the spectrum marshalled as an appropriate list of lists.
 *
 * @param interp - Interpreter on which this is running.
 * @param objv   - Vector of encapsulated Tcl_Obj's that make up the command words.
 * @return int   - TCL_OK if successful or TCL_ERROR if not
 */
int
CSContentsCommand::operator()(CTCLInterpreter& interp, std::vector<CTCLObject>& objv)
{
    // We'll use the exception throw model for handling errors.
    
    try {
        bindAll(interp, objv);
        
        // Should have exactly two command words:
        
        requireExactly(objv, 2, "Usage\n  scontents spectrum-name");
        
        // Locate the spectrum
        
        std::string name      = objv[1];
        SpecTcl*    api       = SpecTcl::getInstance();
        CSpectrum*  pSpectrum = api->FindSpectrum(name);
        
        if (!pSpectrum) {
            std::string msg = "Spectrum: ";
            msg += name;
            msg += " is not defined";
            throw msg;
        }
        // Figure out the dimensionality:
        
        UInt_t nAxes = pSpectrum->Dimensionality();
        Size_t xDim  = pSpectrum->Dimension(0);             // There's always an xdim.
        CTCLObject result;
        result.Bind(interp);
        
        if (nAxes == 1) {
        
            // 1-d spectrum [list [list x y]].
            
            for (UInt_t x = 0; x < xDim; x++) {
                ULong_t y = (*pSpectrum)[&x];
                if (y > 0) {
                    CTCLObject channel;
                    channel.Bind(interp);
                    channel += static_cast<int>(x);
                    channel += static_cast<int>(y);
                    result  += channel;
                }
            }
            
        } else {
            // 2-d spectrum:
            
            Size_t yDim = pSpectrum->Dimension(1);
            UInt_t coords[2];                     // For operator[]
            for (UInt_t y = 0; y < yDim; y++) {
                coords[1] = y;
                for (UInt_t x = 0; x < xDim; x++) {
                    coords[0] = x;
                    ULong_t z = (*pSpectrum)[coords];
                    if (z > 0) {
                        CTCLObject channel;
                        channel.Bind(interp);
                        channel += static_cast<int>(x);
                        channel += static_cast<int>(y);
                        channel += static_cast<int>(z);
                        result  += channel;
                    }
                }
            }
        }
        interp.setResult(result);
        
    }
    catch (std::string msg) {
        interp.setResult(msg);
        return TCL_ERROR;
    }
    return TCL_OK;
}