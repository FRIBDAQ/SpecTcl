
#include "ParameterCmdParser.h"

#include <vector>
#include <stdexcept>
#include "HistInfo.h"
#include <json/json.h>

using namespace std;

namespace SpJs
{
  vector<ParameterInfo> ParameterCmdParser::parseList(const Json::Value& value)
  {
      using Json::Value;

      if (value["status"].asString() != "OK") {
          throw std::runtime_error("Json cannot be parsed because status != OK");
      }

      vector<ParameterInfo> result;
      const Value& detail = value["detail"];

      // Loop through the list of parameters and store the contents
      int nParams = detail.size();
      for (int index=0; index<nParams; ++index) {
          ParameterInfo paramInfo;
          const Value& param = detail[index];

          paramInfo.s_name  = param["name"].asString();
          paramInfo.s_id    = param["id"].asInt();
          paramInfo.s_nBins = param["bins"].asInt();
          paramInfo.s_low   = param["low"].asDouble();
          paramInfo.s_high  = param["hi"].asDouble();
          paramInfo.s_units = param["units"].asString();

          result.push_back(paramInfo);
      }

      return result;
  }

}
