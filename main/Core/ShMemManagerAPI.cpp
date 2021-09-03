/*
    This software is Copyright by the Board of Trustees of Michigan
    State University (c) Copyright 2019.

    You may use this software under the terms of the GNU public license
    (GPL).  The terms of this license are described at:

     http://www.gnu.org/licenses/gpl.txt

     Author:
             Giordano Cerizza
             NSCL
             Michigan State University
             East Lansing, MI 48824-1321
*/

#include <mutex>
#include "zhelpers.hpp"
#include "ShMemManagerAPI.h"
#include "ShMemAPI.h"

zmq::context_t* ShMemManagerAPI::m_pContextSingleton(nullptr);
ShMemManagerAPI* ShMemManagerAPI::m_pInstance = 0;

ShMemManagerAPI*
ShMemManagerAPI::getInstance()
{
  if (!m_pInstance) 
    m_pInstance = new ShMemManagerAPI;

  return m_pInstance;
}

zmq::context_t*
ShMemManagerAPI::getContext()
{
  if (!m_pContextSingleton) {
    m_pContextSingleton =
      new zmq::context_t(1);
  }
  return m_pContextSingleton;
}

void
ShMemManagerAPI::CreateShMem()
{}

void
ShMemManagerAPI::CreateThread()
{
  // Creating publisher task
  if (debug)
    std::cout << "Setting up publisher..." << std::endl; 
  pthread_create(&server, nullptr, ShMemAPI::publisher_task,  (void*)ShMemManagerAPI::getInstance()->getContext());  

  if (debug)
    std::cout << "...Done! " << std::endl;
}

void
ShMemManagerAPI::JoinThread()
{
  if (debug)
    std::cout << "Joining threads" << std::endl;
  pthread_join(server, nullptr);
}

void
ShMemManagerAPI::DetachThread()
{
  if (debug)
    std::cout << "Detaching threads" << std::endl;
  pthread_detach(server);
}

void
ShMemManagerAPI::Destroy()
{
  delete m_pInstance;
  m_pInstance = NULL;
}
