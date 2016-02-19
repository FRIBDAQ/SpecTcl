

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
      std::unique_ptr<GateInfo> parseBand(const Json::Value& value);
      std::unique_ptr<GateInfo> parseContour(const Json::Value& value);
      std::unique_ptr<GateInfo> parseFalse(const Json::Value& value);
      std::unique_ptr<GateInfo> parseTrue(const Json::Value& value);
  };

} // end of namespace

#endif
