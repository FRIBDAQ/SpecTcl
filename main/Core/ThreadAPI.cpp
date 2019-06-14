#include "ThreadAPI.h"
#include "ZMQSenderClass.h"

ThreadAPI* ThreadAPI::m_pInstance = 0;

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

void
ThreadAPI::SetTCLApp(CTCLApplication& app)
{
  std::cout << "\t\t\t\t\tInside SetTCLApp" << std::endl;
  m_app = &app;
}

void
ThreadAPI::CreateThreads()
{
  // Setup sender
  std::cout << "Setting up sender..." << std::endl;
  pthread_create(&sender, nullptr, Sender::sender_task,  nullptr);

  // Setup worker
  std::cout << "Setting up " << NTHREADS << " workers..." << std::endl;
  for (int worker_nbr = 0; worker_nbr < NTHREADS; ++worker_nbr) {
    pthread_create(workers + worker_nbr, NULL, Sender::worker_task, (void *)(intptr_t)worker_nbr);
  }
  std::cout << "...Done! " << std::endl;
}

void
ThreadAPI::JoinThreads()
{
  for (int worker_nbr = 0; worker_nbr < NBR_WORKERS; ++worker_nbr) {
    pthread_join(workers[worker_nbr], NULL);
  }
  pthread_join(sender, nullptr);
}

void
ThreadAPI::Destroy()
{
  delete []workers;
  delete m_pInstance;
  m_pInstance = NULL;
}
