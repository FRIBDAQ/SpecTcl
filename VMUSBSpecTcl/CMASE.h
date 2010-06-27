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


#ifndef __CMASE_H
#define __CMASE_H


#ifndef __CMODULEUNPACKER_H
#include <CModuleUnpacker.h>
#endif


#ifndef __CPARAMMAPCOMMAND_H
#include "CParamMapCommand.h"
#endif


#ifndef __STL_VECTOR
#include <vector>
#ifndef __STL_VECTOR
#define __STL_VECTOR
#endif
#endif



#ifndef __CRT_STDINT_H
#include <stdint.h>
#ifndef __CRT_STDINT_H
#define __CRT_STDINT_H
#endif
#endif

// Forward definitions.

class CEvent;
class CTreeParameterArray;
class CTCLInterpreter;


/*!
  This class unpacks data from the CAENV1x90 TDC.
  In this case the mapping is tacked on to the 
  mapping structure when we first seen an event.
  to do that we use:
  - The fact that the basename is the name in the mapping struct.
  - The tcl global maseCOBCount(basename) is the number of COB boards in this module.
  - The tcl global maseCHBCounts(mase) are the number of CHB boards in this module.
  - Each CHB board has 32 channel all of which are assumed to be connected.
*/
class CMASE : public CModuleUnpacker
{
private:
  // internal data structures/typedefs:

  typedef std::vector<CTreeParameterArray*>  CHBVector;   // Vector of tree parameter arrays.
  typedef std::vector<CHBVector*>            COBVector;   // Vector of COBs.

public:
  // Supported anonicals.

  CMASE();
  virtual ~CMASE();

private:
  // unsupported canonicals.

  CMASE(const CMASE& rhs);
  CMASE& operator=(const CMASE& rhs);
  int operator==(const CMASE& rhs) const;
  int operator!=(const CMASE& rhs) const;

  // Implementing the module Unpacker interface:
public:
  virtual unsigned int operator()(CEvent&                       rEvent,
				  std::vector<unsigned short>&  event,
				  unsigned int                  offset,
				  CParamMapCommand::AdcMapping* pMap);

  // private utilitye functions:

private:
  COBVector&  getInfo(CParamMapCommand::AdcMapping* pMap);
  COBVector*  createCOBVector(std::string baseName);
  CHBVector*  createCHBVector(std::string basename, int cobNumber, int chbCount);

};

#endif
