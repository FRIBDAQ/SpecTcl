#ifndef CS800UNPACKER_H
#define CS800UNPACKER_H

class CS800Unpacker : public  CEventProcessor
{
private:

public:
  virtual Bool_t operator()(const Address_t pEvent,
			    CEvent&         rEvent,
			    CAnalyzer&      rAnalyzer,
			    CBufferDecoder& rDecoder);
};

#endif
