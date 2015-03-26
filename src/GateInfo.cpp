#include "GateInfo.h"
#include <algorithm>
#include <iomanip>
#include <ostream>

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


//////////////////////////////////////////////////////////////////////////////

Contour::Contour() : Contour( "", "", "", {{}})
{}

Contour::Contour(const string &name,
                 const string &parameter0,
                 const string &parameter1,
                 const vector<pair<double, double> >& points)
    : GateInfo(name, ContourGate),
      s_param0(parameter0),
      s_param1(parameter1),
      s_points(points)
{}

Contour::~Contour() {}

bool Contour::operator==(const Contour& rhs) const {

    // honor thy parents
    bool same = GateInfo::operator==(rhs);

    // how about high and low
    same &= (s_param0 == rhs.s_param0);
    same &= (s_param1 == rhs.s_param1);

    // are the params the same
    same &= equal(s_points.begin(), s_points.end(), rhs.s_points.begin());

    return same;
}

bool Contour::operator!=(const Contour& rhs) const {
    return !(*this == rhs);
}



} // end of namespace

std::ostream& operator<<(std::ostream& stream,  const SpJs::Contour& cont)
{
    stream << "\n" << setw(10) << "name" << " : " << cont.getName();
    stream << "\n" << setw(10) << "par0" << " : " << cont.getParameter0();
    stream << "\n" << setw(10) << "par1" << " : " << cont.getParameter1();
    stream << "\n" << setw(10) << "points" << " : ";
    auto points = cont.getPoints();

    for (int i=0; i<points.size(); ++i) {
        stream << "\n" << setw(10) << i << " : ("
               << setw(8) << points[i].first
               << setw(8) << points[i].second << ")";
    }

    stream << endl;

    return stream;
}
