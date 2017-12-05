

#ifndef JSONPARSER_H
#define JSONPARSER_H

#include <vector>
#include <memory>
#include "GateInfo.h"

namespace Json {
  class Value;
}

namespace SpJs
{

  class GateCmdParser
  {
    public:
      std::vector<std::unique_ptr<GateInfo>> parseList(const Json::Value& value);

  private:
      std::unique_ptr<GateInfo> parseSlice(const Json::Value& value);
      std::unique_ptr<GateInfo> parseGammaSlice(const Json::Value& value);
      
      std::unique_ptr<GateInfo> parseBand(const Json::Value& value);
      std::unique_ptr<GateInfo> parseContour(const Json::Value& value);
      std::unique_ptr<GateInfo> parseGammaBand(const Json::Value& value);   //New
      std::unique_ptr<GateInfo> parseGammaContour(const Json::Value& value); //New
      // ?? std::unique_ptr<GateInfo> parserC2Band(const Json::Value& value); //New ?
      

  };

} // end of namespace

#endif
