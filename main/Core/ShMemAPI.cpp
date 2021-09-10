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

void*
ShMemAPI::server_task(void* arg)
{
  zmq::context_t context(1);
  zmq::socket_t server(context, ZMQ_REP);
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
    std::cout << "I: normal request (" << request << ")" << std::endl;
    sleep (1); // Do some heavy work
    s_send (server, request);
  }
  return NULL;
}
