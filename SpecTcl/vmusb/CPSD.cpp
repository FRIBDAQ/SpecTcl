#include <config.h>
#include "CPSD.h"
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

CPSD::CPSD() {}
CPSD::~CPSD() {}

/*
**   unpacker:
**   when called first tack a tree parameter array
**   of arrays on to the parameter map
**   so that we can unpack into it.
**   data looks like:
**     Channel count (uint32_t) (>4095 on is an error)
**     Channel tag  Chipid (8bits) 0 (1bit) (channel# 4bits)
**        Aadc(32), BADC(32), CADC(32), Time(32)
**      ...
**   Note that when processing ADC/TDC data
**   If we run off the end of the event, we just silently stop
**   processing.
**  new code for Unified (16-bit) event format with time stamp  1/7/2011
**
*/
unsigned int
CPSD::operator()(CEvent& rEvent,
		  vector<unsigned short>& event,
		  unsigned int            offset,
		  CParamMapCommand::AdcMapping* pMap)
{
  CTreeParameterArray*** myParameters = getTree(pMap, rEvent);

  uint32_t  channelId, timeStamp[2], channelCount;
  uint32_t          wordCount = getLong(event, offset);
  offset+=2;
  //
  // Word counts > 4095 are error indicators..
  // just skip the data the VMUSB can't help but read
  //
  // mask off the upper 16-bit field
  wordCount &= 0xffff;
  if (wordCount >4095) {
    //  if (wordCount >20) {
    cerr << "Got an error report from " << pMap->name << " "
	 << hex << wordCount << dec << endl;
    wordCount &= 0xfff;
    offset += wordCount*2 +1;
    return offset;
  }
  // word count must be evenly divisible by 5
  // loop through the groups of 5 words per hit channel
  channelCount      = event[offset];
  offset += 1;
  //  printf("word count is %d, channelCount is %d\n",wordCount,channelCount);
  timeStamp[0]  = getLong(event, offset);
  offset+=2;
  timeStamp[1]  = getLong(event, offset);
  offset+=2;
  for (int i =0; i < channelCount/5; i++) {
  // get channel tag
    channelId      = event[offset];
    //    printf("Channel ID = %x\n",channelId);
    offset +=1;
    // now pick up analog data
    uint32_t a     = event[offset] & 0xfff;
    offset += 1;
    uint32_t b     = event[offset] & 0xfff;
    offset += 1;
    uint32_t c     = event[offset] & 0xfff;
    offset += 1;
    uint32_t time  = event[offset] & 0xfff;
    offset += 1;

    uint32_t channel   = channelId & 0x07;
    uint32_t chip      = (channelId >> 5) & 0xff;
    //    printf("chip %d chan %d A %d B %d C %d Time %d\n",chip, channel,a,b,c,time);
    CTreeParameterArray** pChipTree = myParameters[chip-1];
    CTreeParameterArray*  pAs       = pChipTree[0];
    CTreeParameterArray*  pBs       = pChipTree[1];
    CTreeParameterArray*  pCs       = pChipTree[2];
    CTreeParameterArray*  pTs       = pChipTree[3];

    (*pAs)[channel] = a;
    (*pBs)[channel] = b;
    (*pCs)[channel] = c;
    (*pTs)[channel] = time;
  }
  //    printf("offset at end = %d\n",offset);
  // figure out how much garbage to gobble from end of event
    if ((offset & 0x0001) == 0) {
      offset +=2;
    } else {
      offset += 3;  
    }
    //    printf("final offset = %d\n",offset);

  return offset;
}
/*
** Return the extra data that is the tree parameter for this
** unpacking... creating it if it does not yet exist.
** ..and binding it to the event.
*/
CTreeParameterArray***
CPSD::getTree(CParamMapCommand::AdcMapping* pMap,
	       CEvent& rEvent)
{
  if (!pMap->extraData) {
    SpecTcl* api             = SpecTcl::getInstance();
    CTCLInterpreter *pInterp = api->getInterpreter();
    string baseName = pMap->name;
    CTCLVariable PSDChips(pInterp, "PSDChips",false);
    
    const char* pChipList = 
      PSDChips.Get(TCL_GLOBAL_ONLY,
		    const_cast<char*>(baseName.c_str()));
    if (!pChipList) {
      cerr << "PSDChips("<< baseName << ") not defined!!\n";
      exit(-1);
    }
    CTCLList ChipList(pInterp, pChipList);
    StringArray chips;
    ChipList.Split(chips);

    CTreeParameterArray*** chipArray = 
      new CTreeParameterArray**[chips.size()];
    pMap->extraData = chipArray;
    
    // Iterate over the chips making A,B,C and T tree parameter
    // arrays.

    cerr << "PSD GetTree, chips.size = " << chips.size() << endl;
    for (int i =0; i < chips.size(); i++) {
      char chipNumber[100];
      sprintf(chipNumber, "%02d", atoi(chips[i].c_str()));
      CTreeParameterArray** teArray = new CTreeParameterArray*[4];
      chipArray[i] = teArray;
      string aBaseName = baseName;
      aBaseName += ".a.";
      aBaseName += chipNumber;
      teArray[0] = new CTreeParameterArray(aBaseName,
				      14, 16, 0);
      string bBaseName = baseName;
      bBaseName += ".b.";
      bBaseName += chipNumber;
      teArray[1] = new CTreeParameterArray(bBaseName,
				      14, 16, 0);
      string cBaseName = baseName;
      cBaseName += ".c.";
      cBaseName += chipNumber;
      teArray[2] = new CTreeParameterArray(cBaseName,
				      14, 16, 0);
      string tBaseName = baseName;
      tBaseName += ".t.";
      tBaseName += chipNumber;
      teArray[3] = new CTreeParameterArray(tBaseName,
				      14, 16, 0);
    }
    
    CTreeParameter::BindParameters();
    CTreeParameter::setEvent(rEvent);
  }
  return reinterpret_cast<CTreeParameterArray***>(pMap->extraData);
}
