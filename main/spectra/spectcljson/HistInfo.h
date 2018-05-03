
#ifndef HISTINFO_H
#define HISTINFO_H

#include <vector>
#include <string>
#include <iostream>


namespace SpJs
{
  /**! \brief Struct to encapsulate axis spec */
  enum ChanType { Short=0, Long=1 };

  /**! \brief Struct to encapsulate axis spec */
  struct AxisInfo {
    double s_low;
    double s_high;
    int s_nbins;
  };


  /**! \brief Class to hold parsed Json */
  struct HistInfo {
    std::string s_name;
    std::string s_type;
    std::vector<std::string> s_params;
    std::vector<AxisInfo> s_axes;
    ChanType s_chanType;
    bool s_direction;                 // m2 proj - has projection direction.  
    std::vector<std::string> s_roi;  // m2 proj - has ROI defining gates.
  };


}// end of namespace


// comparison
bool operator==(const SpJs::HistInfo& lhs, const SpJs::AxisInfo& rhs);
bool operator!=(const SpJs::HistInfo& lhs, const SpJs::AxisInfo& rhs);

// Extraction operator
//std::ostream& operator<<(std::ostream& stream, const SpJs::AxisInfo& info);
inline std::ostream& operator<<(std::ostream& stream, SpJs::AxisInfo& info) 
{
  stream << "{" << info.s_low << ", ";
  stream << info.s_high << ", ";
  stream << info.s_nbins << "}";
  return stream;
}

bool operator==(const SpJs::HistInfo& lhs, const SpJs::HistInfo& rhs);
bool operator!=(const SpJs::HistInfo& lhs, const SpJs::HistInfo& rhs);

// stream insertion operator
//std::ostream& operator<<(std::ostream& stream, const SpJs::HistInfo& info);
inline std::ostream& operator<<(std::ostream& stream, const SpJs::HistInfo& info) {

  stream << "name=" << info.s_name;
  stream << "\ntype=" << info.s_type;
  stream << "\nparams:";
  for (auto param : info.s_params) {
    stream << "\n  " << param;
  }
  stream << "\nparams:";
  for (auto ainfo : info.s_axes) {
    stream << "\n  " << ainfo;
  }
  return stream;
}


#endif

