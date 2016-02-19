

#ifndef BININFO_H
#define BININFO_H

#include <vector>
#include <cstdlib>

namespace SpJs
{

  struct BinInfo {
    int s_xbin;
    int s_ybin;
    int s_zbin;
    double s_value;


    bool operator==(const BinInfo& rhs) const {
      return ((s_xbin==rhs.s_xbin)
                &&(s_ybin==rhs.s_ybin)
                &&(s_value==rhs.s_value));
    }
  };


  class HistContent {
    private:
      std::vector<BinInfo> m_values;
      std::vector<double>  m_underflows;
      std::vector<double>  m_overflows;

    public:
      HistContent();
      HistContent(const std::vector<BinInfo>& values,
                  const std::vector<double>& underflows,
                  const std::vector<double>& overflows);

      std::vector<BinInfo>& getValues();
      void setValues(const std::vector<BinInfo>& values);

      double getUnderflow(size_t axis) const;
      void setUnderflow(size_t axis, double value);

      double getOverflow(size_t axisIndex) const;
      void setOverflow(size_t axisIndex, double value);
  };

} // end of namespace


inline bool operator==(const SpJs::BinInfo& lhs, const SpJs::BinInfo& rhs)
{
  return ((lhs.s_xbin==rhs.s_xbin)
            &&(lhs.s_ybin==rhs.s_ybin)
            &&(lhs.s_value==rhs.s_value));
}


#endif
