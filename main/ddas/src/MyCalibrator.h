#ifndef __MYCALIBRATOR_H
#define __MYCALIBRATOR_H

#include <EventProcessor.h>
#include <TranslatorPointer.h>
#include <TCLAnalyzer.h>

class MyParameterMapper;

class MyCalibrator : public  CEventProcessor
{
 public:
  MyParameterMapper* m_pParameterMapper; 

  MyCalibrator();
  MyCalibrator(const MyCalibrator& rhs);
  ~MyCalibrator();
  virtual MyCalibrator* clone() { return new MyCalibrator(*this); }

  //  void setParameterMapper(DAQ::DDAS::CParameterMapper& rParameterMapper);
  void setParameterMapper(DAQ::DDAS::FitParameterMapper& rParameterMapper);  
  
  virtual Bool_t operator()(const Address_t pEvent,
			    CEvent&         rEvent,
			    CAnalyzer&      rAnalyzer,
			    CBufferDecoder& rDecoder,
			    BufferTranslator& trans,
			    long thread);
  

};

#endif
