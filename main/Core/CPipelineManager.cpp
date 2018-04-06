#ifdef HAVE_STD_NAMESPACE
using namespace std;
#endif

#include "CPipelineManager.h"

CPipelineManager* CPipelineManager::m_pInstance = NULL;
CPipelineManager* CPipelineManager::getInstance()
{
  if (!m_pInstance)
    m_pInstance = new CPipelineManager;

  return m_pInstance;
}

void
CPipelineManager::showCommands()
{
  std::cout << "Usage:" << std::endl;
  std::cout << "pman mk pipeline_name" << std::endl;
  std::cout << "pman get pipeline_name" << std::endl;   
  std::cout << "pman ls" << std::endl;  
  std::cout << "pman current" << std::endl;
  std::cout << "pman ls-all" << std::endl;
  std::cout << "pman add pipeline_name processor_name" << std::endl;   
  std::cout << "pman rm pipeline_name processor_name" << std::endl;
  std::cout << "pman rm-pipe pipeline_name" << std::endl;   
  std::cout << "pman clear pipeline_name" << std::endl;    
  std::cout << "pman restore pipeline_name" << std::endl;    

}

void
CPipelineManager::createPipeline(std::string name)
{
  SpecTcl* api = SpecTcl::getInstance();
  api->CreatePipeline(name);
}

void
CPipelineManager::listPipelineList()
{
  SpecTcl* api = SpecTcl::getInstance();  
  api->ListPipelineList();
}

void
CPipelineManager::listCurrentPipeline()
{
  SpecTcl* api = SpecTcl::getInstance();  
  api->ListCurrentPipeline();
}

void
CPipelineManager::listAll()
{
  SpecTcl* api = SpecTcl::getInstance();  
  api->ListAll();
}

void
CPipelineManager::getPipeline(std::string name_pipe)
{
  SpecTcl* api = SpecTcl::getInstance();  
  api->GetPipeline(name_pipe);
}

void
CPipelineManager::addProcessorPipeline(std::string name_pipe, const char* name_proc)
{
  SpecTcl* api = SpecTcl::getInstance();
  api->AddEventProcessor(name_pipe, *(new CEventProcessor)  ,name_proc);  
}

void 
CPipelineManager::removePipeline(std::string name_pipe)
{
  SpecTcl* api = SpecTcl::getInstance();
  api->RemovePipeline(name_pipe);
}  

void
CPipelineManager::removeProcessorPipeline(std::string name_pipe, std::string name_proc)
{
  SpecTcl* api = SpecTcl::getInstance();
  api->RemoveEventProcessor(name_pipe, name_proc);
}

void
CPipelineManager::clearPipeline(std::string name_pipe)
{
  SpecTcl* api = SpecTcl::getInstance();
  api->ClearPipeline(name_pipe);
}

void
CPipelineManager::restorePipeline(std::string name_pipe)
{
  SpecTcl* api = SpecTcl::getInstance();  
  api->RestorePipeline(name_pipe);
}
