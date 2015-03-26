
#ifndef GATEINFO_H
#define GATEINFO_H

#include <vector>
#include <string>

namespace SpJs
{

  enum GateType { SliceGate, ContourGate, BandGate };

  struct GateInfo 
  {

    std::string s_name;
    GateType    s_type;

    GateInfo(const std::string& name, GateType type);
    virtual ~GateInfo();

    virtual GateType type() const {
        return s_type;
    }

    virtual bool operator==(const GateInfo& rhs) const;
    virtual bool operator!=(const GateInfo& rhs) const;
  }; // end of GateInfo class


  /**! \brief Slices are 1d gates on a single parameter
   *
   */
  struct Slice : public GateInfo
  {
    std::string s_param;
    double s_low;   
    double s_high;   

    Slice();
    Slice(const std::string& name, const std::string& parameter, double low, double high);
    virtual ~Slice();

    virtual bool operator==(const Slice& rhs) const;
    virtual bool operator!=(const Slice& rhs) const;
  }; // end of class


} // end of namespace


#endif
