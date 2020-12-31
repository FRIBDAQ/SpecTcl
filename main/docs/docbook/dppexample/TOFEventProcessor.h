#ifndef TOFEVENTPROCESSOR_H
#define TOFEVENTPROCESSOR_H
#include <EventProcessor.h>
#include <TreeParameter.h>

class TOFEventProcessor :  public CEventProcessor
{
private:
  CTreeParameter m_start;
  CTreeParameter m_stop;
  CTreeParameter m_tof;
public:
  TOFEventProcessor();
  Bool_t operator()(
    Address_t pEvent, CEvent& rEvent, CAnalyzer& a, CBufferDecoder& d
  );
  
};


#endif
