

#ifndef JSONPARSER_H
#define JSONPARSER_H

#include <vector>
#include "HistInfo.h"
#include "BinInfo.h"
#include "ParameterInfo.h"

namespace Json {
  class Value;
}

namespace SpJs
{

  class JsonParser
  {
    public:
      std::vector<HistInfo>      parseListCmd(const Json::Value& value);
      std::vector<BinInfo>       parseContentCmd(const Json::Value& value);
      std::vector<ParameterInfo> parseParameterCmd(const Json::Value& value);

    private:
      HistInfo  parseListDetail(const Json::Value& value);
      ChanType  mapChanType(const std::string& value);
  };

} // end of namespace

#endif
