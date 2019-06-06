#include <memory>
#include <iostream>
#include <unistd.h>
#include "zhelpers.hpp"
#include "CRingFileBlockReader.h"
#include "TclGrammerApp.h"
#include "TKRunControl.h"
#include "RunControl.h"

extern const int NBR_WORKERS;

class Sender
{
 private:

  static Sender* m_pInstance;
  Sender(); 
  virtual ~Sender();

 public: 
  static Sender* getInstance();

  int m_nFd;  
  void setFd(int fd);
  int  getFd();  

  static size_t* threadBytes;
  static size_t* threadItems;
  
  static void* sender_task(void *args);  
  static void* worker_task(void *args);

  void finish();
  
};
