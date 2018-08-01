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
		     END OF TERMS AND CONDITIONS '
*/

/*!
  This is a dummy unpacker for the CAEN v830 scaler.
  it does nothing, but is necessary in order to make a single
  config file work for both readout and SpecTcl.

 */


#ifndef CCAENV830MODULE_H
#define CCAENV830MODULE_H

//   Include files...

#include "CModule.h"
#include <string>
#include <TranslatorPointer.h>


// Forward class definitions.

class CTCLInterpreter;
class CAnalyzer;
class CBufferDecoder;
class CEvent;
class CHistogrammer;

class CCAENV830Module : public CModule
{
  // Constructors and other canonnical members:

public:
  CCAENV830Module(CTCLInterpreter& rInterp,
		  const std::string& rModuleName);
  virtual ~CCAENV830Module();

  // Illegal canonicals:

private:
  CCAENV830Module            (const CCAENV830Module& rhs);
  CCAENV830Module& operator= (const CCAENV830Module& rhs);
  int              operator==(const CCAENV830Module& rhs);
  int              operator!=(const CCAENV830Module& rhs);

  // minimal class operations.

public:
  virtual  std::string getType() const;
  virtual   void Setup (CAnalyzer& rAnalyzer, 
			CHistogrammer& rHistogrammer)   ; // 
  virtual   TranslatorPointer<UShort_t> 
    Unpack (TranslatorPointer<UShort_t> pEvent, 
	    CEvent& rEvent, 
	    CAnalyzer& rAnalyzer, CBufferDecoder& rDecoder)   ; // 
  
};
#endif
