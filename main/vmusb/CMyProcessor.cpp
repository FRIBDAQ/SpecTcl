#include "CMyProcessor.h"
#include <CRingBufferDecoder.h>
#include <SpecTcl.h>
#include <sstream>
#include <iomanip>
#include <cmath>

CMyProcessor::CMyProcessor():
  var1(0)
{}

CMyProcessor::CMyProcessor(const CMyProcessor& rhs):
  var1(rhs.var1)
{}  

CMyProcessor::~CMyProcessor()
{}



Bool_t
CMyProcessor::operator()(const Address_t pEvent,
			 CEvent&         rEvent,
			 CAnalyzer&      rAnalyzer,
			 CBufferDecoder& rDecoder,
			 BufferTranslator& trans,
			 long thread)
{
  if (tmp.isValid()){

    pars = tmp + vars*var1;
  }
  
  return kfTRUE;    
}

Bool_t
CMyProcessor::OnInitialize()
{
  tmp.Initialize("adc1.06");
  pars.Initialize("test_var", 16384, 0.0, 16383.0, "");
  vars.Initialize("const", 10, "");
  if (!tmp.isBound())
    tmp.Bind();  
  if (!pars.isBound())
    pars.Bind();
  
  return kfTRUE;
}
