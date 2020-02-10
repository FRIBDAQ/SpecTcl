/*!
   Unpacker for XLM/HINP version
*/
#ifndef CHINP_H
#define CHINP_H

#include "CModuleUnpacker.h"
#include "CMutex.h"

class CTreeParameterArray;

class CHINP :  public CModuleUnpacker
{
public:

  CMutex                                 m_observerGuard;
  // Canonicals:

  CHINP();
  virtual ~CHINP();

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
