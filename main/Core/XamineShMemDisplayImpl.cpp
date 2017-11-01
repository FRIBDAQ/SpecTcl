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

#include <string>
#include <stdexcept>
#include <assert.h>
using namespace std;


CXamineShMemDisplayImpl::CXamineShMemDisplayImpl(std::shared_ptr<CXamineSharedMemory> pMemory)
    : m_pMemory( pMemory ),
      m_boundSpectra()
{
}

CXamineShMemDisplayImpl::CXamineShMemDisplayImpl(const CXamineShMemDisplayImpl &rhs)
    : m_pMemory( rhs.m_pMemory ),
      m_boundSpectra( rhs.m_boundSpectra )
{}

CXamineShMemDisplayImpl::~CXamineShMemDisplayImpl()
{
}

void CXamineShMemDisplayImpl::attach()
{
    m_pMemory->attach();
}

void CXamineShMemDisplayImpl::detach()
{
    m_pMemory->detach();
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
  return rSpectrum.getName();
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
    if (slot != -1) {
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

//
//
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

}

/*!
 * \brief CXamineShMemDisplayImpl::removeSpectrum
 * \param rSpectrum  - spectrum to remove
 * \param rSorter    - access to dictionaries
 *
 *  This needs to remove the spectrum but also the gates associated with it.
 */
void CXamineShMemDisplayImpl::removeSpectrum(
    CSpectrum &rSpectrum, CHistogrammer& rSorter
)
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

            // get underflows
            if (stats.size() > 0) {
                m_pMemory->setUnderflows(i, stats[0], (stats.size() == 2 ? stats[1] : 0));
            } else {
                m_pMemory->setUnderflows(i, 0, 0);
            }

            // get overflows
            stats = pSpec->getOverflows();
            if (stats.size() > 0) {
                m_pMemory->setOverflows(i, stats[0], (stats.size() == 2 ? stats[1] : 0));
            } else {
                m_pMemory->setOverflows(i, 0, 0);
            }
        }
    }
}


SpectrumContainer CXamineShMemDisplayImpl::getBoundSpectra() const
{
    return m_boundSpectra;
}

DisplayBindings CXamineShMemDisplayImpl::getDisplayBindings() const
{
    return m_pMemory->getDisplayBindings();
}

/*!
 *
 */
bool CXamineShMemDisplayImpl::spectrumBound(CSpectrum* pSpectrum)
{
    return (m_pMemory->findDisplayBinding(pSpectrum->getName()) >= 0);
}

