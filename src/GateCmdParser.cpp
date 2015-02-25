
#include "GateCmdParser.h"

#include <vector>
#include <memory>
#include <stdexcept>
#include "GateInfo.h"
#include <json/json.h>

using namespace std;

namespace SpJs
{
  vector<unique_ptr<GateInfo>> GateCmdParser::parseList(const Json::Value& value)
  {
    using Json::Value;

    if (value["status"].asString()!="OK") {
      throw std::runtime_error("Cannot parse json because status != OK");
    }

    vector<unique_ptr<GateInfo> > result;
    const Value& detail = value["detail"];

    int nGates = detail.size();
    for (int index=0; index<nGates; ++index) {
      unique_ptr<GateInfo> pInfo;
      const Value& gate = detail[index];

      if (gate["type"].asString()=="s") {
        GateInfo1D* pDerived;
        pInfo.reset(pDerived = new GateInfo1D());

        int nParams = gate["parameters"].size();
        for (int iParam = 0; iParam<nParams; ++iParam) {
          pDerived->s_params.push_back(gate["parameters"][0].asString());
        }
        pDerived->s_low = gate["low"].asDouble();
        pDerived->s_high = gate["high"].asDouble();
      }

      pInfo->s_name = gate["name"].asString();

      result.push_back(std::move(pInfo));
    }
    
    return result;
  }
}
