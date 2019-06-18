#include <limits>
#include <mutex>
#include <chrono>
#include "DataFormat.h"
#include "CRingBufferDecoder.h"
#include "ZMQSenderClass.h"
#include "SpecTcl.h"
#include "ThreadAPI.h"
#include "ThreadAnalyzer.h"
#include "BufferDecoder.h"
#include "SpectrumDictionaryFitObserver.h"
#include "TclGrammerApp.h"
pthread_mutex_t mu;

int status;
bool isDone = false;
bool debug = false;
bool isStart = true;
Sender* Sender::m_pInstance = 0;

const int NBR_WORKERS = 25;
int CHUNK_SIZE  = 1024*1024*96;
double qnan = std::numeric_limits<double>::quiet_NaN();

double stop_time, start_time;

static size_t bytesSent(0);
size_t* Sender::threadBytes = new size_t[NBR_WORKERS];
size_t* Sender::threadItems = new size_t[NBR_WORKERS];
size_t* Sender::physicsItems = new size_t[NBR_WORKERS];

EventProcessingPipeline* Sender::m_pipeline;
BufferTranslator* Sender::m_pTranslator;
static EventProcessingPipeline* pipecopy;
BufferTranslator* pTranslator[NBR_WORKERS];
static CThreadAnalyzer* pAnalyzer;
CBufferDecoder* pDecoder[NBR_WORKERS];
static CEventList* pEventList;

Vpairs* tmp;

CTCLApplication* gpTCLApplication;

typedef struct _HistoEvent {
  Tcl_Event    tclEvent;
  Vpairs*      histoList;
} HistoEvent, *pHistoEvent;

Sender::Sender()
{
  m_pipeline = new EventProcessingPipeline;
  pipecopy = new EventProcessingPipeline[NBR_WORKERS];

  pAnalyzer = new CThreadAnalyzer[NBR_WORKERS];
  pEventList = new CEventList[NBR_WORKERS];
  
  for (int i=0; i<NBR_WORKERS; ++i)
    physicsItems[i] = 0;

}

Sender::~Sender()
{
  delete []threadBytes;
  delete []threadItems;
  delete []physicsItems;
  delete m_pipeline;
}

double
Sender::clock()
{
  struct timeval t;
  gettimeofday(&t, NULL);
  return (1.0e-6*t.tv_usec + t.tv_sec);
}

void
Sender::cleanup()
{
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

  delete m_pTranslator;
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
Sender::copyPipeline(EventProcessingPipeline& oldpipe, EventProcessingPipeline& newpipe)
{
  EventProcessingPipeline::iterator it;
  for (it = oldpipe.begin(); it!=oldpipe.end(); it++){
    newpipe.push_back(PipelineElement(it->first, it->second));
  }
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

  //////////////////////////////////////
  // Buffer translator
  //////////////////////////////////////  
  try {
    Sender::createTranslator(pBuffer);
    if (!pTranslator[thread]){
      if (debug)
	std::cout << "Created translator " << thread << std::endl;
      pTranslator[thread] = m_pTranslator->clone();
    }
    else {
      pTranslator[thread]->newBuffer(pBuffer);
    }
  }
  catch (...) {
    std::cerr << "SpecTcl exiting due to buffer decoder exception\n";
    exit(EXIT_FAILURE);
  }

  //////////////////////////////////////
  // Buffer decoder + analyzer 
  //////////////////////////////////////    
  if (!pDecoder[thread]){
    if (debug)
      std::cout << "Created decoder " << thread << std::endl;
    pDecoder[thread] = new CRingBufferDecoder;
    pAnalyzer[thread].AttachDecoder(*pDecoder[thread]);
    if (debug)
      std::cout << "...and attached to the threaded analyzer" << std::endl;
  }

  //////////////////////////////////////
  // Analysis pipeline
  //////////////////////////////////////  
  if (pipecopy[thread].size() == 0){
    copyPipeline(*m_pipeline, pipecopy[thread]);
    if (debug)
      std::cout << "Created pipecopy " << thread << " of size " << pipecopy[thread].size() << std::endl;
  }

  Address_t    pBody;
  UInt_t       nBodySize;
  
  while(nResidual) {
    
    pRingItem        pItem = reinterpret_cast<pRingItem>(pBufferCursor);
    uint32_t         size  = pTranslator[thread]->TranslateLong(pItem->s_header.s_size);
    uint32_t         type  = pTranslator[thread]->TranslateLong(pItem->s_header.s_type);
    pTranslator[thread]->newBuffer(pItem);

    pBody     = pItem->s_body.u_noBodyHeader.s_body;
    nBodySize = size - (reinterpret_cast<uint8_t*>(pBody) - reinterpret_cast<uint8_t*>(pItem));
    
    switch (type) {
    case BEGIN_RUN:
      {
	if (debug)
	  std::cout << "BEGIN RUN" << std::endl;
      }
      break;
    case END_RUN:
    case PAUSE_RUN:
    case RESUME_RUN:
      {}
      break;
    case PACKET_TYPES:
    case MONITORED_VARIABLES:
      {}
      break;
    case PERIODIC_SCALERS:
      {}
      break;
    case PHYSICS_EVENT:
      {
	physicsItems[thread]++;
	pAnalyzer[thread].OnPhysics(thread, *pDecoder[thread], nBodySize, pBody, pipecopy[thread], *pTranslator[thread], pEventList[thread]);
	marshall(thread, pEventList[thread], vec);
      }
      break;
    case PHYSICS_EVENT_COUNT:
      {}
      break;
    default:
      {}
      break;
    }
    pBufferCursor = reinterpret_cast<uint32_t*>(reinterpret_cast<uint8_t*>(pBufferCursor) + size);
    nResidual    -= size;
  }
}

////////////////////////////////////////////////////////////////////////
///// This version histograms only the CEventList* in the Tcl Thread
///// This choice doesn't work because the flow goes as follows
////  CEventList 1
////  CEventList 2
////  CEventList ...
////  CEventList N
////  HistogramHandle 1
////  HistogramHandle 2
////  HistogramHandle ...
////  HistogramHandle 3
////  Because I'm passing pointer to memory the system blows up before
////  I am able to free the heap
////////////////////////////////////////////////////////////////////////


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
  
  // convert Vpairs* to CEventList*
  unsigned int index, size;
  double value;
  CEvent* e;

  CEventList* outList;
  for (const std::pair<unsigned int, double> &evt : *hlist){
    outList = new CEventList;
    CEventVector& olist(outList->getVector());

    index = evt.first;
    value = evt.second;
    if (std::isnan(value)){
      size = index;
      e = new CEvent;
      continue;
    }
    (*e)[index] = value;
    if(size == e->getDopeVector().size()){
      olist.push_back(e);
      hist->operator()(*outList);    
    }
    delete outList;
  }
  
  return 1;
}

void *
Sender::worker_task(void *args)
{
  long thread = (long)(args);
  zmq::context_t context(1);
  int linger(0);
  ////////////////////////////////////////////
  // ROUTER/DEALER PATTERN TO GET THE DATA
  ////////////////////////////////////////////  
  zmq::socket_t worker(context, ZMQ_DEALER);
  worker.setsockopt(ZMQ_LINGER, &linger, sizeof(int));
  std::string id1 = s_set_id(worker);          //  Set a printable identity
  worker.connect("tcp://localhost:5671");

  std::stringstream ChunkSize;
  ChunkSize << CHUNK_SIZE;
  size_t bytes = 0;
  size_t nItems = 0;
  int total = 0;
  while (1) {
    s_sendmore(worker, "");
    s_sendmore(worker, "fetch");
    s_send(worker, ChunkSize.str());                 // Size of workload

    // Work items are in two types.  all start with delimetr and type.
    // Type eof means we're done and need to clean up.
    // type data means there's two more segments, the descriptor and the data.

    s_recv(worker);                               // Delimeter.
    std::string type = s_recv(worker);

    if (isStart){
      std::cout << "Start!" << std::endl;
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

      tmp = new Vpairs;
      Sender::processRingItems(thread, pDescriptor, pRingItems, *tmp); 
      Sender::histoData(thread, *tmp);
      
    } else {
      std::cerr << "Worker " << (long)args << " got a bad work item type " << type << std::endl;
      break;
    }
  }
  threadBytes[thread] = bytes;
  threadItems[thread]  = nItems;

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
Sender::finish()
{
  isDone = true;
  size_t totalBytes(0);
  size_t totalItems(0);
  size_t totalPhysicsItems(0);
  for (int i = 0; i < NBR_WORKERS; i++) {
    std::cout << "Thread " << i << " processed " <<
      threadItems[i] << " items containing a total of " << threadBytes[i] << " bytes"  << std::endl;
    totalBytes += threadBytes[i];
    totalItems += threadItems[i];
    totalPhysicsItems += physicsItems[i];
    physicsItems[i] = 0;
  }
  std::cout << "Items processed " << totalItems << " totalBytesProcessed  " << totalBytes << std::endl;
  std::cout << "Physics Items " << totalPhysicsItems << std::endl;

  stop_time = clock();
  printf("Elapsed time %lf\n", stop_time-start_time);
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
