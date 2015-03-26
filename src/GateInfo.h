
#ifndef GATEINFO_H
#define GATEINFO_H

#include <vector>
#include <string>

namespace SpJs
{

  enum GateType { SliceGate, ContourGate, BandGate };

  struct GateInfo 
  {

  protected:
    std::string s_name;
    GateType    s_type;

  public:
    GateInfo(const std::string& name, GateType type);
    virtual ~GateInfo();

    virtual GateType getType() const = 0;

    void setName(const std::string& name) { s_name = name; }
    std::string getName() const { return s_name; }

    virtual bool operator==(const GateInfo& rhs) const;
    virtual bool operator!=(const GateInfo& rhs) const;
  }; // end of GateInfo class


  /**! \brief Slices are 1d gates on a single parameter
   *
   */
  struct Slice : public GateInfo
  {
  private:
    std::string s_param;
    double s_low;   
    double s_high;   

  public:
    Slice();
    Slice(const std::string& name, const std::string& parameter, double low, double high);
    virtual ~Slice();

    GateType getType() const { return s_type; }

    void setParameter(const std::string& paramName) { s_param = paramName; }
    std::string getParameter() const { return s_param; }

    void setLowerLimit(double value) { s_low = value; }
    double getLowerLimit() const { return s_low; }

    void setUpperLimit(double value) { s_high = value; }
    double getUpperLimit() const { return s_high; }


    virtual bool operator==(const Slice& rhs) const;
    virtual bool operator!=(const Slice& rhs) const;
  }; // end of class


  /**! \brief Slices are 1d gates on a single parameter
   *
   */
  struct Contour : public GateInfo
  {
  private:
    std::string s_param0;
    std::string s_param1;
    std::vector<std::pair<double,double> > s_points;

  public:
    Contour();
    Contour(const std::string& name,
            const std::string& param0,
            const std::string& param1,
            const std::vector<std::pair<double,double> >& points);
    virtual ~Contour();

    GateType getType() const { return s_type; }

    void setParameter0(const std::string& paramName) { s_param0 = paramName; }
    std::string getParameter0() const { return s_param0; }

    void setParameter1(const std::string& paramName) { s_param1 = paramName; }
    std::string getParameter1() const { return s_param1; }

    std::vector<std::pair<double, double> > getPoints() const { return s_points; }
    std::vector<std::pair<double, double> >& getPoints() { return s_points; }
    void setPoints(const std::vector<std::pair<double, double> >& points) {
        s_points = points;
    }

    virtual bool operator==(const Contour& rhs) const;
    virtual bool operator!=(const Contour& rhs) const;
  }; // end of class


} // end of namespace

extern std::ostream& operator<<(std::ostream& stream,  const SpJs::Contour& cont);

#endif
