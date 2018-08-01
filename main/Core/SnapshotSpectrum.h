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


/*! 

 Snapshot spectrum.  This contains another spectrum which is
 never incremented.  All other operations are delegated to 
 the underlying spectrum, however Increment is a no-op.

 Change Log:
 $Log$
 Revision 5.2  2005/06/03 15:19:23  ron-fox
 Part of breaking off /merging branch to start 3.1 development

 Revision 5.1.2.1  2004/12/21 17:51:25  ron-fox
 Port to gcc 3.x compilers.

 Revision 5.1  2004/11/29 16:56:07  ron-fox
 Begin port to 3.x compilers calling this 3.0

 Revision 4.3  2003/04/19 00:11:13  ron-fox
 Fix a nasty issue with GetDefinition() that was causing death due to a number of problems with the static output struct.  For later: change the struct to a class so that it can be returned by value rather than by reference.. then it wouldn't have to be static.

 Revision 4.2  2003/04/01 19:53:44  ron-fox
 Support for Real valued parameters and spectra with arbitrary binnings.

*/


#ifndef SNAPSHOTSPECTRUM_H  //Required for current class
#define SNAPSHOTSPECTRUM_H

                               //Required for base classes
#include "Spectrum.h"
#include <string>
#include <vector>
                        
                                                               
class CSnapshotSpectrum  : public CSpectrum        
{                       
			
  CSpectrum& m_rActualSpectrum; //1:1 association object data member      
  Bool_t     m_fOwnSpectrum;	// kfTRUE if we can delete spectrum in our
				// destructor.
protected:

public:

   // Constructors and other cannonical operations:

  CSnapshotSpectrum (CSpectrum& rSpectrum, Bool_t fOwner = kfTRUE) :
    CSpectrum( rSpectrum.getName(), 
	      rSpectrum.getNumber(),
	      rSpectrum.getAxisMaps()), // Note, snapshot spectra are not gated.
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
  virtual   Float_t GetLow(UInt_t nDimension) const;
  virtual   Float_t GetHigh(UInt_t nDimension) const;
  virtual   std::string  GetUnits(UInt_t nDimension) const;
  virtual   ULong_t operator[] (const UInt_t* pIndices)  const;
  virtual   Size_t StorageNeeded () const;
  virtual   void Increment (const CEvent& rEvent) ;
  virtual   SpectrumType_t getSpectrumType() ;
  virtual   Bool_t UsesParameter(UInt_t nId) const;
  virtual   void    set(const UInt_t* pIndices, ULong_t nValue);
  virtual   void GetParameterIds(std::vector<UInt_t>& rvIds);
  virtual   void GetResolutions(std::vector<UInt_t>&  rvResolutions);
  virtual   SpectrumDefinition& GetDefinition();

  Bool_t    SetOwnership(Bool_t fNewOwnership) {
    Bool_t fOwn    = m_fOwnSpectrum;
    m_fOwnSpectrum = fNewOwnership;
    return fOwn;
  }
protected:

private:

};

#endif
