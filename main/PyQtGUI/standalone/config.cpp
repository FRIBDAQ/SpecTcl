#include "config.h"

CutiePieConfig::CutiePieConfig()
{
}

CutiePieConfig::~CutiePieConfig()
{
}

bool
CutiePieConfig::dirExist()
{
  struct stat info;
  homedir = std::string(getenv("HOME"))+"/.CutiePie";

  if( stat(homedir.c_str(), &info ) != 0 ){
    std::cout << "Dir doesn't exist!" << std::endl;
    return false;
  } else {
    std::cout << homedir << " exists!" << std::endl;
  }
  return true;
}

void
CutiePieConfig::dirCreate()
{
  if (mkdir(homedir.c_str(), 0777) == -1)
    std::cerr << "Error :  " << std::strerror(errno) << std::endl;
  else
    std::cout << "Directory " << homedir << "  created" << std::endl;
}

bool
CutiePieConfig::fileExist()
{
  filename = homedir+"/path.config";
  std::ifstream infile(filename.c_str());

  if (!infile.good()){
    std::cout << filename << " doesn't exists" << std::endl;
    return false;
  } else {
    std::cout << filename << " exists" << std::endl;
  }

  return true;
}

void
CutiePieConfig::fileCreate()
{
  std::string instPath(INSTALLED_IN);
  std::ofstream outfile (filename.c_str());
  outfile << "MAIN" << std::endl;
  outfile << instPath << std::endl;
  outfile << "USER" << std::endl;
  outfile << "/user/mrbanana/home/algo" << std::endl; 
  outfile.close();
  std::cout << "File " << filename << "  created" << std::endl;  
}

void
CutiePieConfig::fileScan()
{
  std::string line;
  std::ifstream file(filename.c_str());
  int count = 0;
  int iter = 0;
  
  if (file.is_open()){
    while (getline(file,line)){  
      count++;
      if (line == "MAIN"){
	iter = count+1;
	if(count = iter){
	  getline(file,line);
	  homescript = line;	  
	}
      }
      count = 0;
      iter = 0;
      if (line == "USER"){
	iter = count+1;
	if(count = iter){
	  getline(file,line);
	  userscript = line;
	}
      }
    }
    file.close();
  }
  else
    std::cout << "Unable to open config file" << std::endl;
}

std::string
CutiePieConfig::getHomeScriptDir()
{
  return homescript;
}

std::string
CutiePieConfig::getUserScriptDir()
{
  return userscript;
}
