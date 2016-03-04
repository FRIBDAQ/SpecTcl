#include "SharedMemoryKeyParser.h"
#include <json/value.h>
#include <stdexcept>

namespace SpJs {

    std::string SharedMemoryKeyParser::parseKey(const Json::Value &value)
    {
        // fail if we did not retrieve a proper value
        if (value["status"].asString() != "OK") {
          throw std::runtime_error("Cannot parse json, status != OK");
        }

        return value["detail"].asString();
    }
}
