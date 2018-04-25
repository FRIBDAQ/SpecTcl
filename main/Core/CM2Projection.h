/*
    This software is Copyright by the Board of Trustees of Michigan
    State University (c) Copyright 2017.

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

/** @file:  CM2Projection.h
 *  @brief: Provide template class for m2 projection spectrum.
 */

#ifndef CM2PROJECTION_H
#define CM2PROJECTION_H

#include <Spectrum.h>
#include <TH2I.h>
#include <TH2S.h>
#include <TH2C.h>

#include <vector>
#include <string>

class CEvent;
class CGateContainer;

/**
 * @class CM2Projection.
 *     This is a templated class for spectra that are projections of m2 spectra.
 *     This is needed in order to ensure that the gate from the parent spectrum
 *     is properly handled to maintain a faithful projection as events are
 *     analyzed.
 *
 *     Background;  The gate that created the region of interest is an or gate
 *     of a set of identical contours (created on each parameter pair of the parent
 *     spectrum).   What we need to do to increment is; for each x/y pair of
 *     parameters present check that they satisfy the contour for _that_ pair.
 *     If so that pair can cause an increment.
 *
 *     To be generic, the spectrum can also be gated with a regular gate
 *     (we implement the logic above in Increment not in operator()) applied.
 *
 *  The template parameter are in order:
 *     -  The data type of the spectrum.
 *     -  The type of the Root spectrum  that will hold that spectrum.
 *
 *   Note that at the bottom of this file:
 *      - There are typedefs for L, W and B spectra.
 *      -  We include the implementation (CM2Projection.hpp).
 */
template<typename T, typename R>
class CM2Projection : public CSpectrum
{
private:
    std::vector<std::pair<UInt_t, UInt_t> > m_XYParameterPairs;
    std::vector<CGateContainer*>            m_Gates;
    UInt_t                                  m_nChannels;
    Bool_t                                  m_x;
    CSpectrum::SpectrumDefinition           m_def;
    
    // Canonicals:
public:
    CM2Projection(
        const std::string name, UInt_t nId,
        const std::vector<UInt_t>& xParams, const std::vector<UInt_t>& yParams,
        const std::vector<CGateContainer*> roiGates,
        UInt_t nChannels, Float_t fLow, Float_t fHigh,
        std::string units=std::string(""), Bool_t x = kfTRUE
    );
    virtual ~CM2Projection();
    
    // Illegal canonicals:
    
private:
    CM2Projection(const CM2Projection&);              // Copy constructor.
    CM2Projection& operator=(const CM2Projection&);   // assignment operator
    int operator==(const CM2Projection&);             // Equality compare.
    int operator!=(const CM2Projection&);             // Inequality compare.

    // Implementation of the spectrum interface that's mandatory:
public:
  virtual   void Increment (const CEvent& rEvent)  ;
  virtual   ULong_t operator[](const UInt_t* pIndices) const;
  virtual   void    set(const UInt_t* pIndices, ULong_t nValue);
  virtual   Bool_t UsesParameter (UInt_t nId) const;

  virtual void GetParameterIds(std::vector<UInt_t>& rvIds);
  virtual void GetResolutions(std::vector<UInt_t>&  rvResolutions);
  virtual Size_t Dimension(UInt_t n) const;
  virtual UInt_t Dimensionality() const;
  virtual CSpectrum::SpectrumDefinition& GetDefinition();

  virtual Bool_t needParameter() const;

  virtual SpectrumType_t getSpectrumType();
  virtual void setStorage(Address_t pStorage);
  virtual Size_t StorageNeeded() const;
  
  // methods specific to this spectrum type:
  
  Bool_t isXprojection() const;
  std::vector<CGateContainer*> getRoiGates() const;
  
  // Utility methods

private:
    void CreateStorage();
    void OrderGateContainers(const std::vector<CGateContainer*>& gates);
};

// Specific 1d spectrum types.

typedef CM2Projection<Int_t, TH1I>   CM2ProjectionL;
typedef CM2Projection<Short_t, TH1S> CM2ProjectionW;

// Implementation:

#ifndef CM2PROJECTION_HPP
#include "CM2Projection.hpp"
#endif

#endif