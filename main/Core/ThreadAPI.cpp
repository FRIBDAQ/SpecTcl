#include <mutex>
#include "ThreadAPI.h"
#include "ZMQSenderClass.h"

std::mutex mtx;
zmq::context_t* ThreadAPI::m_pContextSingleton(nullptr);
ThreadAPI* ThreadAPI::m_pInstance = 0;
pthread_key_t glob_var_key;

zmq::context_t*
ThreadAPI::getContext()
{
  if (!m_pContextSingleton) {
        m_pContextSingleton =
	  new zmq::context_t(1);
  }
  return m_pContextSingleton;
}

ThreadAPI*
ThreadAPI::getInstance()
{
  if (!m_pInstance) 
    m_pInstance = new ThreadAPI;

  return m_pInstance;
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
  std::cout << "\t\t\t\t\tInside SetTCLApp" << std::endl;
  m_app = &app;
}

void
ThreadAPI::CreateThreads()
{
  //  Sender* snd = Sender::getInstance();
  
  pthread_key_create(&glob_var_key,NULL);
  // Setup sender
  //  std::cout << "Setting up sender..." << std::endl; 
  pthread_create(&sender, nullptr, Sender::sender_task,  nullptr);

  // Setup worker
  //  std::cout << "Setting up " << NTHREADS << " workers..." << std::endl;  
  struct arg_struct* args;
  for (int worker_nbr = 0; worker_nbr < NTHREADS; ++worker_nbr) {
    //    args = new struct arg_struct;
    //    args->thread_id = worker_nbr;
    //    args->thread_state = snd->getThreadState();
    pthread_create(workers + worker_nbr, NULL, Sender::worker_task, (void *)(intptr_t)worker_nbr);
    //    pthread_create(workers + worker_nbr, NULL, Sender::worker_task, (void *)args);    
  }
  //  std::cout << "...Done! " << std::endl;
}

void
ThreadAPI::JoinThreads()
{
  //  std::cout << "\nJoining threads" << std::endl;
  for (int worker_nbr = 0; worker_nbr < NBR_WORKERS; ++worker_nbr) {
    pthread_join(workers[worker_nbr], NULL);
  }
  pthread_join(sender, nullptr);
}

void
ThreadAPI::DetachThreads()
{
  std::cout << "\nDetaching threads" << std::endl;
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
