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
  \class CCAENV830Creator
  \file  CCAENV830Creator.h

  This is a dummy creator for the CAEN v830 scaler module.
  It is defined in order to support a common specification script
  for SpectTcl and the Readout engine.
*/
#ifndef CCAENV830CREATOR_H
#define CCAENV830CREATOR_H

// Include files.

#include "CModuleCreator.h"
#include <string>


// Forward class definitions.

class CModule;
class CTCLInterpreter;
class CModuleCommand;


class CCAENV830Creator : public CModuleCreator 
{
public:
  CCAENV830Creator(const std::string& rName, CModuleCommand* pCommand);
  ~CCAENV830Creator();
  CCAENV830Creator (const CCAENV830Creator& rhs );
  CCAENV830Creator& operator= (const CCAENV830Creator& rhs);
  int operator== (const CCAENV830Creator& rhs) const;
  int operator!= (const CCAENV830Creator& rhs) const {
    return !(operator==(rhs));
  }
  
 public:
  
  CSegmentUnpacker* Create (CTCLInterpreter& rInterp, const std::string& rName);
  virtual std::string TypeInfo() const;
};

#endif
