/*
    This software is Copyright by the Board of Trustees of Michigan
    State University (c) Copyright 2005.

    You may use this software under the terms of the GNU public license
    (GPL).  The terms of this license are described at:

     http://www.gnu.org/licenses/gpl.txt

     Author:
             Ron Fox/Giordano Cerrizza/Genie Jhang
	     Facility for Rare Isotope Beams
	     Michigan State University
	     East Lansing, MI 48824-1321
*/

#ifndef CMDPP32QDCUNPACKER_H
#define CMDPP32QDCUNPACKER_H

#include "CModuleUnpacker.h"

class CMDPP32QDCUnpacker : public CModuleUnpacker
{
public:
    CMDPP32QDCUnpacker();
    virtual ~CMDPP32QDCUnpacker();

public:
    virtual unsigned int operator()(CEvent&                       rEvent,
                                    std::vector<unsigned short>&  event,
                                    unsigned int                  offset,
                                    CParamMapCommand::AdcMapping* pMap);
};

#endif //CMDPP32QDCUNPACKER_H
