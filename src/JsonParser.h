

#ifndef JSONPARSER_H
#define JSONPARSER_H

#include <vector>
#include "HistInfo.h"

namespace Json {
  class Value;
}

namespace SpJs
{
  class JsonParser
  {
    public:
      std::vector<HistInfo> parseListCmd(const Json::Value& value);
//    std::vector<HistInfo> parseContentCmd(const Json::Value& value);
//
    private:
      HistInfo  parseListDetail(const Json::Value& value);
      ChanType  mapChanType(const std::string& value);
  };

} // end of namespace

#endif
