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
#include <cstdint>
#include "SubAPI.h"
#include "SubManagerAPI.h"
#include <sstream>

#define REQUEST_TIMEOUT     10000    //  msecs, (> 1000!)
#define REQUEST_RETRIES     99       //  Before we abandon

bool isShMem = false;
bool debug = true;
SubAPI* SubAPI::m_pInstance = 0;

struct Data
{
  int a;
  int arr[3];
};

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

void
SubAPI::CreateLocalShMem(unsigned int size)
{}

void
SubAPI::SetLocalShMem()
{}

void
SubAPI::DestroyLocalShMem()
{}

static zmq::socket_t*
s_client_socket (zmq::context_t & context)
{
  std::cout << "I: connecting to server..." << std::endl;
  zmq::socket_t * client = new zmq::socket_t (context, ZMQ_REQ);
  client->connect ("tcp://localhost:5555");
  
  //  Configure socket to not wait at close time
  int linger = 0;
  client->setsockopt (ZMQ_LINGER, &linger, sizeof (linger));
  return client;
}

void*
SubAPI::subscriber_task(void* arg)
{
 zmq::context_t context (1);

    zmq::socket_t * client = s_client_socket (context);

    int sequence = 0;
    int retries_left = REQUEST_RETRIES;

    while (retries_left) {
        std::stringstream request;
        request << ++sequence;
        s_send (*client, request.str());
        sleep (1);

        bool expect_reply = true;
        while (expect_reply) {
	  //  Poll socket for a reply, with timeout
	  std::vector<zmq::pollitem_t> items = {{static_cast<void *>(*client), 0, ZMQ_POLLIN, 0}};
	  zmq::poll (&items[0], 1, REQUEST_TIMEOUT);

	  unsigned int size;
	  //  If we got a reply, process it
	  if (items[0].revents & ZMQ_POLLIN) {
	    //  We got a reply from the server, should be size+memory copy
	    size = std::stoi(s_recv(*client));
	    if (debug)
	      std::cout << "Size of the shared memory -> " << size << std::endl;

	    // Create shared memory if doesn't exist
	    if (!isShMem)
	      SubAPI::CreateLocalShMem(size);

	    zmq::message_t shmemData;
	    client->recv(&shmemData);

	    // Content of the test shared memory
	    struct Data* pData =
	      reinterpret_cast<struct Data*>(shmemData.data());

	    if (debug)
	      std::cout << pData->a << " " << pData->arr[0] <<
		" " << pData->arr[1] <<
		" " << pData->arr[2] << std::endl;

	    // Copy shared memory from message to local machine 
	    /* ... */
	    SubAPI::SetLocalShMem();
	    
	    retries_left = REQUEST_RETRIES;
	    expect_reply = false;
	  }
	  else
            if (--retries_left == 0) {
	      std::cout << "E: server seems to be offline, abandoning" << std::endl;
	      expect_reply = false;
	      break;
            }
            else {
	      std::cout << "W: no response from server, retrying..." << std::endl;
	      //  Old socket will be confused; close it and open a new one
	      delete client;
	      client = s_client_socket (context);
	      //  Send request again, on new socket
	      s_send (*client, request.str());
            }
        }
    }
    delete client;
    
    return NULL;
}
