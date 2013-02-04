/*!
   Unpacker for XLM/HINP version
*/
#ifndef __CHINP_H
#define __CHINP_H

#ifndef __CMODULEUNPACKER_H
#include "CModuleUnpacker.h"
#endif

class CTreeParameterArray;

class CHINP :  public CModuleUnpacker
{
public:
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
