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

#include <pthread.h>
#include <iostream>
#include <zmq.hpp>

#include "TCLApplication.h"

class ThreadAPI {

 private:
  // ctor, dtor for ThreadAPI
  ThreadAPI(){}; 
  ThreadAPI(ThreadAPI const&){}; 
  ThreadAPI& operator=(ThreadAPI const&){};

  int NTHREADS; // number of workers set in SpecTclInit.tcl

  // Core of ThreadAPI
  static ThreadAPI* m_pInstance;
  static zmq::context_t* m_pContextSingleton;
  
  pthread_t sender;
  pthread_t* workers;

  CTCLApplication* m_app;
  
 public:

  // Public methods 
  static ThreadAPI* getInstance();
  static zmq::context_t*  getContext();
  void SetTCLApp(CTCLApplication& app);

  // Configuration of pthreads
  void SetNThreads(int nthreads);  
  int GetNThreads();
  
  void CreateThreads(); // Create sender+worker threads
  void JoinThreads(); // Join sender+worker threads
  void DetachThreads(); // Detach sender+worker threads  
  void Destroy();

  // Testing inproc
  /*
  struct arg_thread {
    int thread_id;
    zmq::context_t* thread_ctx;
  };
  void Test1();
  void Test2();
  void Test3();
  void Test4();      
  */
};
