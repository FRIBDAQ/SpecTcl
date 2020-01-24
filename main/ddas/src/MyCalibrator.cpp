#include <ThreadAnalyzer.h>
#include "MyCalibrator.h"
#include "MyParameterMapper.h"
#include "MyParameters.h"

MyCalibrator::MyCalibrator(MyParameterMapper& rParameterMapper, MyPipelineData& rPipelineData) :
  m_pParameterMapper(&rParameterMapper),
  m_pPipelineData(&rPipelineData)
{}

MyCalibrator::MyCalibrator(const MyCalibrator& rhs) :
  m_pParameterMapper(rhs.m_pParameterMapper->clone()),
  m_pPipelineData(rhs.m_pPipelineData->clone())
{}

MyCalibrator::~MyCalibrator() {
  delete m_pParameterMapper;
  delete m_pPipelineData;
}

Bool_t
MyCalibrator::operator()(const Address_t pEvent,
			 CEvent&         rEvent,
			 CAnalyzer&      rAnalyzer,
			 CBufferDecoder& rDecoder,
			 BufferTranslator& trans,
			 long thread)
{
  auto& params = m_pParameterMapper->m_params;

  //  std::cout << params.data.m_chanHit.size() << std::endl;
  
  return kfTRUE; 
  
};
