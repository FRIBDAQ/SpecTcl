#ifndef CPIPELINEMANAGER_H
#define CPIPELINEMANAGER_H
#include "TclGrammerApp.h"
#include <SpecTcl.h>
#include <TCLAnalyzer.h>
#include <map>

class CPipelineManager{
  
 public:

  typedef std::map <std::string, CTclAnalyzer::EventProcessingPipeline > MapEventProcessingPipeline;
  
  
  static CPipelineManager* getInstance();
  std::string fname;

  CTclAnalyzer::EventProcessingPipeline m_pipeline;
  MapEventProcessingPipeline m_pipelist;

  void showCommands();
  void createPipeline(std::string name);
  void listPipelineList();
  void listCurrentPipeline();
  void listAll();
  void getPipeline(std::string name_pipe);  
  void addProcessorPipeline(std::string name_pipe, const char* name_proc);
  void removeProcessorPipeline(std::string name_pipe, std::string name_proc);
  void removePipeline(std::string name_pipe);
  void clearPipeline(std::string name_pipe);
  void restorePipeline(std::string name_pipe);
  
 private:
  static CPipelineManager* m_pInstance;
  CPipelineManager(){}; // Default constructor
  CPipelineManager(const CPipelineManager& aCPipelineManager ){}; // Copy constructor
  virtual ~CPipelineManager(){}; // Destructor
  
  CPipelineManager& operator= (const CPipelineManager& aCPipelineManager); // Assignment operator.
  int operator==(const CPipelineManager& aCPipelineManager) const; // Equality operator. 
  int operator!=(const CPipelineManager& aCPipelineManager) const {
    return !(*this == aCPipelineManager);
  }

};


#endif
