#ifndef CONFIG_H
#define CONFIG_H

#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <vector>
#include <sys/stat.h>
#include <sys/types.h>
#include <cstdint>
#include <errno.h>

class CutiePieConfig;

class CutiePieConfig {

private:  
    
  std::string homedir;
  std::string filename;
  
  std::string homescript;
  std::string userscript;
  
public:
  CutiePieConfig();
  ~CutiePieConfig();

  bool dirExist();
  bool fileExist();
  void dirCreate();
  void fileCreate();
  void fileScan();
  std::string getHomeScriptDir();
  std::string getUserScriptDir();  
  
};

#endif
