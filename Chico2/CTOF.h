/**
 * Compute TOF for Chico 2. 
 * This is done by requiring exactly two times from the anodes
 * and subtracting one from the other.
 */
#ifndef __CTOF_H
#define __CTOF_H
#include <config.h>
#include <EventProcessor.h>
#include <TreeParameter.h>


/**
 * Event processor for computing TOFs.
 *
 * Creates a single parameter TOF which is the time difference
 * between two anode parameters.  This is computed iff only two
 * anode time parameters are present.
 *
 *  Channels are segregated by left and right channels.
 *  Left channel numbers are 0-4 and 10-14.  
 *  rights are 5-9 and 15-19.
 *
 *  Subtraction will always be right - left
 *  and no subtraction is done unless there's a right and a left channel.
 *
 */
class CTOF : public CEventProcessor
{
  bool              m_initialized; // true when we've found/created params.
  CTreeParameter    m_Anodes[20];  // Anode parameters.
  CTreeParameter    m_Tof;	   // Final time of flight parameter.
  
public:
  virtual Bool_t OnAttach(CAnalyzer& rAnalyzer);
  virtual Bool_t operator()(const Address_t pEvent, CEvent& rEvent,
			    CAnalyzer& rAnalyzer, CBufferDecoder& rDecoder);
private:
  void bindParameters();	// Bind m_Anodes to the 

};

#endif	/* __CTOF_H */
