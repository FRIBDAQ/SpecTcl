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
#include <stdio.h>
#include <time.h>
#include <thread>

bool debug = true;
ShMemAPI* ShMemAPI::m_pInstance = 0;
#define SUBS 1

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
  zmq::context_t context(1);
  zmq::socket_t publisher(context, ZMQ_PUB);  

  int sndhwm = 0;
  publisher.setsockopt(ZMQ_SNDHWM, &sndhwm, sizeof(sndhwm));
  publisher.bind("tcp://*:5678");  

  // socket to receive signals
  zmq::socket_t syncservice(context, ZMQ_REP);
  syncservice.bind("tcp://*:1234");

  int subs = 0;
  while (subs < SUBS) {
    // wait for synch request
    std::string req = s_recv(syncservice);
    std::cout << "Request of synch received -> " << req << std::endl;
    // send synch reply
    s_send(syncservice, "Connected");
    subs++;
  }

  std::cout << "Done synchronizing" << std::endl;
  std::cout << "Start sending..." << std::endl;
  
  std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
  auto s = std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch());
  std::chrono::system_clock::time_point next_full_second = std::chrono::system_clock::time_point(++s);
  
  auto interval = std::chrono::seconds(1); // or milliseconds(500) 
  auto wait_until = next_full_second;

  while (1){
    std::this_thread::sleep_until(wait_until);
    std::string msg = "Tie beccate sto porcoddio";
    s_send(publisher, msg);	
    
    wait_until += interval;
  }
  
  return NULL;
}
