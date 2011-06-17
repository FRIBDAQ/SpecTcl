/*
    This software is Copyright by the Board of Trustees of Michigan
    State University (c) Copyright 2008

    You may use this software under the terms of the GNU public license
    (GPL).  The terms of this license are described at:

     http://www.gnu.org/licenses/gpl.txt

     Author:
             Ron Fox
	     NSCL
	     Michigan State University
	     East Lansing, MI 48824-1321
*/

#ifndef _TCLWSCREATESPECTRUM_H
#define _TCLWSCREATESPECTRUM_H

#ifndef _TCLDBCOMMAND_H
#include "TclDBCommand.h"
#endif

// Forward definitions:

typedef struct _spectrum_parameter spectrum_parameter;
 
/**
 * Implements the createSpectrum command.  This command creates a spectrum
 * in a workspace attached to an experiment.  The syntax of this command is:
 * \verbatim
::spectcl::wsCreateSpectrum exp-handle spectrum-name type parameter-list ?attach-point?
\endverbatim
 *
 * Where:
 *    - exp-handle is an experiment database handle.  The experiment database
 *      it refers to must have a workspace attached.  See the attach-point
 *      parameter.
 *    - spectrum-name is the name of the new spectrum being created.
 *    - spectrum-type is the type of the spectrum being created.
 *       This must be one of the possible short values 
 *       returned by wsSpecTypes.
 *    - parameter-list is the list of parameters required by this spectrum.
 *      this is a list containing pairs each of which has in order:
 *      - The parameter name.
 *      - The parameter dimension (1 for x, 2 for y e.g in a 2d spectrum).
 *        not all dimesion types are allowed for all spectrum types...and some
 *   - attach-point is an optional parameter that is the point at which the workspace
 *     is attached to the experiment database.  This can be omitted in which case
 *     the attach point defaults to the default attach point (currently 'WORKSPACE').
 */
class CTclWsCreateSpectrum : public CTclDBCommand
{
  /* Canonicals */
public:
  CTclWsCreateSpectrum(CTCLInterpreter& interp);
  virtual ~CTclWsCreateSpectrum();

private:
  CTclWsCreateSpectrum& operator=(const CTclWsCreateSpectrum&);
  int operator==(const CTclWsCreateSpectrum&) const;
  int operator!=(const CTclWsCreateSpectrum&) const;

  /* Command entry point: */

public:
  int  operator()(CTCLInterpreter& interp,
		  std::vector<CTCLObject>& objv);

  /* Private utilities: */

private:
  spectrum_parameter** parameterListToStructs(CTCLInterpreter& interp, std::string tclList);
};


#endif
