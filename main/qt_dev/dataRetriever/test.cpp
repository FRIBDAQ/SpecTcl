#include <iostream>
#include "dataRetriever.h"

int main(int argc, char *argv[])
{
  dataRetriever* d = dataRetriever::getInstance();
  
  d->InitShMem();
  std::cout << "Shared memory key: " << d->ShMemKeyGet() << " size: " << d->ShMemSizeGet() << std::endl;  
  d->PrintOffsets();

  spec_shared *p = d->GetShMem();

  char **speclist;
  spec_title title;
  int lsize;
  lsize = p->GetSpectrumList(&speclist);
  std::cout << "Number of defined spectra: " << lsize << std::endl;
  for(int i = 0; i < lsize; i++) {
    std::cout << speclist[i] << std::endl;
  }

  return 0;
}

