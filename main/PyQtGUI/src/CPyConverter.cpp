#include "CPyConverter.h"
#include <stdexcept>
#include <sys/shm.h>
#include <sys/stat.h>
#include <cstdint>
#include <iostream>
#include <sstream>
#include <string>
#include "dataRetriever.h"
#include <numpy/arrayobject.h>

bool debug = false;

CPyConverter::CPyConverter()
{}

CPyConverter::~CPyConverter()
{}

void
CPyConverter::extractInfo(char* speclist)
{
  std::stringstream ss;
  int id, dim, binx, biny;
  float minx, miny, maxx, maxy;
  std::string name;
  ss << std::string(speclist);  
  while (!ss.eof() &&
	 (ss >> id >> name >> dim >> binx >> minx >> maxx >> biny >> miny >> maxy)) {
    m_id.push_back(id);
    m_names.push_back(name);
    m_dim.push_back(dim);
    m_binx.push_back(binx);
    m_biny.push_back(biny);
    m_minx.push_back(minx);
    m_miny.push_back(miny);      
    m_maxx.push_back(maxx);
    m_maxy.push_back(maxy);      
  }    
}

PyObject*
CPyConverter::Update(char* hostname, char* port)
{
  std::cout << "Inside CPyConverter::Update()" << std::endl;
  std::cout << "hostname: " << hostname << " port: " << port << std::endl;

  std::string _hostname = hostname;
  std::string _port = port;  
  std::cout << "Hostname --> " << _hostname << " Port --> " << _port << std::endl;
  
  dataRetriever* d = dataRetriever::getInstance();
  d->SetHostPort(_hostname,_port);
  d->InitShMem();
  spec_shared *p = d->GetShMem();
  
  char **speclist;
  int lsize;
  lsize = p->GetSpectrumList(&speclist);

  Address_t addr;
  PyObject* data[lsize];
  PyObject* listData = PyList_New(lsize);
  
  for (int i = 0; i < lsize; i++){
    extractInfo(speclist[i]);
    // access histogram of id i
    addr = p->CreateSpectrum(m_id.at(i));
    // convert memory to np array
    data[i] = ShMemToNpArray(addr, m_dim[i], m_binx[i], m_biny[i]);
    // add nparray to a list
    PyList_SetItem(listData, i, data[i]);
  }
  
  PyObject* result = PyTuple_New(10);
  PyTuple_SetItem(result, 0, vectorToList_Int(m_id));
  PyTuple_SetItem(result, 1, vectorToList_String(m_names));
  PyTuple_SetItem(result, 2, vectorToList_Int(m_dim));  
  PyTuple_SetItem(result, 3, vectorToList_Int(m_binx));
  PyTuple_SetItem(result, 4, vectorToList_Float(m_minx));
  PyTuple_SetItem(result, 5, vectorToList_Float(m_maxx));
  PyTuple_SetItem(result, 6, vectorToList_Int(m_biny));
  PyTuple_SetItem(result, 7, vectorToList_Float(m_miny));
  PyTuple_SetItem(result, 8, vectorToList_Float(m_maxy));
  PyTuple_SetItem(result, 9, listData);
  
  return result;
}

PyObject*
CPyConverter::ShMemToNpArray(void* addr, int size, int nbinx, int nbiny)
{
  PyArrayObject* data;

  import_array();

  if (size == 1){
    npy_intp dims = {nbinx};
    data = (PyArrayObject*)PyArray_SimpleNewFromData(size, &dims, NPY_INT, addr);
  }
  else {
    npy_intp dims[size] = {nbiny, nbinx};
    data = (PyArrayObject*)PyArray_SimpleNewFromData(size, dims, NPY_INT, addr);
  }
  return (PyObject*)data;
}

PyObject*
CPyConverter::DebugFillSpectra()
{
  dataRetriever* d = dataRetriever::getInstance();
  d->InitShMem();
  spec_shared *dp = d->GetShMem();

  import_array();

  char **speclist;
  int lsize;
  lsize = dp->GetSpectrumList(&speclist);

  // testing
  Address_t addr[2];
  PyArrayObject* data[2];
  int id = 0;  
  addr[id] = dp->CreateSpectrum(id);
  
  // 1D this is fine
  npy_intp dims = {dp->getxdim(id)};
  data[id] = (PyArrayObject*)PyArray_SimpleNewFromData(1, &dims, NPY_INT, addr[id]);

  //2d
  id = 1;
  addr[id] = dp->CreateSpectrum(id);  
  npy_intp dims2[2];
  dims2[0] = dp->getxdim(id);
  dims2[1] = dp->getydim(id);  
  data[id] = (PyArrayObject*)PyArray_SimpleNewFromData(2, dims2, NPY_INT, addr[id]);
  
  return Py_BuildValue("NN", (PyObject*)data[0], (PyObject*)data[1]);
}

PyObject*
CPyConverter::vectorToList_Int(const std::vector<int> &data) {
  PyObject* listObj = PyList_New(data.size() );
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
  PyObject* listObj = PyList_New(data.size() );
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

PyObject*
CPyConverter::vectorToList_String(const std::vector<std::string> &data) {
  PyObject* listObj = PyList_New(data.size() );
  if (!listObj) throw std::logic_error("Unable to allocate memory for Python list");  
  for (unsigned int i = 0; i < data.size(); i++) {
    PyObject* newstring;
    newstring = PyUnicode_DecodeUTF8(data[i].c_str(), data[i].length(), NULL);
    if(newstring == NULL) {
      Py_DECREF(listObj);
      throw std::logic_error("Unable to allocate memory for Python list");      
    }
    PyList_SET_ITEM(listObj, i, newstring);  
  }
  return listObj;
}
