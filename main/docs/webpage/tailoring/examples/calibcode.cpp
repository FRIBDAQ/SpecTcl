//
//  UserCode.cpp
//    This module is available for the user to supply code for the histogrammer
//    the normal use for this module is to supply an event unpacker.
//    The sample code in this file unpacks a fixed length event consisting
//    of a word count followed by a bunch of parameters.

// Header files:

#include <histotypes.h>
#include <TCLVariable.h>
#include <Globals.h>
#include <assert.h>

#include "Event.h"
#include "EventUnpacker.h"



/////////////////////////////////////////////////////////////////////////
//
//  Event Unpacker class..
//
class CTestUnpacker : public CEventUnpacker
{
  DFloat_t     m_dSlope;
  DFloat_t     m_dCrossover;

  CTCLVariable* m_pSlope;
  CTCLVariable* m_pCrossover;
public:
  CTestUnpacker() :
    m_pSlope(0),
    m_pCrossover(0)
  { }
  ~CTestUnpacker() {
      delete m_pSlope;
      delete m_pCrossover;
   }
  UInt_t operator()(const Address_t pEvent,
		    CEvent&         rEvent,
		    CAnalyzer&      rAnalyzer,
		    CBufferDecoder& rDecoder);
  virtual   void OnAttach(CAnalyzer& rAnalyzer);
  virtual   void OnDetach(CAnalyzer& rAnalyzer);

};

//////////////////////////////////////////////////////////////////
//
// operator() is responsible for unpacking an event into a CEvent object.
// parameters:
//     const Address_t pEvent:
//          Pointer to the event.  The Event may not be modified.
//     CEvent& rEvent:
//          Reference to a CEvent.  rEvent can be treated like a parameter
//          array, however indexing operators will automatically extend the
//          size of the array if needed.  The intial size was set by 
//          the TCL variable ParameterCount
//     CAnalyzer& rAnalyzer:
//          References the analyzer which is responsible for this run.
//     CBufferDecoder& rDecoder:
//          References the buffer decoder.  The buffer decoder's services
//          may be used if necessary.
// Returns:
//     The number of *BYTES* which were in the unpacked event.
//
UInt_t
CTestUnpacker::operator()(const Address_t pEvent,
			  CEvent&         rEvent,
			  CAnalyzer&      rAnalyzer,
			  CBufferDecoder& rDecoder)
{
  //
  //  The test event consists of a count follwed by a number of parameters.
  //  fixed length fixed format events.
  //
  UShort_t* p    = (UShort_t*)pEvent;
  UShort_t  nWds = *p++;
  UInt_t    pIdx = nWds-1;
  UInt_t    i = 0;
  nWds--;
  while(nWds) {
    rEvent[i] = ((Int_t)*p);
    p++;
    nWds--;
    i++;
  }
  /// Here's a simple psuedo parameter definition.
  //
  rEvent[pIdx] = rEvent[0] + rEvent[1];
  pIdx++;

  DFloat_t calib = (DFloat_t)rEvent[1];                // Compute calibrated var.
  calib = calib*m_dSlope + m_dCrossover;
  rEvent[pIdx] = (Int_t)(calib + 0.5);                 // Round it into integer.  

  p = (UShort_t*)pEvent;
  return (UInt_t)(*p)*sizeof(UShort_t);
}

//////////////////////////////////////////////////////////////////////////
//
//  OnAttach:
//    Called as the unpacker is being hooked to the analyzer.  This is a
//    good point to perform any initialization which cannot be done in
//    a constructor, but requires run time facilities of SpecTcl to be
//    known to be started.  For example, at this point CTCLVariable-s could
//    be linked to member attributes.
//
// Parameters:
//    CAnalyzer& rAnalyzer:
//       The analyzer to which we are being attached.  Analyzer services can
//       be used.
void 
CTestUnpacker::OnAttach(CAnalyzer& rAnalyzer)
{
  CEventUnpacker::OnAttach(rAnalyzer); // Execute base class attach.
  // Create the TCL variable objects:

  m_pSlope     = new CTCLVariable(gpInterpreter, std::string("slope"),
	                            kfFALSE);
  m_pCrossover = new CTCLVariable(gpInterpreter, std::string("crossover"),
                                  kfFALSE);

  // Now link these variables to our variables Any failure results in a
  // crash of the program.

  assert(m_pSlope->Link((void*)&m_dSlope,
                        TCL_LINK_DOUBLE) == TCL_OK);
  assert(m_pCrossover->Link((void*)&m_dCrossover, 
                            TCL_LINK_DOUBLE) == TCL_OK);
}
//////////////////////////////////////////////////////////////////////////
//
// OnDetach
//    Executes as an unpacker is being detached from the analyzer.
//    Perform any finalization which might be required before 
//    the destructor is called.
//
// Parameters:
//    CAnalyzer& rAnalyzer:
//       The analyzer from which we are being detached.  The analyzer's 
//       services may be called.
void
CTestUnpacker::OnDetach(CAnalyzer& rAnalyzer)
{
  CEventUnpacker::OnDetach(rAnalyzer);

 
}

CTestUnpacker   gUnpacker;
CEventUnpacker& grUnpacker = gUnpacker;	// Allows SpecTcl to find us.




