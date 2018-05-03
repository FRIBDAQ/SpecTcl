
#include "JsonParser.h"

#include <vector>
#include <stdexcept>
#include "HistInfo.h"
#include <json/json.h>

using namespace std;

namespace SpJs
{
  std::vector<HistInfo> JsonParser::parseListCmd(const Json::Value& value) 
  {
    using Json::Value;

    // fail if we did not retrieve a proper value
    if (value["status"].asString() != "OK") {
      throw std::runtime_error("Cannot parse json, status != OK");
    }

    vector<HistInfo> results;

    const Value& detail = value["detail"];

    int size = detail.size();
    for (int index=0; index<size; ++index) {
      const Value& item = detail[index];
      results.push_back(parseListDetail(item));
    }

    return results;
  }


  HistInfo JsonParser::parseListDetail(const Json::Value& item) 
  {
    using Json::Value;
    HistInfo info;

    info.s_name = item["name"].asString();
    info.s_type = item["type"].asString();
    info.s_chanType = mapChanType(item["chantype"].asString());

    // load the parameters
    const Value& params = item["parameters"];
    for (int iParam=0; iParam<params.size(); ++iParam) {
      info.s_params.push_back(params[iParam].asString());
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
    // But wait there's more.  If the spectrum is an 2dmproj type,
    // there's a projection direction and an array of ROI gates.
    
    if (info.s_type == "2dmproj") {
      info.s_direction = (item["projection"].asString() == "x");
      const Value& roiGates = item["roigates"];
      for (int i =0; i < roiGates.size(); i++) {
        info.s_roi.push_back(roiGates[i].asString());
      }
    }

    return info;
  }

  ChanType JsonParser::mapChanType(const std::string& type) 
  {
    ChanType index = ChanType::Short;
    if (type == "short") {
      index = ChanType::Short;
    } else if (type=="long") {
      index = ChanType::Long; 
    }
    return index;
  }

  HistContent JsonParser::parseContentCmd(const Json::Value& value)
  {
    using Json::Value;

    // throw if the status is not ok
    if (value["status"].asString() != "OK") {
      throw std::runtime_error("Json cannot be parsed because status != OK");
    }

    HistContent content;
    vector<BinInfo > result;

    // get the detail portion
    const Value& detail = value["detail"];

    const Value& stats  = detail["statistics"];
    content.setUnderflow(0, stats["xunderflow"].asDouble());
    content.setOverflow(0, stats["xoverflow"].asDouble());

    if (stats.isMember("yunderflow")) {
        content.setUnderflow(1, stats["yunderflow"].asDouble());
    }
    if (stats.isMember("yoverflow")) {
        content.setOverflow(1, stats["yoverflow"].asDouble());
    }

    const Value& values = detail["channels"];
    // loop over all entries of the detail list of objects
    int nPoints = values.size();
    for (int index=0; index<nPoints; ++index) {
      const Value& point = values[index];
      BinInfo bin;

      if (point.isMember("x")) {
        bin.s_xbin = point["x"].asInt();
      } 

      if (point.isMember("y")) {
        bin.s_ybin = point["y"].asInt();
      }

      bin.s_value = point["v"].asDouble();

      result.push_back(bin);
    }

    content.setValues(result);
    
    return content;
  }

  vector<ParameterInfo> JsonParser::parseParameterCmd(const Json::Value& value)
  {
      using Json::Value;

      if (value["status"].asString() != "OK") {
          throw std::runtime_error("Json cannot be parsed because status != OK");
      }

      vector<ParameterInfo> result;
      const Value& detail = value["detail"];

      int nParams = detail.size();
      for (int index=0; index<nParams; ++index) {
          ParameterInfo paramInfo;
          const Value& param = detail[index];
          paramInfo.s_name = param["name"].asString();
          paramInfo.s_id = param["id"].asInt();
          paramInfo.s_nBins = param["bins"].asInt();
          paramInfo.s_low = param["low"].asDouble();
          paramInfo.s_high = param["hi"].asDouble();
          paramInfo.s_units = param["units"].asString();

          result.push_back(paramInfo);
      }

      return result;
  }

}
