#ifndef __FILTERBUFFERDECODER_H
#define __FILTERBUFFERDECODER_H

#ifndef __RPC_XDR_H
#include <rpc/xdr.h>
#define __RPC_XDR_H
#endif

// Forward class declarations:
class CBufferDecoder;

// CFilterBufferDecoder.
class CFilterBufferDecoder : public CBufferDecoder {
  // Member data:
  Bool_t m_fActive;
  XDR m_xdrs;
  UInt_t m_nSize;
  string m_sTag; // Documentation buffer, event data, or end-of-record.
  Bool_t m_fXDRError;

  UInt_t m_nParameters;
  Bool_t m_fEventData;
  UInt_t m_nValidParameters;
  //UInt_t *(ValidParameterArray[m_nValidParameters]);
  //UInt_t *m_pValidParameterArray;
  UInt_t*** m_pValidParameterArray;
  UInt_t m_nEvents; // EntityCount.
  UInt_t m_nOffset; // Offset of XDR-formatted buffer.
  UInt_t m_nOutputBufferOffset; // Offset of the output char[] buffer.
  UInt_t m_nBUFFERSIZE; // Initialize to 8K.
  char** m_pOutputBuffer;

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

  // The following read from the XDR file and put into the buffer.
  Bool_t XDRstring(string&);
  Bool_t XDRuint(UInt_t&);
  Bool_t XDRarray();
  Bool_t XDRfloat(Float_t&);
  Bool_t XDRfill(UInt_t); // Fills up the rest of the buffer.

  void incr_offset(UInt_t);

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
