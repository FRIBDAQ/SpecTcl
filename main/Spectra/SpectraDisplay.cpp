#include "SpectraDisplay.h"

namespace Spectra
{

CSpectraDisplay::CSpectraDisplay()
{
}

CSpectraDisplay::CSpectraDisplay(const CSpectraDisplay &rhs)
{

}

CSpectraDisplay::~CSpectraDisplay()
{
}

int CSpectraDisplay::operator==(const CDisplay& rhs)
{
    return 0;
}

CSpectraDisplay* CSpectraDisplay::clone() const
{
    return new CSpectraDisplay(*this);
}

void CSpectraDisplay::start()
{
//    startRESTServer();


}

void CSpectraDisplay::stop()
{
}

bool CSpectraDisplay::isAlive()
{
    return false;
}

void CSpectraDisplay::restart()
{
}

SpectrumContainer CSpectraDisplay::getBoundSpectra() const
{
}

void CSpectraDisplay::addSpectrum(CSpectrum &rSpectrum, CHistogrammer &rSorter)
{
}

void CSpectraDisplay::removeSpectrum(CSpectrum& rSpectrum)
{
}

bool CSpectraDisplay::spectrumBound(CSpectrum* pSpectrum)
{
    return false;
}

void CSpectraDisplay::addFit(CSpectrumFit &fit)
{
}

void CSpectraDisplay::deleteFit(CSpectrumFit &fit)
{
}

void CSpectraDisplay::addGate(CSpectrum &rSpectrum, CGateContainer &rGate)
{
}

void CSpectraDisplay::removeGate(CSpectrum &rSpectrum, CGateContainer &rGate)
{
}

std::vector<CGateContainer> CSpectraDisplay::getAssociatedGates(const std::string &spectrumName,
                                                                CHistogrammer &rSorter)
{
}

std::string CSpectraDisplay::createTitle(CSpectrum &rSpectrum, UInt_t maxLength,
                                         CHistogrammer &rSorter)
{
}

void CSpectraDisplay::setTitle(CSpectrum &rSpectrum, std::string name)
{
}

void CSpectraDisplay::setInfo(CSpectrum &rSpectrum, std::string name)
{
}

UInt_t CSpectraDisplay::getTitleSize() const
{
    return 0;
}

void CSpectraDisplay::updateStatistics()
{
}


} // end namespace Spectra
