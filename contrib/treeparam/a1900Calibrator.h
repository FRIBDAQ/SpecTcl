// Class: CA1900Calibrator
// D. Bazin July 2001

#ifndef __A1900CALIBRATOR_H
#define __A1900CALIBRATOR_H

class CA1900Calibrator : public  CEventProcessor
{
public:
  virtual Bool_t operator()(const Address_t pEvent,
			    CEvent&         rEvent,
			    CAnalyzer&      rAnalyzer,
			    CBufferDecoder& rDecoder);
  void calibrateFPPPAC();
  void calibrateFPPIN();
  void calibratePID();
};

#endif
