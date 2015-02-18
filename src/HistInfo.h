
#ifndef HISTINFO_H
#define HISTINFO_H

#include <vector>
#include <string>


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
    int s_type;
    std::vector<std::string> s_params;
    std::vector<AxisInfo> s_axes;
    ChanType s_chanType; 
  };


}// end of namespace


// comparison
bool operator==(const SpJs::HistInfo& lhs, const SpJs::AxisInfo& rhs);
bool operator!=(const SpJs::HistInfo& lhs, const SpJs::AxisInfo& rhs);

// Extraction operator
std::ostream& operator<<(std::ostream& stream, const SpJs::AxisInfo& info);

bool operator==(const SpJs::HistInfo& lhs, const SpJs::HistInfo& rhs);
bool operator!=(const SpJs::HistInfo& lhs, const SpJs::HistInfo& rhs);

// Extraction operator
std::ostream& operator<<(std::ostream& stream, const SpJs::HistInfo& info);

#endif

