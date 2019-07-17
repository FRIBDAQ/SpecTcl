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

/** @file:  CPipelineMaker.h
 *  @brief: ABC for user to create the analysis pipeline.
 */
#ifndef CPIPELINEMAKER_H
#define CPIPELINEMAKER_H

class CEventProcessor;

/**
 * @class CPipelineMaker
 *     This class is an abstract base class that the user must concretize
 *     and instantiate to set up the analysis pipeline(s).
 *
 *     The base class will set up a default analysis pipeline and
 *     provides base class methods for adding pipeline elements to that
 *     pipeline.  The user could, if they desire, create additional pipelines
 *     and set them up though to do that they'd need to know how to use
 *     the CPipelineManager class.
 *
 *     The operator() is a rough equivalent of the CreateAnalysisPipeline
 *     class for CMySpecTclApp
 *
 *     Note that any registered event processors are available for use
 *     with other pipelines via the pipeline command package.
 */
class CPipelineMaker
{
public:
    CPipelineMaker();
    
    // User must provide this and ensure it's called on an instance
    // of class
    
    virtual void operator()() = 0;            // Create the pipelines
protected:
    void RegisterEventProcessor(              // Require named processors.
        CEventProcessor& rEventProcessor, const char* name
    );
};

#endif