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

/**
 *  @file Gamma2D.h
 *  @brief Template class definition for Gamma 2d spectra.
 *  @note we avoid double inheritance otherwise we could derive from CSpectrum2D<T>  - 
 *  that _would_ however give us the inheritance diamond of death as both derive, in turn from CSpectrum.
 *  Some later time we may figure this out...but not for Issue #128 which is what we're coding.
 */

#ifndef GAMMASPECTRUM_H
#define GAMMASPECTRUM_H

#include "CGammaSpectrum.h"
#include <CAxis.h>
#include "Spectrum.h"
#include <string>
#include <vector>
#include <histotypes.h>
#include <stdint.h>

class CParameter;



/**
 * @class CGamma2D<T>
 *   This template class defines all types of gamma 2d spectra.  It derives from CGammaSpectrum as that
 * places folds and computing what they mean in terms of increments in a common code bucket.
 */

template <typename T>
class CGamma2D :  public CGammaSpectrum {
private: 
    UInt_t m_nXScale;		//!< X channel count.
    UInt_t m_nYScale;		//!< Y Channel count.

public:
			//Constructor(s) with arguments

    CGamma2D(const std::string& rName, UInt_t nId,
            std::vector<CParameter>& rParameters,
            UInt_t nXScale, UInt_t nYScale);

    CGamma2D(const std::string& rName, UInt_t nId,
            std::vector<CParameter>& rParameters,
            UInt_t nXScale, UInt_t nYScale,
            Float_t xLow, Float_t xHigh,
            Float_t yLow, Float_t yHigh);



  virtual  ~ CGamma2D( ) ;
private:
			//Copy constructor [illegal]

    CGamma2D(const CGamma2D& acspectrum1dl); 

                //Operator= Assignment Operator [illegal] 

    CGamma2D& operator= (const CGamma2D& aCGamma1D);


    int operator== (const CGamma2D& aCGamma);
    
  // Selectors 

public:
    UInt_t getXScale() const
    {
        return m_nXScale;
    }
    UInt_t getYScale() const
    {
        return m_nYScale;
    }

    virtual SpectrumType_t getSpectrumType() {
        return keG2D;
    }
  // Mutators (available to derived classes:

protected:
    void setXScale (UInt_t am_nScale)
    { 
        m_nXScale = am_nScale;
    }
    void setYScale(UInt_t nScale)
    {
        m_nYScale = nScale;
    }

  //
  //  Operations:
  //   
public:                 

    virtual   ULong_t operator[](const UInt_t* pIndices) const;
    virtual   void    set(const UInt_t* pIndices, ULong_t nValue);

    virtual void GetResolutions(std::vector<UInt_t>&  rvResolutions);
    virtual   Size_t Dimension (UInt_t n) const;

    virtual   UInt_t Dimensionality () const {
        return 2;
    }

    virtual void Increment(std::vector<std::pair<UInt_t, Float_t> >& rParameters);
    virtual void Increment(std::vector<std::pair<UInt_t, Float_t> >& xParameters,
                std::vector<std::pair<UInt_t, Float_t> >& yParameters);
    virtual void setStorage(Address_t pStorage);
    virtual Size_t StorageNeeded() const;
    
    private:
    static CSpectrum::Axes CreateAxisVector(std::vector<CParameter>& rParams,
                        UInt_t nXchan, UInt_t nYchan,
                        Float_t xLow, Float_t xHigh,
                        Float_t yLow, Float_t yHigh);
    void CreateStorage();
private:
  void createRootSpectrum(
    const char* pName, 
    Double_t xlow, Double_t xhigh, UInt_t xbins, 
    Double_t ylow, Double_t yhigh, UInt_t ybins
  );
  void deleteRootSpectrum();

};


// typedefs to define the traditional spectrum types.

typedef CGamma2D<uint32_t> CGamma2DL;
typedef CGamma2D<uint16_t> CGamma2DW;
typedef CGamma2D<uint8_t>  CGamma2DB;


// template classes need their implementations available so:

#include "Gamma2D.hpp"

#endif