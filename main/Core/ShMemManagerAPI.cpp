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
#include <iostream>
#include <string>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include <mutex>
#include "zhelpers.hpp"
#include "ShMemManagerAPI.h"
#include "ShMemAPI.h"

zmq::context_t* ShMemManagerAPI::m_pContextSingleton(nullptr);
ShMemManagerAPI* ShMemManagerAPI::m_pInstance = 0;

#define NAME_FORMAT "XA%02x"

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

static int genname(char *name)
{
  pid_t pid;
  pid = getpid();		/* Get the process id. */
  pid = (pid & 0xff);		/* Only take the bottom byte of pid. */
  sprintf(name, NAME_FORMAT, (int)pid);	/* Format the name. */
  return 1;
}

void
ShMemManagerAPI::SetSizeShMeM(unsigned int size)
{
  p_size = size;
}

unsigned int
ShMemManagerAPI::GetSizeShMeM()
{
  return p_size;
}

struct Data*
ShMemManagerAPI::GetShMem()
{
  return p_shmem;
}

int
ShMemManagerAPI::CreateTestShMem()
{
  if (debug)
    std::cout << std::endl << "Inside CreateTestShMem" << std::endl;

  pid_t pid;
  key_t key = 1234;

  char name[33];
  if(!genname(name))		/* Generate the shared memory name. */
    return 0;

  SetSizeShMeM(sizeof(struct Data));
  
  memcpy(&key, name, sizeof(key));
  if (debug)
    std::cout << "shmem name: " << name << " key: " << key << std::endl;
  
  shm_id = shmget(key, GetSizeShMeM(), IPC_CREAT | 0666);
  if(shm_id == -1) {
    return 0;
  }
  if (debug)
    std::cout << "shm_id " << shm_id << std::endl;
  
  pid = fork();
  if (debug)
    std::cout << "pid " << pid << std::endl;  

  if (pid == 0) {
    if (debug)
      std::cout << "inside condition pid == 0" << std::endl;
    struct shmid_ds stat;
    
    int sid = setsid();
    shmctl(shm_id, IPC_STAT, &stat);
    
    while (stat.shm_nattch != 0) {
      sleep(1);
      shmctl(shm_id, IPC_STAT, &stat);
    }
    fprintf(stderr, "killing mem %d\n", shm_id);
    shmctl(shm_id, IPC_RMID, 0);
    exit(EXIT_SUCCESS);
  }

  p_shmem = (struct Data *) shmat(shm_id, NULL, 0);
  
  p_shmem->a = 27;
  p_shmem->arr[0] = 1;
  p_shmem->arr[1] = 2;
  p_shmem->arr[2] = 3;

  if (debug){
    std::cout << p_shmem->a << std::endl;
    std::cout << p_shmem->arr[0] << std::endl;
    std::cout << p_shmem->arr[1] << std::endl;
    std::cout << p_shmem->arr[2] << std::endl;
  }
  
  return -1;  
}

void
ShMemManagerAPI::CopyShMem()
{
}

void
ShMemManagerAPI::CreateThread()
{
  ShMemManagerAPI::CreateTestShMem();
  unsigned int size = GetSizeShMeM();
  struct Data* shmem = GetShMem();

  if (debug){
    std::cout << "Inside CreateThread" << std::endl;
    std::cout << "Shmem size: " << size << std::endl;
    std::cout << shmem->a << std::endl;
    std::cout << shmem->arr[0] << std::endl;
    std::cout << shmem->arr[1] << std::endl;
    std::cout << shmem->arr[2] << std::endl;
  }
  
  // passing arguments to the thread
  args = new struct arg_struct;
  args->shmem_size = size;
  args->shmem_data = shmem;
  args->ctx = ShMemManagerAPI::getInstance()->getContext();
  
  // Creating publisher task
  if (debug)
    std::cout << "Setting up server and publisher..." << std::endl; 
  pthread_create(&server, nullptr, ShMemAPI::server_task,  (void*)args);    

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
