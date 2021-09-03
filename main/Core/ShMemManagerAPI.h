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

#include <pthread.h>
#include <iostream>
#include <zmq.hpp>

class ShMemManagerAPI {

 private:
  // ctor, dtor for ShMemManagerAPI
  ShMemManagerAPI(){}; 
  ShMemManagerAPI(ShMemManagerAPI const&){}; 
  ShMemManagerAPI& operator=(ShMemManagerAPI const&){};

  // Core of ShMemManagerAPI
  static ShMemManagerAPI* m_pInstance;
  static zmq::context_t* m_pContextSingleton;
  
  pthread_t server;

 public:

  // Public methods 
  static ShMemManagerAPI* getInstance();
  static zmq::context_t*  getContext();

  void CreateShMem(); // Create shared memory
  void CreateThread(); // Create publisher thread
  void JoinThread(); // Join publisher thread
  void DetachThread(); // Detach publisher thread  
  void Destroy();

};
