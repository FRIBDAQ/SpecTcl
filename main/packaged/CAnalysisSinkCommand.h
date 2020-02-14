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

/** @file:  CAnalysisSinkCommand.h
 *  @brief: Define "analysissink" command to set the data distributor to analyze data.
 */
#ifndef CANALYSISSINKCOMMAND_H
#define CANALYSISSINKCOMMAND_H

#include <TCLObjectProcessor.h>

/**
 * @class CAnalysisSinkCommand
 *    Provides the command "analysissink" that specifies the data distributor
 *    for the analyze command to be an CDistributeToAnalyzer object.
 *    This means that any data that is gotten by the data getter is dispatched
 *    to the user's analysis pipeline and then histogrammed.
 */
class CAnalysisSinkCommand : public CTCLObjectProcessor
{
public:
    CAnalysisSinkCommand(CTCLInterpreter& interp);
    
    int operator()(CTCLInterpreter& interp, std::vector<CTCLObject>& objv);
};


#endif