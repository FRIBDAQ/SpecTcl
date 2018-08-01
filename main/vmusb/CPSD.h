/*!
   Unpacker for XLM/PSD ADC version
*/
#ifndef CPSD_H
#define CPSD_H

#include "CModuleUnpacker.h"

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
