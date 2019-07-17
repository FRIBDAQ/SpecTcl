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

/** @file:  CPipelineMaker.cpp
 *  @brief: Implement the base class.
 */
#include "CPipelineMaker.h"
#include <CPipelineManager.h>

static const std::string standardPipeline("default");

/**
 * constructor
 *    Create the "default" pipeline and make it current.
 */
CPipelineMaker::CPipelineMaker()
{
    CPipelineManager* pMgr = CPipelineManager::getInstance();
    
    pMgr->createPipeline(standardPipeline);
    pMgr->setCurrentPipeline(standardPipeline);
}

//////////////////////////////////////////////////////////////////////////////
// Utilities.

/**
 * RegisterEventProcessor
 *     -  Registers an event processor with the pipeline manager.
 *     -  Appends that processor to the "default" pipeline.
 *
 *  @param rEventProcessor - references the event processor to register.
 *  @param name            - mandatory event processor name.
 */
void
CPipelineMaker::RegisterEventProcessor(
    CEventProcessor& rEventProcessor, const char* name
)
{
    CPipelineManager* pMgr = CPipelineManager::getInstance();
    
    pMgr->registerEventProcessor(name, &rEventProcessor);
    pMgr->appendEventProcessor(standardPipeline, name);
}