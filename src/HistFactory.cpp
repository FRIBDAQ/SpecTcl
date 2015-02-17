

#include <json/json.h>
#include <TH1.h>
#include <TH2.h>
#include <string>
#include <iostream>
#include <vector>
#include <memory>
#include <stdexcept>
#include "HistFactory.h"

using namespace std;

enum ChanType { Short=0, Long=1 };

struct AxisInfo {
  double s_low;
  double s_high;
  int s_nbins;
};

struct HistInfo {
  std::string s_name;
  int s_type;
  std::vector<std::string> s_params;
  std::vector<AxisInfo> s_axes;
  ChanType s_chanType; 
};

std::ostream& operator<<(std::ostream& stream, AxisInfo& info) {

  stream << "{" << info.s_low << ", ";
  stream << info.s_high << ", ";
  stream << info.s_nbins << "}";
  return stream;
}
std::ostream& operator<<(std::ostream& stream, HistInfo& info) {

  stream << "name=" << info.s_name;
  stream << "\ntype=" << info.s_type;
  stream << "\nparams:";
  for (auto param : info.s_params) {
    stream << "\n  " << param;
  }
  stream << "\nparams:";
  for (auto ainfo : info.s_axes) {
    stream << "\n  " << ainfo;
  }
  return stream;
}

namespace SpJs
{

  ChanType mapChanType(const std::string& type) {
    ChanType index = ChanType::Short;
    if (type == "short") {
      index = ChanType::Short;
    } else if (type=="long") {
      index = ChanType::Long; 
    }
    return index;
  }


  std::unique_ptr<TH1> HistFactory::create(const char* jsonByteRep) 
  {
    return create(Json::Value(jsonByteRep));
  }

  std::unique_ptr<TH1> HistFactory::create(const Json::Value& value) 
  {
    using Json::Value;

    std::unique_ptr<TH1> pHist;
    // fail if we did not retrieve a proper value
    if (value["status"] != "OK") {
      return pHist;
    }
    
    HistInfo info;

    const Value& item = value["detail"][0];
    info.s_name = item["name"].asCString();
    info.s_type = std::stoi(item["type"].asCString());
    info.s_chanType = mapChanType(item["chantype"].asCString());

    // load the parameters
    const Value& params = item["parameters"];
    for (int iParam=0; iParam<params.size(); ++iParam) {
      info.s_params.push_back(params[iParam].asCString());
    }

    // load the axes
    const Value& axes = item["axes"];
    size_t size = axes.size();
    for (int iAxis=0; iAxis<size; ++iAxis) {
      const Value* axis = &axes[iAxis];
      AxisInfo axInfo;
      axInfo.s_low = (*axis)["low"].asFloat();
      axInfo.s_high = (*axis)["high"].asFloat();
      axInfo.s_nbins = (*axis)["bins"].asInt();
      info.s_axes.push_back(axInfo);
    }

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



