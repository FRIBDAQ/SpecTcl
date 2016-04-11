//    This software is Copyright by the Board of Trustees of Michigan
//    State University (c) Copyright 2016.
//
//    This program is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
//    Authors:
//    Jeromy Tompkins
//    NSCL
//    Michigan State University
//    East Lansing, MI 48824-1321

#ifndef TESTDISPLAY_H
#define TESTDISPLAY_H

#include "Display.h"
#include "DisplayFactory.h"
#include <set>

class CSpectrum;

class CTestDisplay : public CDisplay
{
    bool                     m_alive;
    std::set<CSpectrum*>     m_boundSpectra;        // Spectrum if bound.
    std::set<CSpectrumFit*>  m_fits;

public:
    CTestDisplay();

    virtual ~CTestDisplay();

    virtual int operator==(const CDisplay&);

    virtual CTestDisplay* clone() const;

    virtual void start();
    virtual void stop();
    virtual bool isAlive();
    virtual void restart();

    SpectrumContainer getBoundSpectra() const;

    virtual void addSpectrum(CSpectrum& rSpectrum, CHistogrammer& rSorter);
    virtual void removeSpectrum(CSpectrum& rSpectrum, CHistogrammer& rSorter);

    virtual void addFit(CSpectrumFit& fit);
    virtual void deleteFit(CSpectrumFit& fit);

    virtual void updateStatistics();

    virtual std::vector<CGateContainer> getAssociatedGates(const std::string& rSpectrum,
                                                       CHistogrammer& rSorter);

    virtual bool spectrumBound(CSpectrum *pSpectrum);

    virtual std::string createTitle(CSpectrum& rSpectrum, UInt_t maxLength, CHistogrammer &);
    virtual void setInfo(CSpectrum& rSpectrum, std::string name);
    virtual void setTitle(CSpectrum& rSpectrum, std::string name);
    virtual UInt_t getTitleSize() const;

    virtual void addGate(CSpectrum& rSpectrum, CGateContainer& rGate);
    virtual void removeGate(CSpectrum &rSpectrum, CGateContainer &rGate);

};



class CTestDisplayCreator : public CDisplayCreator
{

public:
    CTestDisplay* create();

};

#endif // TESTDISPLAY_H
