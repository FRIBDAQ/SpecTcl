

#ifndef JSONTOTH1_H
#define JSONTOTH1_H

#include <iosfwd>
#include <memory>
#include <TH1.h>

namespace Json {
  class Value;
}

namespace SpJs
{
  class HistInfo;

  class HistFactory {
    public:
      // Designed to parse json retrieved from list cmd and 
      // turn into an empty TH1
      std::unique_ptr<TH1> create(const char* jsonByteRep);
      std::unique_ptr<TH1> create(const Json::Value& value);

      // Given an already parsed list command, create the hist
      // with no content
      std::unique_ptr<TH1> create(const HistInfo& info);
  };

}

#endif
