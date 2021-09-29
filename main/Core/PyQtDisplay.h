/*
    This software is Copyright by the Board of Trustees of Michigan
    State University (c) Copyright 2020.

    You may use this software under the terms of the GNU public license
    (GPL).  The terms of this license are described at:

     http://www.gnu.org/licenses/gpl.txt

     Authors:
             Ron Fox
             Giordano Cerizza
             NSCL
             Michigan State University
             East Lansing, MI 48824-1321
*/

#ifndef PYQTDISPLAY_H  //Required for current class
#define PYQTDISPLAY_H

#include "Display.h"
#include <Xamine.h>

#include <histotypes.h>
#include "XamineSpectrumIterator.h"

#include "DisplayFactory.h"

#include <string>
#include <memory>


// Forward declarations
class CPyQtProcess;

//
//  forward references to classes:
//
class CXamineGate;
class CXamineGates;		// Forward reference.
class CXamineEvent;
class CXamineSpectrum;
class CXamineButton;
class CXamineSharedMemory;
class CXamineShMemDisplayImpl;
class CSpectrum;

class CPyQt : public CDisplay
{
  std::unique_ptr<CPyQtProcess>         m_pProcess;
  std::unique_ptr<CXamineShMemDisplayImpl> m_pMemory;

public:
  // Constructors:

  CPyQt (std::shared_ptr<CXamineSharedMemory> pSharedMem);
  virtual ~CPyQt ();       //Destructor

  // Copy Constructor.
  CPyQt (const CPyQt& aCPyQt );

  CPyQt* clone() const;

public:                       

  bool isAlive ()  ;
  void start ()  ;
  void stop ()  ;
  void restart();

  UInt_t addSpectrum(CSpectrum& rSpectrum, CHistogrammer& rSorter);
  UInt_t removeSpectrum(CSpectrum &rSpectrum, CHistogrammer& rSorter);
  SpectrumContainer getBoundSpectra() const;

  void addFit(CSpectrumFit& fit);
  void deleteFit(CSpectrumFit& fit);

  void updateStatistics();

  bool spectrumBound(CSpectrum *pSpectrum);

  std::string createTitle(CSpectrum& rSpectrum,
                          UInt_t maxLength,
                          CHistogrammer &rSorter);
  UInt_t getTitleSize()  const;
  void setTitle(CSpectrum& rSpectrum, std::string name);
  void setInfo(CSpectrum& rSpectrum, std::string name);

  void addGate (CSpectrum& rSpectrum, CGateContainer& rGate)  ;

  void removeGate(CGateContainer& rGate);
  void removeGate(CSpectrum& rSpectrum, CGateContainer& rGate);

  std::vector<CGateContainer> getAssociatedGates(const std::string& spectrumName,
                                                 CHistogrammer& rSorter);

};

class CPyQtCreator : public CDisplayCreator
{
private:
    std::shared_ptr<CXamineSharedMemory> m_pSharedMem;

public:
    CPyQtCreator();

    CPyQt* create();

    void setSharedMemory(std::shared_ptr<CXamineSharedMemory> pShMem) { m_pSharedMem = pShMem; }
    std::weak_ptr<CXamineSharedMemory> getDisplayBytes() const { return m_pSharedMem; }
};

#endif
