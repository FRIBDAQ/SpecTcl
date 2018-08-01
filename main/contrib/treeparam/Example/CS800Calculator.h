#ifndef CS800CALCULATOR_H
#define CS800CALCULATOR_H

class CS800Calculator : public  CEventProcessor
{
private:

public:
  virtual Bool_t operator()(const Address_t pEvent,
			    CEvent&         rEvent,
			    CAnalyzer&      rAnalyzer,
			    CBufferDecoder& rDecoder);
};

#endif
