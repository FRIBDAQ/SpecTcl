//  CSpectrumFactoryException.cpp
     
//
//   Author:
//      Ron Fox
//      NSCL
//      Michigan State University
//      East Lansing, MI 48824-1321
//      mailto:fox@nscl.msu.edu
//
//////////////////////////.cpp file/////////////////////////////////////////////////////

//
// Header Files:
//


#include "SpectrumFactoryException.h"                               
#include <stdio.h>

static const char* Copyright = 
"CSpectrumFactoryException.cpp: Copyright 1999 NSCL, All rights reserved\n";

// Static variables:

static const char* pMessages[] = {
  "Bad Spectrum type",
  "Bad Spectrum data type",
  "Incorrect parameter count",
  "Incorrect resolution count"
};
static const UInt_t nMessages = sizeof(pMessages)/sizeof(char*);

static const UInt_t MAXMESSAGESIZE = 1024;

// Functions for class CSpectrumFactoryException
/////////////////////////////////////////////////////////////////////////
//
// Function:
//     CSpectrumFactoryException (  DataType_t            am_eType,  
//			            SpectrumType_t        am_eSpectrumType,  
//		                    std::string           am_sName,  
//		                    SpectrumErrorReason_t am_eReason,  
//		                    std::string&          rDoing);
// Operation Type:
//     Constructor.
//

CSpectrumFactoryException:: CSpectrumFactoryException(
                                      DataType_t            am_eType,  
				      SpectrumType_t        am_eSpectrumType,  
				      const std::string           am_sName,  
				      SpectrumErrorReason_t am_eReason,  
				      const std::string&          rDoing) :
  CException(rDoing),
  m_eType(am_eType),
  m_eSpectrumType(am_eSpectrumType),
  m_sName(am_sName),
  m_eReason((SpectrumErrorReason_t)am_eReason),
  m_pMessage(new char [MAXMESSAGESIZE])
{
  UpdateMessage();
}


//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    const char* ReasonText (  )
//  Operation Type:
//     Selector.
//
const char* 
CSpectrumFactoryException::ReasonText() const
{
// Reason for the exception
// Exceptions:  

  return m_pMessage;

  

}
//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    Int_t ReasonCode (  )
//  Operation Type:
//     Selector.
//
Int_t 
CSpectrumFactoryException::ReasonCode() const 
{
// Get the reason code. for the exception.
// Exceptions:  

  return (Int_t)m_eReason;

}

//  Below are protected static functions which are conversion utilities..

////////////////////////////////////////////////////////////////////////
//
// Function:
//    char* DataType(DataType_t eType)
// Operation Type:
//     Protected utility:
//
char*
CSpectrumFactoryException::DataType(DataType_t eType)
{
  switch(eType) {
  case keByte:
    return "Byte";
  case keWord:
    return "Word";
  case keLong:
    return "Longword";
  case keFloat:
    return "Floating point";
  case keDouble:
    return "Double precision floating point";
  default:
    return "*unknown data type*";

  }
}
/////////////////////////////////////////////////////////////////////
// 
// Function:
//    char* SpectrumType(SpectrumType_t eType)
// Operation Type:
//   Protected utility.
//
char*
CSpectrumFactoryException::SpectrumType(SpectrumType_t eType)
{
  switch (eType) {
  case ke1D:
    return "One Dimensional";
  case ke2D:
    return "Two Dimensional";
  case keBitmask:
    return "Bit mask spectrum";
  case keSummary:
    return "Summary spectrum";
  default:
    return "Unknown spectrum type";
  }
}
///////////////////////////////////////////////////////////////////////
// 
// Function:
//    char* MessageText(SpectrumErrorReason_t eReason)
// Operation Type:
//    Protected utility.
//
const char*
CSpectrumFactoryException::MessageText(SpectrumErrorReason_t eReason)
{
  UInt_t nReason = (UInt_t)eReason;
  if(nReason < nMessages) {
    return pMessages[nReason];
  }
  else {
    return "*Unrecognized reason*";
  }
}
/////////////////////////////////////////////////////////////////////////
//
// Function:
//   void UpdateMessage()
// Operation Type:
//   Utility
//
void
CSpectrumFactoryException::UpdateMessage()
{
  sprintf(m_pMessage,
	  "%s for %s spectrum to contain %s\nName=%s\n",
	  MessageText(m_eReason),
	  SpectrumType(m_eSpectrumType), DataType(m_eType),
	  m_sName.c_str());

}						
