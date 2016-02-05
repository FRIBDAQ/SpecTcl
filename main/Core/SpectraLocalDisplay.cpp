#include "SpectraLocalDisplay.h"
#include "SpectraProcess.h"

#include "XamineSharedMemory.h"
#include "Spectrum.h"

namespace Spectra
{

CSpectraLocalDisplay::CSpectraLocalDisplay(size_t nBytes)
    : m_pProcess(new CSpectraProcess),
      m_pMemory(new CXamineSharedMemory(nBytes))
{
}

CSpectraLocalDisplay::CSpectraLocalDisplay(const CSpectraLocalDisplay &rhs)
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

}

void CSpectraLocalDisplay::stopRESTServer()
{

}

SpectrumContainer CSpectraLocalDisplay::getBoundSpectra() const
{
}

void CSpectraLocalDisplay::addSpectrum(CSpectrum &rSpectrum, CHistogrammer &rSorter)
{
}

void CSpectraLocalDisplay::removeSpectrum(CSpectrum& rSpectrum)
{
}

bool CSpectraLocalDisplay::spectrumBound(CSpectrum* pSpectrum)
{
    return false;
}

void CSpectraLocalDisplay::addFit(CSpectrumFit &fit)
{
}

void CSpectraLocalDisplay::deleteFit(CSpectrumFit &fit)
{
}

void CSpectraLocalDisplay::addGate(CSpectrum &rSpectrum, CGateContainer &rGate)
{
}

void CSpectraLocalDisplay::removeGate(CSpectrum &rSpectrum, CGateContainer &rGate)
{
}

std::vector<CGateContainer> CSpectraLocalDisplay::getAssociatedGates(const std::string &spectrumName,
                                                                CHistogrammer &rSorter)
{
    // not used.
}

std::string CSpectraLocalDisplay::createTitle(CSpectrum &rSpectrum, UInt_t maxLength,
                                         CHistogrammer &rSorter)
{
    return rSpectrum.getName();
}

void CSpectraLocalDisplay::setTitle(CSpectrum &rSpectrum, std::string name)
{
}

void CSpectraLocalDisplay::setInfo(CSpectrum &rSpectrum, std::string name)
{
}

UInt_t CSpectraLocalDisplay::getTitleSize() const
{
    return 0;
}

void CSpectraLocalDisplay::updateStatistics()
{
}


} // end namespace Spectra
