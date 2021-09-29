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

struct Data
{
  int a;
  int arr[3];
};

struct arg_struct {
  unsigned int shmem_size;
  struct Data* shmem_data;
  zmq::context_t* ctx;
};


class ShMemManagerAPI {

 private:
  // ctor, dtor for ShMemManagerAPI
  ShMemManagerAPI(){}; 
  ShMemManagerAPI(ShMemManagerAPI const&){}; 
  ShMemManagerAPI& operator=(ShMemManagerAPI const&){ return *this; };

  // Core of ShMemManagerAPI
  static ShMemManagerAPI* m_pInstance;
  static zmq::context_t* m_pContextSingleton;

  int shm_id;
  struct arg_struct* args;
  
  unsigned int p_size;
  struct Data *p_shmem;  // test shmem
  pthread_t server;

 public:

  // Public methods 
  static ShMemManagerAPI* getInstance();
  static zmq::context_t*  getContext();

  int CreateTestShMem(); // Create test shared memory
  struct Data * GetShMem(); // Returns the test shmem

  void CopyShMem(/* */); // Copy shared memory  
  void SetSizeShMeM(unsigned int size); // Set shared memory size for hoisting 
  unsigned int GetSizeShMeM(); // Get shared memory size for hoisting 

  void CreateThread(); // Create publisher thread
  void JoinThread(); // Join publisher thread
  void DetachThread(); // Detach publisher thread  
  void Destroy();

};
