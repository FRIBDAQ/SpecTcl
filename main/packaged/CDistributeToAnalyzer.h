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

/** @file:  CDistributeToAnalyzer
 *  @brief: Data distributor that distibutes data to the analyzer.
 */
#ifndef CDISTRIBUTETOANALYZER_H
#define CDISTRIBUTETOANALYZER_H
#include "CDataDistributor.h"

/**
 * @class CDistributeToAnalyzer
 *    This class takes data in the form a data getter receives it
 *    and passes it on to the analyzer for analysis.
 */
class CDistributeToAnalyzer : public CDataDistributor
{
public:
    virtual void handleData(std::pair<size_t, void*>& info);
};

#endif