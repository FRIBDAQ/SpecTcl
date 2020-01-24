#ifndef __MYCALIBRATOR_H
#define __MYCALIBRATOR_H

#include <EventProcessor.h>
#include <TranslatorPointer.h>
#include <TCLAnalyzer.h>

class MyParameterMapper;
class MyPipelineData;

class MyCalibrator : public  CEventProcessor
{
 public:
  MyParameterMapper*     m_pParameterMapper; 
  MyPipelineData*        m_pPipelineData;

  MyCalibrator(MyParameterMapper& rParameterMapper, MyPipelineData& rPipelineData);
  MyCalibrator(const MyCalibrator& rhs);
  ~MyCalibrator();
  virtual MyCalibrator* clone() { return new MyCalibrator(*this); }

  virtual Bool_t operator()(const Address_t pEvent,
			    CEvent&         rEvent,
			    CAnalyzer&      rAnalyzer,
			    CBufferDecoder& rDecoder,
			    BufferTranslator& trans,
			    long thread);
  

};

#endif
