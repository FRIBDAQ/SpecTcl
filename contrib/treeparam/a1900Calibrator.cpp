// A1900 calibrator
// Daniel Bazin
// Monday July 16 2001

#include "EventProcessor.h"
#include "TCLAnalyzer.h"
#include "a1900Parameters.h"
#include "a1900Variables.h"
#include <Event.h> 
#include "a1900Calibrator.h"

Bool_t
CA1900Calibrator::operator()(const Address_t pEvent,
				CEvent&         rEvent,
				CAnalyzer&      rAnalyzer,
				CBufferDecoder& rDecoder)
{
	calibrateFPPPAC();
	calibrateFPPIN();
	calibratePID(); 
};

void CA1900Calibrator::calibrateFPPPAC()
{
	a1900.fp.ppac0.sumx = a1900.fp.ppac0.left + a1900.fp.ppac0.right;
	a1900.fp.ppac0.sumy = a1900.fp.ppac0.up + a1900.fp.ppac0.down;
	a1900.fp.ppac0.sumxy = a1900.fp.ppac0.sumx + a1900.fp.ppac0.sumy;
};

void CA1900Calibrator::calibrateFPPIN()
{
	a1900.fp.pin.mev = a1900.fp.pin.de * a1900v.pin.gain + a1900v.pin.offset;
};

void CA1900Calibrator::calibratePID()
{
};
