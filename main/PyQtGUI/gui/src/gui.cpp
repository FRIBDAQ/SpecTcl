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

int main(int argc, char *argv[])
{
  CPyHelper pInstance;
  
  PyObject *obj = Py_BuildValue("s", "Main.py");
  FILE *file = _Py_fopen_obj(obj, "r+");
  if(file != NULL) {
    PyRun_SimpleFile(file, "Main.py");
  }
  
  return 0;
}

