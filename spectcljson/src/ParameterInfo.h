#ifndef PARAMETERINFO_H
#define PARAMETERINFO_H

#include <string>

namespace SpJs
{

struct ParameterInfo
{
    std::string     s_name;
    int             s_id;
    int             s_nBins;
    double          s_low;
    double          s_high;
    std::string     s_units;

    bool operator==(const SpJs::ParameterInfo& rhs) const {
        return ((s_name == rhs.s_name)
                && (s_id == rhs.s_id)
                && (s_nBins == rhs.s_nBins)
                && (s_low == rhs.s_low)
                && (s_high == rhs.s_high)
                && (s_units == rhs.s_units));
    }
};

} // end of namespace

inline bool operator==(const SpJs::ParameterInfo& lhs, const SpJs::ParameterInfo& rhs)
{
    return lhs.operator==(rhs);
}


#endif // PARAMETERINFO_H
