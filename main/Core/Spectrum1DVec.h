/*
    This software is Copyright by the Board of Trustees of Michigan
    State University (c) Copyright 2026.

    You may use this software under the terms of the GNU public license
    (GPL).  The terms of this license are described at:

     http://www.gnu.org/licenses/gpl.txt

     Authors:
             Ron Fox
             FRIB
             Michigan State University
             East Lansing, MI 48824-1321

 */

 /** @file Spectrum1DVec.h
  *  @brief Defines the template class for 1d spectra on vectors.
  */
#ifndef SPECTRUM1DVEC_H
#define SPECTRUM1DVEC_H

#include <Spectrum.h>
#include <CTreeParameterVector.h>
#include <histotypes.h>

#include <string>
#include <vector>
#include <stdint.h>

class CTreeParameterVector;

/**
 * This spectrum type represents a spectrum with similar properties
 * to a gamma spectrum, however, it has a CTreeParameterVector rather than 
 * a vector of paraemters.  It is incremented once for each
 * parameter in its event vector.  Note that that elements in the
 * vector have always been given a value...though we won't assume it since
 * there is the pathalogical case of someone doing a push_back then using []
 * to reset a specfic element.
 * 
 * Unlike a gamma spectrum, this type cannot be folded (until someone asks for that).
 */
template <typename T>
class CSpectrum1DVec : public CSpectrum {
private:
    CTreeParameterVector m_parameters;
    UInt_t                m_nChannels;

    // Canonicals:
public:
    CSpectrum1DVec(
        const std::string& name, UInt_t id, CTreeParameterVector& params, 
        UInt_t nChannels
    );
    CSpectrum1DVec(
        const std::string& name, UInt_t id, CTreeParameterVector& params,
        UInt_t nChannels, Float_t fLow, Float_t fHigh
    );

    virtual ~CSpectrum1DVec();

private:                                                // illegal canonicals:
    CSpectrum1DVec(const CSpectrum1DVec&);              // copy construction.
    CSpectrum1DVec& operator=(const CSpectrum1DVec&);   // assignment.

public:
    int operator==(const CSpectrum1DVec& rhs) const;

     // overrides:

public:
  virtual   void Increment (const CEvent& rE);
  virtual   ULong_t operator[](const UInt_t* pIndices) const;
  virtual   void    set(const UInt_t* pIndices, ULong_t nValue);
  virtual   Bool_t UsesParameter (UInt_t nId) const;

  virtual   void GetParameterIds(std::vector<UInt_t>& rvIds);
  virtual   void GetResolutions(std::vector<UInt_t>&  rvResolutions);

  virtual   Size_t StorageNeeded() const;
  virtual   Size_t Dimension (UInt_t nDimension) const;
  virtual   Bool_t needParameter() const;
  virtual SpectrumType_t getSpectrumType();
  // Other operations:

  std::string getVectorName() const;
protected:
    void CreateRootSpectrum(const char* name, Double_t low, Double_t high, UInt_t bins);
    virtual void deleteRootSpectrum();
    void CreateChannels();	//!< Create storage.
    virtual void setStorage(Address_t pStorage);
};

typedef CSpectrum1DVec<uint32_t> CSpectrum1DVecL;
typedef CSpectrum1DVec<uint16_t> CSpectrum1DVecW;

#include "Spectrum1DVec.hpp"
#endif