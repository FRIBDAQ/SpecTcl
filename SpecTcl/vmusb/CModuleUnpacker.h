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


#ifndef __CMODULEUNPACKER_H
#define __CMODULEUNPACKER_H



#ifndef __CPARAMMAPCOMMAND_H
#include "CParamMapCommand.h"
#endif


#ifndef __STL_VECTOR
#include <vector>
#ifndef __STL_VECTOR
#define __STL_VECTOR
#endif
#endif


// Forward definitions:
class CEvent;

/*!
  This is the abstract base class for module unpackers used by the CStackUnpacker
  class.  A concrete class must be created for each actual module type.
  The key method each class must implement is, of course the operator() method.
  This method must unpack the next chunk of the event into the parameters defined
  by its parameter map.  The operator must return a pointer to the next 
  unconsumed part of the event. 
*/
class CModuleUnpacker 
{
  // Canonicals:
public:
  virtual ~CModuleUnpacker();	// To support destructor chaining in a complex class hierarchy.

  // pure virtual functions:

public:
  virtual unsigned int operator()(CEvent&                       rEvent,
				  std::vector<unsigned short>&  event,
				  unsigned int                  offset,
				  CParamMapCommand::AdcMapping* pMap) = 0;

protected:
  static  unsigned long getLong(std::vector<unsigned short>& event, 
				unsigned int offset);
};



#endif
