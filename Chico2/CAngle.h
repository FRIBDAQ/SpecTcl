/**
 * This file declares an event processor that computes angles (theta an phi)
 * for the Chico 2 detector.  These come from differences in Cathode times.
 * Specifically, each four parameters define a theta and phi angle via
 * their differences as follows:
 *
 *  Name            Channels
 *  Front.Theta-0  0 - 1
 *  Front.Phi-0    2 - 3
 *  ..
 * Front.Theta-9   36-37
 * Front.Phi-9     38-39
 *
 * Back.Theta-0    41 - 40
 * Back.Phi-0      43 - 42
 * ..
 * Back.Theta-9    77 - 76
 * Back.Phi-9      79 - 78
 *
 */
#ifndef __CANGLE_H
#define __CANGLE_H
#include <config.h>
#include <EventProcessor.h>
#include <TreeParameter.h>


class CAngle : public CEventProcessor 
{
private:
  bool           m_initialized;
  CTreeParameter m_FrontRaw[40];
  CTreeParameter m_BackRaw[40];

  CTreeParameter m_FrontTheta[10];
  CTreeParameter m_FrontPhi[10];
  
  CTreeParameter m_BackTheta[10];
  CTreeParameter m_BackPhi[10];

public:
  virtual Bool_t OnAttach(CAnalyzer& rAnalyzer);
  virtual Bool_t operator()(const Address_t pEvent, CEvent& rEvent,
			    CAnalyzer& rAnalyzer, CBufferDecoder& rDecoder);

private:
  void bindParameters();
  
};


#endif
