#include "GateInfo.h"
#include <algorithm>

using namespace std;

namespace SpJs
{

GateInfo::GateInfo(const string &name, GateType type)
    : s_name(name), s_type(type)
{}


GateInfo::~GateInfo()
{}

bool GateInfo::operator==(const GateInfo& rhs) const {

    return ((s_name == rhs.s_name) && (s_type == rhs.s_type));
}

bool GateInfo::operator!=(const GateInfo& rhs) const {
    return !(*this == rhs);
}



//////////////////////////////////////////////////////////////////////////////

Slice::Slice()
    : Slice( "", "", 0, 0)

{}

Slice::Slice(const string &name,
             const string &parameter,
             double low,
             double high)
    : GateInfo(name, SliceGate),
      s_param(parameter),
      s_low(low),
      s_high(high)
{}

Slice::~Slice() {}

bool Slice::operator==(const Slice& rhs) const {

    // honor thy parents
    bool same = GateInfo::operator==(rhs);

    // are the params the same
    same &= equal(s_param.begin(), s_param.end(), rhs.s_param.begin());

    // how about high and low
    same &= (s_low == rhs.s_low);
    same &= (s_high == rhs.s_high);

    return same;
}

bool Slice::operator!=(const Slice& rhs) const {
    return !(*this == rhs);
}



}
