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
 * @file SContentsCommand.h
 * @brief Declare class to implement the scontents command.
 * @author Ron Fox <fox@nscl.msu.edu>
 */
#ifndef SCONTENTSCOMMAND_H
#define SCONTENTSCOMMAND_H

#include <TCLObjectProcessor.h>

class CTCLInterpreter;
class CTCLObject;

/**
 * @class CSContentsCommand
 *
 *    This class  will implement the scontents command;  The form of this
 *    command is:
 *\verbatim
 *    scontents ?-json? spectrum-name
 *\endverbatim
 *
 *   The return value is the list of non-zero channels from that spectrum
 *   (could be empty).
 *
 *  Each list is a pair (for 1-d spectra) and a triplet (for 2-d spectra)
 *  the coordinates of each channel are the first (one or two) items fo the
 *  sublists
 *
 *  E.g. for a 1-d spectrum you might see:
 * \verbatim
 *   { {1 5} {3 100} ...}
 * \endverbatim
 *
 * For a 2-d spectrum you might see:
 *
 * \verbatim
 *  { {1 2 100}, {2 4 1234} ...}
 * \endverbatim
 *
 * If -json is specified the output is a JSON array of nonzero channel objects
 * where the attributes of the object are:
 *
 * -  xchan - The X channel coordinate.
 * -  ychan - The Y channel coordinate (only if 2d).
 * -  value - The number of counts at that channel.
 */
class CSContentsCommand : public CTCLObjectProcessor
{
public:
    CSContentsCommand(CTCLInterpreter& interp, const char* command = "scontents");
    virtual ~CSContentsCommand();
    
    int operator()(CTCLInterpreter& interp, std::vector<CTCLObject>& objv);
};
#endif