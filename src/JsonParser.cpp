
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

}
