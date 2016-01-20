/*
    This software is Copyright by the Board of Trustees of Michigan
    State University (c) Copyright 2014.

    You may use this software under the terms of the GNU public license
    (GPL).  The terms of this license are described at:

     http://www.gnu.org/licenses/gpl.txt

     Authors:
             Ron Fox
             Jeromy Tompkins
             NSCL
             Michigan State University
             East Lansing, MI 48824-1321

*/
//  CXamine.h:
//
//    This file defines the CXamine class.
//    Note this file does not adheare to file nameing conventions to prevent
//    clashes with <Xamine.h>  Xamineplus is short for the C++ bindings
//    to the Xamine API.
//
// Author:
//    Ron Fox
//    NSCL
//    Michigan State University
//    East Lansing, MI 48824-1321
//    mailto:fox@nscl.msu.edu
//
//  Copyright 1999 NSCL, All Rights Reserved.
//
/////////////////////////////////////////////////////////////

#ifndef XAMINEPLUS_H  //Required for current class
#define XAMINEPLUS_H

#include "Display.h"

#ifndef __XAMINE_XAMINE_H
#include <Xamine.h>
#define __XAMINE_XAMINE_H
#endif


#ifndef __STL_STRING
#include <string>
#ifndef __STL_STRING
#define __STL_STRING
#endif
#endif

#ifndef __HISTOTYPES_H
#include <histotypes.h>
#endif

#ifndef __XAMINESPECTRUMITERATOR_H
#include "XamineSpectrumIterator.h"
#endif

#include "DisplayFactory.h"

//
//  forward references to classes:
//
class CXamineGate;
class CXamineGates;		// Forward reference.
class CXamineEvent;
class CXamineSpectrum;
class CXamineButton;
class CSpectrum;

class CXamine : public CDisplay
{
   volatile Xamine_shared* m_pDisplay;  //  Pointer to Xamine shared memory region.
   Bool_t         m_fManaged;  //  Set TRUE if memory management started. 
   UInt_t         m_nBytes;    //  Size of shared memory region.
  
   DisplayBindings         m_DisplayBindings;     // Display id to spectrum name map.
   std::vector<CSpectrum*> m_boundSpectra;        // Spectrum if bound.
   FitlineBindings         m_FitlineBindings;     // Fitlines bound to displayer.

   static int              m_nextFitlineId;       // Next Xamine fitline id.

public:
  // Constructors:

  CXamine();			// Bind to existing Xamine
  CXamine (UInt_t nBytes);	
				// New Xamine region nbytes of spectrum space
  ~CXamine ( ) { }       //Destructor

				// Copy Constructor.

  CXamine (const CXamine& aCXamine ) :
    m_pDisplay(aCXamine.m_pDisplay),
    m_fManaged(aCXamine.m_fManaged),
    m_nBytes(aCXamine.m_nBytes)
  {   
                
  }                                     

  CXamine* clone() const { return new CXamine(*this); }

            //Operator= Assignment Operator

  CXamine& operator= (const CXamine& aCXamine)
  { 
    if (this == &aCXamine) return *this;          
    m_pDisplay = aCXamine.m_pDisplay;
    m_fManaged = aCXamine.m_fManaged;
    m_nBytes   = aCXamine.m_nBytes;
        
    return *this;
  }                                     

			//Operator== Equality Operator

  int operator== (const CXamine& aCXamine)
  { 
    return (
	    (m_pDisplay == aCXamine.m_pDisplay) 
	    );
  }

  int operator==(const CDisplay& rhs) {
      return (*this == dynamic_cast<const CXamine&>(rhs));
  }

  // Selectors:

public:
  volatile Xamine_shared* getXamineMemory() const
  {
    return m_pDisplay;
  }
                       
  // Mutator (for derivec classes):

protected:
  void setXamineMemory (Xamine_shared* am_pDisplay)
  { 
    m_pDisplay = am_pDisplay;
  }
  //
  //  Bindings to the Xamine API
  //
public:                       
  std::string GetMemoryName ()  ;  
  void MapMemory (const std::string& rsName, 
                 UInt_t nBytes=knDefaultSpectrumSize)  ;

  // CDisplay interface methods
  Bool_t isAlive ()  ;
  void start ()  ;
  void stop ()  ;
  void restart();

  UInt_t addSpectrum(CSpectrum& rSpectrum, CHistogrammer& rSorter);
  void   removeSpectrum(UInt_t nSpec, CSpectrum &rSpectrum);

  void addFit(CSpectrumFit& fit);
  void deleteFit(CSpectrumFit& fit);

  void updateStatistics();

//  void AddGateToBoundSpectra(CGateContainer& rGate);
//  void RemoveGateFromBoundSpectra(CGateContainer& rGate);

  std::vector<CGateContainer> getAssociatedGates(const std::string& spectrumName,
                                             CHistogrammer& rSorter);

  CSpectrum* getSpectrum(UInt_t xid);
  DisplayBindingsIterator DisplayBindingsBegin();
  DisplayBindingsIterator DisplayBindingsEnd();
  UInt_t DisplayBindingsSize() const;
  Int_t FindDisplayBinding(std::string name);
  Int_t FindDisplayBinding(CSpectrum& rSpectrum);
  bool spectrumBound(CSpectrum &rSpectrum);

  DisplayBindings  getDisplayBindings() const;

  std::string createTitle(CSpectrum& rSpectrum, UInt_t maxLength, CHistogrammer &rSorter);
  UInt_t getTitleSize()  const;
  void setTitle(CSpectrum& rSpectrum, std::string name);
  void setTitle(std::string name, UInt_t slot);
  void setInfo(CSpectrum& rSpectrum, std::string name);
  void setInfo(std::string  name, UInt_t slot);

  // Spectrum statistics.

  void setOverflows(unsigned slot, unsigned x, unsigned y);
  void setUnderflows(unsigned slot, unsigned x, unsigned y);
  void clearStatistics(unsigned slot);

 // end CDisplay interface

  void addGate (CSpectrum& rSpectrum, CGateContainer& rGate)  ;
  void addGate (CXamineGate& rGate)  ;
  void removeGate (UInt_t nSpectrum, UInt_t nId, GateType_t eType);
  void EnterPeakMarker (UInt_t nSpectrum, 
                        UInt_t nId,
                        const std::string& rsName,
                        Float_t fCentroid, Float_t fWidth)  ;
  CXamineGates* GetGates (UInt_t nSpectrum)  ;
  UInt_t GetEventFd ()  ;
  Bool_t PollEvent (Int_t nTimeout, CXamineEvent& rEvent)  ;
  Address_t DefineSpectrum (CXamineSpectrum& rSpectrum)  ;
  void FreeSpectrum (UInt_t nSpectrum)  ;
  void DefineButtonBox (UInt_t nColumns=8, UInt_t NRows=3)  ;
  void DefineButton (UInt_t nColumn, UInt_t nRow, 
                        const CXamineButton& rButton)  ;
  void ModifyButton (UInt_t nColumn, UInt_t nRow, 
                        const CXamineButton& rButton)  ;
  void EnableButton (UInt_t nColumn, UInt_t nRow)  ;
  void DisableButton (UInt_t  nColumn, UInt_t nRow)  ;
  void DeleteButton (UInt_t nColumn, UInt_t nRow)  ;
  void DeleteButtonBox ()  ;
  CXamineButton* InquireButton (UInt_t nColumn, UInt_t nRow)  ;

  CXamineSpectrum& operator[] (UInt_t n)  ;
  CXamineSpectrumIterator begin ()  ;
  CXamineSpectrumIterator end ()  ;

  // Utility Functions:

protected:
  void ThrowGateStatus(Int_t nStatus, const CXamineGate& rGate,
			      const std::string& doing);
  void ThrowButtonStatus(Int_t nStatus, const CXamineButton& rButton,
				const std::string& doing);
  DialogSpectrumType_t MaptoSpec_t(ButtonDialogSpectrumType t);
  Xamine_gatetype      MapFromGate_t(GateType_t type);

  CXamineGate* GateToXamineGate(CSpectrum& rSpectrum, CGateContainer& rGate);

  /**
   * flip2dGatePoints
   *   Determine if the gate point coordinates must be flipped.  This happens
   *   for e.g. a gate on p1, p2 displayed on a spectrum with axes p2, p1
   *
   *  There's an implicit assumption that the gate is displayable on this spectrum
   *  because all we do is see if the X parameter is a match for a spectrum x parameter
   *  and, if not, flip.
   *
   * @param pSpectrum - pointer to the target spectrum.
   * @param gXparam   - Id of the x parameter of the spectrum.
   *
   * @return bool - true if it's necessary to flip axes.
   *
   */
  bool flip2dGatePoints(CSpectrum* pSpectrum, UInt_t gXparam);


  std::string
  createTrialTitle(std::string type, std::vector<std::string>      axes,
                  std::vector<std::string>      parameters,
                  std::vector<std::string>      yparameters,
                  std::string gate);
};


/**
 * @brief The CXamineCreator class
 *
 */
class CXamineCreator : public CDisplayCreator
{
private:
    UInt_t m_nDisplayBytes;

public:
    CXamineCreator(UInt_t nDisplayBytes);

    CXamine* create();

    void setDisplayBytes(UInt_t nBytes) { m_nDisplayBytes = nBytes; }
    UInt_t getDisplayBytes() const { return m_nDisplayBytes; }
};




#endif
