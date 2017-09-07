#include "GateInfo.h"
#include <algorithm>
#include <iomanip>
#include <ostream>
#include <stdexcept>

using namespace std;

namespace SpJs
{

GateInfo::GateInfo(const string &name, GateType type)
    : m_name(name), m_type(type)
{}

GateInfo::GateInfo(const GateInfo &rhs)
    : m_name(rhs.m_name), m_type(rhs.m_type)
{}

GateInfo::~GateInfo()
{}

bool GateInfo::operator==(const GateInfo& rhs) const {

    return ((m_name == rhs.m_name) && (m_type == rhs.m_type));
}

bool GateInfo::operator!=(const GateInfo& rhs) const {
    return !(*this == rhs);
}



GateInfo2D::GateInfo2D(const string &name, GateType type)
    : GateInfo(name, type)
{}

GateInfo2D::GateInfo2D(const GateInfo2D &rhs)
    : GateInfo(rhs)
{}

GateInfo2D::~GateInfo2D()
{}

bool GateInfo2D::operator==(const GateInfo2D& rhs) const
{
    return GateInfo::operator==(rhs);
}

bool GateInfo2D::operator!=(const GateInfo2D& rhs) const
{
    return !(GateInfo2D::operator==(rhs));
}

//////////////////////////////////////////////////////////////////////////////
// Cut Implementation:

/**
 * Closest to a default constructor we have:
 *    @param type - actual gate type -should be compatible with a cut-like gate.
*/
Cut::Cut(GateType type) :
    Cut("", type, 0, 0)
{}

/**
 *  Full constructor
 *  @param name - name of the gate.
 *  @param type - gate type.
 *  @param low  - initial low limit.
 *  @param high - initial high limit.
 */
Cut::Cut(const std::string& name, GateType type, double low, double high) :
    GateInfo(name, type),
    m_low(low), m_high(high)
{}
/**
 * Copy construtor
 */
Cut::Cut(const Cut& rhs) :
    GateInfo(rhs),
    m_low(rhs.m_low), m_high(rhs.m_high)
{}

/**
 * Nothing dynamic to destruct:
 */
Cut::~Cut() {}

/**
 * clone basically virtual copy constructor:
 */
std::unique_ptr<GateInfo>
Cut::clone() const
{
    return std::unique_ptr<GateInfo>(new Cut(*this));
}

/**
 * Comparison:
 */

bool
Cut::operator==(const Cut& rhs) const
{
    return GateInfo::operator==(rhs) &&
            (m_low == rhs.m_low) && (m_high == rhs.m_high);
}
bool
Cut::operator!=(const Cut& rhs) const
{
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
    : Cut(name, SliceGate, low, high),
      m_param(parameter)
{}

Slice::Slice(const Slice &rhs)
    : Cut(rhs),
      m_param(rhs.m_param)
{}

Slice::~Slice() {}

bool Slice::operator==(const Slice& rhs) const {

    // honor thy parents
    bool same = Cut::operator==(rhs);

    // are the params the same
    same &= (m_param == rhs.m_param);

    return same;
}

bool Slice::operator!=(const Slice& rhs) const {
    return !(*this == rhs);
}


unique_ptr<GateInfo> Slice::clone() const
{
    return unique_ptr<GateInfo>(new Slice(*this));
}

//////////////////////////////////////////////////////////////////////////////

Contour::Contour() : Contour( "", "", "", {{}})
{}

Contour::Contour(const string &name,
                 const string &parameter0,
                 const string &parameter1,
                 const vector<pair<double, double> >& points)
    : GateInfo2D(name, ContourGate),
      m_param0(parameter0),
      m_param1(parameter1),
      m_points(points)
{}

Contour::Contour(const Contour& rhs)
    : GateInfo2D(rhs),
      m_param0(rhs.m_param0),
      m_param1(rhs.m_param1),
      m_points(rhs.m_points)
{}

Contour::Contour(const GateInfo2D& rhs)
    : GateInfo2D(rhs.getName(), ContourGate),
      m_param0(rhs.getParameter0()),
      m_param1(rhs.getParameter1()),
      m_points(rhs.getPoints())
{}

Contour::~Contour() {}

bool Contour::operator==(const Contour& rhs) const {

    // honor thy parents
    bool same = GateInfo2D::operator==(rhs);

    // how about high and low
    same &= (m_param0 == rhs.m_param0);
    same &= (m_param1 == rhs.m_param1);

    // are the params the same
    same &= equal(m_points.begin(), m_points.end(), rhs.m_points.begin());

    return same;
}

void Contour::setPoint(size_t index, double x, double y)
{
    if (index >= m_points.size()) {
        string msg("Contour::setPoint(size_t, x, y) ::");
        msg += "Cannot set a point that does not exist";
        throw out_of_range(msg);
    } else {
        auto& point = m_points.at(index);
        point.first = x;
        point.second = y;
    }
}

bool Contour::operator!=(const Contour& rhs) const {
    return !(*this == rhs);
}

unique_ptr<GateInfo> Contour::clone() const {
    return unique_ptr<GateInfo>(new Contour(*this));
}

//////////////////////////////////////////////////////////////////////////////

Band::Band() : Band("", "", "", {{}})
{}

Band::Band(const string &name,
                 const string &parameter0,
                 const string &parameter1,
                 const vector<pair<double, double> >& points)
    : GateInfo2D(name, BandGate),
      m_param0(parameter0),
      m_param1(parameter1),
      m_points(points)
{}

Band::Band(const Band& rhs)
    : GateInfo2D(rhs),
      m_param0(rhs.m_param0),
      m_param1(rhs.m_param1),
      m_points(rhs.m_points)
{}

Band::Band(const GateInfo2D& rhs)
    : GateInfo2D(rhs.getName(), BandGate),
      m_param0(rhs.getParameter0()),
      m_param1(rhs.getParameter1()),
      m_points(rhs.getPoints())
{}

Band::~Band() {}

bool Band::operator==(const Band& rhs) const {

    // honor thy parents
    bool same = GateInfo2D::operator==(rhs);

    // how about high and low
    same &= (m_param0 == rhs.m_param0);
    same &= (m_param1 == rhs.m_param1);

    // are the params the same
    same &= equal(m_points.begin(), m_points.end(), rhs.m_points.begin());

    return same;
}

void Band::setPoint(size_t index, double x, double y)
{
    if (index >= m_points.size()) {
        string msg("Contour::setPoint(size_t, x, y) ::");
        msg += "Cannot set a point that does not exist";
        throw out_of_range(msg);
    } else {
        auto& point = m_points.at(index);
        point.first = x;
        point.second = y;
    }
}

bool Band::operator!=(const Band& rhs) const {
    return !(*this == rhs);
}

unique_ptr<GateInfo> Band::clone() const {
    return unique_ptr<GateInfo>(new Band(*this));
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
