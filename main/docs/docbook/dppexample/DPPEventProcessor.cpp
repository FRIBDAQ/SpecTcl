#include "DPPEventProcessor.h"
#include <CAENParameterMap.h>
#include <vector>
#include <string>

MyDPPEventProcessor::MyDPPEventProcessor()
{
  addParameterMap(1, new CAENPHAArrayMapper(
       "pha.time", "pha.energy", nullptr, "pha.extra2", "pha.finetime"
  ), 2);
  addParameterMap(2, new CAENPSDArrayMapper(
     "psd1.time" ,"psd1.shortQ", "psd1.longQ", "psd1.baseline", "psd1.pileup"),
     2   
  );
  std::vector<std::string> src3times={"start.time", "", "stop.time"};
  std::vector<std::string> src3charge={"start.E", "", "stop.E"};
  std::vector<std::string> src3bl={"start.baseline", "", "stop.baseline"};
  std::vector<std::string> src3pup={"start.pileup","", "stop.pileup"};
  std::vector<std::string> src3unused;
  addParameterMap(3, new CAENPSDParameterMapper(
      src3charge, src3unused, src3bl, src3times, src3pup),
      4
  );
}
