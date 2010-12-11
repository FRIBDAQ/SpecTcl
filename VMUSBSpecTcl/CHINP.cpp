#include <config.h>
#include "CHINP.h"
#include <Event.h>
#include <stdint.h>
#include <TreeParameter.h>
#include <iostream>
#include <TCLVariable.h>
#include <TCLList.h>
#include <SpecTcl.h>
#include <stdlib.h>
#include <stdio.h>

using namespace std;

CHINP::CHINP() {}
CHINP::~CHINP() {}

/*
**   unpacker:
**   when called first tack a tree parameter array
**   of arrays on to the parameter map
**   so that we can unpack into it.
**   data looks like:
**     Channel count (uint32_t) (>4095 on is an error)
**     Channel tags  Chipid (8bits) 0 (1bit) (channel# 4bits)
**     ...
**     Channel count
**        tdc(32), adc(32)
**      ...
**   Note that when processing ADC/TDC data
**   If we run off the end of the event, we just silently stop
**   processing.
**
*/
unsigned int
CHINP::operator()(CEvent& rEvent,
		  vector<unsigned short>& event,
		  unsigned int            offset,
		  CParamMapCommand::AdcMapping* pMap)
{
  CTreeParameterArray*** myParameters = getTree(pMap, rEvent);

  vector<uint32_t>  channelIds;
  uint32_t          channelCount = getLong(event, offset);
  offset+=2;
  //
  // Channel counts > 4095 are error indicators..
  // just skip the data the VMUSB can't help but read
  //
  if (channelCount >4095) {
    cerr << "Got an error report from " << pMap->name << " "
	 << channelCount << endl;
    channelCount &= 0xfff;
    offset += channelCount*2 +1;
    return offset;
  }
  // Stuff the channel tags into the channelIds vector
  for (int i =0; i < channelCount; i++) {
    channelIds.push_back(getLong(event, offset));
    offset += 2;
  }
  offset += 2;			// Skip over the second channel count.

  // for testing we need to watch for running off the end of
  // the event as we pick up adc data.
  //
  for (int i = 0; i < channelCount; i++) {
    if (offset >= event.size()) break;
    uint32_t time  = getLong(event, offset);
    offset += 2;
    if (offset >= event.size()) break;
    uint32_t e     = getLong(event, offset);
    offset += 2;

    uint32_t channelId = channelIds[i];
    uint32_t channel   = channelId & 0x0f;
    uint32_t chip      = (channelId >> 5) & 0xff;
    CTreeParameterArray** pChipTree = myParameters[chip-1];
    CTreeParameterArray*  pEs       = pChipTree[0];
    CTreeParameterArray*  pTs       = pChipTree[1];

    (*pEs)[channel] = e;
    (*pTs)[channel] = time;
  }

  return offset;
}
/*
** Return the extra data that is the tree parameter for this
** unpacking... creating it if it does not yet exist.
** ..and binding it to the event.
*/
CTreeParameterArray***
CHINP::getTree(CParamMapCommand::AdcMapping* pMap,
	       CEvent& rEvent)
{
  if (!pMap->extraData) {
    SpecTcl* api             = SpecTcl::getInstance();
    CTCLInterpreter *pInterp = api->getInterpreter();
    string baseName = pMap->name;
    CTCLVariable HINPChips(pInterp, "HINPChips",false);
    
    const char* pChipList = 
      HINPChips.Get(TCL_GLOBAL_ONLY,
		    const_cast<char*>(baseName.c_str()));
    if (!pChipList) {
      cerr << "HINPChips("<< baseName << ") not defined!!\n";
      exit(-1);
    }
    CTCLList ChipList(pInterp, pChipList);
    StringArray chips;
    ChipList.Split(chips);

    CTreeParameterArray*** chipArray = 
      new CTreeParameterArray**[chips.size()];
    pMap->extraData = chipArray;
    
    // Iterate over the chips making an E and T tree parameter
    // array.

    for (int i =0; i < chips.size(); i++) {
      char chipNumber[100];
      sprintf(chipNumber, "%02d", atoi(chips[i].c_str()));
      CTreeParameterArray** teArray = new CTreeParameterArray*[2];
      chipArray[i] = teArray;
      string eBaseName = baseName;
      eBaseName += ".e.";
      eBaseName += chipNumber;
      teArray[0] = new CTreeParameterArray(eBaseName,
				      14,
				      16, 0);
      string tBaseName = baseName;
      tBaseName += ".t.";
      tBaseName += chipNumber;
      teArray[1] = new CTreeParameterArray(tBaseName,
				      14, 16, 0);
    }
    
    CTreeParameter::BindParameters();
    CTreeParameter::setEvent(rEvent);
  }
  return reinterpret_cast<CTreeParameterArray***>(pMap->extraData);
}
