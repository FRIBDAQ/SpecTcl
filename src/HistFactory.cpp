

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

//  void fillTH1(const Json::Value& value, TH1& hist)
//  {
//    using Json::Value;
//
//    // this is all about setting the values and not about filling.
//    if (value["status"] != "OK") {
//      throw std::runtime_error("Content status is not OK");
//    }
//
//    const Value& detail = value["detail"];
//    size_t size = detail.size();
//    for(int index=0; index<size; ++index) {
//      const Value& bc = detail[index];
//      int xbin = bc["xchan"].asInt();
//      // if this is 2d spectrum there would be a "ychan"
//      int val = bc["value"].asInt();
//      hist.SetBinContent(xbin,val);
//    }
//  }
}



