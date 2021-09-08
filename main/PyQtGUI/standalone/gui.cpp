#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <random>
#include <chrono>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <Python.h>
#include "CPyHelper.h"
#include <cstdint>
#include <errno.h>
#include "config.h"
#include "SubManagerAPI.h"

int main(int argc, char *argv[])
{
  CPyHelper pInstance;
  CutiePieConfig conf;
  std::string path;
  
  if(!getenv("INSTDIR") || !getenv("USERDIR")){
    if (!conf.dirExist())
      conf.dirCreate();

    if (!conf.fileExist())
      conf.fileCreate();

    conf.fileScan();

    std::cout << "\t#################################" << std::endl;
    std::cout << "\t# Usage for CutiePie standalone #:" << std::endl;
    std::cout << "\t#################################" << std::endl;
    std::cout << "\t Please set the following env variables" << std::endl;
    std::cout << "\t export INSTDIR="+conf.getHomeScriptDir() << std::endl;
    std::cout << "\t and"<< std::endl;
    std::cout << "\t export USERDIR="+conf.getUserScriptDir() << std::endl;    

    return 0;
  } else {
    std::cout << "All good!" << std::endl;
  }

  SubManagerAPI* api = SubManagerAPI::getInstance();
  api->CreateThread();
    
  std::string filename = std::string(getenv("INSTDIR"))+"/Script/Main.py";
  std::cout << "filename: " << filename << std::endl;

  try {
    PyObject *obj = Py_BuildValue("s", filename.c_str());
    FILE *file = _Py_fopen_obj(obj, "r+");
    if(file != NULL) {
      PyRun_SimpleFile(file, filename.c_str());
    }
  }
  catch (std::exception& e)
    {
      std::cout << e.what() << '\n';
    }

  return 0;
}

