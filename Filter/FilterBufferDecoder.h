#ifndef __FILTERBUFFERDECODER_H
#define __FILTERBUFFERDECODER_H

#ifndef __RPC_XDR_H
#include <rpc/xdr.h>
#define __RPC_XDR_H
#endif

// Forward class declarations:
class CBufferDecoder;

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

// CFilterBufferDecoder.
class CFilterBufferDecoder : public CBufferDecoder {
  // Member data:
  Bool_t m_fActive;
  XDR m_xdrs;
  UInt_t m_nSize;
  string m_sSection;
  Bool_t m_fXDRError;

  UInt_t m_nParameters;
  Bool_t m_fEventData;
  UInt_t m_nValidParameters;
  //UInt_t *(ValidParameterArray[m_nValidParameters]);
  UInt_t *(ValidParameterArray[10]); // Temporary fix to give the compiler something to work with. *****************************************
  UInt_t m_nEvents; // EntityCount.
  UInt_t m_nBodyOffset;
  UInt_t m_nOffset;
  UInt_t m_nBufferSize;
  //char m_pOutputBuffer[m_nBufferSize];
  char m_pOutputBuffer[8192]; // 8K. // Temporary fix to give the compiler something to work with. *****************************************

  vector<CParInfo*> m_vParInfo;
  vector<CEvent*> m_vBuffer;

 public:
  // Constructors:
  CFilterBufferDecoder();
  CFilterBufferDecoder(string sFileName);
  ~CFilterBufferDecoder();

  // Operators:
  virtual void operator()(UInt_t nBytes,
			  Address_t pBuffer,
			  CAnalyzer& rAnalyzer);

  // Functions:
  Bool_t isActive();

  Bool_t XDRstring(string&);
  Bool_t XDRuint(UInt_t&);
  Bool_t XDRarray();
  Bool_t XDRfloat(Float_t&);
  Bool_t XDRfill(UInt_t);

  virtual const Address_t getBody();
  virtual UInt_t getBodySize();
  virtual UInt_t getRun();
  virtual UInt_t getEntityCount();
  virtual UInt_t getSequenceNo();
  virtual UInt_t getLamCount();
  virtual UInt_t getPatternCount();
  //  virtual UInt_t getBufferCount();
  virtual UInt_t getBufferType();
  virtual void getByteOrder(Short_t& Signature16, Int_t& Signature32);
  virtual string getTitle();
}; // CFilterBufferDecoder.

#endif
