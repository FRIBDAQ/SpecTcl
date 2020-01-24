#ifndef CPYCONVERTER_H
#define CPYCONVERTER_H
#include <Python.h>
#include <iostream>
#include <string>
#include <vector>
#include <random>
#include <chrono>

class CPyConverter {

 public:
  CPyConverter();
  virtual ~CPyConverter();

  std::vector<int> m_v;

  std::string print() const;
  int getRandomNumber(std::string func);

  PyObject* generate(std::string func, int size); 
  PyObject* vectorToList_Int(const std::vector<int> &data);  
  PyObject* vectorToList_Float(const std::vector<float> &data);
  //  PyObject* arrayFromSpectrum(Address_t p, int id);
  
};

#endif
