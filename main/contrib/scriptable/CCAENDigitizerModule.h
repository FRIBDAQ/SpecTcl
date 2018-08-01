//! \class: CCAENDigitizerModule           
//! \file:  .h
// Author:
//   Ron Fox
//   NSCL
//   Michigan State University
//   East Lansing, MI 48824-1321
//   mailto:fox@nscl.msu.edu
//
// Copyright 
/*!

  Unpacks data from a CAEN digitizer.  The CAEN Digitizer modules
  handed by this are the CAEN V775, 785, 792.  These modules produce
  essentially identical data formats.  For more information about the
  data format see the documentation of the Unpack member.

  Most of the configuration parameters that are supported by this 
  module are ignored but are defined so that the module is compatible with
  the readout configuration parameters.  This allows the readout and analysis
  software to share configuration data.
*/

#ifndef CCAENDIGITIZERMODULE_H  //Required for current class
#define CCAENDIGITIZERMODULE_H

//
// Include files:
//

                               //Required for base classes
#include "CModule.h"
#include <string>
#include <string.h>
#include <TranslatorPointer.h> 

// Forward Class Defintions:

class CTCLInterpreter;
class CAnalyzer;
class CBufferDecoder;
class CEvent;


class CCAENDigitizerModule  : public CModule        
{
private:
  
  // Private Member data:
  int                m_nCrate;	     //!  Crate number (from crate register).  
  int                m_nSlot;        //!  Slot number from GEO address or register.  
  CIntConfigParam*   m_pCrateConfig; //!< Pointer to "crate" parameter
  CIntConfigParam*   m_pSlotConfig;  //!< Pointer to "slot" parameter.
  CStringArrayparam* m_pParamConfig; //!< Pointer to "parameters" param.
   
public:
   // Constructors and other canonical functions
    CCAENDigitizerModule (CTCLInterpreter& rInterp, 
				    const std::string& rName);
    virtual ~ CCAENDigitizerModule ( );
private:
 
    CCAENDigitizerModule (const CCAENDigitizerModule& aCCAENDigitizerModule );
    CCAENDigitizerModule& operator= (const CCAENDigitizerModule& aCCAENDigitizerModule);
    int operator== (const CCAENDigitizerModule& aCCAENDigitizerModule) const;
    int operator!=(const CCAENDigitizerModule& rhs) const;
public:
// Selectors:

public:

            //Get accessor function for non-static attribute data member
  int getCrate() const
  { 
      return m_nCrate;
  }  
            //Get accessor function for non-static attribute data member
  int getSlot() const
  { 
      return m_nSlot;
  }   

// Attribute mutators:

protected:


            //Set accessor function for non-static attribute data member
  void setCrate (const int am_nCrate)
  { 
    m_nCrate = am_nCrate;
  }  
            //Set accessor function for non-static attribute data member
  void setSlot (const int am_nSlot)
  { 
    m_nSlot = am_nSlot;
  }   

  // Class operations:

public:
  virtual  std::string getType() const;
  virtual   void Setup (CAnalyzer& rAnalyzer, CHistogrammer& rHistogrammer)   ; // 
  virtual   TranslatorPointer<UShort_t> 
    Unpack (TranslatorPointer<UShort_t> pEvent, 
	    CEvent& rEvent, 
	    CAnalyzer& rAnalyzer, CBufferDecoder& rDecoder)   ; // 
  
};

#endif
