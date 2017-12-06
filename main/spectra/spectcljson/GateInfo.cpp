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
    : GateInfo(rhs),
    m_points(rhs.m_points)
{}

GateInfo2D::~GateInfo2D()
{}

bool GateInfo2D::operator==(const GateInfo2D& rhs) const
{
    return GateInfo::operator==(rhs) && (m_points == rhs.m_points);
}

bool GateInfo2D::operator!=(const GateInfo2D& rhs) const
{
    return !(GateInfo2D::operator==(rhs));
}

void GateInfo2D::setPoint(size_t index, double x, double y)
{
    if (index >= m_points.size()) {
        string msg("GateInfo2D::setPoint(size_t, x, y) ::");
        msg += "Cannot set a point that does not exist";
        throw out_of_range(msg);
    } else {
        auto& point = m_points.at(index);
        point.first = x;
        point.second = y;
    }
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
// Implement gamma slice.

/**
 * Default constructor.
 */


GammaSlice::GammaSlice() :
    GammaSlice("", {}, 0, 0)
{}

/**
 * Fully parameterized constructor:
 *   @param name - name of the gate.
 *   @param parameters - vector of parameter names.
 *   @param low        - low limit of the slice.
 *   @param high       - high limit of the slice.
 */
GammaSlice::GammaSlice(
    const std::string& name, const std::vector<std::string>& parameters,
    double low, double high
) :
    Cut(name, GammaSliceGate, low, high),
    m_params(parameters)
{}

/** Copy constructor
 */
GammaSlice::GammaSlice(const GammaSlice& rhs) :
    Cut(rhs),
    m_params(rhs.m_params)
{}

/**
 *     Destructor is empty for now
 */
GammaSlice::~GammaSlice() {}

/**
 *  clone - this just does a copy construction of this wrapped ina new:
 *
 * @param rhs - the object to be cloned.
 */
std::unique_ptr<GateInfo>
GammaSlice::clone() const
{
    return std::unique_ptr<GateInfo>(new GammaSlice(*this));
}

/**
 * setParameter
 *    Intended to replace the name of an existing parameter with a new one.
 * @param newParam - new parameter name.
 * @param idx      - parameter index.
 * @throw std::out_of_range if idx is not in the range of defined indices.
 */
void
GammaSlice::setParameter(const std::string& newParam, int idx)
{
    m_params.at(idx) = newParam;   // Throws if idx is bad.
}

/**
 * appendParameter
 *    Provided to allow the list of parameters to be incrementally
 *    constructed.
 *
 *  @param newParam - name of a parameter to add to the list.
 */
void
GammaSlice::appendParameter(const std::string& newParam)
{
    m_params.push_back(newParam);
}

/**
 * setAllParameters
 *    Replace m_params with a completely new vector of params.
 *
 *  @param parameters - new vector of parameters.
 *
 */
void
GammaSlice::setAllParameters(const std::vector<std::string>& parameters)
{
    m_params = parameters;
}


/**
 * getParameters
 *   @return const std::vector<std::string>& vector of parameter names.
 */
const std::vector<std::string>&
GammaSlice::getParameters() const
{
    return m_params;
}

/**
 * getParameter
 *    Return the name of one parameter
 *
 *  @param idx - index
 *  @return std::string the parameter indexe by idx.
 *  @throws std::out_of_range - if idx is out of range of the param vector.
 */
std::string
GammaSlice::getParameter(int idx) const
{
    return m_params.at(idx);          // Throws on range rror.
}

/**
 * comparisons
 */

bool
GammaSlice::operator==(const GammaSlice& rhs) const
{
    return Cut::operator==(rhs)  &&
            (m_params == rhs.m_params);
}

bool
GammaSlice::operator!=(const GammaSlice& rhs) const
{
    return !(*this == rhs);
}
//////////////////////////////////////////////////////////////////////////////

Ordinary2DGate::Ordinary2DGate() :
    Ordinary2DGate("", "", "", ContourGate)  // just guessing the type.
{}                                           // hopefully it gets overridden.

Ordinary2DGate::Ordinary2DGate(
    const std::string& name,
    const std::string& param0,
    const std::string& param1,
    GateType type
) :
    GateInfo2D(name, type),
    m_param0(param0), m_param1(param1)
{
}
Ordinary2DGate::Ordinary2DGate(const GateInfo2D& base) :
    GateInfo2D(base),
    m_param0(base.getParameter0()),
    m_param1(base.getParameter1())
{}
Ordinary2DGate::Ordinary2DGate(const Ordinary2DGate& rhs) :
    GateInfo2D(rhs),
    m_param0(rhs.getParameter0()),
    m_param1(rhs.getParameter1())
{}
Ordinary2DGate::~Ordinary2DGate() {}

bool
Ordinary2DGate::operator==(const Ordinary2DGate& rhs) const
{
    return  (m_param0 == rhs.m_param0)  &&
            (m_param1 == rhs.m_param1)  &&
            (GateInfo2D::operator==(rhs));
}
bool
Ordinary2DGate::operator!=(const Ordinary2DGate& rhs) const
{
    return !(operator==(rhs));
}

std::unique_ptr<GateInfo>
Ordinary2DGate::clone() const
{
    return std::unique_ptr<GateInfo>(new Ordinary2DGate(*this));
}

//////////////////////////////////////////////////////////////////////////////

Contour::Contour() : Contour( "", "", "", {{}})
{}

Contour::Contour(const string &name,
                 const string &parameter0,
                 const string &parameter1,
                 const vector<pair<double, double> >& points)
    : Ordinary2DGate(name, parameter0, parameter1, ContourGate)
{
    setPoints(points);        
}

Contour::Contour(const Contour& rhs)
    : Ordinary2DGate(rhs)
{     
}

Contour::Contour(const Ordinary2DGate& rhs)
    : Ordinary2DGate(
        rhs.getName(), rhs.getParameter0(), rhs.getParameter1(), ContourGate
      )
{
    setPoints(rhs.getPoints());
}

Contour::~Contour() {}

bool Contour::operator==(const Contour& rhs) const {

    // honor thy parents b/c they have all the data.
    
    bool same = Ordinary2DGate::operator==(rhs);


    return same;
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
    : Ordinary2DGate(name, parameter0, parameter1,  BandGate)
      
{
    setPoints(points);    
}

Band::Band(const Band& rhs)
    : Ordinary2DGate(rhs)
{
        
    setPoints(rhs.m_points);        
}

Band::Band(const Ordinary2DGate& rhs)
    : Ordinary2DGate(
        rhs.getName(), rhs.getParameter0(), rhs.getParameter1(), BandGate
      )
{
    setPoints(rhs.getPoints());        
}

Band::~Band() {}

bool Band::operator==(const Band& rhs) const {

    // honor thy parents -- they actually have all the data.
    
    return Ordinary2DGate::operator==(rhs);
}


bool Band::operator!=(const Band& rhs) const {
    return !(*this == rhs);
}

unique_ptr<GateInfo> Band::clone() const {
    return unique_ptr<GateInfo>(new Band(*this));
}
  ///////////////////////////////////////////////////////////////////////

  Gamma2DGate::Gamma2DGate() :
    Gamma2DGate("", {}, ContourGate) {} // Default gate type is most common.
  Gamma2DGate::Gamma2DGate(const std::string& name,
			   const std::vector<std::string>& params,
			   GateType type) :
    GateInfo2D(name, type),
    m_parameters(params) {}
  Gamma2DGate::Gamma2DGate(const GateInfo2D& base) :
    GateInfo2D(base),
    m_parameters(base.getParameters())
  {}
  Gamma2DGate::Gamma2DGate(const Gamma2DGate& rhs) :
    GateInfo2D(rhs),
    m_parameters(rhs.getParameters())
  {}
  Gamma2DGate::~Gamma2DGate() {}

  std::unique_ptr<GateInfo>
  Gamma2DGate::clone() const
  {
    return std::unique_ptr<GateInfo>(new Gamma2DGate(*this));
  }

  void
  Gamma2DGate::setParameter0(const std::string& paramName)
  {
    if (m_parameters.size() > 0) {
      m_parameters[0] = paramName;
    } else {
      m_parameters.push_back(paramName);
    }
  }
  void
  Gamma2DGate::setParameter1(const std::string& paramName)
  {
    if (m_parameters.size() > 1) {
      m_parameters[1] = paramName;
    } else {
      if (m_parameters.size() == 0) {
	m_parameters.push_back("");
      }
      m_parameters.push_back(paramName);
    }
  }
  void
  Gamma2DGate::setParameter(unsigned offset, const std::string& paramName)
  {
    if (m_parameters.size() < offset) {
      m_parameters[offset] = paramName;
    } else {
      throw std::out_of_range("Parameter index out of range (Gamma2DGate::setParameter)");
    }
  }
  void
  Gamma2DGate::setParameters(const std::vector<std::string>& parameters)
  {
    m_parameters = parameters;
  }

  std::string
  Gamma2DGate::getParameter0() const
  {
    if (m_parameters.size() > 0) {
      return m_parameters[0];
    } else {
      return "";                    // Consistent with regular.
    }
  }
  std::string
  Gamma2DGate::getParameter1() const
  {
    if (m_parameters.size() > 1) {
      return m_parameters[1];
    } else {
      return "";
    }
  }

  std::string
  Gamma2DGate::getParameter(unsigned n ) const
  {
    if (m_parameters.size() > n) {
      return m_parameters[n];
    } else {
      throw std::out_of_range("No such parameter offset (Gamma2DGate::getParameter");
    }
  }
  std::vector<std::string>
  Gamma2DGate::getParameters() const
  {
    return m_parameters;
  }
  bool
  Gamma2DGate::operator==(const Gamma2DGate& rhs) const
  {
    return (m_parameters == rhs.m_parameters) &&
      (Gamma2DGate::operator==(rhs));
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
