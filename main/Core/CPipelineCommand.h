#ifndef CPIPELINECOMMAND_H
#define CPIPELINECOMMAND_H
#include <MPITclCommandAll.h>
#include <TCLObjectProcessor.h>
#include <TCLAnalyzer.h>
#include "CPipelineManager.h"
#include <iostream>
class CTCLInterpreter;
class CTCLObject;


class CPipelineCommandActual : public CTCLObjectProcessor
{

 private:
  CPipelineManager* m_manager;
  
 public: 
  void showCommands(CTCLInterpreter& interp, std::vector<CTCLObject>& objv);    
  void createPipeline(CTCLInterpreter& interp, std::vector<CTCLObject>& objv);  
  void listPipelines(CTCLInterpreter& interp, std::vector<CTCLObject>& objv);
  void listCurrentPipeline(CTCLInterpreter& interp, std::vector<CTCLObject>& objv);
  void listAll(CTCLInterpreter& interp, std::vector<CTCLObject>& objv);  
  void usePipeline(CTCLInterpreter& interp, std::vector<CTCLObject>& objv);  
  void addProcessor(CTCLInterpreter& interp, std::vector<CTCLObject>& objv);
  void clearPipeline(CTCLInterpreter& interp, std::vector<CTCLObject>& objv);  
  void removeProcessor(CTCLInterpreter& interp, std::vector<CTCLObject>& objv);
  void clonePipeline(CTCLInterpreter& interp, std::vector<CTCLObject>& objv);
  void listEvp(CTCLInterpreter& interp, std::vector<CTCLObject>& objv);
  void rmEvp(CTCLInterpreter& interp, std::vector<CTCLObject>& objv);

  CPipelineCommandActual(CTCLInterpreter& interp);
  int operator()(CTCLInterpreter& interp, std::vector<CTCLObject>& objv);
private:
    void listPipeline(CTCLObject& list, std::string pipename);

};

// Encapsulator  for MPI spectcl:

class CPipelineCommand : public CMPITclCommandAll {
public:
  CPipelineCommand(CTCLInterpreter& rInterp) ;
  ~CPipelineCommand() {}
};

#endif
