
#ifndef GATEINFO_H
#define GATEINFO_H

#include <vector>
#include <string>

namespace SpJs
{
  enum GateType { S, C };

  struct GateInfo 
  {
    std::string s_name;
    GateType    s_type;
  };

  struct GateInfo1D : public GateInfo 
  {
    std::vector<std::string> s_params;
    double s_low;   
    double s_high;   
  };

}
#endif
