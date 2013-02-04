/**
 * Implements CAngle -- computing angles from
 * CHICO II cathode times.  See CAngle.h for a full(er?) discussion.
 */

#include "CAngle.h"
#include <stdio.h>

/**
 * OnAttach
 *   Called once when the event processor is attached to the
 *   event processing pipeline
 *   - set m_initialized false (have not bound the cathode times).
 *   - Initialize the m_Front/BackTheta and m_Front/BackPhi tree parameters.
 *     these are not CTreeParameterArrays so that we have better
 *     control over their element names.
 *
 * @param rAnalyzer - references the analyzer that contains
 *                    the analysis pipeline (not needed).
 * 
 * @return Bool_t kfTRUE unconditionally indicating success.
 */
Bool_t
CAngle::OnAttach(CAnalyzer& rAnalyzer)
{
  m_initialized = false;
  
  for (int i = 0; i < 10; i++) {
    char parameterName[1000];
    sprintf(parameterName, "Front.Theta-%d", i);
    m_FrontTheta[i].Initialize(parameterName, 1024,
			       -100.0, 100.0, "ns");

    sprintf(parameterName, "Front.Phi-%d", i);
    m_FrontPhi[i].Initialize(parameterName, 1024,
			     -100.0, 100.0, "ns");

    sprintf(parameterName, "Back.Theta-%d", i);
    m_BackTheta[i].Initialize(parameterName, 1024,
			      -100.0, 100.0, "ns");

    sprintf(parameterName, "Back.Phi-%d", i);
    m_BackPhi[i].Initialize(parameterName, 1024,
			    -100.0, 100.0, "ns");

    
  }
}
/**
 * operator()
 *
 * Called for every event.  This event processor only creates pseudo
 * parameters.  As such it will only access existing unpacked parameter.
 * The unpacked parameters it references will also be via arrays of
 * tree parameters (m_FrontRaw and m_BackRaw).
 *
 * @param pEvent - pointer to the raw event (not needed).
 * @param rEvent - Reference to the unpacked events (accessed
 *                 indirectly via the tree parameters.
 * @param rAnalyzer- References the analyzer (not needed)
 * @param rDecoder - References the buffer decoder (not needed).
 *
 * @return Bool_t - kfTRUE unconditionally.
 */
Bool_t
CAngle::operator()(const Address_t pEvent, CEvent& rEvent,
		   CAnalyzer& rAnalyzer, CBufferDecoder& rDecoder)
{
  // If necessary bind the frong/back arrays to parameters;

  bindParameters();

  // For each pair that's present compute the results.

  for (int i = 0; i < 10; i++) {
    int c0 = i*4;		// index of first parameter we care about.
    
    // Front theta:

    if (m_FrontRaw[c0].isValid() && m_FrontRaw[c0+1].isValid()) {
      m_FrontTheta[i] = m_FrontRaw[c0] - m_FrontRaw[c0+1];
    }
    // Front phi:

    if (m_FrontRaw[c0+2].isValid() && m_FrontRaw[c0+3].isValid()) {
      m_FrontPhi[i] = m_FrontRaw[c0+2] - m_FrontRaw[c0+3];
    }

    // Back theta:

    if (m_BackRaw[c0].isValid() && m_BackRaw[c0+1].isValid()) {
      m_BackTheta[i] = m_BackRaw[c0+1] - m_BackRaw[c0];
    }
    // Front phi:

    if (m_BackRaw[c0+2].isValid() && m_BackRaw[c0+3].isValid()) {
      m_BackPhi[i] = m_BackRaw[c0+3] - m_BackRaw[c0+2];
    }

  }

  return kfTRUE;
}
/*--------------------------------------------------------------
 *
 *  Private utility method(s)
 */

/** 
 * bindParameters - if the raw (existing) parameters have not yet
 *                  been bound bind them at this time.
 *
 */
void
CAngle::bindParameters()
{
  if (!m_initialized) {
    m_initialized = true;

    for(int i = 0; i < 40; i++) {
      int iF = i;		// Front
      int ib = i+40;		// Back
      char name[1000];

      // Front:

      sprintf(name, "cathodeT.%02d.0", iF);
      m_FrontRaw[i].Initialize(name);

      // Back:

      sprintf(name, "cathodeT.%02d.0", ib);
      m_BackRaw[i].Initialize(name);


    }
    CTreeParameter::BindParameters(); // Bind to underlying unpacked params
  }
}
