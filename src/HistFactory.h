

#ifndef JSONTOTH1_H
#define JSONTOTH1_H

#include <iosfwd>
#include <memory>
#include <TH1.h>

class AxisInfo;
class HistInfo;

namespace Json {
  class Value;
}

namespace SpJs
{

  class HistFactory {
    public:
    std::unique_ptr<TH1> create(const char* jsonByteRep);
    std::unique_ptr<TH1> create(const Json::Value& value);
  };

}

std::ostream& operator<<(std::ostream& stream, AxisInfo& info);
std::ostream& operator<<(std::ostream& stream, HistInfo& info);

#endif
