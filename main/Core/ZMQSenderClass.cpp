#include "ZMQSenderClass.h"
#include "SpecTcl.h"
#include "ThreadAPI.h"

int status;
bool debug = false;
Sender* Sender::m_pInstance = 0;

const int NBR_WORKERS = 10;
int CHUNK_SIZE  = 1024*1024;

static size_t bytesSent(0);
size_t* Sender::threadBytes = new size_t[NBR_WORKERS];
size_t* Sender::threadItems = new size_t[NBR_WORKERS];

Sender::Sender(){}
Sender::~Sender()
{
  delete []threadBytes;
  delete []threadItems;
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

static void
processRingItems(CRingFileBlockReader::pDataDescriptor descrip, void* pData)
{

}

void *
Sender::worker_task(void *args)
{
  long thread = (long)(args);
  zmq::context_t context(1);
  zmq::socket_t worker(context, ZMQ_DEALER);
  int linger(0);
  worker.setsockopt(ZMQ_LINGER, &linger, sizeof(int));

  s_set_id(worker);          //  Set a printable identity

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

      processRingItems(pDescriptor, pRingItems); // any interesting work goes here.
      
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
  size_t totalBytes(0);
  size_t totalItems(0);
  for (int i = 0; i < NBR_WORKERS; i++) {
    std::cout << "Thread " << i << " processed " <<
      threadItems[i] << " items containing a total of " << threadBytes[i] << " bytes"  << std::endl;
    totalBytes += threadBytes[i];
    totalItems += threadItems[i];
  }
  std::cout << "Items processed " << totalItems << " totalBytesProcessed  " << totalBytes << std::endl;

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

