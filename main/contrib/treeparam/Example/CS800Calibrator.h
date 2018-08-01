#ifndef CS800CALIBRATOR_H
#define CS800CALIBRATOR_H

class CS800Calibrator : public  CEventProcessor
{
private:

public:
  virtual Bool_t operator()(const Address_t pEvent,
			    CEvent&         rEvent,
			    CAnalyzer&      rAnalyzer,
			    CBufferDecoder& rDecoder);
};

#endif
