#include "TestDisplay.h"
#include "GateContainer.h"
#include "Spectrum.h"
#include "CSpectrumFit.h"

#include <iostream>

using namespace std;


CTestDisplay* CTestDisplayCreator::create()
{
    return new CTestDisplay;
}



int CTestDisplay::m_nextFitlineId = 0;


CTestDisplay::CTestDisplay()
    : m_alive(false), m_DisplayBindings()
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

void CTestDisplay::Start() { m_alive = true;}
void CTestDisplay::Stop() { m_alive = false; }

// By definition, Test displays are alive. They never turn off.
Bool_t CTestDisplay::isAlive() { return m_alive;}
void CTestDisplay::Restart() { m_alive = true; }

UInt_t CTestDisplay::BindToDisplay(CSpectrum &rSpectrum, CHistogrammer&)
{
    m_DisplayBindings.push_back(rSpectrum.getName());
    m_boundSpectra.push_back(&rSpectrum);

    return m_DisplayBindings.size()-1;
}

void CTestDisplay::UnBindFromDisplay(UInt_t nSpec, CSpectrum &rSpectrum)
{
    m_DisplayBindings.at(nSpec) = "";
    m_boundSpectra.at(nSpec) = static_cast<CSpectrum*>(kpNULL);
}

void CTestDisplay::addFit(CSpectrumFit &fit) {
    // get the fit name and spectrum name... both of which we'll need to
    //   ensure we can add/bind the fit.

    string fitName      = fit.fitName();
    string spectrumName = fit.getName();
    Int_t  xSpectrumId  = FindDisplayBinding(spectrumName);
    if (xSpectrumId < 0) {
      // Display is not bound to Xamine.

      return;
    }
    // The display is bound... ensure that our fitlines binding array is large
    // enough.

    while (m_FitlineBindings.size() <=  xSpectrumId) {
      FitlineList empty;
      m_FitlineBindings.push_back(empty);
    }

    // Now we must:
    //  1. Allocate a fitline id.
    //  2. Enter the fit line in Xamine.
    //  3. Add the fitline name/id to our m_FitlineBindings

    int fitId = m_nextFitlineId++;
    pair <int, string> fitInfo(fitId, fitName);
    m_FitlineBindings[xSpectrumId].push_back(fitInfo);

}
void CTestDisplay::deleteFit(CSpectrumFit &fit) {
    string spectrumName =  fit.getName();
    string fitName      = fit.fitName();
    int    xSpectrumId  = FindDisplayBinding(spectrumName);
    if (xSpectrumId >= 0 && ( xSpectrumId <  m_FitlineBindings.size())) {

        // xSpectrumId < 0 means spectrum not bound.
        // xSpectrumId >= size of the bindings vector means no fitlines on spectrum.

        FitlineList::iterator i = m_FitlineBindings[xSpectrumId].begin();
        FitlineList::iterator e = m_FitlineBindings[xSpectrumId].end();
        while (i != e) {
            if (fitName == i->second) {
                // found it.. delete this one and return... don't delete all
                // occurences as
                // a. there's only supposed to be one occurence.
                // b. Depending on the underlying representation of a FitlineList,
                //    deletion may invalidate i.

                m_FitlineBindings[xSpectrumId].erase(i);
                return;
            }
            i++;
        }
        // Falling through here means no matching fit lines...which is a no-op.

    }
}

void CTestDisplay::updateStatistics() {}


std::vector<CGateContainer> CTestDisplay::GatesToDisplay(const std::string &rSpectrum)
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

DisplayBindings CTestDisplay::getDisplayBindings() const
{
    return m_DisplayBindings;
}

void CTestDisplay::setDisplayBindings(const DisplayBindings &am_DisplayBindings)
{
    m_DisplayBindings = am_DisplayBindings;
}

CSpectrum* CTestDisplay::DisplayBinding(UInt_t xid)
{
    // Returns  a pointer to a spectrum which is bound on a particular
    // xid.
    // Formal Parameters:
    //    UInt_t xid:
    //       The xid.
    // Returns:
    //     POinter to the spectrum or kpNULL Iff:
    //     xid is out of range.
    //     xid does not map to a spectrum.
    //
    if(xid >= DisplayBindingsSize())
      return (CSpectrum*)kpNULL;

    return m_boundSpectra[xid];
}

DisplayBindingsIterator CTestDisplay::DisplayBindingsBegin()
{
    return m_DisplayBindings.begin();
}

DisplayBindingsIterator CTestDisplay::DisplayBindingsEnd()
{
    return m_DisplayBindings.end();
}

UInt_t CTestDisplay::DisplayBindingsSize() const
{
    return m_DisplayBindings.size();
}

/*!
    Find the bindings for a spectrum by name.
   \param name  : string
       Name of the spectrum
   \return
   \retval -1   - Spectrum has no binding.
   \retval >= 0 - The binding index (xamine slot).

*/
Int_t CTestDisplay::FindDisplayBinding(std::string name) {

    for (int i = 0; i < DisplayBindingsSize(); i++) {
      if (name == m_DisplayBindings[i]) {
        return i;
      }
    }
    return -1;
}

std::string CTestDisplay::createTitle(CSpectrum& rSpectrum, UInt_t maxLength, CHistogrammer&)
{
    return rSpectrum.getName();
}

void CTestDisplay::setInfo(std::string name, UInt_t slot) {}
void CTestDisplay::setTitle(std::string name, UInt_t slot) {}
UInt_t CTestDisplay::getTitleSize() const { return 0; }

CDisplayGate* CTestDisplay::GateToDisplayGate(CSpectrum &rSpectrum, CGateContainer &rGate)
{
    return NULL;
}

void CTestDisplay::EnterGate(CDisplayGate &rGate) {}
void CTestDisplay::RemoveGate(UInt_t nSpectrum, UInt_t nId, GateType_t eType) {}

void CTestDisplay::setOverflows(unsigned slot, unsigned x, unsigned y) {}
void CTestDisplay::setUnderflows(unsigned slot, unsigned x, unsigned y) {}



