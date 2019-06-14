#include "TCLApplication.h"
#include <pthread.h>
#include <iostream>

class ThreadAPI {
 public:
  static ThreadAPI* getInstance();
  void Destroy();

  void SetNThreads(int nthreads);  
  void CreateThreads();
  void JoinThreads(); // Join sender+worker threads
  void SetTCLApp(CTCLApplication& app);
  
 private:
  ThreadAPI(){}; 
  ThreadAPI(ThreadAPI const&){}; 
  ThreadAPI& operator=(ThreadAPI const&){};

  int NTHREADS;

  pthread_t sender;
  pthread_t* workers;
  
  static ThreadAPI* m_pInstance;
  CTCLApplication* m_app;
};
