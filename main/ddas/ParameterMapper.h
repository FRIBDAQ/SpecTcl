/*
    This software is Copyright by the Board of Trustees of Michigan
    State University (c) Copyright 2014.

    You may use this software under the terms of the GNU public license
    (GPL).  The terms of this license are described at:

     http://www.gnu.org/licenses/gpl.txt

     Authors:
             Ron Fox
             Jeromy Tompkins 
	     NSCL
	     Michigan State University
	     East Lansing, MI 48824-1321
*/


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
