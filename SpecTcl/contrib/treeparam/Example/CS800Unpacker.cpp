#include <Event.h> 
#include <EventProcessor.h>
#include <TCLAnalyzer.h>

#include "CS800.h"
#include "CS800Unpacker.h" 

#include "S800Packets.h" 


Bool_t
CS800Unpacker::operator()(const Address_t pEvent,
				CEvent&         rEvent,
				CAnalyzer&      rAnalyzer,
				CBufferDecoder& rDecoder)
{

  CTclAnalyzer&      rAna((CTclAnalyzer&)rAnalyzer);
  TranslatorPointer<UShort_t> p(*(rDecoder.getBufferTranslator()), pEvent);
//  UShort_t* p      = (UShort_t*)pEvent;
  UShort_t  tWords = *p++, subEventLength, nWords;

  // At least one member of the pipeline must tell the analyzer how
  // many bytes were in the raw event so it knows where to find the
  // next event.

  rAna.SetEventSize(tWords*sizeof(UShort_t)); // Set event size.

  tWords--;			// The word count is self inclusive.

	// get parameter array from analyzer
	CTreeParameter::setEvent(rEvent);
	
    s800.Reset(); // Reset the whole S800 tree
	int foundEvents = 0;

	while (tWords > 0) {
		nWords = *p++;	// sub-event length
		subEventLength = nWords;
		nWords--;
		if (*p++ != S800_PACKET) {	// this is not my stuff so I'll keep looking further down
			tWords -= subEventLength;
			p += subEventLength-2;
		} else {					// found S800 data! break out of the loop
			nWords--;
			foundEvents = 1;
			break;
		}
	}


	if (foundEvents == 0) return kfTRUE;// give up, we didn't find our stuff
//	if (*p++ != S800_PACKET_VERSION) {
//		printf("S800Unpacker version mismatch, aborted\n");
//		return kfFALSE; // version mismatch
//	}
//	nWords--;

	while (nWords > 0) {
		UShort_t		packetlength = *p++;
		UShort_t		packettag    = *p++;
		UShort_t		ID;

		switch(packettag) {

			case S800_FP_SCINT_PACKET:
				p = s800.fp.e1.Unpack(p);
				p = s800.fp.e2.Unpack(p);
				p = s800.fp.e3.Unpack(p);
			break;
			
			case S800_FP_IC_PACKET:
				p = s800.fp.ic.Unpack(p);
			break;
			
			case S800_FP_CRDC_PACKET:
				ID = *p++;
				if (ID == 0) p = s800.fp.crdc1.Unpack(p);
				if (ID == 1) p = s800.fp.crdc2.Unpack(p);
			break;
			
			case S800_PPAC_PACKET:
				ID = *p++;
				if (ID == 0) p = s800.im.ppac0.Unpack(p);
				if (ID == 1) p = s800.im.ppac1.Unpack(p);
				if (ID == 2) p = s800.ta.ppac2.Unpack(p);
			break;
			
			case S800_PIN_PACKET:
				ID = *p++;
				if (ID == 1) p = s800.ta.pin1.Unpack(p);
				if (ID == 2) p = s800.ta.pin2.Unpack(p);
			break;
			
			case S800_TOF_PACKET:
				p = s800.pid.tof.Unpack(p);
			break;

			case S800_BIT_PACKET:
				p = s800.pid.bit.Unpack(p);
			break;

			default:
				printf("Unrecognized packet tag: 0x%x\n", packettag);
				p += packetlength-2;
			break;
		}	
		nWords -= packetlength;
	}
	
	s800.fp.crdc1.FillSnapshotSpectra(rAnalyzer);
	s800.fp.crdc2.FillSnapshotSpectra(rAnalyzer);

  return kfTRUE;
};
