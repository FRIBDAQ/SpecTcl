#include "XamineShMemDisplayImpl.h"
#include "XamineGates.h"
#include "ErrnoException.h"
#include "RangeError.h"
#include "XamineGateException.h"
#include "XamineButtonException.h"
#include "XamineEvent.h"
#include "XamineSpectrum.h"
#include "Xamine1D.h"
#include "Xamine2D.h"
#include "XamineSpectrumIterator.h"
#include "XamineNoPrompt.h"
#include "XamineConfirmPrompt.h"
#include "XamineFilePrompt.h"
#include "XaminePointsPrompt.h"
#include "XamineTextPrompt.h"
#include "XamineSpectrumPrompt.h"
#include <XamineGate.h>
#include "XamineSharedMemory.h"
#include "XamineGateFactory.h"

#include <Display.h>
#include <Histogrammer.h>
#include <Dictionary.h>
#include <DictionaryException.h>
#include <CFitDictionary.h>
#include <Histogrammer.h>
#include <CSpectrumFit.h>
#include <XamineGates.h>
#include <PointlistGate.h>
#include <Cut.h>
#include <Gamma2DW.h>
#include <GateMediator.h>
#include "Spectrum.h"

#include <string>
#include <stdexcept>
#include <assert.h>
using namespace std;


CXamineShMemDisplayImpl::CXamineShMemDisplayImpl(UInt_t nBytes)
    : m_pMemory()
{
    try {
        m_pMemory = new CXamineSharedMemory(nBytes);
    } catch (CErrnoException& exc) {
        cerr << "Caught errno exception: " << exc.ReasonText()
             << " while: " << exc.WasDoing() << endl;
        exit(errno);
    }
}

CXamineShMemDisplayImpl::~CXamineShMemDisplayImpl()
{
    delete m_pMemory;
}

void CXamineShMemDisplayImpl::attach()
{
    m_pMemory->attach();
}

void CXamineShMemDisplayImpl::detach()
{
    m_pMemory->detach();
}

//////////////////////////////////////////////////////////////////////////
//
//  Function:
//    void EnterGate (CSpectrum& rSpectrum, CGateContainer& rGate )
//  Operation Type:
//     mutator
//
void
CXamineShMemDisplayImpl::addGate(CSpectrum &rSpectrum, CGateContainer &rGate)
{
    CXamineGateFactory factory(m_pMemory);
    CXamineGate* pDisplayed = factory.fromSpecTclGate(rSpectrum, rGate);
    if (pDisplayed)
        m_pMemory->addGate(*pDisplayed);
    delete pDisplayed;
}

void
CXamineShMemDisplayImpl::removeGate(CSpectrum& rSpectrum, CGateContainer& rGate)
{
    m_pMemory->removeGate(rSpectrum, rGate);
}

//////////////////////////////////////////////////////////////////////////
//
//  Function:
//    void RemoveGate ( UInt_t nSpectrum, UInt_t nId, GateType_t eType )
//  Operation Type:
//     Mutator
//
void
CXamineShMemDisplayImpl::removeGate(UInt_t nSpectrum, UInt_t nId, GateType_t eType)
{
// Removes a specified gate from the spectrum.
// The gate is defined by its spectrum, id, and type.
//
// Formal Parameters:
//      UInt_t nSpectrum:
//           Number of spectrum from which to remove gate.
//      UInt_t nId:
//            Identification of the spectrum.
//      GateType_t eType:
//            Type of the gate to remove (See EnterGate for
//             details).
//
   m_pMemory->removeGate(nSpectrum, nId, eType);
}

//////////////////////////////////////////////////////////////////////////
//
//  Function:
//    CXamineShMemDisplayImplShMemDisplayImplGates GetGates ( UInt_t nSpectrum )
//  Operation Type:
//     Selector
//
CXamineGates*
CXamineShMemDisplayImpl::GetGates(UInt_t nSpectrum)
{
// Retrieves a set of gates from Xamine.
// Copies of the gates described are retrieved into
//  an XamineGates object.  From there, iterators
// can be created to step through the gates,
// or the gate set can be directly indexed as desired
// (the XamineGates object looks a lot like a
// readonly STL vector of gate description objects.
//
// Formal Parameters:
//     UInt_t nSpectrum:
//            The spectrum for which
//            to retrieve the gate.
// Returns:
//    CXamineGates* - gate object ptr to dynamically allocated gate list.
//
    return m_pMemory->GetGates(nSpectrum);
}

/**
 * setOverflows
 *    Set overflow statistics for a spectrum.
 * @param slot - Which slot in Xamine.
 * @param x    - X overflow counts.
 * @param y    - Y overflow couts.
 */
void
CXamineShMemDisplayImpl::setOverflows(unsigned slot, unsigned x, unsigned y)
{
    m_pMemory->setOverflows(slot, x, y);
}
/**
 * setUnderflows
 *    Same as above but the underflow counters get set.
 */
void
CXamineShMemDisplayImpl::setUnderflows(unsigned slot, unsigned x, unsigned y)
{
    m_pMemory->setUnderflows(slot, x, y);
}
/**
 * clearStatistics
 *    Clear the over/underflow statistics associated witha bound spectrum.
 */
void
CXamineShMemDisplayImpl::clearStatistics(unsigned slot)
{
    m_pMemory->clearStatistics(slot);
}

std::string
CXamineShMemDisplayImpl::createTitle(CSpectrum& rSpectrum,
                        UInt_t maxLength,
                        CHistogrammer &rSorter)
{
return std::string();
}

/*!
    Return the size of the spectrum title string.
*/
UInt_t
CXamineShMemDisplayImpl::getTitleSize() const
{
    m_pMemory->getTitleSize();
}

/*!
   Set the title of a specific slot in Xamine memory.
   The title will be truncated to the size of the spec_title if
   necessary
*/
void
CXamineShMemDisplayImpl::setTitle(CSpectrum& rSpectrum, string name)
{
    Int_t slot = m_pMemory->findDisplayBinding(rSpectrum);
    if (slot == -1) {
        m_pMemory->setTitle(name, slot);
    } else {
        throw std::runtime_error("CXamineShMemDisplayImpl::SetTitle() Cannot set title for unbound spectrum.");
    }
}

/*!
   Set the title of a specific slot in Xamine memory.
   The title will be truncated to the size of the spec_title if
   necessary
*/
void
CXamineShMemDisplayImpl::setTitle(string name, UInt_t slot)
{
  m_pMemory->setTitle(name, slot);
}

void
CXamineShMemDisplayImpl::setInfo(CSpectrum &rSpectrum, std::string name)
{
    m_pMemory->setInfo(rSpectrum, name);
}

/*!
  Set the info string of a specific slot in xamine memory.
  the info string will be truncated to spec_title size if needed.
*/
void
CXamineShMemDisplayImpl::setInfo(string info, UInt_t slot)
{
    m_pMemory->setInfo(info, slot);
}

/*!
    Binds a spectrum to the display.
    The first free spectrum will be allocated.
   If the spectrum is already bound, then we short circuit and
   just return the current binding number:
   This is not an error, just a user mistake we can handle.


    \param sName : const std::string
             Name of the spectrum to bind

   \throw   CDictionaryException - if spectrum of given name does not exist.
   \throw  CErrnoException      - may be thrown by routines we call.


  */
void CXamineShMemDisplayImpl::addSpectrum(CSpectrum &rSpectrum, CHistogrammer &rSorter)
{
    // allocate the shared memory slot and swap out the storage for the spectrum
    UInt_t slot = m_pMemory->addSpectrum(rSpectrum, rSorter);
    if (m_boundSpectra.size() <= slot) {
        m_boundSpectra.resize(slot+1);
    }
    m_boundSpectra.at(slot) = &rSpectrum;

    // set the title
    string title = createTitle(rSpectrum, m_pMemory->getTitleSize(), rSorter);
    m_pMemory->setInfo(title, slot);

    // We must locate all of the gates which are relevant to this spectrum
    // and enter them as well:
    //
    vector<CGateContainer> DisplayGates = getAssociatedGates(rSpectrum.getName(), rSorter);

    UInt_t Size = DisplayGates.size();
    for(UInt_t i = 0; i < DisplayGates.size(); i++) {
        CXamineGateFactory factory(m_pMemory);
        CXamineGate* pXgate = factory.fromSpecTclGate(rSpectrum, DisplayGates[i]);
        if(pXgate) m_pMemory->addGate(*pXgate);
        delete pXgate;
    }

    // same for the fitlines:
    //
    CFitDictionary& dict(CFitDictionary::getInstance());
    CFitDictionary::iterator pf = dict.begin();

    while (pf != dict.end()) {
        CSpectrumFit* pFit = pf->second;
        if (pFit->getName() == rSpectrum.getName()) {
            m_pMemory->addFit(*pFit);		// not very efficient, but doesn't need to be
        }
        pf++;
    }

}

void CXamineShMemDisplayImpl::removeSpectrum(CSpectrum &rSpectrum)
{
    Int_t slot = m_pMemory->findDisplayBinding(rSpectrum);
    if (slot >=0) {
        m_pMemory->removeSpectrum(slot, rSpectrum);
        m_boundSpectra.at(slot) = NULL;
    }
}

//////////////////////////////////////////////////////////////////////////
//
//  Function:
//    void UnBindFromDisplay ( UInt_t nSpec )
//  Operation Type:
//     mutator
//
void CXamineShMemDisplayImpl::removeSpectrum(UInt_t nSpec, CSpectrum& rSpectrum) {
  // Unbinds the spectrum which is
  // attached to the specified Displayer spectrum number.
  //
  // Formal Parameters:
  //    UInt_t nSpec:
  //       Display spectrum id to unbind.

    m_pMemory->removeSpectrum(nSpec, rSpectrum);
}

/*!
   addFit : adds a fit to the Xamine bindings.  We keep track of
   these fits in m_fitlineBindings.  This is a vector of lists.
   The index of each vector element is the Xamine 'display slot' fitlines
   are bound to. Each element is a list of pairs.  Each pair is the fitline
   id and fitline name.
   \param fit : CSpectrumFit&
     Reference to the fit to add.
*/
void
CXamineShMemDisplayImpl::addFit(CSpectrumFit& fit)
{
    // get the fit name and spectrum name... both of which we'll need to
    //   ensure we can add/bind the fit.

    m_pMemory->addFit(fit);

}
/*!
  Remove a fit.  It is  a no-op to delete a fit that does not exist or is
  on an unbound spectrum. The rough cut of what we will do is
  - Locate the spectrum id of the binding.
  - Locate any fit that matches the name of the fit we are given
    in the fit bindings list assocated with that spectrum.
  - Ask Xamine to delete that fit (fits are like gates).
  - Remove this fit from our bindings list.
  \param fit : CSpectrumFit&
     referenced to the fit to remove.
*/
void
CXamineShMemDisplayImpl::deleteFit(CSpectrumFit& fit)
{
    m_pMemory->deleteFit(fit);
}

/**
 * updateStatistics
 *    Update the Xamine statistics for each bound spectrum.
 */
void
CXamineShMemDisplayImpl::updateStatistics()
{
    SpectrumContainer spectra = getBoundSpectra();
    for (int i =0; i < spectra.size(); i++) {
        CSpectrum* pSpec = spectra[i];
        if (pSpec) {
            std::vector<unsigned> stats = pSpec->getUnderflows();
            m_pMemory->setUnderflows(i, stats[0], (stats.size() == 2 ? stats[1] : 0));
            stats = pSpec->getOverflows();
            m_pMemory->setOverflows(i, stats[0], (stats.size() == 2 ? stats[1] : 0));
        }
    }
}

/////////////////////////////////////////////////////////////////////////////
//
//  Function:
//    std::vector<CGateContainer> GatesToDisplay(const std::string& rSpectrum)
// Operation Type:
//    Protected utility.
//
std::vector<CGateContainer>
CXamineShMemDisplayImpl::getAssociatedGates(const std::string& spectrumName, CHistogrammer &rSorter)
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

    return m_pMemory->getAssociatedGates(spectrumName, rSorter);
}

SpectrumContainer CXamineShMemDisplayImpl::getBoundSpectra() const
{
    return m_boundSpectra;
}

DisplayBindings CXamineShMemDisplayImpl::getDisplayBindings() const
{
    return m_pMemory->getDisplayBindings();
}

//////////////////////////////////////////////////////////////////////
//
// Function:
//      CSpectrum*  DisplayBinding(UInt_t xid)
// Operation type:
//      Selector.
//
CSpectrum* CXamineShMemDisplayImpl::getSpectrum(UInt_t xid) {
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
  if(xid >= m_pMemory->displayBindingsSize())
    return (CSpectrum*)kpNULL;

  return m_boundSpectra[xid];
}


/*!
 *
 */
bool CXamineShMemDisplayImpl::spectrumBound(CSpectrum* pSpectrum)
{
    return (m_pMemory->findDisplayBinding(pSpectrum->getName()) >= 0);
}

//////////////////////////////////////////////////////////////////////////
//
// Function:
//   ThrowGateStatus(Int_t nStatus, const CDisplayGate& rGate,
//                   const std::string& doing)
// Operation Type:
//    Protected utility.
//
void
CXamineShMemDisplayImpl::ThrowGateStatus(Int_t nStatus, const CXamineGate& rGate,
             const std::string& doing)
{
  // Maps Xamine gate maniplation status into either the appropriate
  // exception or normal return.
  //
  // The following possible cases:
  //    nStatus >= 0:         success.
  //    nStatus in [-1..-6] - Throw CXamineGateException
  //    nStatus == CheckErrno       - Throw CErrnoException
  //

  if(nStatus == CheckErrno) {
    throw CErrnoException(doing);
  }
  if(nStatus < 0) {
    throw CXamineGateException(nStatus, rGate,
                   doing);
  }
  // Normal completion.

  assert(nStatus >= 0);		// In case status allocations change.

}


