/*
    This software is Copyright by the Board of Trustees of Michigan
    State University (c) Copyright 2014-2018.

    You may use this software under the terms of the GNU public license
    (GPL).  The terms of this license are described at:

     http://www.gnu.org/licenses/gpl.txt

     Authors:
             Giordano Cerizza
             Ron Fox
             NSCL
             Michigan State University
             East Lansing, MI 48824-1321
*/

/**
 * @file CPipelineManager.h
 * @brief Define the pipeline manager singleton.
 */
#ifndef CPIPELINEMANAGER_H
#define CPIPELINEMANAGER_H
#include <TCLAnalyzer.h>
#include <map>
#include <vector>

class CPipelineManager{
  
public:

  typedef std::map <std::string, CTclAnalyzer::EventProcessingPipeline* > MapEventProcessingPipeline;
  typedef std::map <std::string, CEventProcessor*>                       MapEventProcessors;

  // Instance data:
private:
  MapEventProcessingPipelines            m_pipelines;
  MapEventProcessors                     m_processors;
  CTCLAnalyzer::EventProcessingPipeline* m_pCurrentPipeline;
  std::string                            m_currentPipelineName;
  
  static CPipelineManager*              m_pInstance;
  // API:
public:
  static CPipelineManager* getInstance();
  
  void registerEventProcessor(const std::string& name, CEventProcessor* pProcessor);
  void createPipeline(const std::string& name);
  
  void appendEventProcessor(const std::string& pipeName, const std::string& evpName);
  void insertEventProcessor(
    const std::string& pipename, const std::string& evpname,
    CTclAnalyzer::EventProcessorIterator where
  );

  void removeEventProcessor(const std::string& pipename, const std::string& evpname);
  void removeEventProcessor(const std::string& pipename, CTclAnalyzer::EventProcessorIterator here);
  
  void setCurrentPipeline(const std::string& pipename);
  
  void clonePipeline(const std::string& from, const std::string& to);

  // inquiries:
  
  CTclAnalyzer::EventProcessingPipeline* getCurrentPipeline();
  std::string                            getCurrentPipelineName() const;
  std::vector<std::string>               getPipelineNames() const;
  std::vector<std::string>               getEventProcessorNames() const;
  std::vector<std::string>               getEventProcesorsInPipeline(const std::string& pipename) const;
  std::string                            lookupEventProcessor(const CEventProcessor* p) const;
  
  size_t pipelineCount() const;
  size_t eventProcessorCount() const;
  
  MapEventProcessingPipeline::const_iterator pipelineBegin() const;
  MapEventProcessingPipeline::const_iterator piplineEnd() const;
  
  MapEventProcessors::const_iterator processorsBegin() const;
  MapEventProcessors::const_iterator processorsEnd() const;

private:
  CPipelineManager();
  ~CPipelineManager();          // Singletons are always final.
};


#endif
