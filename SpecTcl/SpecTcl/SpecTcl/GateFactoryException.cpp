// Class: CGateFactoryException
// Exceptions which can be thrown by the gate factory.

//
// Author:
//    Ron Fox
//    NSCL
//    Michigan State University
//    East Lansing, MI 48824-1321
//
//
//////////////////////////.cpp file/////////////////////////////////////////////////////
#include "GateFactoryException.h"    				
#include <strstream.h>

static char* pCopyrightNotice = 
"(C) Copyright 1999 NSCL, All rights reserved .cpp \n";

static char* ReasonTexts[] =  {
  "Incorrect instantiation member called",
  "Incorrect number of gates for a compound gate",
  "Invalid number of parameters for a primitive gate",
  "Invalid number of points supplied for a primitive gate",
  "Nonexistent parameter supplied for a primitive gate",
  "Gate constituents must be bands",
  "No such gate exists for complex gate",
  "Parameters of both gates must match",
  "Nonexistent spectrum supplied for gamma gate"

} ; 

char**  CGateFactoryException::m_svReasonTexts = ReasonTexts;

static char* GateTypeStrings[] = {
  "And ",
  "Band ",
  "Band-Contour ",
  "Contour ",
  "Cut ",
  "-Deleted- ",
  "False ",
  "Not ",
  "Or ",
  "True "
};

char** CGateFactoryException::m_svGateTypeStrings = GateTypeStrings;


// Functions for class CGateFactoryException

//////////////////////////////////////////////////////////////////////////////
//
//  Function:       
//     ReasonText()
//  Operation Type: 
//     Selector
string CGateFactoryException::ReasonText()  
{
  // Returns a complete string describing the error which
  // caused the exception.
  // 
  // Implicit inputs:
  //    m_eReason
  //   m_svReasonTexts
  //   m_eGateType
  //
  ostrstream Text;
  Text << m_svReasonTexts[(UInt_t) m_eReason] << endl;
  Text << "While instantiating a " 
       << m_svGateTypeStrings[(UInt_t) m_eGateType] << endl;
  return Text.str();
  
  
}
//////////////////////////////////////////////////////////////////////////////
//
//  Function:       
//     ReasonCode()
//  Operation Type: 
//     Selector
Int_t CGateFactoryException::ReasonCode()  
{
  // Returns the reason code describing the error
  // that caused the exception to be thrown.
  
  return (Int_t)m_eReason;
}
//////////////////////////////////////////////////////////////////////////////
//
//  Function:       
//     ReasonCodeToText(GateFactory...Reason eReason)
//  Operation Type: 
//     Selector
char*  
CGateFactoryException::ReasonCodeToText(GateFactoryExceptionReason eReason)  
{
  // Returns the reason code transated to text.
  //
  return m_svReasonTexts[(UInt_t)eReason];
}
