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

// CParInfo.
class CParInfo {
  // Member data:
  string m_sParName;
  UInt_t m_nParId;
  Bool_t m_fActive;

 public:
  // Constructors:
  CParInfo() {
    setParName("");
    setParId(-1);
    setActive(kfFALSE);
  };

  CParInfo(string sParName, UInt_t nParId, Bool_t fActive) {
    setParName(sParName);
    setParId(nParId);
    setActive(fActive);
  };

  ~CParInfo() {};

  // Functions:
  string getParName() {return m_sParName;};
  void setParName(string sParName) {m_sParName = sParName;};
  UInt_t getParId() {return m_nParId;};
  void setParId(UInt_t nParId) {m_nParId = nParId;};
  Bool_t isActive() {return m_fActive;};
  void setActive(Bool_t fActive) {m_fActive = fActive;};
}; // CParInfo.

// CFilterEventProcessor.
class CFilterEventProcessor : public CEventProcessor {
  // Member data:
  Bool_t m_fReadError;
  Bool_t m_fHeaderReceived;
  UInt_t m_nBUFFERSIZE; // Initialize to 8K.
  char* m_pBuffer;
  UInt_t m_nOffset;
  string m_sTag;
  UInt_t m_nParameters;
  UInt_t** m_pValidParameterArray;

  vector<CParInfo*> m_vParInfo;
  vector<CEvent*> m_vBuffer;

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

  Bool_t read_string(string&);
  Bool_t read_uint(UInt_t&);
  Bool_t read_array();
  Bool_t read_float(Float_t&);
  Bool_t read_char(char&);

  void incr_offset(UInt_t);
}; // CFilterEventProcessor.

#endif
