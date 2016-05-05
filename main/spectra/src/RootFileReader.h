
#ifndef ROOTFILEREADER_H
#define ROOTFILEREADER_H

#include <TFile.h>

#include <vector>

namespace Viewer 
{

  class TabWorkspace;

  class RootFileReader 
  {

    public:
      std::vector<TabWorkspace*> readFile(const TFile& file);
  };

} // end Viewer namespace

#endif
