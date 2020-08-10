#ifndef CMYPROCESSOR_H 
#define CMYPROCESSOR_H 

#include "EventProcessor.h"
#include "ThreadAnalyzer.h"
#include <Event.h>
#include <TreeParameter.h>

class CParameterMapper;

class CMyProcessor : public  CEventProcessor
{
 public:
  long var1;
  CTreeParameter pars;  
  CTreeParameter tmp;
  CTreeVariable vars;
  
  CMyProcessor();
  CMyProcessor(const CMyProcessor& rhs);
  virtual ~CMyProcessor();
  virtual CMyProcessor* clone() { return new CMyProcessor(*this); }

  void setParameterMapper(DAQ::DDAS::CParameterMapper& rParameterMapper){};
  
  virtual Bool_t operator()(const Address_t pEvent,
			    CEvent&         rEvent,
			    CAnalyzer&      rAnalyzer,
			    CBufferDecoder& rDecoder,
			    BufferTranslator& trans,
			    long thread);

  virtual Bool_t OnInitialize();

};

#endif
