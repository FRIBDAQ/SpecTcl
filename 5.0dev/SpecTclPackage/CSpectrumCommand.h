/*
    This software is Copyright by the Board of Trustees of Michigan
    State University (c) Copyright 2013.

    You may use this software under the terms of the GNU public license
    (GPL).  The terms of this license are described at:

     http://www.gnu.org/licenses/gpl.txt

     Author:
             Ron Fox
	     NSCL
	     Michigan State University
	     East Lansing, MI 48824-1321
*/

#ifndef _CSPECTRUMCOMMAND_H
#define _CSPECTRUMCOMMAND_H

/**
 * @file CSpectrumCommand.h
 * @brief Define the ::spectcl::spectrum command.
 */

#ifndef _TCLOBJECTPROCESSOR_H
#include <TCLObjectProcessor.h>
#endif

#ifndef _STL_STRING
#include <string>
#ifndef _STL_STRING
#define _STL_STRING
#endif
#endif

/**
 * @class CSpectrumCommand
 *
 *   This class implements the CSpectrumCommand.  The purpose of the command
 *   is to create, delete, and provide informationabout histograms (spectra).
 *   The command also supports placing traces onthe spectrum dictionary
 *   which is an object that is used to maintain histograms and their metadata.
 *   Spectrum creation requires the assembly of a spectrum an incrementer and a Condition
 *   object, as well as the use of an allocator.
 *   -   The incrementer is responsible for allocating the actual spectrum storage for
 *       the underlying histogramming engine.
 *   -   The incrementer is responsible for knowing how to take an event and turn it into
 *       increments of the spectrum.
 *   -   The condition is responsible for determining, given an event, if this spectrum's
 *       incrementer should be called into action.  The condition allows spectra like
 *       The energy spectra of all events identified as producing a 6Li e.g.
 *
 *   The spectrum command has the following general forms:
 *     - spectrum create name type parameters axis-specs  ?-condition gate-name? ?-histogrammer histogrammer?
 *     - spectrum list   pattern
 *     - spectrum delete name
 *     - spectrum trace  what script
 *
 *   Where:
 *    
 *      Item name          |    Meaning
 *      ------------------ | -----------------------------------------
 *      name               | The name of a spectrum being operated on
 *      type               | A spectrum type identifier.
 *      parameters         | this is a one or possibly two element list of parameters
 *      axis-specs         | a one or possibly two element list of axis specifications
 *      condition-name     | The name of a gate that will the condition for the spectrum.
 *      histogrammer       | The underlying histogramming engine.  Currently Root or SpecTcl default is SpecTcl.
 *      pattern            | A pattern with glob wild-card characters used to select spectra
 *      what               | The event to trade: can be add or delete
 *      script             | A script that runs when the trace fires. The
 *                         | script will have the trace operation (e.g add and the spectrum name added. 
 *   
 *   Parameters
 *   ----------
 *    The parameter list has at least one element that is a list of parameter names used by
 *    its incrementer.  Some spectrum types may require a second parameter list (e.g. 2-d spectra).
 *  
 *   Axis specifications
 *    -------------------
 *    An axis specification is a three element list consisting of a low limit, a high limit and
 *    The number of bins in that range.  The axis-specs parameter is a one or two element list of
 *    axis specifications.  Some spectra require two axis specifications.  If a spectrum
 *    only requires a single axis specifiation there need not be an enclosing list e.g.
 *    {0 127 128}  is a valid single axis specification while the corresponding 2-d specification
 *    would ned to be:  {{0 127 128} {0 127 128}} (two element list of axis specifications.
 */
class CSpectrumCommand  : public CTCLObjectProcessor {
private:
  static CSpectrumCommand* m_pInstance; // Makes this a singleton.

  // Construction and destruction are therefore private:

private:
  CSpectrumCommand(CTCLInterpreter& interp, std::string command);
  virtual ~CSpectrumCommand();

  // public methods:

public:
  static CSpectrumCommand* instance();

  int operator()(CTCLInterpreter& interp, std::vector<CTCLObject>& objv) {} // stub.
};

#endif
