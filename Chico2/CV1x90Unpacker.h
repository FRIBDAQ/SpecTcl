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


#ifndef __CV1X90UNPACKER_H
#define __CV1X90UNPACKER_H


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
  In addition to the usual mapping information we use data from the
  Tcl array ::CAENV1x90.  This should have an element
  with our module name... if it does, then 
  we retrieve a pair of numbers from it, the reference channel number
  and the number of hits we will accept per channel


*/


class CV1x90Unpacker : public CModuleUnpacker
{
  // Internal data structures.

private:
  // Each tdc module has associated with it the following struct:

  typedef struct _tdcInfo {
    int                       s_refchannel; // reference channel number.
    int                       s_depth;      // max # of hits per channel
    uint32_t                  s_channelCount; // Number of channels.
    uint32_t                  s_chanmask;   // Mask for channel number.
    uint32_t                  s_chanshift;  // shift for channel number.
    uint32_t                  s_datamask;   // Time field mask.
    double                    s_chansToNs;  // Channels -> ns conversion factor.
    CTreeParameterArray**     s_parameters; // An array per channel
  } TdcInfo;



  // Canonicals:

public:
  CV1x90Unpacker();
  virtual ~CV1x90Unpacker();

private:
  CV1x90Unpacker(const CV1x90Unpacker& rhs);
  CV1x90Unpacker& operator=(const CV1x90Unpacker& rhs);
  int operator==(const CV1x90Unpacker& rhs) const;
  int operator!=(const CV1x90Unpacker& rhs) const;


  // Implementing the CModuleUnpacker interface:

public:
  virtual unsigned int operator()(CEvent&                       rEvent,
				  std::vector<unsigned short>&  event,
				  unsigned int                  offset,
				  CParamMapCommand::AdcMapping* pMap);

  // Private utility functions:
private:

  TdcInfo& getInfo(CParamMapCommand::AdcMapping* pMap);
  TdcInfo* newTdc(std::string name);
  void     reportError(uint32_t errorWord, int slot);
  void     makeTreeParams(CTCLInterpreter& interp, 
			  std::string      name, 
			  TdcInfo&         Info);
  
};
  



#endif
