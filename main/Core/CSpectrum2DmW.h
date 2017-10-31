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

#ifndef __CSPECTRUM2DMW_H
#define __CSPECTRUM2DMW_H

#ifndef __CSPECTRUM2DM_H
#include "CSpectrum2Dm.h"
#endif

class CEvent;
class TH2S;

/*!
   This is a specialization of the 2dm spectrum that understands
   how to handle word spectra.  For more information about this spectrum
   class See CSpectrum2Dm.h
*/
class CSpectrum2DmW : public CSpectrum2Dm
{
private:

public:
  CSpectrum2DmW(std::string              name,
	       UInt_t                   id,
	       std::vector<CParameter>& parameters,
	       UInt_t                   xscale,
	       UInt_t                   yscale);
  CSpectrum2DmW(std::string              name,
	       UInt_t                   id,
	       std::vector<CParameter>& parameters,
	       UInt_t                   xchans,
	       UInt_t                   ychans,
	       Float_t  xlow, Float_t   xhigh,
	       Float_t  ylow, Float_t   yhigh);
  virtual ~CSpectrum2DmW();
private:
  CSpectrum2DmW(const CSpectrum2DmW& rhs);
  CSpectrum2DmW& operator=(const CSpectrum2DmW& rhs);
  int operator==(const CSpectrum2DmW& rhs) const;
  int operator!=(const CSpectrum2DmW& rhs) const;
public:
  // Virtual function overrides.

  virtual ULong_t operator[] (const UInt_t* pIndices) const;
  virtual void    set(const UInt_t* pIndices, ULong_t nValue);
  virtual void    Increment(const CEvent& rEvent);
  virtual void    setStorage(Address_t pStorage);
  virtual Size_t  StorageNeeded() const;


private:
  void CreateChannels();
  void IncPair(const CEvent& rEvent, UInt_t nx, UInt_t ny, int i);

};


#endif
