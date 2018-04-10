
#ifndef DAQ_DDAS_CPARAMETERMAPPER_H
#define DAQ_DDAS_CPARAMETERMAPPER_H

#include "DDASHit.h"
#include <memory>
#include <vector>

class CEvent;

namespace DAQ {
namespace DDAS {

  class CParameterMapper {
    public:
      virtual void mapToParameters(const std::vector<DDASHit>& data, CEvent& rEvent) = 0;
  };

} // end DDAS namespace
} // end DAQ namespace

#endif
