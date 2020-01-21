/*
    This software is Copyright by the Board of Trustees of Michigan
    State University (c) Copyright 2020.

    You may use this software under the terms of the GNU public license
    (GPL).  The terms of this license are described at:

     http://www.gnu.org/licenses/gpl.txt

     Authors:
             Giordano Cerizza
	     Ron Fox
             NSCL
             Michigan State University
             East Lansing, MI 48824-1321
*/
#ifndef DAQ_DDAS_PIPELINEDATA_H
#define DAQ_DDAS_PIPELINEDATA_H

#include "DDASHit.h"
#include <memory>
#include <vector>

namespace DAQ {
  namespace DDAS {
    
    class CPipelineData {
    public:
      virtual CPipelineData* clone() = 0;
    };
  }
}

#endif
