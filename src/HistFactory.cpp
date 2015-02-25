

#include <json/json.h>
#include <TH1.h>
#include <TH2.h>
#include <string>
#include <iostream>
#include <vector>
#include <memory>
#include <stdexcept>
#include "HistFactory.h"
#include "HistInfo.h"
#include "JsonParser.h"

using namespace std;

namespace SpJs
{

  std::unique_ptr<TH1> HistFactory::create(const char* jsonByteRep) 
  {
    return create(Json::Value(jsonByteRep));
  }

  std::unique_ptr<TH1> HistFactory::create(const Json::Value& value) 
  {
    // throws if bad
    std::vector<HistInfo> infos;
    try {
      infos = JsonParser().parseListCmd(value);
    } catch (std::exception& exc) {
      return std::unique_ptr<TH1>();
    }

    // only deals with the first one
    return create(infos.at(0));
  }

  std::unique_ptr<TH1> HistFactory::create(const HistInfo& info) {
    std::unique_ptr<TH1> pHist;

    if (info.s_axes.size() == 1) {
      pHist.reset(new TH1D(info.s_name.c_str(), 
                           info.s_name.c_str(), 
                           info.s_axes.at(0).s_nbins,
                           info.s_axes.at(0).s_low, 
                           info.s_axes.at(0).s_high));
      pHist->SetDirectory(0);
    } else if (info.s_axes.size() == 2) {
      pHist.reset(new TH2D(info.s_name.c_str(), 
                           info.s_name.c_str(), 
                           info.s_axes.at(0).s_nbins,
                           info.s_axes.at(0).s_low, 
                           info.s_axes.at(0).s_high,
                           info.s_axes.at(1).s_nbins,
                           info.s_axes.at(1).s_low, 
                           info.s_axes.at(1).s_high));
      pHist->SetDirectory(0);
    }
    return pHist;
  }

}



