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

class SubManagerAPI {

 private:
  // ctor, dtor for SubscriberManagerAPI
  SubManagerAPI(){}; 
  SubManagerAPI(SubManagerAPI const&){}; 
  SubManagerAPI& operator=(SubManagerAPI const&){};
  
  // Core of SubscriberManagerAPI
  static SubManagerAPI* m_pInstance;
  static zmq::context_t* m_pContextSingleton;
  
  pthread_t subscriber;

 public:

  // Public methods 
  static SubManagerAPI* getInstance();
  static zmq::context_t*  getContext();

  void CreateThread(); // Create publisher thread
  void JoinThread(); // Join publisher thread
  void DetachThread(); // Detach publisher thread  
  void Destroy();

};
