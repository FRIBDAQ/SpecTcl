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
#include "zmsg.hpp"
#include <stdio.h>
#include <time.h>
#include <thread>
#include <sstream>
#include <vector>

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

void
freeData(void* pData, void* pHint)
{
  free(pData);
}

static void
sendData(zmq::socket_t& sock, unsigned int size, const struct Data& data)
{
  s_sendmore(sock, std::to_string(size));
  size_t const dataSize = sizeof(data);

  struct Data* shmemData = reinterpret_cast<struct Data*>(malloc(dataSize));
  *shmemData = data;

  zmq::message_t dataShMem(shmemData, dataSize, freeData);
  sock.send(dataShMem, 0);
}

void*
ShMemAPI::server_task(void* arg)
{
  struct arg_struct* a = (struct arg_struct*)(arg); 
  unsigned int size = (long)a->shmem_size;
  struct Data* data = (struct Data*)a->shmem_data;  
  zmq::context_t * context = static_cast<zmq::context_t*>(a->ctx);

  zmq::socket_t server(*context, ZMQ_REP);  
  server.bind("tcp://*:5555");
  
  int cycles = 0;
  while (1) {
    std::string request = s_recv (server);
    cycles++;
    
    // Simulate various problems, after a few cycles
    /*
    if (cycles > 3 && within (3) == 0) {
      std::cout << "I: simulating a crash" << std::endl;
      break;
    }
    else
      if (cycles > 3 && within (3) == 0) {
	std::cout << "I: simulating CPU overload" << std::endl;
	sleep (2);
      }
    */
    if (debug)
      std::cout << "I: normal request (" << request << ")" << "...sending shmem " << std::endl;
    sleep (1); // set time for updating shmem (this may be pass as parameter from qtpy)
    sendData(server, size, *data);
    
  }

  return NULL;
}
