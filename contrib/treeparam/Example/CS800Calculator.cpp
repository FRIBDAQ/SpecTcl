#include <Event.h> 
#include <EventProcessor.h>
#include <TCLAnalyzer.h>

#include "CS800.h"
#include "CS800Calculator.h" 


Bool_t
CS800Calculator::operator()(const Address_t pEvent,
				CEvent&         rEvent,
				CAnalyzer&      rAnalyzer,
				CBufferDecoder& rDecoder)
{

// Focal Plane
  if (s800.fp.e1.HasData()) s800.fp.e1.Calculate();
  if (s800.fp.e2.HasData()) s800.fp.e2.Calculate();
  if (s800.fp.e3.HasData()) s800.fp.e3.Calculate();
  if (s800.fp.ic.HasData()) s800.fp.ic.Calculate();
  if (s800.fp.crdc1.HasData()) s800.fp.crdc1.CalculatePosition();
  if (s800.fp.crdc2.HasData()) s800.fp.crdc2.CalculatePosition();
  if (s800.fp.crdc1.HasData() && s800.fp.crdc2.HasData()) {
    s800.fp.LoadValues();
    s800.fp.track.CalculateTracking();
  }

// Image
  if (s800.im.ppac0.HasData()) s800.im.ppac0.CalculatePosition();
  if (s800.im.ppac1.HasData()) s800.im.ppac1.CalculatePosition();
  if (s800.im.ppac0.HasData() && s800.im.ppac1.HasData())
  	s800.im.Calculate();

// Target
  if (s800.ta.ppac2.HasData()) s800.ta.ppac2.CalculatePosition();
  if (s800.ta.pin1.HasData() && s800.ta.pin2.HasData()) 
  	s800.ta.Calculate();

// PID
  if (s800.pid.tof.HasData()) s800.pid.tof.CalculateTOF();

  return kfTRUE;
};
