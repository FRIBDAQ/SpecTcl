#include <iostream>
#include <vector>
#include <string>
#include <random>
#include <chrono>
#include <sys/shm.h>
#include <sys/stat.h>
#include <Python.h>
#include "CPyHelper.h"
#include <cstdint>
#include <errno.h>
#include "shm_ex.h"

int
getRandomNumber(std::string func)
{
  unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
  std::default_random_engine generator (seed);

  std::uniform_int_distribution<int> mean(400,600);
  std::uniform_int_distribution<int> sigma(20,50);

  std::normal_distribution<double> distribution(mean(generator),sigma(generator));

  int randomNumber = 0;
  randomNumber = (int)distribution(generator);

  return randomNumber;
}

void
create_shm()
{
  key_t          shm_key;
  int            shm_id;
  struct Memory  *shm_ptr;

  std::cout << "Creating shared memory..." << std::endl;

  shm_key = ftok(".", 'x');
  shm_id = shmget(shm_key, sizeof(struct Memory), IPC_CREAT | 0666);
  if (shm_id < 0) {
    printf("*** shmget error (server) ***\n");
    printf("%s\n",strerror(errno));
    exit(1);
  }
  printf("Server has received a shared memory ...\n");

  shm_ptr = (struct Memory *) shmat(shm_id, NULL, 0);
  if ((intptr_t) shm_ptr == -1) {
    printf("*** shmat error (server) ***\n");
    exit(1);
  }
  printf("Server has attached the shared memory...\n");
  std::cout << "Server shm_key " << shm_key << " shm_id " << shm_id << " size: " << sizeof(struct Memory) << std::endl;

  // fill the data  
  shm_ptr->status  = NOT_READY;

  for (int i = 0; i < data_size ; i++){
    shm_ptr->data[i] = getRandomNumber("");
  }

  printf("Server has filled shared memory...\n");
  shm_ptr->status = FILLED;
  
  std::cout << "Done writing..." << std::endl;  

  /*
  while (shm_ptr->status != TAKEN)
    sleep(1);

  printf("Server has detected the completion of its child...\n");
  shmdt((void *) shm_ptr);
  printf("Server has detached its shared memory...\n");
  shmctl(shm_id, IPC_RMID, NULL);
  printf("Server has removed its shared memory...\n");
  printf("Server exits...\n");
  */
  
}

int main(int argc, char *argv[])
{
  // create shared memory segment
  create_shm();
  
  CPyHelper pInstance;
  
  PyObject *obj = Py_BuildValue("s", "gui.py");
  FILE *file = _Py_fopen_obj(obj, "r+");
  if(file != NULL) {
    PyRun_SimpleFile(file, "gui.py");
  }
  
  return 0;
}

