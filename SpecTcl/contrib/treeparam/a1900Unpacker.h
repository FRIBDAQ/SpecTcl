// Class: CA1900Unpacker
// D. Bazin July 2001

#ifndef __A1900UNPACKER_H
#define __A1900UNPACKER_H

const int A1900_PACKET = 0x1900;
const int ADC_PACKET = 0x1901;
const int FPPAC0_PACKET = 0x1910;
const int FPPAC1_PACKET = 0x1911;
const int FPPIN_PACKET = 0x1920;
const int XFPPIN_L_PACKET = 0x1921;
const int XFPPIN_M_PACKET = 0x1922;
const int XFPPIN_R_PACKET = 0x1923;
const int FPSCINT_PACKET = 0x1930;
const int FPCHAMBER_PACKET = 0x1940;
const int FPSTACK_PACKET = 0x1950;
const int IM2PLAST_PACKET = 0x1960;
const int IM2PPAC0_PACKET = 0x1970;
const int IM2PPAC_A0_PACKET = 0x1971;
const int IM2PPAC1_PACKET = 0x1980;
const int IM2PPAC_A1_PACKET = 0x1981;
const int Im2PPACChannels = 192;
const int Im2PPACSamples = 512;

class CA1900Unpacker : public  CEventProcessor
{
private:
  UShort_t ppac[Im2PPACChannels][Im2PPACSamples],valid[Im2PPACChannels];
  UShort_t strip[Im2PPACChannels];
public:
  void Im2PPACUnpacker(UShort_t*);
  virtual Bool_t operator()(const Address_t pEvent,
			    CEvent&         rEvent,
			    CAnalyzer&      rAnalyzer,
			    CBufferDecoder& rDecoder);
};

#endif
