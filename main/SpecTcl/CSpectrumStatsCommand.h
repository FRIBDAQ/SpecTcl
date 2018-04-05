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
# @file   CSpectrumStatsCommand.h
# @brief  Class implementing command to fetch spectrum statistics.
# @author <fox@nscl.msu.edu>
*/
#ifndef CSPECTRUMSTATSCOMMAND_H
#define CSPECTRUMSTATSCOMMAND_H


#include <TCLObjectProcessor.h>
class CTCLInterpreter;
class CTCLObject;
class CSpectrum;

/**
 * @class CSpectrumStatsCommand
 *     Command that produces statistics for spectra.  Command syntax:
 * \verbatim
 *    specstats ?pattern?
 * \endverbatim
 *
 *   Value is a list of dicts.  The list has an element for each spectrum
 *   that matches ?pattern? which defaults to * if not provided.
 *   Each list item is a dict which has the following set of keys:
 *   -   name      - Name of the spectrum.
 *   -   overflows - List of overflow counters, one element for each axis.  In general
 *                   this is a 0,1 or 2 element list of integers. If there are
 *                   2 elements, usually these are x, y axis overflows, however
 *                   note that (gamma and regular) summary spectra really have
 *                   only one axis and that is the y axis.
 *   -  underflows - List of underflows countersm, one element for each axis.
 *                   see 'overflows' above for a discussion about this list.
 */
class CSpectrumStatsCommand : public CTCLObjectProcessor
{
public:
    CSpectrumStatsCommand(CTCLInterpreter& interp, const char* command="specstats");
    virtual ~CSpectrumStatsCommand();
    
public:
    virtual int operator()(CTCLInterpreter& interp, std::vector<CTCLObject>& objv);
private:
    std::string Usage(std::string command);
    CTCLObject* makeStatsDict(
        CTCLInterpreter& interp, std::string name, CSpectrum* pSpectrum
    );
};

#endif
