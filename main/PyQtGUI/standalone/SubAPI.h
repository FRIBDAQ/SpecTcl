/*
    This software is Copyright by the Board of Trustees of Michigan
    State University (c) Copyright 2021.

    You may use this software under the terms of the GNU public license
    (GPL).  The terms of this license are described at:

     http://www.gnu.org/licenses/gpl.txt

     Author:
             Giordano Cerizza
             NSCL
             Michigan State University
             East Lansing, MI 48824-1321
*/

#include <utility>
#include <memory>
#include <iostream>
#include <unistd.h>
#include <chrono>
#include <map>

#include "zhelpers.hpp"

extern bool debug;

struct arg_struct {
  int thread_id;
  zmq::context_t* thread_ctx;
};

class SubAPI
{
 private:

  static SubAPI* m_pInstance;
  SubAPI(); 
  virtual ~SubAPI();
  
 public: 

  static SubAPI* getInstance();
  static void* subscriber_task(void *arg);  

};
