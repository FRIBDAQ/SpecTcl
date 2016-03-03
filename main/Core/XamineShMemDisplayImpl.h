#ifndef XAMINESHMEMDISPLAYIMPL_H
#define XAMINESHMEMDISPLAYIMPL_H

#include "Display.h"

#include <Xamine.h>


#include <histotypes.h>
#include "XamineSpectrumIterator.h"
#include "DisplayFactory.h"

#include <string>
#include <memory>

class CXamineGate;
class CXamineGates;		// Forward reference.
class CXamineEvent;
class CXamineSpectrum;
class CXamineButton;
class CXamineSharedMemory;
class CSpectrum;

/*!
 * \brief The CXamineShMemDisplayImpl class
 *
 * Both CXamine and CSpectraLocalDisplay need to do just about the same thing to
 * implement the CDisplay interface in terms of a CXamineSharedMemory.
 * Basically, they need to translate between SpecTcl-side information and
 * Xamine shared memory information. A lot of this is translation between
 * a SpecTcl-side object and the slot it is associated  with in shared
 * memory. In any case, this implementation class exists so that the CXamine
 * and CSpectraLocalDisplay classes can delegate to an object that implements the
 * common logic.
 *
 */
class CXamineShMemDisplayImpl
{
    std::shared_ptr<CXamineSharedMemory> m_pMemory;
    std::vector<CSpectrum*>              m_boundSpectra;

public:
    /*!
     * \brief Constructor
     *
     * \param pMemory   shared memory to manipulate
     */
    CXamineShMemDisplayImpl(std::shared_ptr<CXamineSharedMemory> pMemory);

    /*!
     * \brief Shallow copy
     * \param rhs   object whose state will be copied
     */
    CXamineShMemDisplayImpl( const CXamineShMemDisplayImpl& rhs );

    /*!
     * Empty implementation
     */
    ~CXamineShMemDisplayImpl();

    /*!
     * \brief Obtain a pointer to the shared memory
     *
     *  The weak pointer means that the object can expire. It is essentially
     *  a non-owning reference to the object.
     *
     * \return  weak pointer to the shared memory
     */
    std::weak_ptr<CXamineSharedMemory> getSharedMemory() const { return m_pMemory; }

    // The following two methods delegate directly to m_pMemory
    void detach();
    void attach();

    /*!
     * \brief Add spectrum to shared memory
     *
     * \param rSpectrum  the spectrum to bind
     * \param rSorter    access to the dictionaries
     *
     * \throw  CDictionaryException - if spectrum of given name does not exist.
     * \throw  CErrnoException      - may be thrown by routines we call.
     *
     * The spectrum is added to the shared memory by the shared memory
     * addSpectrum method. Next the info is set using the createTitle of this
     * class to produce a very verbose informative string about the histogram.
     * The associated gates are then added to the shared memory as well as the
     * fits.
     */
    void addSpectrum(CSpectrum& rSpectrum, CHistogrammer& rSorter);
    void removeSpectrum(UInt_t nSpec, CSpectrum &rSpectrum);
    void removeSpectrum(CSpectrum &rSpectrum);
    SpectrumContainer getBoundSpectra() const;

    void addFit(CSpectrumFit& fit);
    void deleteFit(CSpectrumFit& fit);

    void updateStatistics();

    std::vector<CGateContainer> getAssociatedGates(const std::string& spectrumName,
                                                   CHistogrammer& rSorter);
    bool spectrumBound(CSpectrum *pSpectrum);

    DisplayBindings  getDisplayBindings() const;

    std::string createTitle(CSpectrum& rSpectrum,
                            UInt_t maxLength,
                            CHistogrammer &rSorter);
    UInt_t getTitleSize()  const;
    void setTitle(CSpectrum& rSpectrum, std::string name);
    void setTitle(std::string name, UInt_t slot);
    void setInfo(CSpectrum& rSpectrum, std::string name);
    void setInfo(std::string  name, UInt_t slot);

    // Spectrum statistics.

    void setOverflows(unsigned slot, unsigned x, unsigned y);
    void setUnderflows(unsigned slot, unsigned x, unsigned y);
    void clearStatistics(unsigned slot);

    void addGate (CSpectrum& rSpectrum, CGateContainer& rGate)  ;
    void removeGate(CSpectrum& rSpectrum, CGateContainer& rGate);
    void removeGate (UInt_t nSpectrum, UInt_t nId, GateType_t eType);
    CXamineGates* GetGates (UInt_t nSpectrum);

private:
    void ThrowGateStatus(Int_t nStatus, const CXamineGate& rGate,
                         const std::string& doing);
};

#endif // XAMINESHMEMDISPLAYIMPL_H
