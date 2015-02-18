

#ifndef JSONTOTH1_H
#define JSONTOTH1_H

#include <iosfwd>
#include <memory>
#include <TH1.h>
#include "HistInfo.h"

namespace Json {
  class Value;
}

namespace SpJs
{

  class HistFactory {
    public:
      std::unique_ptr<TH1> create(const char* jsonByteRep);
      std::unique_ptr<TH1> create(const Json::Value& value);

    private:
      ChanType mapChanType(const std::string& type);
  };

}

#endif
