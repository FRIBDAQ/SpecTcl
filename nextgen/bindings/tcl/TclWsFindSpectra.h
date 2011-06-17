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

#ifndef _TCLWSFINDSPECTRA_H
#define _TCLWSFINDSPECTRA_H

#ifndef _TCLDBCOMMAND_H
#include "TclDBCommand.h"
#endif


// forward definitions:

typedef struct _spectrum_definition spectrum_definition;
typedef struct _spectrum_parameter  spectrum_parameter;
class CTCLObject;

/**
 * Implement the spectcl::wsFindspectra command.
 * This command is responsible for listing spectra whose names
 * match specific patterns.  Full spectrum definitions are 
 * Provided for each matching spectrum.
 * The syntax of the command is:
 * \verbatim
::spectcl::wsFindSpectra exp-handle ?pattern? ?allversions? ?attach-point?
\endverbatim
 * Where:
 *    - exp-handle is the handle of an epxeriment database that has a workspace
 *                 attached to it.  See the attach-point parameter description.
 *    - pattern is an optional glob pattern that specifies which spectra will be 
 *              matched by the find.  If omitted the pattern "*" is used which
 *              will match all defined spectra.
 *    - allversions - Is an optional parameter that, if true, requests that all
 *                   versions of all matching spectra will be matched.
 *                   otherwise only the most recent version of each spectrum
 *                   definition is returned.
 *    - attach-point - is an optional parameter that, if provided, describes where the
 *                   workspace is attached to the experiment.  If not provided the
 *                   default value of WORKSPACE is used.
 * 
 *  On success the result of this command is a Tcl list with one element for each
 *  matched spectrum (the list can be empty if there are no matches.
 *  Each element of the list describes a spectrum and is itself a list containing
 *  in order:
 *  - id    - The unique id of the spectrum (primary key in the database table).
 *  - name  - The name of the spectrum.
 *  - type  - The brief type name for the spectrum.
 *  - version - The version number of the spectrum (indicating how many times it has been
 *              redefined.
 *  - parameters - The parameters associated with the spectrum.  This is itself a list
 *              (possibly empty?) where each element describes a paramter and contains the
 *               following elements:
 *       - dimension number (e.g. 0 for X, 1 for Y in a 2d spectrum).
 *       - name the name of the parameter (in the experiment database parameter table.
 *
 *  On failure the command makes a Tcl Script level error and returns as a result a
 *  human readable string that describes the error condition.
 *
 */
class CTclWsFindSpectra : public CTclDBCommand
{
  /* Canonicals */
public:
  CTclWsFindSpectra(CTCLInterpreter& interp);
  virtual ~CTclWsFindSpectra();

private:
  CTclWsFindSpectra& operator=(const CTclWsFindSpectra&);
  int operator==(const CTclWsFindSpectra&) const;
  int operator!=(const CTclWsFindSpectra&) const;

  /* Command entry point: */

public:
  int  operator()(CTCLInterpreter& interp,
		  std::vector<CTCLObject>& objv);


  /* Private utilities: */

private:
  std::string definitionsToList(CTCLInterpreter& interp, spectrum_definition** ppDefinitions);
  CTCLObject  spectrumDefToList(CTCLInterpreter& interp, spectrum_definition*  pDefinition);
  CTCLObject  parameterDefsToList(CTCLInterpreter& interp, spectrum_parameter** ppParams);
};
#endif
