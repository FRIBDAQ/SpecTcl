#include "SpectraDisplay.h"

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

UInt_t CSpectraDisplay::addSpectrum(CSpectrum &rSpectrum, CHistogrammer &rSorter)
{
}

void CSpectraDisplay::removeSpectrum(UInt_t nSpec, CSpectrum &rSpectrum)
{
}

void CSpectraDisplay::removeSpectrum(CSpectrum& rSpectrum)
{
}

CSpectrum* CSpectraDisplay::getSpectrum(UInt_t xid)
{
    return NULL;
}

bool CSpectraDisplay::spectrumBound(CSpectrum &rSpectrum)
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

UInt_t CSpectraDisplay::DisplayBindingsSize() const
{
}

std::string CSpectraDisplay::createTitle(CSpectrum &rSpectrum, UInt_t maxLength, CHistogrammer &rSorter)
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

void CSpectraDisplay::setOverflows(unsigned slot, unsigned x, unsigned y)
{
}

void CSpectraDisplay::setUnderflows(unsigned slot, unsigned x, unsigned y)
{
}


