
#ifndef HISTFILLER_H
#define HISTFILLER_H

#include <vector>
#include <utility>
#include "BinInfo.h"

class TH1;

namespace SpJs 
{


  class HistFiller {

    public:
      void operator()(TH1& hist, 
                      const std::vector<BinInfo>& points);
  };

}

#endif
