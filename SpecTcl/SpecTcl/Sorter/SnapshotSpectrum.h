// Class: CSnapshotSpectrum                     //ANSI C++
//
// Snapshot spectrum.  This contains another spectrum which is
// never incremented.  All other operations are delegated to 
// the underlying spectrum, however Increment is a no-op.
//
//
// Author:
//     Ron Fox
//     NSCL
//     Michigan State University
//     East Lansing, MI 48824-1321
//     mailto: fox@nscl.msu.edu
// 
// (c) Copyright NSCL 1999, All rights reserved SnapshotSpectrum.h
//

#ifndef __SNAPSHOTSPECTRUM_H  //Required for current class
#define __SNAPSHOTSPECTRUM_H

                               //Required for base classes
#ifndef __SPECTRUM_H
#include "Spectrum.h"
#endif
                               
                                                               
class CSnapshotSpectrum  : public CSpectrum        
{                       
			
  CSpectrum& m_rActualSpectrum; //1:1 association object data member      
  Bool_t     m_fOwnSpectrum;	// kfTRUE if we can delete spectrum in our
				// destructor.
protected:

public:

   // Constructors and other cannonical operations:

  CSnapshotSpectrum (CSpectrum& rSpectrum, Bool_t fOwner = kfTRUE) :
    CSpectrum(kpNULL, kfFALSE, rSpectrum.getName(), 
	      rSpectrum.getNumber()), // Note, snapshot spectra are not gated.
    m_rActualSpectrum(rSpectrum),
    m_fOwnSpectrum(fOwner)
  { 
    setStorageType(m_rActualSpectrum.StorageType());
  }      
  virtual ~CSnapshotSpectrum() {
    if(m_fOwnSpectrum) delete &m_rActualSpectrum;
    m_fOwnSpectrum = 0;
    setOwnStorage(kfFALSE);	// In case this was modified.
  }
   //Copy constructor :  Not allowed.
private:
  CSnapshotSpectrum (const CSnapshotSpectrum& aCSnapshotSpectrum );
public:

   // Operator= Assignment Operator : not allowed
private:
  CSnapshotSpectrum& operator= (const CSnapshotSpectrum& aCSnapshotSpectrum);
public:
 
   //Operator== Equality Operator:
  //   Snapshots are equal iff, the underlying spectra are equal.

  int operator== (const CSnapshotSpectrum& aCSnapshotSpectrum) {
    return (m_rActualSpectrum == aCSnapshotSpectrum.m_rActualSpectrum);
  }
	
// Selectors:

public:
                       
  const CSpectrum& getCSpectrum() const
  { 
    return m_rActualSpectrum;
  }
                       

public:

  virtual   Bool_t UsesParameter (const CParameter& rParameter) const;
  virtual   void ReplaceStorage (Address_t pNewLoc, 
				 Bool_t fTransferOwnership=kfTRUE)    ;
  virtual   void Clear ()    ;
  virtual   Size_t Dimension (UInt_t nDimension)  const;
  virtual   UInt_t Dimensionality ()  const;
  virtual   ULong_t operator[] (const UInt_t* pIndices)  const;
  virtual   Size_t StorageNeeded () const;
  virtual   void Increment (const CEvent& rEvent) ;
  virtual   SpectrumType_t getSpectrumType() ;
  virtual   Bool_t UsesParameter(UInt_t nId) const;
  virtual   void    set(const UInt_t* pIndices, ULong_t nValue);
  virtual   void GetParameterIds(vector<UInt_t>& rvIds);
  virtual   void GetResolutions(vector<UInt_t>&  rvResolutions);
  virtual   Int_t getScale(UInt_t index);


  Bool_t    SetOwnership(Bool_t fNewOwnership) {
    Bool_t fOwn    = m_fOwnSpectrum;
    m_fOwnSpectrum = fNewOwnership;
    return fOwn;
  }
protected:

private:

};

#endif
