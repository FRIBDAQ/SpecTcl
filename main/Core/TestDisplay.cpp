#include "TestDisplay.h"
#include "GateContainer.h"
#include "Spectrum.h"
#include "CSpectrumFit.h"

#include <iostream>
#include <algorithm>
#include <iterator>

using namespace std;


CTestDisplay* CTestDisplayCreator::create()
{
    return new CTestDisplay;
}


CTestDisplay::CTestDisplay()
    : m_alive(false), m_boundSpectra()
{
}

CTestDisplay::~CTestDisplay()
{
}

CTestDisplay* CTestDisplay::clone() const
{
    return new CTestDisplay(*this);
}

// All Test displays are identical by definition.
int CTestDisplay::operator==(const CDisplay& disp)
{ return 1; }

void CTestDisplay::start() { m_alive = true;}
void CTestDisplay::stop() { m_alive = false; }

// By definition, Test displays are alive. They never turn off.
bool CTestDisplay::isAlive() { return m_alive;}
void CTestDisplay::restart() { m_alive = true; }

void CTestDisplay::addSpectrum(CSpectrum &rSpectrum, CHistogrammer&)
{
    m_boundSpectra.insert(&rSpectrum);
}

void CTestDisplay::removeSpectrum(CSpectrum &rSpectrum)
{
    m_boundSpectra.erase(&rSpectrum);
}

void CTestDisplay::addFit(CSpectrumFit &fit) {

    m_fits.insert(&fit);
}

void CTestDisplay::deleteFit(CSpectrumFit &fit) {
    m_fits.erase(&fit);
}

void CTestDisplay::updateStatistics() {}


std::vector<CGateContainer> CTestDisplay::getAssociatedGates(const std::string &rSpectrum,
                                                         CHistogrammer &rSorter)
{

      // Returns a vector of gates which can be displayed on the spectrum.
      // Gates are considered displayable on a spectrum iff the gate parameter set
      // is the same as the spectrum parameter set. If the spectrum is a gamma
      // spectrum, then the gate is only displayed on one spectrum. Note that
      // displayable gates at present are one of the following types only:
      //   Contour (type = 'c')
      //   Band    (type = 'b')
      //   Cut     (type = 's')
      //   Sum2d   {type = 'm2'}
      //   GammaContour   (type = 'gc')
      //   GammaBand      (type = 'gb')
      //   GammaCut       (type = 'gs')
      // All other gates are not displayable.
      //

      std::vector<CGateContainer> vGates;
    //  CSpectrum *pSpec = m_pSorter->FindSpectrum(rSpectrum);
    //  if(!pSpec) {
    //    throw CDictionaryException(CDictionaryException::knNoSuchKey,
    //                   "No such spectrum CXamine::GatesToDisplay",
    //                   rSpectrum);
    //  }
    //  //
    //  // The mediator tells us whether the spectrum can display the gate:
    //  //
    //  CGateDictionaryIterator pGate = m_pSorter->GateBegin();
    //  while(pGate != m_pSorter->GateEnd()) {
    //    CGateMediator DisplayableGate(((*pGate).second), pSpec);
    //    if(DisplayableGate()) {
    //      vGates.push_back((*pGate).second);
    //    }
    //    pGate++;
    //  }

      return vGates;
}

SpectrumContainer CTestDisplay::getBoundSpectra() const
{
//    return m_boundSpectra;
    SpectrumContainer container;
    for (auto pSpec : m_boundSpectra) {
        container.push_back(pSpec);
    }
    return container;
}

/*!
    Test that the spectrum is bound to the display
   \param rSpectrum  : CSpectrum&
       The spectrum
   \return boolean
   \retval false   - Spectrum has no binding.
   \retval true    - Spectrum has binding

*/
bool CTestDisplay::spectrumBound(CSpectrum* pSpectrum) {
    auto found = m_boundSpectra.find(pSpectrum);
    return (found != m_boundSpectra.end());
}


std::string CTestDisplay::createTitle(CSpectrum& rSpectrum, UInt_t maxLength, CHistogrammer&)
{
    return rSpectrum.getName();
}

void CTestDisplay::setInfo(CSpectrum &rSpectrum, string name) {}
void CTestDisplay::setTitle(CSpectrum &rSpectrum, string name) {}
UInt_t CTestDisplay::getTitleSize() const { return 0; }

void CTestDisplay::addGate(CSpectrum &rSpectrum, CGateContainer &rGate) {}
void CTestDisplay::removeGate(CSpectrum &rSpectrum, CGateContainer& rGate) {}


