#include "SpectraLocalDisplay.h"
#include "SpectraProcess.h"

#include "XamineSharedMemory.h"
#include "XamineShMemDisplayImpl.h"
#include "Spectrum.h"
#include "SpecTcl.h"
#include "TCLInterpreter.h"

#include <memory>

namespace Spectra
{

CSpectraLocalDisplayCreator::CSpectraLocalDisplayCreator()
    : m_pSharedMem()
{}

CSpectraLocalDisplay* CSpectraLocalDisplayCreator::create()
{
    return new CSpectraLocalDisplay(m_pSharedMem, *SpecTcl::getInstance());
}



CSpectraLocalDisplay::CSpectraLocalDisplay(std::shared_ptr<CXamineSharedMemory> pSharedMem, SpecTcl &pSpecTcl)
    : m_pProcess(new CSpectraProcess),
      m_pMemory(),
      m_rSpecTcl(pSpecTcl)
{
    m_pMemory.reset(new CXamineShMemDisplayImpl(pSharedMem));
}

CSpectraLocalDisplay::CSpectraLocalDisplay(const CSpectraLocalDisplay &rhs)
    : m_pProcess(new CSpectraProcess),
      m_pMemory( new CXamineShMemDisplayImpl( *(rhs.m_pMemory))  ),
      m_rSpecTcl(rhs.m_rSpecTcl)
{
}

CSpectraLocalDisplay::~CSpectraLocalDisplay()
{
    // shared memory will clean up automatically because of unique_ptr
}

int CSpectraLocalDisplay::operator==(const CDisplay& rhs)
{
    return 0;
}

CSpectraLocalDisplay* CSpectraLocalDisplay::clone() const
{
    return new CSpectraLocalDisplay(*this);
}

void CSpectraLocalDisplay::start()
{

    startRESTServer();
    m_pMemory->attach();
    m_pProcess->exec();

}

void CSpectraLocalDisplay::stop()
{
    m_pProcess->kill();
    stopRESTServer();
    m_pMemory->detach();
}

bool CSpectraLocalDisplay::isAlive()
{
    return m_pProcess->isRunning();
}

void CSpectraLocalDisplay::restart()
{
    stop();
    start();
}

void CSpectraLocalDisplay::startRESTServer()
{
    CTCLInterpreter* pInterp = m_rSpecTcl.getInterpreter();

    auto resultStr = pInterp->GlobalEval("package require SpecTclHttpdServer");
    cout << resultStr << endl;
    resultStr = pInterp->GlobalEval("startSpecTclHttpdServer [findFreePort 8080]");
    cout << resultStr << endl;
}

void CSpectraLocalDisplay::stopRESTServer()
{
    CTCLInterpreter* pInterp = m_rSpecTcl.getInterpreter();

    auto resultStr = pInterp->GlobalEval("Httpd_ServerShutdown");
    cout << resultStr << endl;

}

SpectrumContainer CSpectraLocalDisplay::getBoundSpectra() const
{
    return m_pMemory->getBoundSpectra();
}

void CSpectraLocalDisplay::addSpectrum(CSpectrum &rSpectrum, CHistogrammer &rSorter)
{
    m_pMemory->addSpectrum(rSpectrum, rSorter);
}

void CSpectraLocalDisplay::removeSpectrum(CSpectrum& rSpectrum)
{
    m_pMemory->removeSpectrum(rSpectrum);
}

bool CSpectraLocalDisplay::spectrumBound(CSpectrum* pSpectrum)
{
    return m_pMemory->spectrumBound(pSpectrum);
}

void CSpectraLocalDisplay::addFit(CSpectrumFit &fit)
{
     // no op b/c we don't use it
}

void CSpectraLocalDisplay::deleteFit(CSpectrumFit &fit)
{
    // no op b/c we don't use it
}

void CSpectraLocalDisplay::addGate(CSpectrum &rSpectrum, CGateContainer &rGate)
{
    // no op b/c we don't use it
}

void CSpectraLocalDisplay::removeGate(CSpectrum &rSpectrum, CGateContainer &rGate)
{
    // no op b/c we don't use it
}

std::vector<CGateContainer> CSpectraLocalDisplay::getAssociatedGates(const std::string &spectrumName,
                                                                CHistogrammer &rSorter)
{
    // no op b/c we don't use it
}

std::string CSpectraLocalDisplay::createTitle(CSpectrum &rSpectrum, UInt_t maxLength,
                                         CHistogrammer &rSorter)
{
    return rSpectrum.getName();
}

void CSpectraLocalDisplay::setTitle(CSpectrum &rSpectrum, std::string name)
{
    m_pMemory->setTitle(rSpectrum, name);
}

void CSpectraLocalDisplay::setInfo(CSpectrum &rSpectrum, std::string name)
{
    m_pMemory->setInfo(rSpectrum, name);
}

UInt_t CSpectraLocalDisplay::getTitleSize() const
{
    return m_pMemory->getTitleSize();
}

void CSpectraLocalDisplay::updateStatistics()
{
    m_pMemory->updateStatistics();
}


} // end namespace Spectra
