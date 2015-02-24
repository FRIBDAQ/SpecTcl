

#ifndef BININFO_H
#define BININFO_H

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

} // end of namespace


inline bool operator==(const SpJs::BinInfo& lhs, const SpJs::BinInfo& rhs)
{
  return ((lhs.s_xbin==rhs.s_xbin)
            &&(lhs.s_ybin==rhs.s_ybin)
            &&(lhs.s_value==rhs.s_value));
}


#endif
