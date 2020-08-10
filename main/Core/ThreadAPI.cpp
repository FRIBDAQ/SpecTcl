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

#include <mutex>
#include "zhelpers.hpp"
#include "ThreadAPI.h"
#include "ZMQRDPatternClass.h"

zmq::context_t* ThreadAPI::m_pContextSingleton(nullptr);
ThreadAPI* ThreadAPI::m_pInstance = 0;

pthread_key_t glob_var_key;
static const std::string service("inproc://test3");

ThreadAPI*
ThreadAPI::getInstance()
{
  if (!m_pInstance) 
    m_pInstance = new ThreadAPI;

  return m_pInstance;
}

zmq::context_t*
ThreadAPI::getContext()
{
  if (!m_pContextSingleton) {
    m_pContextSingleton =
      new zmq::context_t(1);
  }
  return m_pContextSingleton;
}

void
ThreadAPI::SetNThreads(int nthreads)
{
  NTHREADS = nthreads;
  workers = new pthread_t[NTHREADS];
}

int
ThreadAPI::GetNThreads()
{
  return NTHREADS;
}

void
ThreadAPI::SetTCLApp(CTCLApplication& app)
{
  m_app = &app;
}

void
ThreadAPI::CreateThreads()
{
  pthread_key_create(&glob_var_key,NULL);

  // Creating sender task
  if (debug)
    std::cout << "Setting up sender..." << std::endl; 
  pthread_create(&sender, nullptr, ZMQRDClass::sender_task,  (void*)ThreadAPI::getInstance()->getContext());

  // Creating worker tasks
  if (debug)
    std::cout << "Setting up " << NTHREADS << " workers..." << std::endl;  
  struct arg_struct* args;
  for (int worker_nbr = 0; worker_nbr < NTHREADS; ++worker_nbr) {
    //    pthread_create(workers + worker_nbr, NULL, ZMQRDClass::worker_task, (void *)(intptr_t)worker_nbr);
    args = new struct arg_struct;
    args->thread_id = worker_nbr;
    args->thread_state = ZMQRDClass::getInstance()->getThreadState();
    args->thread_ctx = ThreadAPI::getInstance()->getContext();
    pthread_create(workers + worker_nbr, NULL, ZMQRDClass::worker_task, (void *)args);    
  }
  if (debug)
    std::cout << "...Done! " << std::endl;
}

void
ThreadAPI::JoinThreads()
{
  if (debug)
    std::cout << "Joining threads" << std::endl;
  for (int worker_nbr = 0; worker_nbr < NBR_WORKERS; ++worker_nbr) {
    pthread_join(workers[worker_nbr], NULL);
  }
  pthread_join(sender, nullptr);
}

void
ThreadAPI::DetachThreads()
{
  if (debug)
    std::cout << "Detaching threads" << std::endl;
  for (int worker_nbr = 0; worker_nbr < NBR_WORKERS; ++worker_nbr) {
    pthread_detach(workers[worker_nbr]);
  }
  pthread_detach(sender);
}

void
ThreadAPI::Destroy()
{
  delete []workers;
  delete m_pInstance;
  m_pInstance = NULL;
}

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
/*
void *step1 (void *arg) {

  zmq::context_t * context = static_cast<zmq::context_t*>(arg);

  //  Signal downstream to step 2
  zmq::socket_t sender (*context, ZMQ_PAIR);
  sender.connect("inproc://step2");

  s_send (sender, "");

  return (NULL);
}


void *step2 (void *arg) {

  zmq::context_t * context = static_cast<zmq::context_t*>(arg);

  //  Bind to inproc: endpoint, then start upstream thread
  zmq::socket_t receiver (*context, ZMQ_PAIR);
  receiver.bind("inproc://step2");

  pthread_t thread;
  pthread_create (&thread, NULL, step1, context);

  //  Wait for signal
  s_recv (receiver);

  //  Signal downstream to step 3
  zmq::socket_t sender (*context, ZMQ_PAIR);
  sender.connect("inproc://step3");
  s_send (sender, "");

  return (NULL);
}

static void *
worker_task1(void *args)
{
  zmq::context_t * context = static_cast<zmq::context_t*>(args);

  //  zmq::context_t context(1);  
  zmq::socket_t worker(*context, ZMQ_DEALER);
  s_set_id(worker);          //  Set a printable identity

  //  worker.connect("tcp://localhost:5671");
  worker.connect(service.c_str());  

  int total = 0;
  while (1) {
    //  Tell the broker we're ready for work
    s_sendmore(worker, "");
    s_send(worker, "Hi Boss");

    //  Get workload from broker, until finished
    s_recv(worker);     //  Envelope delimiter
    std::string workload = s_recv(worker);
    if ("Fired!" == workload) {
      std::cout << "Completed: " << total << " tasks" << std::endl;
      break;
    }
    total++;

    //  Do some random work
    s_sleep(within(500) + 1);
  }

  return NULL;
}

static void *
worker_task2(void *args)
{
  struct ThreadAPI::arg_thread* a = (struct ThreadAPI::arg_thread*)(args);

  int thread = (int)a->thread_id;
  zmq::context_t * context = static_cast<zmq::context_t*>(a->thread_ctx);

  //  zmq::context_t context(1);  
  zmq::socket_t worker(*context, ZMQ_DEALER);
  s_set_id(worker);          //  Set a printable identity

  //  worker.connect("tcp://localhost:5671");
  worker.connect(service.c_str());  

  int total = 0;
  while (1) {
    //  Tell the broker we're ready for work
    s_sendmore(worker, "");
    s_send(worker, "Hi Boss");

    //  Get workload from broker, until finished
    s_recv(worker);     //  Envelope delimiter
    std::string workload = s_recv(worker);
    if ("Fired!" == workload) {
      std::cout << "Thread " << thread << " completed: " << total << " tasks" << std::endl;
      break;
    }
    total++;

    //  Do some random work
    s_sleep(within(500) + 1);
  }
  
  return NULL;
}

void
ThreadAPI::Test1()
{
  std::cout << "Beginning Test1" << std::endl;
  zmq::context_t context(1);
  //  Bind to inproc: endpoint, then start upstream thread
  zmq::socket_t receiver (context, ZMQ_PAIR);
  receiver.bind("inproc://step3");

  pthread_t thread;
  pthread_create (&thread, NULL, step2, &context);

  //  Wait for signal
  s_recv (receiver);

  std::cout << "Test successful!" << std::endl;
  
}

void
ThreadAPI::Test2()
{
  std::cout << "Beginning Test2" << std::endl;  
  zmq::context_t context(1);

  zmq::socket_t sink(context, ZMQ_ROUTER);
  sink.bind( "inproc://example");

  //  First allow 0MQ to set the identity
  zmq::socket_t anonymous(context, ZMQ_REQ);
  anonymous.connect( "inproc://example");

  s_send (anonymous, "ROUTER uses a generated 5 byte identity");
  s_dump (sink);

  //  Then set the identity ourselves
  zmq::socket_t identified (context, ZMQ_REQ);
  identified.setsockopt( ZMQ_IDENTITY, "PEER2", 5);
  identified.connect( "inproc://example");

  s_send (identified, "ROUTER socket uses REQ's socket identity");
  s_dump (sink);

  std::cout << "Test successful!" << std::endl;
}

void
ThreadAPI::Test3()
{
  std::cout << "Beginning Test3" << std::endl;  
  zmq::context_t context(1);
  zmq::socket_t broker(context, ZMQ_ROUTER);

  //  broker.bind("tcp://*:5671");
  broker.bind(service.c_str());  
  srandom((unsigned)time(NULL));

  const int NBR_WORKERS = 3;
  pthread_t workers[NBR_WORKERS];

  for (int worker_nbr = 0; worker_nbr < NBR_WORKERS; ++worker_nbr) {
    //    pthread_create(workers + worker_nbr, NULL, worker_task, (void *)(intptr_t)worker_nbr);
    pthread_create(workers + worker_nbr, NULL, worker_task1, (void *)&context);    
  }

  
  //  Run for five seconds and then tell workers to end
  int64_t end_time = s_clock() + 2000;
  int workers_fired = 0;
  while (1) {
    //  Next message gives us least recently used worker
    std::string identity = s_recv(broker);
    {
      s_recv(broker);     //  Envelope delimiter
      s_recv(broker);     //  Response from worker
    }

    s_sendmore(broker, identity);
    s_sendmore(broker, "");

    //  Encourage workers until it's time to fire them
    if (s_clock() < end_time)
      s_send(broker, "Work harder");
    else {
      s_send(broker, "Fired!");
      if (++workers_fired == NBR_WORKERS)
	break;
    }
  }

  for (int worker_nbr = 0; worker_nbr < NBR_WORKERS; ++worker_nbr) {
    pthread_join(workers[worker_nbr], NULL);
  }

  std::cout << "Test successful!" << std::endl;
}

void
ThreadAPI::Test4()
{
  std::cout << "Beginning Test4" << std::endl;  
  zmq::context_t* ctx = ThreadAPI::getContext();
  zmq::socket_t broker(*ctx, ZMQ_ROUTER);

  //  broker.bind("tcp://*:5671");
  broker.bind(service.c_str());  
  srandom((unsigned)time(NULL));

  const int NBR_WORKERS = 3;
  pthread_t workers[NBR_WORKERS];

  ThreadAPI::arg_thread m_args;
  
  for (int worker_nbr = 0; worker_nbr < NBR_WORKERS; ++worker_nbr) {
    //    pthread_create(workers + worker_nbr, NULL, worker_task, (void *)(intptr_t)worker_nbr);
    //    pthread_create(workers + worker_nbr, NULL, worker_task, (void *)ctx);
    m_args.thread_id = worker_nbr;
    m_args.thread_ctx = ThreadAPI::getContext();
    
    pthread_create(workers + worker_nbr, NULL, worker_task2, (void *)&m_args);        
  }

  
  //  Run for five seconds and then tell workers to end
  int64_t end_time = s_clock() + 2000;
  int workers_fired = 0;
  while (1) {
    //  Next message gives us least recently used worker
    std::string identity = s_recv(broker);
    {
      s_recv(broker);     //  Envelope delimiter
      s_recv(broker);     //  Response from worker
    }

    s_sendmore(broker, identity);
    s_sendmore(broker, "");

    //  Encourage workers until it's time to fire them
    if (s_clock() < end_time)
      s_send(broker, "Work harder");
    else {
      s_send(broker, "Fired!");
      if (++workers_fired == NBR_WORKERS)
	break;
    }
  }

  for (int worker_nbr = 0; worker_nbr < NBR_WORKERS; ++worker_nbr) {
    pthread_join(workers[worker_nbr], NULL);
  }

  std::cout << "Test successful!" << std::endl;
}
*/
