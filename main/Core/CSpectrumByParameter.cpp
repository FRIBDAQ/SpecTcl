/*
    This software is Copyright by the Board of Trustees of Michigan
    State University (c) Copyright 2005.

    You may use this software under the terms of the GNU public license
    (GPL).  The terms of this license are described at:

     http://www.gnu.org/licenses/gpl.txt

     Author:
             Ron Fox
	     NSCL
	     Michigan State University
	     East Lansing, MI 48824-1321
*/
#include <config.h>
#include "CSpectrumByParameter.h"
#include "Spectrum.h"

#include <limits.h>

 
using namespace std;

/*!
   Construct an object from whole cloth.
*/
CSpectrumByParameter::CSpectrumByParameter()
{
}
/*!
  Destruction is going to require that we destroy all the lists
*/
CSpectrumByParameter::~CSpectrumByParameter()
{
  for (int i =0; i < m_spectraByParameter.size(); i++) {
    delete m_spectraByParameter[i];
  }

}
/*!
  Copy construction:
  We'll use cloneIn to get the job done.
*/
CSpectrumByParameter::CSpectrumByParameter(const CSpectrumByParameter& rhs)
{
  cloneIn(rhs);
}
/*!
  Assignment is also basically a cloneIn:
*/
CSpectrumByParameter&
CSpectrumByParameter::operator=(const CSpectrumByParameter& rhs)
{
  if (this != &rhs) {
    cloneIn(rhs);
  }
  return *this;
}

/*!
   Return a list of spectra that require a specific parameter
   \param parameterId - Id of the parameter required.

   \return CSpectrum**
   \retval NULL - There have never been spectra tha require parameterId.
   \retval other- A pointer to a null terminate list of CSpectra* which in
                  turn point to the spectra that require parameterId.
                  Note that if there were once spectra requiring parameterId
                 and they have all been deleted, this list could be a single NULL.
*/
CSpectrum**
CSpectrumByParameter::getSpectrumList(int parameterId)
{


  if (m_spectraByParameter.size() <= parameterId) {
    return reinterpret_cast<CSpectrum**>(NULL);
  }
  if (m_spectraByParameter[parameterId]) {
    return m_spectraByParameter[parameterId]->getList();
  } 
  else {
    return reinterpret_cast<CSpectrum**>(NULL);
  }
}
/*!
  Return a list of spectra that have no requirements for parameters.

  See the return specification above, but replace require parameterId
  with don't require a parameter.
*/
CSpectrum**
CSpectrumByParameter::getSpectrumList()
{
  return m_remainingSpectra.getList();

}

/*!
  Called to increment the spectra.
  \param event - reference to the event to use to increment the spectra.

*/
void
CSpectrumByParameter::operator()(const CEvent& event)
{

  CEvent& e(const_cast<CEvent&>(event));
  // We have to always do the misc group:

  CSpectrum** miscList = m_remainingSpectra.getList();
  incrementList(event, miscList);

  // We have to do all lists for which a parameter is present.. until
  // either we run out of parameters or run out of spectrumlists.


  DopeVector& dope = e.getDopeVector();
  unsigned int numParameters = dope.size();

  for (int i =0; i < numParameters; i++) {
    unsigned int parameterNumber = dope[i];
    if (parameterNumber < m_spectraByParameter.size()) {
      SpectrumList *sList = m_spectraByParameter[parameterNumber];
      if (sList) {
	CSpectrum** aList = sList->getList();
	incrementList(event, aList);
      }
    }
  }
  

}

/*!
  Called when a spectrum is added to the histogrammer. 
  - If the spectrum does not need a parameter it is added to the m_remainingSpectra
    list.
  - If the spectrum does need a parameter, the first parameter is extracted,
    then the spectrum is added to the appropriate m_spectraByParameter list..making
    new lists if needed using addToParameter.

    \param name      - Name of the new spectrum.
    \param spectrum  - Reference to the spectrum pointer to save.
*/

void CSpectrumByParameter::onAdd(std::string name, CSpectrum*& spectrum)
{
  if (!spectrum->needParameter()) {
    m_remainingSpectra.add(spectrum);
  }
  else {
    vector<UInt_t> parameterIds;


    spectrum->GetParameterIds(parameterIds);
    // If any parameter ids are UINT_MAX this spectrum is not incrementable
    // (must be a spectrum read in that was not re-connected).

    for (int i=0; i < parameterIds.size(); i++) {
      if (parameterIds[i] == UINT_MAX) return;
    }

    addToParameter(spectrum, parameterIds[0]);
  }
}
/*!
  Called when a spectrum has been removed from the histogrammer.
  - If the spectrum does not need a parameter it is removed from m_remainingSpectra.
  - If the spectrum does need a parameter it is removed from the appropriate element of
    m_spectraByParameter.
  - If the list the spectrum is supposed to be removed from does not exist, the call is a no-op.
  - If the list the spectrum is supposed to be removed from exists, but does not have the
    spectrum, this is also a no-op.

    \param name      - Name of the new spectrum.
    \param spectrum  - Reference to the spectrum pointer to save.
*/
void CSpectrumByParameter::onRemove(std::string name, CSpectrum*& spectrum)
{

  if (spectrum->needParameter()) {
    vector<UInt_t> parameters;
    spectrum->GetParameterIds(parameters);

    // We're not going to be in any of these lists if we have a parameter id
    // that's UINT_MAX (see onAdd)

    for (int i=0; i < parameters.size(); i++) {
      if (parameters[i] == UINT_MAX) return;
    }

    UInt_t         param  = parameters[0];
    if ((param < m_spectraByParameter.size())  && m_spectraByParameter[param]) {
      m_spectraByParameter[param]->remove(spectrum);
    }
  }
  else {
    m_remainingSpectra.remove(spectrum);
  }

}



/////////////////////////////////////////////////////////////////////
/*
  Clone ourselves into this.
*/
void
CSpectrumByParameter::cloneIn(const CSpectrumByParameter& rhs)
{
  for (int i = 0; i < rhs.m_spectraByParameter.size(); i++) {
    SpectrumList* newList = reinterpret_cast<SpectrumList*>(NULL);
    if(rhs.m_spectraByParameter[i]) {
      newList = new SpectrumList(*m_spectraByParameter[i]);
    }
    m_spectraByParameter.push_back(newList);
  }
  m_remainingSpectra = rhs.m_remainingSpectra;
}


/*
   Add a new spectrum to the ones needed for a specific parameter.
   If needed, new spectrum lists are added, or the vector is expanded.

*/
void
CSpectrumByParameter::addToParameter(CSpectrum* spectrum, unsigned int parameterId)
{
  // extend m_spectraByParameter as needed:

  while(m_spectraByParameter.size() <= parameterId) {
    m_spectraByParameter.push_back(reinterpret_cast<SpectrumList*>(0));
  }

  // At this point we know there are enough vector elements...
  // if necessary, create a new list:

  if (!m_spectraByParameter[parameterId]) {
    m_spectraByParameter[parameterId] = new SpectrumList;
  }
  // Add the spectrum pointer:

  m_spectraByParameter[parameterId]->add(spectrum);
}
/*
   Increment the spectra in a list:
*/
void
CSpectrumByParameter::incrementList(const CEvent& event, CSpectrum** spectra)
{
  if(spectra) {
    while (*spectra) {
      (**spectra)(event);
      spectra++;
    }
  }
}
