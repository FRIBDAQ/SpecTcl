

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

  class ParameterCmdParser
  {
    public:
      std::vector<ParameterInfo> parseList(const Json::Value& value);
  };

} // end of namespace

#endif
