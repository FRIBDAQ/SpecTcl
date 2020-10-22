/*
    This software is Copyright by the Board of Trustees of Michigan
    State University (c) Copyright 2020.

    You may use this software under the terms of the GNU public license
    (GPL).  The terms of this license are described at:

     http://www.gnu.org/licenses/gpl.txt

     Author:
             Giordano Cerizza
	     NSCL
	     Michigan State University
	     East Lansing, MI 48824-1321
*/

//
// Header Files:
//
#include <config.h>
#include "PyQtDisplay.h"                               
#include "PyQtProcess.h"
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
#include "ProductionXamineShMem.h"
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

#include "Point.h"
extern "C" {
#include <Xamine.h>
#include <clientgates.h>

}

#include <errno.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdexcept>
#include <memory>

#ifdef HAVE_STD_NAMESPACE
using namespace std;
#endif

//
// External References:
//
//extern "C" {
//  Xamine_shared* Xamine_memory;	   // Pointer to shared memory.
//}
extern   int            Xamine_newgates;  // fd for events.


//int CXamine::m_nextFitlineId(1); // Next fitline id assigned.

//
// Static declarations:
//
 static CXamineNoPrompt none;



 // Functions for CPyQtCreator
 ////////////////////////////////////////////////////////////////////////

 CPyQtCreator::CPyQtCreator()
     :  m_pSharedMem()
 {}

 CPyQt* CPyQtCreator::create()
 {
     return new CPyQt(m_pSharedMem);
 }

/////////////////////////////////////////////////////////////////////////
//
// Function:
//    CPyQt::CPyQt(UInt_t nBytes)
// Operation Type:
//    Parameterized Constructor.
//
CPyQt::CPyQt(std::shared_ptr<CXamineSharedMemory> pSharedMem)
  : m_pProcess(new CPyQtProcess),
    m_pMemory()
{
  m_pMemory.reset(new CXamineShMemDisplayImpl(pSharedMem));
}

CPyQt::CPyQt (const CPyQt& aCPyQt )
  : m_pProcess(new CPyQtProcess),
    m_pMemory( new CXamineShMemDisplayImpl( *(aCPyQt.m_pMemory))  )
{
}

CPyQt* CPyQt::clone() const { return new CPyQt(*this); }


CPyQt::~CPyQt() {
}

//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    Bool_t isAlive (  )
//  Operation Type:
//     Selector
//
bool
CPyQt::isAlive() 
{
  return m_pProcess->isRunning();
}
//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    void Start (  )
//  Operation Type:
//     Initialization
//
void 
CPyQt::start()
{
  // Starts the autonomous display subsystem.
  std::cout << "Starting CutiePy display..." << std::endl;
   try {
     m_pMemory->attach();
     m_pProcess->exec();
    }
    catch (...) {
      perror("CutiePie failed to start!! ");
      exit(errno);      
    }
}
//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    void Stop (  )
//  Operation Type:
//     Finalization
//
void 
CPyQt::stop()
{
  m_pProcess->kill();
  m_pMemory->detach();
}
////////////////////////////////////////////////////////////
//
void
CPyQt::restart()
{
  stop();
  start();
}

//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    void EnterGate (CSpectrum& rSpectrum, CGateContainer& rGate )
//  Operation Type:
//     mutator
//
void 
CPyQt::addGate(CSpectrum &rSpectrum, CGateContainer &rGate)
{
  // not implemented
}

void
CPyQt::removeGate(CGateContainer& rGate)
{
  // not implemented
}

void
CPyQt::removeGate(CSpectrum& rSpectrum, CGateContainer& rGate)
{
  // not implemented
}

/*
    Return the size of the spectrum title string.
*/
UInt_t
CPyQt::getTitleSize() const
{
  return m_pMemory->getTitleSize();
}

/*!
   Set the title of a specific slot in Xamine memory.
   The title will be truncated to the size of the spec_title if
   necessary
*/
void
CPyQt::setTitle(CSpectrum& rSpectrum, string name)
{
  m_pMemory->setTitle(rSpectrum, name);
}

void
CPyQt::setInfo(CSpectrum &rSpectrum, std::string name)
{
  m_pMemory->setInfo(rSpectrum, name);
}

void
CPyQt::addSpectrum(CSpectrum &rSpectrum, CHistogrammer &rSorter)
{
  m_pMemory->addSpectrum(rSpectrum, rSorter);
}

void
CPyQt::removeSpectrum(CSpectrum &rSpectrum, CHistogrammer& rSorter)
{
  m_pMemory->removeSpectrum(rSpectrum, rSorter);
}

/////////////////////////////////////////////////////////////////////////////
//
//  Function:
//    std::vector<CGateContainer> GatesToDisplay(const std::string& rSpectrum)
// Operation Type:
//    Protected utility.
//
std::vector<CGateContainer>
CPyQt::getAssociatedGates(const std::string& spectrumName, CHistogrammer &rSorter)
{
  // no op b/c we don't use it
  return std::vector<CGateContainer>();
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
CPyQt::addFit(CSpectrumFit& fit)
{
  // not implemented
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
CPyQt::deleteFit(CSpectrumFit& fit)
{
  // not implemented
}

/**
 * updateStatistics
 *    Update the Xamine statistics for each bound spectrum.
 */
void
CPyQt::updateStatistics()
{
  m_pMemory->updateStatistics(); 
}


SpectrumContainer
CPyQt::getBoundSpectra() const
{
  return m_pMemory->getBoundSpectra();
}

/*!
 *
 */
bool
CPyQt::spectrumBound(CSpectrum* pSpectrum)
{
  return m_pMemory->spectrumBound(pSpectrum);
}

//////////////////////////////////////////////////////////////////////////////
//
// Function:
//   Create a spectrum displayer title from the information in the
//   spectrum definition.   As needed items will be dropped from the
//   definition to ensure that this will all fit in the limited
//   number of characters avaialable to a spectrum title.
//
string
CPyQt::createTitle(CSpectrum& rSpectrum, UInt_t maxLength, CHistogrammer& rSorter)
{
  return rSpectrum.getName();
}
