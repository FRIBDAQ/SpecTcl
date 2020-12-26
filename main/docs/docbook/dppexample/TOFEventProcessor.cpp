#include "TOFEventProcessor.h"


TOFEventProcessor::TOFEventProcessor() :
  m_start("start.time",  "ns"),
  m_stop("stop.time", "ns"),
  m_tof("TOF",  "ns")
{}

Bool_t
TOFEventProcessor::operator()(
     Address_t pEvent, CEvent& rEvent, CAnalyzer& a, CBufferDecoder& d
)
{
  if (m_start.isValid() && m_stop.isValid()) {
    m_tof = m_stop - m_start;
  }
  return kfTRUE;
}
			      
