/*
    This software is Copyright by the Board of Trustees of Michigan
    State University (c) Copyright 2005.

    You may use this software under the terms of the GNU public license
    (GPL).  The terms of this license are described at:

     http://www.gnu.org/licenses/gpl.txt

     Author:
             Ron Fox
	     NSCL
	     Michigan State University
	     East Lansing, MI 48824-1321
*/

#ifndef __CV977UNPACKER_H
#define __CV977UNPACKER_H

#ifndef __CMODULEUNPACKER_H
#include "CModuleUnpacker.h"
#endif



/*!
  This unpacker is responsible for unpacking CAEN V977 Input registers.
  The unpacker will pull the next single 16 bit word into the buffer
  mapping it to the single pararameter that represents.
  Typically, this parameter is used to e.g. create a bit-mask spectrum or
  to set bit-mask gates.

*/
class CV977Unpacker : public CModuleUnpacker
{
public:
  // Canonicals:

  CV977Unpacker();
  virtual ~CV977Unpacker();

  // The unpacker entry:

public:
  virtual unsigned int operator()(CEvent&                       rEvent,
				  std::vector<unsigned short>&  event,
				  unsigned int                  offset,
				  CParamMapCommand::AdcMapping* pMap);


};


#endif
