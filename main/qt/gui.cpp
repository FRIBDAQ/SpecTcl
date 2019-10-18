#include <iostream>
#include <Python.h>
#include "CPyHelper.h"

int main(int argc, char *argv[])
{
  CPyHelper pInstance;
  
  PyObject *obj = Py_BuildValue("s", "gui.py");
  FILE *file = _Py_fopen_obj(obj, "r+");
  if(file != NULL) {
    PyRun_SimpleFile(file, "gui.py");
  }
  
  return 0;
}
