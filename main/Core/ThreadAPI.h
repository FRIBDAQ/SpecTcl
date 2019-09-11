#include "TCLApplication.h"
#include <pthread.h>
#include <iostream>
#include <zmq.hpp>

class ThreadAPI {
 public:
  static ThreadAPI* getInstance();
  void Destroy();

  void SetNThreads(int nthreads);  
  int GetNThreads();
  void CreateThreads();
  void JoinThreads(); // Join sender+worker threads
  void DetachThreads(); // Join sender+worker threads  
  void SetTCLApp(CTCLApplication& app);
  static zmq::context_t*  getContext();
  
 private:
  ThreadAPI(){}; 
  ThreadAPI(ThreadAPI const&){}; 
  ThreadAPI& operator=(ThreadAPI const&){};

  int NTHREADS;

  pthread_t sender;
  pthread_t* workers;
  
  static ThreadAPI* m_pInstance;
  static zmq::context_t* m_pContextSingleton;
  CTCLApplication* m_app;
};
