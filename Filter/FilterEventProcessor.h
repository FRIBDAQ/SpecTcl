#ifndef __FILTEREVENTPROCESSOR_H
#define __FILTEREVENTPROCESSOR_H

// Include files:
#ifndef __HISTOTYPES_H
#include <histotypes.h>
#endif

// Forward class declarations:
class CEvent;
class CAnalyzer;
class CBufferDecoder;
class CEventProcessor;

// CFilterEventProcessor.
class CFilterEventProcessor : public CEventProcessor {
  // Member data:
  char m_pBuffer[8192]; // 8K.

 public:
  // Constructors:
  CFilterEventProcessor();
  ~CFilterEventProcessor();

  // Operators:
  virtual Bool_t operator()(const Address_t pEvent,
			    CEvent& rEvent,
			    CAnalyzer& rAnalyzer,
			    CBufferDecoder& rDecoder);

  // Functions:
  Bool_t OnOther();
}; // CFilterEventProcessor.

#endif
