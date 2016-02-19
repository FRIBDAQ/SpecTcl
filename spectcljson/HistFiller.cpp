
#include "HistFiller.h"
#include <vector>
#include <utility>
#include <TH1.h>

using namespace std;

namespace SpJs
{
  void HistFiller::operator()(TH1& hist,
                              const vector<BinInfo>& points)
  {
    for (auto point : points) {
      int xbin = point.s_xbin;
      int ybin = point.s_ybin;
      double content = point.s_value;
      hist.SetBinContent(xbin,ybin,content);
    }

  }

} // end of namespace
