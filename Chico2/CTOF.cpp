/**
 * Implementation of TOF code.  See .h file for
 * information that describes what this does.
 */

#include "CTOF.h"
#include <stdio.h>

/**
 * OnAttach - Called once when the event processor is
 *            attached to the SpecTcl event processor
 *            pipeline.  we set m_initialized false and
 *            bind m_Tof to the 'TOF' parameter.
 *
 * @param rAnalyzer - references the analyzer.
 * @return Bool_t kfTRUE unconditionally.
 */
Bool_t
CTOF::OnAttach(CAnalyzer& rAnalyzer)
{
  m_initialized = false;	// Need to bind anodes.

  // Initialize the TOF tree parameter -- bind it to the 'TOF'
  // parameter.

  m_Tof.Initialize("TOF", 1024, -50.0, 50.0, "ns");


  return kfTRUE;
}
/**
 * operator() 
 *
 *  This computes the parameter.
 *  - if necessary bind the m_Anodes values to the
 *    underlying parameters.
 *  - Pull out the left and right parameter
 *    Ensure that there are exactly one of each.
 *    ..if not don't do anything.
 *  - Compute right-left as TOF.
 *
 * @param pEvent - Raw event always ignored.
 * @param rEvent - Cooked event - we access that indirectly via
 *                 the tree parameters.
 * @param rAnalyzer - analyzer rerference (ignored)
 * @param rDecodere - Buffer decoder (ignored).
 *
 * @return Bool_t kfTRUE alwyas.
 */
Bool_t
CTOF::operator()(const Address_t pEvent, CEvent& rEvent,
		 CAnalyzer& rAnalyzer, CBufferDecoder& rDecoder)
{
  bindParameters();		// If necessary bind m_Anodes to parameters.

  double_t left, right;		// Left/right values.
  int      nLeft(0), nRight(0);	// Number of left/right values.

  for (int i = 0; i < 5; i++) {

    // Lefts are 0-4 and 10-14:

    if (m_Anodes[i].isValid()) {
      left = m_Anodes[i];
      nLeft++;
    }
    if (m_Anodes[i+10].isValid()) {
      left = m_Anodes[i+10];
      nLeft++;
    }

    // Rights are 5-9 and 15-19:

    if (m_Anodes[i+5].isValid()) {
      right = m_Anodes[i+5];
      nRight++;
    }
    if (m_Anodes[i+15].isValid()) {
      right = m_Anodes[i + 15];
      nRight++;
    }

  }
  // Compute TOF if there's exactly 1 left and 1 right.

  if ((nRight == 1) && (nLeft == 1)) {
    m_Tof = right - left;
  }

  return kfTRUE;
}
/*-----------------------------------------------------
** private methods
*/

/**
 * bind m_Anodes to the underlying tree parameters:
 */

void
CTOF::bindParameters()
{
  if (!m_initialized) {
    m_initialized = true;
    for (int i = 0; i < 20; i++) {
      char parameterName[1000];	// Should be big enough.
      sprintf(parameterName, "anodeT.%02d.0", i);
      m_Anodes[i].Initialize(parameterName);
    }
    CTreeParameter::BindParameters(); // bind to previously existing params.
    
  }
}
