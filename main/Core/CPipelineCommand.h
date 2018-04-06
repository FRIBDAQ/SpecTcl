#ifndef CPIPELINECOMMAND_H
#define CPIPELINECOMMAND_H
#include <TCLObjectProcessor.h>
#include <TCLAnalyzer.h>
#include "CPipelineManager.h"
#include <iostream>
class CTCLInterpreter;
class CTCLObject;


class CPipelineCommand : public CTCLObjectProcessor
{

 private:
  CPipelineManager* m_manager = CPipelineManager::getInstance();
  CPipelineManager::MapEventProcessingPipeline m_activepipelist;
  CTclAnalyzer::EventProcessingPipeline m_activepipe;

 public: 
  void showCommands(CTCLInterpreter& interp, std::vector<CTCLObject>& objv);    
  void createPipeline(CTCLInterpreter& interp, std::vector<CTCLObject>& objv);  
  void listPipelineList(CTCLInterpreter& interp, std::vector<CTCLObject>& objv);
  void listCurrentPipeline(CTCLInterpreter& interp, std::vector<CTCLObject>& objv);
  void listAll(CTCLInterpreter& interp, std::vector<CTCLObject>& objv);  
  void getPipeline(CTCLInterpreter& interp, std::vector<CTCLObject>& objv);  
  void addProcessorPipeline(CTCLInterpreter& interp, std::vector<CTCLObject>& objv);
  void clearPipeline(CTCLInterpreter& interp, std::vector<CTCLObject>& objv);  
  void removeProcessorPipeline(CTCLInterpreter& interp, std::vector<CTCLObject>& objv);
  void removePipeline(CTCLInterpreter& interp, std::vector<CTCLObject>& objv);  
  void restorePipeline(CTCLInterpreter& interp, std::vector<CTCLObject>& objv);

  CPipelineCommand(CTCLInterpreter& interp);
  int operator()(CTCLInterpreter& interp, std::vector<CTCLObject>& objv); 

};

#endif
