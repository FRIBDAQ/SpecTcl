#include <string>
#include <vector>
#include "dataAccess.h"
#include "dataTypes.h"

class dataRetriever{
  
 private:
  std::vector<std::string> shm;
  std::string key;
  unsigned int size;

  std::string _hostname;
  std::string _port;
  
  spec_shared *shmem;
  spec_shared *spectra;
  
  dataRetriever(){};  // Private so that it can  not be called
  dataRetriever(dataRetriever const&){};             // copy constructor is private
  dataRetriever& operator=(dataRetriever const&){ return *this; };  // assignment operator is private
  static dataRetriever* m_pInstance;

 public:

  static dataRetriever* getInstance();

  void SetHostPort(std::string host, std::string port);  
  void InitShMem();
  std::string ShMemKeyGet() { return key; }
  unsigned int ShMemSizeGet() { return size; }

  static spec_shared* mapmem(char* name, unsigned int size);
  spec_shared* GetShMem();
  static void PrintOffsets();
  char *MemoryTop();
  
};
