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

#include "SpectraLocalDisplay.h"
#include "SpectraProcess.h"

#include "XamineSharedMemory.h"
#include "XamineShMemDisplayImpl.h"
#include "Spectrum.h"
#include "SpecTcl.h"
#include "CHttpdServer.h"
#include "TCLInterpreter.h"
#include <Exception.h>
#include <tcl.h>

#include <memory>
#include <iostream>
#include <stdexcept>

namespace Spectra
{

//
//
CSpectraLocalDisplayCreator::CSpectraLocalDisplayCreator()
    : m_pSharedMem()
{}

//
//
CSpectraLocalDisplay* CSpectraLocalDisplayCreator::create()
{
    return new CSpectraLocalDisplay(m_pSharedMem, *SpecTcl::getInstance());
}



//
//
CSpectraLocalDisplay::CSpectraLocalDisplay(std::shared_ptr<CXamineSharedMemory> pSharedMem, SpecTcl &pSpecTcl)
    : m_pProcess(new CSpectraProcess),
      m_pMemory(),
      m_rSpecTcl(pSpecTcl)
{
    m_pMemory.reset(new CXamineShMemDisplayImpl(pSharedMem));
}

//
//
CSpectraLocalDisplay::CSpectraLocalDisplay(const CSpectraLocalDisplay &rhs)
    : m_pProcess(new CSpectraProcess),
      m_pMemory( new CXamineShMemDisplayImpl( *(rhs.m_pMemory))  ),
      m_rSpecTcl(rhs.m_rSpecTcl)
{
}

//
//
CSpectraLocalDisplay::~CSpectraLocalDisplay()
{
    // shared memory will clean up automatically because of unique_ptr
}

//
//
int CSpectraLocalDisplay::operator==(const CDisplay& rhs)
{
    return 0;
}

//
//
CSpectraLocalDisplay* CSpectraLocalDisplay::clone() const
{
    return new CSpectraLocalDisplay(*this);
}

//
//
void CSpectraLocalDisplay::start()
{
    // Code is here to avoid repetition in the catch blocks below:
    
    SpecTcl* api = SpecTcl::getInstance();
    CTCLInterpreter* pInterp = api->getInterpreter();
    Tcl_Interp*      pRawInterp = pInterp->getInterpreter();

    try {
        startRESTServer();
        m_pMemory->attach();
        m_pProcess->exec();
    }
    catch (CException& e) {
        // Failed to start the local server... most likely
        // REST package plugin was not installed.
        // Declare a background error and return.
        
        
        Tcl_AppendResult(
            pRawInterp,
            "\nUnable to start SpecTclHttpdServer. \nCheck that the SpecTcl REST plugin was installed.\n",
            "The errror messages above this should tell if you if not.\n",
            "If that's the case, either use the Xamine displayer or \n",
            "get your SpecTcl installer/manager to install the REST plugin\n",
            "SpecTcl will run headless.",  nullptr
        );
        Tcl_BackgroundError(pRawInterp);
    }
    catch (std::exception &e) {
    
        Tcl_AppendResult(pRawInterp,
            "Unable to start the SpecTclHttpdServer: ", e.what(), nullptr);
    }

}

//
//
void CSpectraLocalDisplay::stop()
{
    m_pProcess->kill();
    stopRESTServer();
    m_pMemory->detach();
}

//
//
bool CSpectraLocalDisplay::isAlive()
{
    return m_pProcess->isRunning();
}

//
//
void CSpectraLocalDisplay::restart()
{
    stop();
    start();
}

//
//
void CSpectraLocalDisplay::startRESTServer()
{
    CTCLInterpreter* pInterp = m_rSpecTcl.getInterpreter();
    CHttpdServer starter(pInterp);
    if (!starter.isRunning()) {
        starter.start(8080);
    }
}

//
//
void CSpectraLocalDisplay::stopRESTServer()
{
    CTCLInterpreter* pInterp = m_rSpecTcl.getInterpreter();
    CHttpdServer stopper(pInterp);
    if (stopper.isRunning()) {
        stopper.stop();
    }
 
}

//
//
SpectrumContainer CSpectraLocalDisplay::getBoundSpectra() const
{
    return m_pMemory->getBoundSpectra();
}

//
//
void CSpectraLocalDisplay::addSpectrum(CSpectrum &rSpectrum, CHistogrammer &rSorter)
{
    m_pMemory->addSpectrum(rSpectrum, rSorter);
}

//
//
void CSpectraLocalDisplay::removeSpectrum(CSpectrum& rSpectrum, CHistogrammer& rSorter)
{
    m_pMemory->removeSpectrum(rSpectrum, rSorter);
}

//
//
bool CSpectraLocalDisplay::spectrumBound(CSpectrum* pSpectrum)
{
    return m_pMemory->spectrumBound(pSpectrum);
}

//
//
void CSpectraLocalDisplay::addFit(CSpectrumFit &fit)
{
     // no op b/c we don't use it
}

//
//
void CSpectraLocalDisplay::deleteFit(CSpectrumFit &fit)
{
    // no op b/c we don't use it
}

//
//
void CSpectraLocalDisplay::addGate(CSpectrum &rSpectrum, CGateContainer &rGate)
{
    // no op b/c we don't use it
}

//
//
void CSpectraLocalDisplay::removeGate(CSpectrum &rSpectrum, CGateContainer &rGate)
{
    // no op b/c we don't use it
}

//
//
std::vector<CGateContainer> CSpectraLocalDisplay::getAssociatedGates(const std::string &spectrumName,
                                                                CHistogrammer &rSorter)
{
    // no op b/c we don't use it
    return std::vector<CGateContainer>();
}

//
//
std::string CSpectraLocalDisplay::createTitle(CSpectrum &rSpectrum, UInt_t maxLength,
                                         CHistogrammer &rSorter)
{
    return rSpectrum.getName();
}

//
//
void CSpectraLocalDisplay::setTitle(CSpectrum &rSpectrum, std::string name)
{
    m_pMemory->setTitle(rSpectrum, name);
}

//
//
void CSpectraLocalDisplay::setInfo(CSpectrum &rSpectrum, std::string name)
{
    m_pMemory->setInfo(rSpectrum, name);
}

//
//
UInt_t CSpectraLocalDisplay::getTitleSize() const
{
    return m_pMemory->getTitleSize();
}

//
//
void CSpectraLocalDisplay::updateStatistics()
{
    m_pMemory->updateStatistics();
}


} // end namespace Spectra
