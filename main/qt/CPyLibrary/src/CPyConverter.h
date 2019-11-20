#ifndef CPYCONVERTER_H
#define CPYCONVERTER_H
#include <Python.h>
#include <iostream>
#include <string>
#include <vector>
#include <random>
#include <chrono>
#include "shm_ex.h"

class CPyConverter {

 public:
  CPyConverter();
  virtual ~CPyConverter();

  std::vector<int> m_v;

  std::string print() const;
  int getRandomNumber(std::string func);

  struct Memory* access_shm();
  void clean_shm();
  
  PyObject* generate(std::string func, int size); 
  PyObject* vectorToList_Int(const std::vector<int> &data);  
  PyObject* vectorToList_Float(const std::vector<float> &data);
  
};

#endif
