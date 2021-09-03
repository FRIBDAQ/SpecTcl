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
#include <buftypes.h>
#include "ShMemAPI.h"
#include "ShMemManagerAPI.h"

bool debug = true;
ShMemAPI* ShMemAPI::m_pInstance = 0;

ShMemAPI::ShMemAPI()
{
}

ShMemAPI::~ShMemAPI()
{
  ShMemManagerAPI::getInstance()->Destroy();
}

ShMemAPI*
ShMemAPI::getInstance() 
{
  if(!m_pInstance) {
    m_pInstance = new ShMemAPI();
  }
  // Regardless return it to the caller.
  return m_pInstance;
  
}

void*
ShMemAPI::publisher_task(void* arg)
{
  zmq::context_t* context = static_cast<zmq::context_t*>(arg);
  zmq::socket_t server(*context, ZMQ_PUB);  

  int linger(0);
  server.setsockopt(ZMQ_LINGER, &linger, sizeof(int));
  server.bind("tcp://*:5678");

  while (1) {
    s_send(server, "porcodio chiudi tutto");
  }

  server.close();
  return NULL;
}
