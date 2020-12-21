#include <ThreadAnalyzer.h>
#include "MyCalibrator.h"
#include "MyParameterMapper.h"
#include "MyParameters.h"
#include <ZMQRDPatternClass.h>

MyCalibrator::MyCalibrator()
{}

MyCalibrator::MyCalibrator(const MyCalibrator& rhs) 
{}

MyCalibrator::~MyCalibrator() {
  delete m_pParameterMapper;
}

void
//MyCalibrator::setParameterMapper(DAQ::DDAS::CParameterMapper& rParameterMapper)
MyCalibrator::setParameterMapper(DAQ::DDAS::FitParameterMapper& rParameterMapper)
{
  m_pParameterMapper = reinterpret_cast<MyParameterMapper*>(&rParameterMapper);
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

  // loop over hits  
  for(int i= 0; i<params.data.m_chanHit.size(); i++){

    int id = params.data.m_chanHit[i];
    double ran = ( static_cast<double>(rand())) / (static_cast<double>(RAND_MAX));

    if( id == 341) {
      if (rEvent[params.chan[id].energy.getId()].isValid()){
	params.example.ex1.energy = (params.chan[id].energy + ran) + 0.0;
	params.example.ex1.ecal = params.example.ex1.energy*params.example.var.var1.c1;
      }
      if (rEvent[params.chan[id].timestamp.getId()].isValid())
	params.example.ex1.time = params.chan[id].timestamp + 10.0;
    }

  } 

  return kfTRUE; 
  
};
