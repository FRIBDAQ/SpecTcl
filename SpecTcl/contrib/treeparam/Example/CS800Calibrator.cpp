#include <Event.h> 
#include <EventProcessor.h>
#include <TCLAnalyzer.h>

#include "CS800.h"
#include "CS800Calibrator.h" 


Bool_t
CS800Calibrator::operator()(const Address_t pEvent,
				CEvent&         rEvent,
				CAnalyzer&      rAnalyzer,
				CBufferDecoder& rDecoder)
{

// Focal Plane
  if (s800.fp.ic.HasData()) s800.fp.ic.Calibrate();
  if (s800.fp.crdc1.HasData()) {
  	s800.fp.crdc1.pad.Calibrate();
  	s800.fp.crdc1.drift.Calibrate();
  }
  if (s800.fp.crdc2.HasData()) {
  	s800.fp.crdc2.pad.Calibrate();
  	s800.fp.crdc2.drift.Calibrate();
  }

// Image
  if (s800.im.ppac0.HasData()) s800.im.ppac0.Calibrate();
  if (s800.im.ppac1.HasData()) s800.im.ppac1.Calibrate();

// Target
  if (s800.ta.ppac2.HasData()) s800.ta.ppac2.Calibrate();
  if (s800.ta.pin1.HasData()) s800.ta.pin1.Calibrate();
  if (s800.ta.pin2.HasData()) s800.ta.pin2.Calibrate();

  return kfTRUE;
};
