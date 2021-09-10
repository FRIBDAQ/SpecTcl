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

class ShMemAPI
{
 private:

  static ShMemAPI* m_pInstance;
  ShMemAPI(); 
  virtual ~ShMemAPI();
  
 public: 

  static ShMemAPI* getInstance();
  static void* server_task(void *arg);  

  static inline uint64_t hrDiff(const timespec& end, const timespec& start)
  {
    timespec temp;
    if ((end.tv_nsec-start.tv_nsec)<0) {
      temp.tv_sec = end.tv_sec-start.tv_sec-1;
      temp.tv_nsec = 1000000000+end.tv_nsec-start.tv_nsec;
    } else {
      temp.tv_sec = end.tv_sec-start.tv_sec;
      temp.tv_nsec = end.tv_nsec-start.tv_nsec;
    }
    return (uint64_t)((temp.tv_sec * 1.0e9) + temp.tv_nsec);
  }
};
