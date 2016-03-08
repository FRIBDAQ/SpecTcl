
#include "TestUtils.h"

#include <ostream>

std::ostream& operator<<(std::ostream& str, const QString& msg)
{
   str << msg.toStdString();
   return str;
}
