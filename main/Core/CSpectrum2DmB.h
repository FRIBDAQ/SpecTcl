/*
    This software is Copyright by the Board of Trustees of Michigan
    State University (c) Copyright 2005.

    You may use this software under the terms of the GNU public license
    (GPL).  The terms of this license are described at:

     http://www.gnu.org/licenses/gpl.txt

     Author:
             Ron Fox
	     NSCL
	     Michigan State University
	     East Lansing, MI 48824-1321
*/

#ifndef __CSPECTRUM2DMB_H
#define __CSPECTRUM2DMB_H

#ifndef __CSPECTRUM2DM_H
#include "CSpectrum2Dm.h"
#endif

class CEvent;
class TH2C;

/*!
   This is a specialization of the 2dm spectrum that understands
   how to handle byte spectra.  For more information about this spectrum
   class See CSpectrum2Dm.h
*/
class CSpectrum2DmB : public CSpectrum2Dm
{
private:
    TH2C*   m_pRootSpectrum;
public:
  CSpectrum2DmB(std::string              name,
	       UInt_t                   id,
	       std::vector<CParameter>& parameters,
	       UInt_t                   xscale,
	       UInt_t                   yscale);
  CSpectrum2DmB(std::string              name,
	       UInt_t                   id,
	       std::vector<CParameter>& parameters,
	       UInt_t                   xchans,
	       UInt_t                   ychans,
	       Float_t  xlow, Float_t   xhigh,
	       Float_t  ylow, Float_t   yhigh);
  virtual ~CSpectrum2DmB();
private:
  CSpectrum2DmB(const CSpectrum2DmB& rhs);
  CSpectrum2DmB& operator=(const CSpectrum2DmB& rhs);
  int operator==(const CSpectrum2DmB& rhs) const;
  int operator!=(const CSpectrum2DmB& rhs) const;
public:
  // Virtual function overrides.

  virtual ULong_t operator[] (const UInt_t* pIndices) const;
  virtual void    set(const UInt_t* pIndices, ULong_t nValue);
  virtual void    Increment(const CEvent& rEvent);
  virtual void    setStorage(Address_t pEvent);
  virtual Size_t  StorageNeeded() const;


private:
  void CreateChannels();
  void IncPair(const CEvent& rEvent, UInt_t nx, UInt_t ny, int i);

};


#endif
