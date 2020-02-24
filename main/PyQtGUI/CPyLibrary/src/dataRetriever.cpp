#include <bits/stdc++.h>
#include <cstring>
#include <algorithm>
#include <iostream>
#include <array>
#include <memory>
#include <stddef.h>  // defines NULL
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#include "dataRetriever.h"

dataRetriever* dataRetriever::m_pInstance = NULL;
static int memsize;

dataRetriever*
dataRetriever::getInstance()
{
  if (!m_pInstance)   
    m_pInstance = new dataRetriever;

  return m_pInstance;
}

void
dataRetriever::InitShMem()
{
  std::array<char, 512> buffer;
  std::string filename, tmp;

  std::string command = "python3 ";
  const char* pwd = getenv("PWD");
  filename = std::string(pwd)+"/shm_parser.py";
  command += filename;
  FILE* pipe = popen(command.c_str(), "r");

  if (!pipe)
    std::cerr << "Couldn't start command." << std::endl;

  while (fgets(buffer.data(), 512, pipe) != NULL) {
    tmp += buffer.data();
  }
  auto returnCode = pclose(pipe);

  tmp.erase(std::remove(tmp.begin(), tmp.end(), '\n'), tmp.end());

  std::istringstream ss(tmp);
  do {
    std::string word;
    ss >> word;
    shm.push_back(word);
  } while (ss); 
  
  key = shm.at(0);
  char* name = new char[key.length()+1];
  std::strcpy(name, key.c_str());
  if(name == NULL) {
    perror("Could not translate shared memory name");
    exit(errno);
  }
  size = stoi(shm.at(1));
  if(size == 0) {
    fprintf(stderr, "Shared memory size string illegal");
    exit(-1);
  }
  
  // mapping to memory
  memsize = sizeof(spec_shared) - SPECBYTES + size;

  shmem = dataRetriever::mapmem(name, memsize);
  spectra = shmem;
  if(spectra == (shared_memory *)NULL) {
    perror("Map to shared memory failed!!");
    exit(errno);
  }

  
}

spec_shared*
dataRetriever::mapmem(char* name, unsigned int size)
#ifdef HAVE_SHM_OPEN
{
  int fd = shm_open(name, O_RDWR, S_IRUSR | S_IWUSR);
  if(fd < 0) {
    perror("shm_open failed!");
    return NULL;
  }

  void* pMem;
#ifdef HAVE_MMAP
  pMem = mmap(NULL, size, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
#else
  read(fd, pMem, size);
#endif

  if(pMem == (char*)-1) {
    perror("mmap failed!");
    return NULL;
  }
  close(fd);
  shm_unlink(name);
  return (spec_shared*)pMem;
}
#else
{
  key_t key; // Shared memory key.
  int   id; // Shared memory size. 
  char *memory;

  memcpy(&key, name, sizeof(key));


  id  = shmget(key, size, 0); // Get the memory key
  if(id < 0) {
    return (spec_shared *)NULL;
  }

  memory = (char *)shmat(id, NULL, SHM_RDONLY);

  return (spec_shared *)memory;
}
#endif

void
dataRetriever::PrintOffsets()
{
  spec_shared *p(0);
  printf("Offsets into shared mem: \n");
  printf("  dsp_xy      = %p\n", (void*)p->dsp_xy);
  printf("  dsp_titles  = %p\n", (void*)p->dsp_titles);
  printf("  dsp_types   = %p\n", (void*)p->dsp_types);
  printf("  dsp_map     = %p\n", (void*)p->dsp_map);
  printf("  dsp_spectra = %p\n", (void*)&(p->dsp_spectra));
  printf("  Total size  = %d\n", sizeof(spec_shared));

}

spec_shared*
dataRetriever::GetShMem()
{
  return shmem;
}

char*
dataRetriever::MemoryTop()
{
  char *bottom = (char *)shmem;
  bottom      += (memsize-1);
  return      bottom;
}
