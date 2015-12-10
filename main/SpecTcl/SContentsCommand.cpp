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
#include <stdio.h>

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
    
    const char *pUsage = "Usage\n    \
scontents ?-json? ?-xlow xl? ?-xhigh xh? ?-ylow yl? ?-yhigh yh?  \\\
          ?-xstride xs? ?-ystride ys? spectrum-name";
    try {
        bindAll(interp, objv);
        
        // Should have at most 3 parameters.
        
        requireAtLeast(objv, 2, pUsage);
        
        // The last parameter must be a valid spectrum.
        
        unsigned spectrumIndex = objv.size() - 1;
        std::string name       = objv[spectrumIndex];
                SpecTcl*    api       = SpecTcl::getInstance();
        CSpectrum*  pSpectrum = api->FindSpectrum(name);
        
        if (!pSpectrum) {
            std::string msg = "Spectrum: ";
            msg += name;
            msg += " is not defined";
            throw msg;
        }
        // Figure out the dimensionality and the default values for the settable
        // parameters:
        
        UInt_t nAxes = pSpectrum->Dimensionality();
        Size_t xDim  = pSpectrum->Dimension(0);             // There's always an xdim.
        Size_t yDim = 0;
        if (nAxes == 2) yDim = pSpectrum->Dimension(1);
        Size_t xStart = 0;
        Size_t yStart = 0;
        Size_t xEnd   = xDim - 1;
        Size_t yEnd   = nAxes == 2 ? (yDim -1) : 0;
        bool json    = false;
        unsigned xStride = 1;
        unsigned yStride = 1;
        
        
        // Process the options
        
        for (int i = 1; i < spectrumIndex; i++) {
            std::string optname = objv[i];
            if (optname == "-json") {
                json = true;
            }
            else if (optname == "-xlow") {
                i++;
                xStart = (int)objv[i];
            }
            else if (optname == "-xhigh") {
                i++;
                xEnd    = (int)objv[i];
            }
            else if (optname == "-ylow") {
                i++;
                yStart = (int)objv[i];
            }
            else if (optname == "-yhigh") {
                i++;
                yEnd = (int)objv[i];
            }
            else if (optname == "-xstride") {
                i++;
                xStride = (int)objv[i];
            }
            else if (optname == "-ystride") {
                i++;
                yStride = (int)objv[i];
            } else {
                std::string msg = "Invalid option: ";
                msg += optname;
                throw msg;
            }
            
        }
        
        // Validate the settings:
        
        // 1-d spectra with yend > 0 are not allowed:
        
        if ((nAxes == 1) && ((yEnd > 0) || (yStart > 0))) {
            throw std::string("-ylow and/or -yhigh not allowed for 1-d spectra");
        }
        // xEnd and yEnd must be inside the spectra:
        
        if (xEnd >= xDim) {
            throw std::string("-xhigh must be inside the spectrum");
        }

        if ((nAxes == 2) && (yEnd >= yDim)) {
            throw std::string("-yhigh must be inside the spectrum");
        }
        
        // -xlow < -xhigh
        
        if (xStart >= xEnd) {
            throw std::string("-xlow must be < -xhigh");
        }
        // -ylow < -yhigh

        if ((nAxes == 2) && (yStart >= yEnd)) {
            throw std::string("-ylow must be < -yhigh");
        }
        // strides must be non zero
        
        if (xStride == 0) {
            throw std::string("-xstride must be > 0");
        }
        if ((nAxes == 2) && (yStride == 0)) {
            throw std::string("-ystride must be > 0");
        }
        // Produce the result.
        
        
        CTCLObject result;
        result.Bind(interp);
        
        std::string jsonString = "[";                      // Result is an array.
        if (nAxes == 1) {
        
            // 1-d spectrum [list [list x y]].
            
            for (UInt_t x = xStart; x <= xEnd; x += xStride) {
                ULong_t y = (*pSpectrum)[&x];
                if (y > 0) {
                    if (json) {
                        char aChannel[200];
                        sprintf(aChannel, "{\"x\": %d, \"v\":%d},\n", x, y);
                        jsonString += aChannel;
                    } else {
                        CTCLObject channel;
                        channel.Bind(interp);
                        channel += static_cast<int>(x);
                        channel += static_cast<int>(y);
                        result  += channel;                        
                    }

                }
            }
                        
        } else {
            // 2-d spectrum:
            
            UInt_t coords[2];                     // For operator[]
            for (UInt_t y = yStart; y <= yEnd; y+= yStride) {
                coords[1] = y;
                for (UInt_t x = xStart; x <= xEnd; x += xStride) {
                    coords[0] = x;
                    ULong_t z = (*pSpectrum)[coords];
                    if (z > 0) {
                        if (json) {
                            char aChannel[200];
                            sprintf(aChannel, "{\"x\":%d, \"y\": %d, \"v\": %d},\n", x, y, z);
                            jsonString += aChannel;
                        } else {
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
        }
        /* If making JSon we need to:
             -   Get rid of the last trailing comma in the array
             -   Close off the array.
             -   Set the result from the jsonString.
            */
        if (json) {
            jsonString = jsonString.substr(0, jsonString.size() - 2);
            jsonString += "]";
            result = jsonString;
        }
            
            
        interp.setResult(result);
        
    }
    catch (std::string msg) {
        if (msg != std::string(pUsage)) {
            msg += "\n";
            msg += pUsage;
        }
        interp.setResult(msg);
        return TCL_ERROR;
    }
    return TCL_OK;
}
