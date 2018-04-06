#include "CPipelineCommand.h"
#include <TCLInterpreter.h>
#include <TCLObject.h>

CPipelineCommand::CPipelineCommand(CTCLInterpreter& interp) :
  CTCLObjectProcessor(interp, "pman" , true)
{
  /*
  m_manager->createPipeline(m_activepipelist,"Default");
  m_manager->createPipeline(m_activepipelist,"Default-filt");  
  */
  m_manager->createPipeline("Default-filt");    
}

int
CPipelineCommand::operator()(CTCLInterpreter& interp, std::vector<CTCLObject>& objv)
{
  bindAll(interp, objv);

  int nArgs = objv.size();
  if (nArgs < 2)
    showCommands(interp, objv);            
  else {
    std::string subcommand = objv[1];    
    try{
      if (subcommand == "mk"){
	createPipeline(interp, objv);      
      } else if (subcommand == "ls"){
	listPipelineList(interp, objv);      
      } else if (subcommand == "current"){
	listCurrentPipeline(interp, objv);
      } else if (subcommand == "ls-all"){
	listAll(interp, objv);
      } else if (subcommand == "get"){
	getPipeline(interp, objv);      	
      } else if (subcommand == "add"){
	addProcessorPipeline(interp, objv);
      } else if (subcommand == "rm"){
	removeProcessorPipeline(interp, objv);
      } else if (subcommand == "rm-pipe"){
	removePipeline(interp, objv);	
      } else if (subcommand == "clear"){
	clearPipeline(interp, objv);            
      } else if (subcommand == "restore"){
	restorePipeline(interp, objv);
      } else {                                                  
	throw std::string("Invalid command!!");
      }
    }
    catch (...) {
      interp.setResult("Something went wrong"); 
      return TCL_ERROR;
    }
  }
  return TCL_OK;
}

void
CPipelineCommand::showCommands(CTCLInterpreter& interp, std::vector<CTCLObject>& objv)
{
  m_manager->showCommands();
}

void
CPipelineCommand::createPipeline(CTCLInterpreter& interp, std::vector<CTCLObject>& objv)
{
  requireExactly(objv, 3);  
  const std::string name = objv[2];
  m_manager->createPipeline(name);  
}

void
CPipelineCommand::listPipelineList(CTCLInterpreter& interp, std::vector<CTCLObject>& objv)
{
  requireExactly(objv, 2);  
  m_manager->listPipelineList();
}

void
CPipelineCommand::listCurrentPipeline(CTCLInterpreter& interp, std::vector<CTCLObject>& objv)
{
  requireExactly(objv, 2);  
  m_manager->listCurrentPipeline();
} 

void
CPipelineCommand::listAll(CTCLInterpreter& interp, std::vector<CTCLObject>& objv)
{
  requireExactly(objv, 2);  
  m_manager->listAll(); 
}

void
CPipelineCommand::getPipeline(CTCLInterpreter& interp, std::vector<CTCLObject>& objv)
{
  requireExactly(objv, 3);  
  const std::string name_pipe = objv[2];
  m_manager->getPipeline(name_pipe); 
}

void
CPipelineCommand::addProcessorPipeline(CTCLInterpreter& interp, std::vector<CTCLObject>& objv)
{
  requireExactly(objv, 4);
  const std::string name_pipe = objv[2];
  const std::string name_proc = objv[3];
  
  const char *name_p = name_proc.c_str();
  m_manager->addProcessorPipeline(name_pipe, name_p);
}

void
CPipelineCommand::removeProcessorPipeline(CTCLInterpreter& interp, std::vector<CTCLObject>& objv)
{
  requireExactly(objv, 4); 
  const std::string name_pipe = objv[2];
  const std::string name_proc = objv[3];    
  m_manager->removeProcessorPipeline(name_pipe, name_proc);
}

void
CPipelineCommand::removePipeline(CTCLInterpreter& interp, std::vector<CTCLObject>& objv)
{
  requireExactly(objv, 3); 
  const std::string name_pipe = objv[2];
  m_manager->removePipeline(name_pipe);
}

void
CPipelineCommand::clearPipeline(CTCLInterpreter& interp, std::vector<CTCLObject>& objv)
{
  requireExactly(objv, 3);
  const std::string name_pipe = objv[2];  
  m_manager->clearPipeline(name_pipe);
}

void
CPipelineCommand::restorePipeline(CTCLInterpreter& interp, std::vector<CTCLObject>& objv)
{
  requireExactly(objv, 3);
  const std::string name_pipe = objv[2];  
  m_manager->restorePipeline(name_pipe);
}
