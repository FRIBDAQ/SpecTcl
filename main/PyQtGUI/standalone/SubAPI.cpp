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

#include <map>
#include <sys/types.h>
#include <sys/stat.h>
#include <regex>
#include <time.h>
#include <mutex>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <limits>
#include <chrono>
#include "SubAPI.h"
#include "SubManagerAPI.h"

bool debug = true;
SubAPI* SubAPI::m_pInstance = 0;

SubAPI::SubAPI()
{
}

SubAPI::~SubAPI()
{
  SubManagerAPI::getInstance()->Destroy();
}

SubAPI*
SubAPI::getInstance() 
{
  if(!m_pInstance) {
    m_pInstance = new SubAPI();
  }
  // Regardless return it to the caller.
  return m_pInstance;
  
}

void*
SubAPI::subscriber_task(void* arg)
{
  zmq::context_t context(1);
  zmq::socket_t subscriber(context, ZMQ_SUB);
  subscriber.connect("tcp://localhost:5678");
  subscriber.setsockopt(ZMQ_SUBSCRIBE, "", 0);

  
  // Synch with publisher
  zmq::socket_t syncservice(context, ZMQ_REQ);
  syncservice.connect("tcp://localhost:1234");

  // Send synch request
  s_send(syncservice, "Ready");
  // wait for synch reply
  std::string ack = s_recv(syncservice);
  std::cout << "Synch reply -> " << ack << std::endl;

  while(1) {
    std::string msg = s_recv(subscriber);
    std::cout << "Received -> " << msg << std::endl;
  }
    
  return NULL;
}
