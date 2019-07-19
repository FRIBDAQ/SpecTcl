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

/** @file:  CDistributeToAnalyzer.cpp
 *  @brief: Implement class to distribute data to the analyzer.
 */
#include "CDistributeToAnalyzer.h"
#include <Globals.h>
/**
 * handleData
 *    Handles a block of data:
 */
void
CDistributeToAnalyzer::handleData(std::pair<size_t, void*>& info)
{
    gpAnalyzer->OnBuffer(info.first, info.second);
}