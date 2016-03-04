#ifndef SHAREDMEMORYKEYPARSER_H
#define SHAREDMEMORYKEYPARSER_H

#include <string>

namespace Json {
    class Value;

}

namespace SpJs
{

    class SharedMemoryKeyParser
    {
        public:
            std::string parseKey(const Json::Value& value);
    };

}
#endif // SHAREDMEMORYKEYPARSER_H
