#include <memory>
#include <iostream>
#include <unistd.h>
#include <map>

#include "zhelpers.hpp"
#include "CRingFileBlockReader.h"
#include "TclGrammerApp.h"
#include "TKRunControl.h"
#include "RunControl.h"
#include "EventProcessor.h"
#include <BufferTranslator.h>

extern const int NBR_WORKERS;

typedef std::pair<std::string, CEventProcessor*>                       PipelineElement;
typedef std::list<PipelineElement>                                     EventProcessingPipeline;
typedef std::map <std::string, EventProcessingPipeline >               MapEventProcessingPipeline;
typedef EventProcessingPipeline::iterator                              EventProcessorIterator;
typedef std::map <std::string, CEventProcessor*>                       MapEventProcessors;

class Sender
{
 private:

  static Sender* m_pInstance;
  Sender(); 
  virtual ~Sender();

 public: 
  static Sender* getInstance();

  int m_nFd;  
  void setFd(int fd);
  int  getFd();  

  MapEventProcessors                 m_processors;
  static EventProcessingPipeline*    m_pipeline;
  static BufferTranslator*           m_pTranslator;
  
  static size_t* threadBytes;
  static size_t* threadItems;
  static size_t* physicsItems;  
  
  static void* sender_task(void *args);  
  static void* worker_task(void *args);

  uint32_t*    pBuffer;
  
  void AddEventProcessor(CEventProcessor& eventProcessor, const char* name_proc = 0);
  static void  createTranslator(uint32_t* pBuffer);
  static void  CopyPipeline(EventProcessingPipeline& oldpipe, EventProcessingPipeline& newpipe);
  static void  processRingItems(long thread, CRingFileBlockReader::pDataDescriptor descrip, void* pData);
  
  void finish();
  
};
