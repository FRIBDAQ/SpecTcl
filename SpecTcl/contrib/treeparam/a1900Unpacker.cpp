// A1900 unpacker
// Daniel Bazin
// Friday July 13 2001

// A1900 Unpacker for a1900SpecTcl (SpecTcl version 0.4)
#include "EventProcessor.h"
#include "TCLAnalyzer.h"
#include "a1900Parameters.h"
#include <Event.h> 
#include "a1900Unpacker.h" 

Bool_t
CA1900Unpacker::operator()(const Address_t pEvent,
				CEvent&         rEvent,
				CAnalyzer&      rAnalyzer,
				CBufferDecoder& rDecoder)
{

  CTclAnalyzer&      rAna((CTclAnalyzer&)rAnalyzer);
  UShort_t* p      = (UShort_t*)pEvent;
  UShort_t  nWords = *p++;

  // At least one member of the pipeline must tell the analyzer how
  // many bytes were in the raw event so it knows where to find the
  // next event.

  rAna.SetEventSize(nWords*sizeof(UShort_t)); // Set event size.

  nWords--;			// The word count is self inclusive.

  nWords = *p++;		// Don't ask me why
  nWords--;			// there are two consecutive lengthes in this packet!!!

	// get parameter array from analyzer
	CTreeParameter::setEvent(rEvent);

	// test is we are dealing with an A1900 packet
	if (*p++ != A1900_PACKET) return kfFALSE;	// abort pipeline: this is not an A1900 packet
	nWords--;

	// first data is the rf
	a1900.rf = *p++;
	nWords--;

	// then comes a switch board for various internal packets
	while(nWords > 0) {
		UShort_t		packetlength = *p++;
		UShort_t		packettag = *p++;
		switch(packettag) {
			// Cathode data from image 2 PPAC0
			case IM2PPAC0_PACKET:
				Im2PPACUnpacker(p);
				for(short i=0; i<Im2PPACChannels; i++) {
					if (valid[i] > 3) a1900.im2.ppac0.cathode[i] = strip[i];
				}
			break;
			// Cathode data from image 2 PPAC1
			case IM2PPAC1_PACKET:
				Im2PPACUnpacker(p);
				for(short i=0; i<Im2PPACChannels; i++) {
					if (valid[i] > 3) a1900.im2.ppac1.cathode[i] = strip[i];
				}
			break;
			// Anode data from image 2 PPAC0
			case IM2PPAC_A0_PACKET:
				for(short i=0; i<4; i++) {
					a1900.im2.ppac0.anode[i] = *p++;
					a1900.im2.ppac0.time[i] = *p++;
				}
			break;
			// Anode data from image 2 PPAC1
			case IM2PPAC_A1_PACKET:
				for(short i=0; i<4; i++) {
					a1900.im2.ppac1.anode[i] = *p++;
					a1900.im2.ppac1.time[i] = *p++;
				}
			break;
			// Image 2 plastic scintillator
			case IM2PLAST_PACKET:
				a1900.im2.scint.denorth = *p++;
				a1900.im2.scint.timenorth = *p++;
				a1900.im2.scint.desouth = *p++;
				a1900.im2.scint.timesouth = *p++;
			break;
			// Focal plane PPAC0
			case FPPAC0_PACKET:
				a1900.fp.ppac0.down = *p++;
				p++;	// skip tdc
				a1900.fp.ppac0.up = *p++;
				p++;	// skip tdc
				a1900.fp.ppac0.left = *p++;
				p++;	// skip tdc
				a1900.fp.ppac0.right = *p++;
				p++;	// skip tdc
				a1900.fp.ppac0.anode = *p++;
				a1900.fp.ppac0.time = *p++;
			break;
			// Focal plane PPAC1
			case FPPAC1_PACKET:
				a1900.fp.ppac1.down = *p++;
				p++;	// skip tdc
				a1900.fp.ppac1.up = *p++;
				p++;	// skip tdc
				a1900.fp.ppac1.left = *p++;
				p++;	// skip tdc
				a1900.fp.ppac1.right = *p++;
				p++;	// skip tdc
				a1900.fp.ppac1.anode = *p++;
				a1900.fp.ppac1.time = *p++;
			break;
			// Focal plane PIN detector
			case FPPIN_PACKET:
				a1900.fp.pin.de = *p++;
				a1900.fp.pin.time = *p++;
			break;
			// Focal plane stack of Si detectors
			case FPSTACK_PACKET:
				for(short i=0; i<4; i++) {
					a1900.fp.stack[i].de = *p++;
					p++;	// skip tdc
				}
			break;
			// Focal plane scintillator
			case FPSCINT_PACKET:
				a1900.fp.scint.enorth = *p++;
				a1900.fp.scint.timenorth = *p++;
				a1900.fp.scint.esouth = *p++;
				a1900.fp.scint.timesouth = *p++;
			break;
			// Focal plane ion chamber
			case FPCHAMBER_PACKET:
				// not installed yet
			break;
			// Extended focal plane (after slits)
			case XFPPIN_L_PACKET:
				p++;	// ignore for now
				p++;
			break;
			case XFPPIN_M_PACKET:
				p++;	// ignore for now
				p++;
			break;
			case XFPPIN_R_PACKET:
				p++;	// ignore for now
				p++;
			break;
			default:
				printf("Unrecognized packet tag: 0x%x\n", packettag);
				p += packetlength-2;
			break;
		}	// end case(packettag)
		nWords -= packetlength;
	}	// end while(nWords > 0)
  return kfTRUE;		// kfFALSE would abort pipeline.
};

void 
CA1900Unpacker::Im2PPACUnpacker(UShort_t *p)
{
  // ppac1.bit configuration
  // event structure:
  // • last memory address
  // • threshold
  // for each address 2x24 bits words are read
  // • first 24 bits word:
  // bits 0-9   : FEE 10 bits from cards 0 & 1
  // bits 10-19  : FEE 10 bits from cards 2 & 3
  // bits 20-23 : FEE 4 LSB bits from cards 4 & 5
  // • second 24 bits word:
  // bits 0-5 : FEE 6 MSB bits from card 4 & 5
  // bits 6-7   : 0
  // bits 8-13  : Channel number from 0 to 63
  // bits 14-22 : Sample number from 0 to 511
  // bit 23     : Should be 0
  // The data below the threshold is suppressed
  
  ULong_t		index,Channel,Sample,Word1,Word2, minSample=511;
  UShort_t		word1, word2, ch1, ch2, data1, data2;
  UShort_t		i, j;
    
  UShort_t		Addr, ULM2367Add = *p++;
  UShort_t		ULM2367thres = *p++;
  ULong_t*		pl = (ULong_t*)p;		// set long pointer

  memset(ppac, 0, sizeof(ppac));
  memset(valid, 0, sizeof(valid));
  memset(strip, 0, sizeof(strip));
  for(Addr=0; Addr<ULM2367Add; Addr++) {
    Word1 = *pl++;			// read first 24 bits word
    Word2 = *pl++;			// read second 24 bits word
    Channel = (Word2&0x3F00)>>8;
    Sample = (Word2&0x7FC000)>>14;
    ppac[Channel][Sample] = Word1&0x3FF;
    ppac[Channel+64][Sample] = (Word1&0xFFC00)>>10;
    ppac[Channel+128][Sample] = ((Word1&0xF00000)>>20)+((Word2&0x3F)<<4);
    if (ppac[Channel][Sample] != 0) (valid[Channel])++;  
    if (ppac[Channel+64][Sample] != 0) (valid[Channel+64])++;
    if (ppac[Channel+128][Sample] != 0) (valid[Channel+128])++;
  } 	
	
  UInt_t		ch;
  for (i=0; i < Im2PPACChannels; i++) {
    if (valid[i]) {
      for (j=0; j < Im2PPACSamples; j++) {
	if (ppac[i][j] > 0) {
	  if (ppac[i][j] < ULM2367thres) 
	    printf ("Oops! found data lower than threshold at Channel %d, Sample %d: %d\n", i, j, ppac[i][j]);
	  else
	    strip[i] += (ppac[i][j] - ULM2367thres);
	}
      }
    }
  }
};
