#include "CPyConverter.h"
#include <stdexcept>
#include <sys/shm.h>
#include <sys/stat.h>
#include <cstdint>

bool debug = false;

CPyConverter::CPyConverter()
{
}

CPyConverter::~CPyConverter()
{
}

int
CPyConverter::getRandomNumber(std::string func)
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


struct Memory  *
CPyConverter::access_shm()
{
  key_t          shm_key;
  int            shm_id;
  struct Memory  *shm_ptr;
  
  shm_key = ftok(".", 'x');
  shm_id = shmget(shm_key, sizeof(struct Memory), IPC_CREAT | 0666);
  if (shm_id < 0) {
    printf("*** shmget error (client) ***\n");
    exit(1);
  }
  printf("Client has received a shared memory...\n");

  shm_ptr = (struct Memory *) shmat(shm_id, NULL, 0);
  if ((intptr_t) shm_ptr == -1) {
    printf("*** shmat error (client) ***\n");
    exit(1);
  }
  printf("Client has attached the shared memory...\n");
  std::cout << "Client shm_key " << shm_key << " shm_id " << shm_id << std::endl;
  
  while (shm_ptr->status != FILLED)
    ;

  printf("   Client found the data is ready...\n");  

  return shm_ptr;
}

/*
void
CPyConverter::clean_shm()
{
  shm_ptr->status = TAKEN;
  printf("   Client has informed server data have been taken...\n");
  shmdt((void *) shm_ptr);
  printf("   Client has detached its shared memory...\n");
  printf("   Client exits...\n");
}
*/

PyObject*
CPyConverter::generate(std::string func, int size)
{
  struct Memory *shm_ptr = access_shm();
  std::cout << "Status " << shm_ptr->status << std::endl;
  for (int i=0; i<size; i++)
    std::cout << shm_ptr->data[i] << " ";
  std::cout << std::endl;
  
  m_v.clear();
  for (int i = 0;i < size ; i++)
    {
      m_v.push_back(getRandomNumber(func));
    }
  if (debug){
    if (m_v.size() == size)
      std::cout << "Generated vector of size: " << size << std::endl;
  }
  
  return vectorToList_Int(m_v);  
}

PyObject*
CPyConverter::vectorToList_Int(const std::vector<int> &data) {
  PyObject* listObj = PyList_New( data.size() );
  if (!listObj) throw std::logic_error("Unable to allocate memory for Python list");
  for (unsigned int i = 0; i < data.size(); i++) {
    PyObject *num = PyLong_FromLong(data[i]);
    if (!num) {
      Py_DECREF(listObj);
      throw std::logic_error("Unable to allocate memory for Python list");
    }
    PyList_SET_ITEM(listObj, i, num);
  }
  return listObj;
}

PyObject*
CPyConverter::vectorToList_Float(const std::vector<float> &data) {
  PyObject* listObj = PyList_New( data.size() );
  if (!listObj) throw std::logic_error("Unable to allocate memory for Python list");
  for (unsigned int i = 0; i < data.size(); i++) {
    PyObject *num = PyFloat_FromDouble( (double) data[i]);
    if (!num) {
      Py_DECREF(listObj);
      throw std::logic_error("Unable to allocate memory for Python list");
    }
    PyList_SET_ITEM(listObj, i, num);
  }
  return listObj;
}

std::string
CPyConverter::print() const
{
  std::string tmp = "test";
  return tmp;
}
