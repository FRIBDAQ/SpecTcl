
#ifndef GATEINFO_H
#define GATEINFO_H

#include <vector>
#include <string>
#include <memory>

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
    GateInfo(const GateInfo& rhs);

    virtual ~GateInfo();

    virtual GateType getType() const = 0;
    virtual std::unique_ptr<GateInfo> clone() const = 0;

    void setName(const std::string& name) { s_name = name; }
    std::string getName() const { return s_name; }

    virtual bool operator==(const GateInfo& rhs) const;
    virtual bool operator!=(const GateInfo& rhs) const;
  }; // end of GateInfo class


  struct GateInfo2D : public GateInfo
  {
    public:
      GateInfo2D(const std::string& name, GateType type);
      GateInfo2D(const GateInfo2D& rhs);
      virtual ~GateInfo2D();
      bool operator==(const GateInfo2D& rhs) const;
      bool operator!=(const GateInfo2D& rhs) const;

      virtual GateType getType() const = 0;
      virtual std::unique_ptr<GateInfo> clone() const = 0;

      virtual std::vector<std::pair<double, double> > getPoints() const = 0;
      virtual std::vector<std::pair<double, double> >& getPoints() = 0;

      virtual std::string getParameter0() const = 0;
      virtual std::string getParameter1() const = 0;
  };

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
    Slice(const Slice& rhs);
    virtual ~Slice();

    GateType getType() const { return s_type; }
    std::unique_ptr<GateInfo> clone() const;

    void setParameter(const std::string& paramName) { s_param = paramName; }
    std::string getParameter() const { return s_param; }

    void setLowerLimit(double value) { s_low = value; }
    double getLowerLimit() const { return s_low; }

    void setUpperLimit(double value) { s_high = value; }
    double getUpperLimit() const { return s_high; }


    virtual bool operator==(const Slice& rhs) const;
    virtual bool operator!=(const Slice& rhs) const;
  }; // end of class


  /**! \brief Bands are 2d gates on two parameter (open)
   *
   */
  struct Band : public GateInfo2D
  {
  private:
    std::string s_param0;
    std::string s_param1;
    std::vector<std::pair<double, double> > s_points;

  public:
    Band();
    Band(const std::string& name,
            const std::string& param0,
            const std::string& param1,
            const std::vector<std::pair<double,double> >& points);
    Band(const Band& rhs);
    virtual ~Band();

    GateType getType() const { return s_type; }
    std::unique_ptr<GateInfo> clone() const;

    void setParameter0(const std::string& paramName) { s_param0 = paramName; }
    std::string getParameter0() const { return s_param0; }

    void setParameter1(const std::string& paramName) { s_param1 = paramName; }
    std::string getParameter1() const { return s_param1; }

    std::vector<std::pair<double, double> > getPoints() const { return s_points; }
    std::vector<std::pair<double, double> >& getPoints() { return s_points; }
    void setPoints(const std::vector<std::pair<double, double> >& points) {
        s_points = points;
    }

    virtual bool operator==(const Band& rhs) const;
    virtual bool operator!=(const Band& rhs) const;
  }; // end of class


  /**! \brief Contours are 2d gates on a two parameters (closed areas)
   *
   */
  struct Contour : public GateInfo2D
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
    Contour(const Contour& rhs);
    virtual ~Contour();

    GateType getType() const { return s_type; }
    std::unique_ptr<GateInfo> clone() const;

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
