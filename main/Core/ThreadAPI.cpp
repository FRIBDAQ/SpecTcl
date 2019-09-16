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

#include "ThreadAPI.h"
#include "ZMQRDPatternClass.h"

zmq::context_t* ThreadAPI::m_pContextSingleton(nullptr);
ThreadAPI* ThreadAPI::m_pInstance = 0;

std::mutex mtx;
pthread_key_t glob_var_key;

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
  pthread_create(&sender, nullptr, ZMQRDClass::sender_task,  nullptr);

  // Creating worker tasks
  if (debug)
    std::cout << "Setting up " << NTHREADS << " workers..." << std::endl;  
  // This is only for future reference - see below
  //  struct arg_struct* args;
  for (int worker_nbr = 0; worker_nbr < NTHREADS; ++worker_nbr) {
    pthread_create(workers + worker_nbr, NULL, ZMQRDClass::worker_task, (void *)(intptr_t)worker_nbr);
    // This is for future reference in case I need to pass more than one argument to the threads
    /*
      args = new struct arg_struct;
      args->thread_id = worker_nbr;
      args->thread_state = snd->getThreadState();
      pthread_create(workers + worker_nbr, NULL, Sender::worker_task, (void *)args);    
    */
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
