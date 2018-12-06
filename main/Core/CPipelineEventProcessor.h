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

/** @file:  CPipelineEventProcessor.h
 *  @brief: Event processor that encapsulates an event processing pipeline.
 */
#ifndef CPIPELINEEVENTPROCESSOR_H
#define CPIPELINEEVENTPROCESSOR_H

#include "EventProcessor.h"
#include "TCLAnalyzer.h"

/**
 * @class CPipelineEventProcessor
 *
 * This event processor encapsulates a complete event processing pipeline.
 * For each method associated with event processors, it just traverses
 * the list of processors in the pipeline calling the corresponding method.
 *
 * The primary purpose of this processor is to support registering a full
 * event processing pipeline to analyze the data from an event source in the
 * CEventBuilderEventProcessor class.
 *
 * This is also used in the commands to manage constructing event processors
 * of that sort.
 */
class CPipelineEventProcessor : public CEventProcessor
{
private:
    CTclAnalyzer::EventProcessingPipeline* m_pPipeline;
public:
    CPipelineEventProcessor(CTclAnalyzer::EventProcessingPipeline* pipe);
    virtual ~CPipelineEventProcessor();
    
public:
  virtual Bool_t operator()(const Address_t pEvent,
                            CEvent& rEvent,
                            CAnalyzer& rAnalyzer,
                            CBufferDecoder& rDecoder); // Physics Event.

  // Functions:
  virtual Bool_t OnAttach(CAnalyzer& rAnalyzer); // Called on registration.
  virtual Bool_t OnDetach(CAnalyzer& rAnalyzer);
  virtual Bool_t OnBegin(CAnalyzer& rAnalyzer,
                         CBufferDecoder& rDecoder); // Begin Run.
  virtual Bool_t OnEnd(CAnalyzer& rAnalyzer,
                       CBufferDecoder& rBuffer); // End Run.
  virtual Bool_t OnPause(CAnalyzer& rAnalyzer,
                         CBufferDecoder& rDecoder); // Pause Run.
  virtual Bool_t OnResume(CAnalyzer& rAnalyzer,
                          CBufferDecoder& rDecoder); // Resume Run.
  virtual Bool_t OnOther(UInt_t nType,
                         CAnalyzer& rAnalyzer,
                         CBufferDecoder& rDecoder); // Unrecognized buftype.

  virtual Bool_t OnEventSourceOpen(std::string name);
  virtual Bool_t OnEventSourceEOF();
  virtual Bool_t OnInitialize();

};

#endif