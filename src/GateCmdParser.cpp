
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

      // parse each gate object

      auto typeStr = gate["type"].asString();
      if (typeStr == "s") {
          pInfo = parseSlice(gate);

      } else if (typeStr == "b") {
          pInfo = parseBand(gate);

      } else if (typeStr == "c") {
          pInfo = parseContour(gate);

      } else if (typeStr == "F") {
          pInfo = parseFalse(gate);


      } else {
          string err = "Gate type ";
          err += typeStr;
          err += " is not understood";
          throw runtime_error(err);
      }

      result.push_back(std::move(pInfo));
    }
    
    return result;
  }


  std::unique_ptr<GateInfo> GateCmdParser::parseSlice(const Json::Value &gate)
  {
      Slice* pDerived;
      unique_ptr<GateInfo> pInfo(pDerived = new Slice());

      if (gate["parameters"].size() != 1) {
          throw runtime_error("Gate type \"s\" expects only "
                              "1 parameter but a different number were provided");
      } else {
          pDerived->setParameter(gate["parameters"][0].asString());
      }

      pDerived->setLowerLimit( gate["low"].asDouble() );
      pDerived->setUpperLimit( gate["high"].asDouble() );

      pDerived->setName( gate["name"].asString() );

      return std::move(pInfo);
  }

  std::unique_ptr<GateInfo> GateCmdParser::parseBand(const Json::Value &gate)
  {
      Band* pDerived;
      unique_ptr<GateInfo> pInfo(pDerived = new Band());
      int nParams = gate["parameters"].size();

      if (gate["parameters"].size() == 2) {
          pDerived->setParameter0(gate["parameters"][0].asString());
          pDerived->setParameter1(gate["parameters"][1].asString());
      } else {
          throw runtime_error("Gate type \"b\" expects "
                              "2 parameters but a different amount was provided");
      }

      vector<pair<double,double> > points;
      auto iter = gate["points"].begin();
      auto end = gate["points"].end();
      while (iter!=end) {
            auto& val = *iter;
            auto x = val["x"].asDouble();
            auto y = val["y"].asDouble();

            points.push_back({x,y});

            ++iter;
      }
      pDerived->setPoints(points);
      pDerived->setName( gate["name"].asString() );

      return std::move(pInfo);
  }


  std::unique_ptr<GateInfo> GateCmdParser::parseContour(const Json::Value &gate)
  {
      Contour* pDerived;
      unique_ptr<GateInfo> pInfo(pDerived = new Contour());
      int nParams = gate["parameters"].size();

      if (gate["parameters"].size() == 2) {
          pDerived->setParameter0(gate["parameters"][0].asString());
          pDerived->setParameter1(gate["parameters"][1].asString());
      } else {
          throw runtime_error("Gate type \"c\" expects "
                              "2 parameters but a different amount was provided");
      }

      vector<pair<double,double> > points;
      auto iter = gate["points"].begin();
      auto end = gate["points"].end();
      while (iter!=end) {
            auto& val = *iter;
            auto x = val["x"].asDouble();
            auto y = val["y"].asDouble();

            points.push_back({x,y});

            ++iter;
      }
      pDerived->setPoints(points);
      pDerived->setName( gate["name"].asString() );

      return std::move(pInfo);
  }

  std::unique_ptr<GateInfo> GateCmdParser::parseFalse(const Json::Value& gate)
  {
    False* pDerived;
    unique_ptr<GateInfo> pInfo(pDerived = new False());

    // there is only one element... the name
    pInfo->setName(gate["name"].asString());

    return std::move(pInfo);
  }

} // end of namespace


