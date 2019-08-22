#include <map>
#include <regex>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <limits>
#include <chrono>
#include <buftypes.h>
#include "DataFormat.h"
#include "CRingBufferDecoder.h"
#include "ZMQSenderClass.h"
#include "SpecTcl.h"
#include "ThreadAPI.h"
#include "ThreadAnalyzer.h"
#include "BufferDecoder.h"
#include "SpectrumDictionaryFitObserver.h"
#include "TclGrammerApp.h"
#include "RingFormatHelper.h"
#include "RingFormatHelper11Creator.h"
#include "RingFormatHelperFactory.h"

int status;
bool isDone = false;
bool debug = false;
bool isStart = true;
Sender* Sender::m_pInstance = 0;

int NBR_WORKERS = 1;
int CHUNK_SIZE  = 1024*512;
UInt_t m_nParametersInEvent = 512;
CEventList Sender::m_eventPool;
CEventList Sender::m_eventList;

double qnan = std::numeric_limits<double>::quiet_NaN();
double stop_time, start_time, tdiff, etime;

CTCLApplication* gpTCLApplication;

// statistics 
static size_t bytesSent(0);
std::vector<size_t> Sender::threadBytes(NBR_WORKERS);
std::vector<size_t> Sender::threadItems(NBR_WORKERS);
std::vector<size_t> Sender::physicsItems(NBR_WORKERS);
std::vector<size_t> Sender::entityItems(NBR_WORKERS);

CTCLVariable* Sender::m_pBuffersAnalyzed;
CTCLVariable* Sender::m_pRunNumber;
CTCLVariable* Sender::m_pRunTitle;
CTCLVariable* Sender::m_pRunState;
CTCLVariable* Sender::m_pElapsedTime;
int  Sender::m_nBuffersAnalyzed(0);

size_t totalBytes(0);
size_t totalItems(0);
size_t totalPhysicsItems(0);
size_t totalEntityItems(0);

CRingFormatHelperFactory* Sender::m_pFactory(new CRingFormatHelperFactory);
BufferTranslator* Sender::m_pTranslator;
EventProcessingPipeline* Sender::m_pipeline(new EventProcessingPipeline);

static CEventList* pEventList;
typedef struct _HistoEvent {
  Tcl_Event    tclEvent;
  Vpairs*      histoList;
} HistoEvent, *pHistoEvent;

void
Sender::ResizeAll()
{
  threadBytes.resize(NBR_WORKERS);
  threadItems.resize(NBR_WORKERS);
  physicsItems.resize(NBR_WORKERS);
  entityItems.resize(NBR_WORKERS);

  for (int i=0; i<NBR_WORKERS; ++i){
    threadBytes[i] = 0;
    threadItems[i] = 0;
    physicsItems[i] = 0;
    entityItems[i] = 0;    
  }
  
}

int
Sender::converter(const char* s)
{
  std::string ss = s;
  std::regex e(R"(\d+)");

  std::sregex_iterator iter(ss.begin(), ss.end(), e);
  std::sregex_iterator end;

  int value = 1;
  while(iter != end)
    {
      for(unsigned i = 0; i < iter->size(); ++i)
	{
	  value *= std::stoi((*iter)[i]);
	}
      ++iter;
    }

  return value;
}

Sender::Sender()
{
  // Resizing all the vectors according to SpecTclInit.tcl
  if (debug)
    std::cout << "Original NBR_WORKERS " << NBR_WORKERS << std::endl;
  int nthreads = CTclGrammerApp::getInstance()->getNthreads();
  std::cout << "CHUNK_SIZE: " << CHUNK_SIZE << std::endl;
  const char* c_size = CTclGrammerApp::getInstance()->getDataChunkSizeVar().Get();
  std::cout << "const char* c_size: " << c_size << std::endl;
  int size = converter(c_size);
  std::cout << "int size: " << size << std::endl;  
  CHUNK_SIZE = size;
  std::cout << "CHUNK_SIZE: " << CHUNK_SIZE << std::endl;  
  
  if (debug)
    std::cout << "Number of threads requested via SpecTclInit.tcl " << nthreads << std::endl;
  if (nthreads != NBR_WORKERS){
    NBR_WORKERS = nthreads;
    if (debug)
      std::cout << "New NBR_WORKERS " << NBR_WORKERS << std::endl;
    ResizeAll();
  }

  // Register the creators for 11.x ring with body headers
  CRingFormatHelper11Creator create11;
  m_pFactory->addCreator(11, 0, create11);
  
  // Create event list
  pEventList = new CEventList[NBR_WORKERS];

  // TCL Variables for GUI
  SpecTcl *pApi = SpecTcl::getInstance();
  CTCLInterpreter* rInterp = pApi->getInterpreter();

  m_pBuffersAnalyzed = new CTCLVariable(rInterp, std::string("BuffersAnalyzed"), kfFALSE);
  m_pBuffersAnalyzed->Link(&m_nBuffersAnalyzed, TCL_LINK_INT);
  m_nBuffersAnalyzed = 0;  

  m_pRunNumber = new CTCLVariable(rInterp, std::string("RunNumber"), kfFALSE);
  ClearVariable(*m_pRunNumber);

  m_pRunTitle = new CTCLVariable(rInterp, std::string("RunTitle"), kfFALSE);
  m_pRunTitle->Set(">>> Unknown <<<");

  m_pRunState = new CTCLVariable(rInterp, std::string("OnlineState"), kfFALSE);
  m_pRunState->Set(">>> Unknown <<<");

  m_pElapsedTime = new CTCLVariable(rInterp, std::string("ElapsedTime"), kfFALSE);
  m_pElapsedTime->Set("0");

}

Sender::~Sender()
{
}

CEvent*
Sender::CreateEvent()
{
  CEventVector& rVec(m_eventPool.getVector());
  CEvent* pEvent;
  if(rVec.empty()) {
    pEvent =  new CEvent(m_nParametersInEvent);
    
  } else {
    pEvent = rVec.back();
    rVec.pop_back();
  }
  if(pEvent) pEvent->clear(); // BUG prevention.
  else pEvent= new CEvent(m_nParametersInEvent);
  return pEvent;
}

void
Sender::ClearEventList()
{
  CEventVector& evlist(m_eventList.getVector());
  CEventVector& evpool(m_eventPool.getVector());
  CEventListIterator p = evlist.begin();
  for(; p != evlist.end(); p++) {
    if(*p) {
      CEvent* pEvent = *p;
      evpool.push_back(pEvent);
      *p = (CEvent*)kpNULL;
    }
  }
}

double
Sender::clock()
{
  struct timeval t;
  gettimeofday(&t, NULL);
  return (1.0e-6*t.tv_usec + t.tv_sec);
}

Sender*
Sender::getInstance() 
{
  if(!m_pInstance) {
    m_pInstance = new Sender();
  }
  // Regardless return it to the caller.
  return m_pInstance;
  
}

void
Sender::setFd(int fd)
{
  m_nFd = fd;
}

int
Sender::getFd()
{
  return m_nFd;
}

void
Sender::createTranslator(uint32_t* pBuffer)
{
  pRingItem pItem = reinterpret_cast<pRingItem>(pBuffer);

  //  delete m_pTranslator;
  if (pItem->s_header.s_type & 0xffff0000) {
    m_pTranslator = new SwappingBufferTranslator(pBuffer);
  }
  else {
    m_pTranslator = new NonSwappingBufferTranslator(pBuffer);
  }
}

void
Sender::addEventProcessor(CEventProcessor& eventProcessor, const char* name_proc)
{

  static unsigned evp_nameSerial(0);
  std::stringstream nameStream;
  std::string       nameString;
  if(!name_proc) {
    nameStream << "_anonymous_event_processor_" << evp_nameSerial++;
    nameString = nameStream.str();
    name_proc = nameString.c_str();
    std::cerr << "Assigning event processor name: " << name_proc <<std::endl;
  }
  
  // register the processors
  try {
    if(m_processors.count(name_proc)) {
      std::string msg = "Duplicate event processor name: ";
      msg += name_proc;
      throw std::logic_error(msg);
    }
    m_processors[name_proc] = &eventProcessor;
  }
  catch (...) {}

  // append the processors
  MapEventProcessors::iterator evp  = m_processors.find(name_proc);
  m_pipeline->push_back(PipelineElement(evp->first, evp->second));

}

void
Sender::marshall(long thread, CEventList& lst, Vpairs& vec)
{
  DopeVector dp;
  CEventVector& evlist(lst.getVector());
  CEventListIterator p = evlist.begin();
  for(; p != evlist.end(); p++) {
    if(*p) {
      CEvent* pEvent = *p;
      dp = pEvent->getDopeVector();
      // add size of the dope vector and id
      vec.push_back(std::make_pair(dp.size(), qnan));
      for (int i =0; i < dp.size(); i++){ 
	unsigned int n = dp[i];
	double value = (*pEvent)[n];
	//	std::cout << n << " " << value << std::endl;
	vec.push_back(std::make_pair(n, value));
      }
    }
  }
}

void
Sender::processRingItems(long thread, CRingFileBlockReader::pDataDescriptor descrip, void* pData, Vpairs& vec)
{
  uint32_t*    pBuffer = reinterpret_cast<uint32_t*>(pData);
  uint32_t*    pBufferCursor = pBuffer;
  uint32_t     nResidual = descrip->s_nBytes; 

  CRingFormatHelperFactory* m_factory;
  CRingFormatHelper* pHelper;
  
  EventProcessingPipeline* pipecopy;
  BufferTranslator* m_translator;

  //////////////////////////////////////
  // Buffer translator
  //////////////////////////////////////  
  try {
    Sender::createTranslator(pBuffer);
    if (!m_translator){    
      if (debug)
	std::cout << "Created translator " << thread << std::endl;
      m_translator = m_pTranslator->clone();
    }
    else {
      m_translator->newBuffer(pBuffer);
    }
  }
  catch (...) {
    std::cerr << "SpecTcl exiting due to buffer decoder exception\n";
    exit(EXIT_FAILURE);
  }

  //////////////////////////////////////
  // Buffer decoder + analyzer 
  //////////////////////////////////////    
  CThreadAnalyzer* pAnalyzer(new CThreadAnalyzer);
  CBufferDecoder* pDecoder(new CRingBufferDecoder);
  if (!pDecoder){
    if (debug)
      std::cout << "Created decoder " << thread << std::endl;
    pAnalyzer->AttachDecoder(*pDecoder);
    if (debug)
      std::cout << "...and attached to the threaded analyzer" << std::endl;
  }
  
  //////////////////////////////////////
  // Analysis pipeline
  //////////////////////////////////////  
  pipecopy = new EventProcessingPipeline(*m_pipeline);
  if (debug)
    std::cout << "Created pipecopy " << thread << " of size " << pipecopy->size() << std::endl;      
  
  Address_t    pBody;
  UInt_t       nBodySize;

  std::string  m_title;         // Last title gotten from a transition event.
  UInt_t       m_runNumber;     // Last run number   ""    ""
  UInt_t       m_nCurrentItemType;
  
  while(nResidual) {
    
    pRingItem        pItem = reinterpret_cast<pRingItem>(pBufferCursor);
    uint32_t         size  = m_translator->TranslateLong(pItem->s_header.s_size);
    uint32_t         type  = m_translator->TranslateLong(pItem->s_header.s_type);
    m_translator->newBuffer(pItem);
    
    // RingHelper
    m_factory = new CRingFormatHelperFactory(*m_pFactory);
    pHelper = m_factory->create(11,0);    
    
    pBody = pHelper->getBodyPointer(pItem);
    nBodySize = size - (reinterpret_cast<uint8_t*>(pBody) - reinterpret_cast<uint8_t*>(pItem));

    m_nCurrentItemType     = mapType(type);
    switch (type) {
    case BEGIN_RUN:
      {
	m_pRunState->Set("Active");
	physicsItems[thread] = 0;
	m_title        = pHelper->getTitle(pItem);
	m_runNumber    = pHelper->getRunNumber(pItem, m_translator);	
	Sender::SetVariable(*m_pRunNumber, m_runNumber);
	m_pRunTitle->Set(m_title.c_str());
      }
      break;
    case END_RUN:
      {
	m_pRunState->Set("Halted");
      }
      break;
    case PAUSE_RUN:
      {
	m_pRunState->Set("Paused");
      }
      break;
    case RESUME_RUN:
      {
	m_pRunState->Set("Active");
	entityItems[thread] = 0;
	m_title        = pHelper->getTitle(pItem);
	m_runNumber    = pHelper->getRunNumber(pItem, m_translator);	
	Sender::SetVariable(*m_pRunNumber, m_runNumber);
	m_pRunTitle->Set(pDecoder->getTitle().c_str());		
      }
      break;
    case PACKET_TYPES:
    case MONITORED_VARIABLES:
      {
	entityItems[thread] = pHelper->getStringCount(pItem, m_translator);	
      }
      break;
    case PERIODIC_SCALERS:
      {
	entityItems[thread] = pHelper->getScalerCount(pItem, m_translator);	
      }
      break;
    case PHYSICS_EVENT:
      {
	entityItems[thread] = 1;
	physicsItems[thread]++;
	pAnalyzer->OnPhysics(thread, *pDecoder, nBodySize, pBody, *pipecopy, *m_translator, pEventList[thread]);	
	marshall(thread, pEventList[thread], vec);
      }
      break;
    case PHYSICS_EVENT_COUNT:
      {
      }
      break;
    default:
      {
      }
      break;
    }
    pBufferCursor = reinterpret_cast<uint32_t*>(reinterpret_cast<uint8_t*>(pBufferCursor) + size);
    nResidual    -= size;
  }

  delete m_factory;
  delete m_translator;
  delete pipecopy;
  delete pAnalyzer;
  delete pDecoder;

}

UInt_t
Sender::mapType(UInt_t type)
{
  static bool                mapSetup(false);
  static std::map<int, int>  typeMapping;

  if (!mapSetup) {
    typeMapping[BEGIN_RUN]           = BEGRUNBF;
    typeMapping[END_RUN]             = ENDRUNBF;
    typeMapping[PAUSE_RUN]           = PAUSEBF;
    typeMapping[RESUME_RUN]          = RESUMEBF;
    typeMapping[PACKET_TYPES]        = PKTDOCBF;
    typeMapping[MONITORED_VARIABLES] = RUNVARBF;
    typeMapping[PERIODIC_SCALERS]    = SCALERBF;
    typeMapping[PHYSICS_EVENT]       = DATABF;
      
    mapSetup = true;
  }
  if (typeMapping.find(type) != typeMapping.end()) {
    return typeMapping[type];
  }
  else {
    return type;
  }
}

////////////////////////////////////////////////////////////////////////
///// This version histograms inside the Tcl Thread
////////////////////////////////////////////////////////////////////////
void
Sender::histoData(long thread, Vpairs& vec)
{
  Tcl_ThreadId tid = CTclGrammerApp::getInstance()->getThread();
  
  pHistoEvent pHEvent = reinterpret_cast<pHistoEvent>(Tcl_Alloc(sizeof(HistoEvent)));
  pHEvent->tclEvent.proc = Sender::HistogramHandler;
  pHEvent->histoList     = &vec;

  Tcl_ThreadQueueEvent(tid,
		       reinterpret_cast<Tcl_Event*>(pHEvent),
		       TCL_QUEUE_TAIL);

}

int
Sender::HistogramHandler(Tcl_Event* evPtr, int flags)
{
  pHistoEvent pEvent = reinterpret_cast<pHistoEvent>(evPtr);
  Vpairs* hlist = pEvent->histoList;
  
  SpecTcl *pApi = SpecTcl::getInstance();
  CHistogrammer* hist = pApi->GetHistogrammer();  
  
  // convert Vpairs* to CEvent*
  unsigned int index, size;
  double value;
  UInt_t nEventNo = 0;

  CEvent* e = 0;
  for (const std::pair<unsigned int, double> &evt : *hlist){
    index = evt.first;
    value = evt.second;
    if (std::isnan(value)){
      size = index;
      e = Sender::CreateEvent();
      continue;
    }
    (*e)[index] = value;
    if(size == e->getDopeVector().size()){
      m_eventList[nEventNo] = e;
      hist->operator()(*e); 
      Sender::ClearEventList();
    }
  }
  
  delete hlist;

  while (Tcl_DoOneEvent(TCL_WINDOW_EVENTS | TCL_TIMER_EVENTS | TCL_DONT_WAIT | TCL_IDLE_EVENTS))
    ;
  
  return 1; 
}

void *
Sender::worker_task(void *args)
{
  long thread = (long)(args);
  long* p = (long*)malloc(sizeof(long));
  *p = thread;
  pthread_setspecific(glob_var_key, p);

  zmq::context_t context(1);
  int linger(0);
  ////////////////////////////////////////////
  // ROUTER/DEALER PATTERN TO GET THE DATA
  ////////////////////////////////////////////  
  zmq::socket_t worker(context, ZMQ_DEALER);
  worker.setsockopt(ZMQ_LINGER, &linger, sizeof(int));
  std::string id1 = s_set_id(worker);          //  Set a printable identity
  worker.connect("tcp://localhost:5671");

  const char* c_size = CTclGrammerApp::getInstance()->getDataChunkSizeVar().Get();
  int size = converter(c_size);
  
  if (CHUNK_SIZE != size){
    std::cout << "Inside worker_task CHUNK_SIZE before " << CHUNK_SIZE << std::endl;
    CHUNK_SIZE = size;
    std::cout << "Inside worker_task CHUNK_SIZE after " << CHUNK_SIZE << std::endl;    
  }
  
  std::stringstream ChunkSize;
  ChunkSize << CHUNK_SIZE;
  size_t bytes = 0;
  size_t nItems = 0;
  int total = 0;
  while (1) {

    Vpairs* tmp = new Vpairs;
    
    s_sendmore(worker, "");
    s_sendmore(worker, "fetch");
    s_send(worker, ChunkSize.str());                 // Size of workload

    // Work items are in two types.  all start with delimetr and type.
    // Type eof means we're done and need to clean up.
    // type data means there's two more segments, the descriptor and the data.

    s_recv(worker);                               // Delimeter.
    std::string type = s_recv(worker);

    if (isStart){
      start_time = Sender::clock();
      isStart = false;
    }
      
    if (type == "eof") {
      break;
    } else if (type == "data") {
      zmq::message_t descriptor;
      zmq::message_t bulkData;

      worker.recv(&descriptor);
      worker.recv(&bulkData);

      void* pRingItems = bulkData.data();
      CRingFileBlockReader::pDataDescriptor pDescriptor =
	reinterpret_cast<CRingFileBlockReader::pDataDescriptor>(descriptor.data());

      nItems += pDescriptor->s_nItems;
      bytes  += pDescriptor->s_nBytes;

      Sender::processRingItems(thread, pDescriptor, pRingItems, *tmp); 
      Sender::histoData(thread, *tmp);
      
    } else {
      std::cerr << "Worker " << (long)args << " got a bad work item type " << type << std::endl;
      break;
    }
  }
  threadBytes[thread] = bytes;
  threadItems[thread]  = nItems;

  pthread_setspecific(glob_var_key, NULL);
  free(p);

  if (debug)
    std::cout << "Thread " << thread << " threadBytes: " << threadBytes[thread]  << " threadItems: " << threadItems[thread] << std::endl;
    
  worker.close();
  return NULL;
  
}

void
freeData(void* pData, void* pHint)
{
  free(pData);
}

static void
sendHeader(zmq::socket_t& sock, const std::string& identity)
{
  s_sendmore(sock, identity);
  s_sendmore(sock, "");
}

static void
sendEOF(zmq::socket_t& sock, const std::string& identity)
{
  sendHeader(sock, identity);
  s_send(sock, "eof");
}

static void
sendData(zmq::socket_t& sock, const std::string& identity, const CRingFileBlockReader::pDataDescriptor data)
{
  sendHeader(sock, identity);
  s_sendmore(sock, "data");

  size_t dataSize = data->s_nBytes;
  zmq::message_t descriptor(data, sizeof(CRingFileBlockReader::DataDescriptor), freeData);
  zmq::message_t dataBytes(data->s_pData, dataSize, freeData);

  if (debug)
    std::cout << "Thread " << identity << " is sending " << dataSize << " bytes..." << std::endl;
  
  sock.send(descriptor, ZMQ_SNDMORE);
  sock.send(dataBytes, 0);
}

static int
sendChunk(zmq::socket_t& sock, const std::string& identity, CRingFileBlockReader& reader,  size_t nItems)
{
  CRingFileBlockReader::pDataDescriptor pDesc =
    reinterpret_cast<CRingFileBlockReader::pDataDescriptor>(malloc(sizeof(CRingFileBlockReader::DataDescriptor)));

  *pDesc = reader.read(nItems);

  if (pDesc->s_nBytes > 0) {
    size_t nSent = pDesc->s_nBytes;
    bytesSent += nSent;;
    sendData(sock ,identity, pDesc);
    return nSent;

  } else {
    free(pDesc->s_pData);
    free(pDesc);
    sendEOF(sock, identity);
    return 0;
  }
}

void
Sender::printStats()
{
  for (int i = 0; i < NBR_WORKERS; i++) {
    std::cout << "Thread " << i << " processed " <<
      threadItems[i] << " items containing a total of " << threadBytes[i] << " bytes"  << std::endl;
  }
  std::cout << "Items processed " << totalItems << " totalBytesProcessed  " << totalBytes << std::endl;
  std::cout << "Physics Items " << totalPhysicsItems << std::endl;
  std::cout << "Entity Items " << totalEntityItems << std::endl;  
  
  printf("Elapsed time %lf\n", tdiff);
  
}

void
Sender::finish()
{
  isDone = true;
  for (int i = 0; i < NBR_WORKERS; i++) {
    totalBytes += threadBytes[i];
    totalItems += threadItems[i];
    totalPhysicsItems += physicsItems[i];
    totalEntityItems += entityItems[i];
    physicsItems[i] = 0;
    entityItems[i] = 0;
  }

  Sender::SetVariable(*m_pBuffersAnalyzed, totalPhysicsItems);

  stop_time = clock();
  tdiff = stop_time-start_time;
  etime += tdiff;
  printf("Running time %lf\n", tdiff);
  printf("Elapsed time %lf\n", etime);  
  m_pElapsedTime->Set(std::to_string(etime).c_str());
  isStart = true;
  
  if (debug)
    printStats();
  
  m_pRunState->Set("Halted");
  
}

void
Sender::SetVariable(CTCLVariable& rVar, int newval) {
  std::string Script("set ");
  char sval[100];
  sprintf(sval, "%d", newval);
  Script += rVar.getVariableName();
  Script += " ";
  Script += sval;
  rVar.getInterpreter()->GlobalEval(Script);
}

void*
Sender::sender_task(void* args)
{
  zmq::context_t context(1);
  zmq::socket_t broker(context, ZMQ_ROUTER);
  int linger(0);
  broker.setsockopt(ZMQ_LINGER, &linger, sizeof(int));

  broker.bind("tcp://*:5671");

  Sender* api = Sender::getInstance();
  int fd = api->getFd();

  CRingFileBlockReader reader(fd);  
  int workers_fired = 0;
  bool done = false;
  while (1) {
    //  Next message gives us least recently used worker
    std::string identity = s_recv(broker);
    std::string size;
    {
      s_recv(broker);     //  Envelope delimiter
      std::string command = s_recv(broker);     //  Command
      size    = s_recv(broker);     //  size:
      if (debug)
	std::cout << "Received " << command << " from id " << identity << std::endl;
    }
    if (!done) {
      int status =  sendChunk(broker, identity, reader, atoi(size.c_str()));
      if (status == 0) {
	done = true;
	sendEOF(broker, identity);

	++workers_fired;
	if (workers_fired == NBR_WORKERS) {
	  break;
	}
      }
    } else {
      sendEOF(broker, identity);
      if (++workers_fired == NBR_WORKERS){
	break;
      }
    }
  }

  broker.close();
}
