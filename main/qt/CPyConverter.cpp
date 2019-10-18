#include "CPyConverter.h"
#include <stdexcept>
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

PyObject*
CPyConverter::generate(std::string func, int size)
{
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
