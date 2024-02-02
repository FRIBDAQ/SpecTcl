#ifndef CPYCONVERTER_H
#define CPYCONVERTER_H
#include <Python.h>
#include <iostream>
#include <string>
#include <vector>
#include <random>
#include <chrono>
#include "dataAccess.h"
#include "SpecTclMirrorClient.h"

class CPyConverter {

 public:
  CPyConverter();
  virtual ~CPyConverter();

  std::vector<int> m_id;  
  std::vector<std::string> m_names;
  std::vector<int> m_dim;
  std::vector<int> m_binx;
  std::vector<float> m_minx;
  std::vector<float> m_maxx;
  std::vector<int> m_biny;      
  std::vector<float> m_miny;      
  std::vector<float> m_maxy;      
  std::vector<int> m_types;    

  void extractInfo(char* speclist);
  PyObject* Update(char* hostname, char* port, char* mirror, char* user);
  PyObject* ShMemToNpArray(void* addr, int dim, int nbinx, int nbiny, int type);
  PyObject* DebugFillSpectra();

  PyObject* vectorToList_Int(const std::vector<int> &data);  
  PyObject* vectorToList_Float(const std::vector<float> &data);
  PyObject* vectorToList_String(const std::vector<std::string> &data);  
  
};

#endif
