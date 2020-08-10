/*
    This software is Copyright by the Board of Trustees of Michigan
    State University (c) Copyright 2019.

    You may use this software under the terms of the GNU public license
    (GPL).  The terms of this license are described at:

     http://www.gnu.org/licenses/gpl.txt

     Author:
             Giordano Cerizza
             NSCL
             Michigan State University
             East Lansing, MI 48824-1321
*/

#include <utility>
#include <memory>
#include <iostream>
#include <unistd.h>
#include <chrono>
#include <map>

#include "zhelpers.hpp"
#include "CRingFileBlockReader.h"
#include "TclGrammerApp.h"
#include "TKRunControl.h"
#include "RunControl.h"
#include "EventProcessor.h"
#include "ThreadAnalyzer.h"
#include "CRingBufferDecoder.h" 
#include <BufferTranslator.h>

extern int NBR_WORKERS;
extern bool debug;

typedef std::pair<std::string, CEventProcessor*>                       PipelineElement;
typedef std::list<PipelineElement>                                     EventProcessingPipeline;
typedef std::map <std::string, EventProcessingPipeline >               MapEventProcessingPipeline;
typedef EventProcessingPipeline::iterator                              EventProcessorIterator;
typedef std::map <std::string, CEventProcessor*>                       MapEventProcessors;

typedef std::vector<std::pair<unsigned int, double>> Vpairs;

extern double start_time, stop_time;
extern pthread_key_t glob_var_key;

class CRingFormatHelper;
class CRingFormatHelperFactory;

struct arg_struct {
  int thread_id;
  int thread_state;
  zmq::context_t* thread_ctx;
};

class ZMQRDClass
{
 private:

  static ZMQRDClass* m_pInstance;
  ZMQRDClass(); 
  virtual ~ZMQRDClass();
  
  static CTCLVariable* m_pBuffersAnalyzed; // # buffers analyzed.
  static int  m_nBuffersAnalyzed; // linkedto m_pBuffersAnalyzed.
  static CTCLVariable* m_pRunNumber;
  static CTCLVariable* m_pRunTitle;
  static CTCLVariable* m_pRunState;
  static CTCLVariable* m_pAverageRate;    
  static CTCLVariable* m_pElapsedTime;  

  int m_nFd;  
  static int m_threadState;  
  
 public: 

  static ZMQRDClass* getInstance();
  void ResizeAll();
  
  void setFd(int fd);
  int  getFd();  
  static void setThreadState(int state); // 0: running 1: abrupt exit mode
  static int getThreadState();
  
  MapEventProcessors                 m_processors;
  static EventProcessingPipeline*    m_pipeline;
  static BufferTranslator*           m_pTranslator;

  void* m_data;
  void  RegisterData(void*);
  void* GetData();
  
  static std::vector<float> tmpBytes;
  static std::vector<size_t> threadBytes;
  static std::vector<size_t> threadItems;
  static std::vector<size_t> physicsItems;
  static std::vector<size_t>  entityItems;

  static CRingFormatHelperFactory* m_pFactory;  

  static void* sender_task(void *arg);  
  static void* worker_task(void *args);

  uint32_t*    pBuffer;

  void OnInitialize();
  void addEventProcessor(CEventProcessor& eventProcessor, const char* name_proc = 0);
  static void createTranslator(uint32_t* pBuffer);
  static void processRingItems(long thread, CRingFileBlockReader::pDataDescriptor descrip, void* pData, Vpairs& vec);
  static void marshall(long thread, CEventList& lst, Vpairs& vec);
  static void histoData(long thread, Vpairs& vec);
  static int HistogramHandler(Tcl_Event* evPtr, int flags);
  static double clock();
  static uint64_t converter(const char* s);

  static EventProcessingPipeline* getPipeline();
  static void clonePipeline(EventProcessingPipeline* copy, EventProcessingPipeline* source);
  
  static inline uint64_t hrDiff(const timespec& end, const timespec& start)
  {
    timespec temp;
    if ((end.tv_nsec-start.tv_nsec)<0) {
      temp.tv_sec = end.tv_sec-start.tv_sec-1;
      temp.tv_nsec = 1000000000+end.tv_nsec-start.tv_nsec;
    } else {
      temp.tv_sec = end.tv_sec-start.tv_sec;
      temp.tv_nsec = end.tv_nsec-start.tv_nsec;
    }
    return (uint64_t)((temp.tv_sec * 1.0e9) + temp.tv_nsec);
  }
  static void show_progress_bar(std::ostream& os, float bytes, size_t items, std::string message, char symbol);
  
  
  void printStats();
  void finish();
  static int workDone();
  
  static CEventList m_eventPool;
  static CEventList m_eventList;    
  static CEvent* CreateEvent();
  static void ClearEventList();

  static void SetVariable(CTCLVariable& rVar, int newval);
  
 private:
  static UInt_t mapType(UInt_t type);

  //! Match pipeline element by name.
    class MatchName
    {
    private:
      std::string       m_sName;
      CEventProcessor*  m_pLastMatch;
    public:
      MatchName(std::string name);
      bool operator()(PipelineElement& element);
      CEventProcessor*  getLastMatch() const;
    };
    //! Match pipeline element by pointer.
      class MatchAddress
      {
      private:
	CEventProcessor* m_pProcessor;
	CEventProcessor* m_pLastMatch;
      public:
	MatchAddress(CEventProcessor& processor);
	bool operator()(PipelineElement& element);
	CEventProcessor* getLastMatch() const;
      };
};
