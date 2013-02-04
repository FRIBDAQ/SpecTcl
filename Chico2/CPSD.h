/*!
   Unpacker for XLM/PSD ADC version
*/
#ifndef __CPSD_H
#define __CPSD_H

#ifndef __CMODULEUNPACKER_H
#include "CModuleUnpacker.h"
#endif

class CTreeParameterArray;

class CPSD :  public CModuleUnpacker
{
public:
  // Canonicals:

  CPSD();
  virtual ~CPSD();

  // The unpacker entry:

public:
  virtual unsigned int operator()(CEvent&                       rEvent,
				  std::vector<unsigned short>&  event,
				  unsigned int                  offset,
				  CParamMapCommand::AdcMapping* pMap);


private:
  CTreeParameterArray*** getTree(CParamMapCommand::AdcMapping* pMap,
				 CEvent& rEvent);
};

#endif
