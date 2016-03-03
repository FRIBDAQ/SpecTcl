#ifndef SPECTRALOCALDISPLAY_H
#define SPECTRALOCALDISPLAY_H

#include <Display.h>
#include <DisplayFactory.h>
#include <memory>

class SpecTcl;
class CXamineShMemDisplayImpl;
class CXamineSharedMemory;

namespace Spectra
{

// Forward declarations
class CSpectraProcess;

/*!
 * \brief The CSpectraLocalDisplay class
 *
 * This is the SpecTcl-side support for the local mode of Spectra. It is the Spectra
 * equivalent of CXamine for Xamine in that it supports the starting and stopping of the
 * child process and also handles the calls for manipulating what gets bound
 * to the display. This delegates just about all logic to two different data members:
 * the CXamineShMemDisplayImpl instance and the CSpectraProcess instance. The only thing
 * that this adds to the logic is that it can package require the rest server and start it up.
 */
class CSpectraLocalDisplay : public CDisplay
{

private:

    std::unique_ptr<CSpectraProcess>         m_pProcess;
    std::unique_ptr<CXamineShMemDisplayImpl> m_pMemory;
    SpecTcl&                                 m_rSpecTcl;

public:
    CSpectraLocalDisplay(std::shared_ptr<CXamineSharedMemory> pSharedMem, SpecTcl& pSpecTcl);
    CSpectraLocalDisplay(const CSpectraLocalDisplay& rhs);

    virtual ~CSpectraLocalDisplay();
    virtual int operator==(const CDisplay& rhs);

    virtual CSpectraLocalDisplay* clone() const;

    /*!
     * \brief Load and start the REST server in the main interpreter
     *
     *  This method executes the following tcl code in the
     *  main interpreter.
     *
     *  \code
     *  lappend auto_path @prefix@/TclLibs
     *  package require SpecTclHttpdServer
     *  startSpecTclHttpdServer [::SpecTcl::findFreePort 8080]
     *  \endcode
     *
     *  For that reason, the server will run on port 8080 unless it there is
     *  another process that port has been allocated to. The port nubmer will
     *  increment up until a free port is found. The resulting port should be printed on cout.
     */
    void startRESTServer();

    /*!
     * \brief Shutdown the server
     */
    void stopRESTServer();

    /*!
     * \brief Start the REST server, attach to shared mem, and execute the Spectra
     */
    virtual void start();

    /*!
     * \brief Kill Spectra, stop the rest server, detach from shared mem
     */
    virtual void stop();

    virtual bool isAlive();

    /*!
     * \brief Stop then start
     */
    virtual void restart();

    // The following 4 methods delegate directly to m_pMemory
    virtual SpectrumContainer getBoundSpectra() const;
    virtual void addSpectrum(CSpectrum &rSpectrum, CHistogrammer &rSorter);
    virtual void removeSpectrum(CSpectrum& rSpectrum);
    virtual bool spectrumBound(CSpectrum *pSpectrum);


    // The following six methods are no ops
    virtual void addFit(CSpectrumFit &fit);
    virtual void deleteFit(CSpectrumFit &fit);
    virtual void addGate(CSpectrum &rSpectrum, CGateContainer &rGate);
    virtual void removeGate(CSpectrum &rSpectrum, CGateContainer &rGate);
    virtual std::vector<CGateContainer> getAssociatedGates(const std::string &spectrumName,
                                                           CHistogrammer &rSorter);

    /*!
     * \brief Write the spectrum name to shared memory
     *
     * \param rSpectrum     spectrum in question
     * \param maxLength     max length of title
     * \param rSorter       access to dictionaries
     *
     * \return title
     */
    virtual std::string createTitle(CSpectrum &rSpectrum, UInt_t maxLength, CHistogrammer &rSorter);

    // The following 4 methods delegate to the m_pMemory
    virtual void setTitle(CSpectrum &rSpectrum, std::string name);
    virtual void setInfo(CSpectrum &rSpectrum, std::string name);
    virtual UInt_t getTitleSize() const;
    virtual void updateStatistics();


};

class CSpectraLocalDisplayCreator : public CDisplayCreator
{
private:
    std::shared_ptr<CXamineSharedMemory> m_pSharedMem;

public:
    CSpectraLocalDisplayCreator();

    void setSharedMemory(std::shared_ptr<CXamineSharedMemory> pShMem) { m_pSharedMem = pShMem; }

    CSpectraLocalDisplay* create();
};

}
#endif // SPECTRADISPLAY_H
