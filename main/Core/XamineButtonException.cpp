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

*/
static const char* Copyright = "(C) Copyright Michigan State University 2011, All rights reserved";
//  Cxaminebuttonexception.cpp
// Encapsulates exceptions thrown by Xamine's user button manipulation
// software.
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
#include <config.h>
#include "XamineButtonException.h"                               
#include "XamineButton.h"
#include "XaminePushButton.h"
#include "XamineToggleButton.h"

#include <assert.h>

#ifdef HAVE_STD_NAMESPACE
using namespace std;
#endif

// Static constants:

const static char* pMessages[] = {
  "Successful completion -- code should not have thrown this ",
  "NoSuchSpectrum        -- Requested Spectrum does not exist ",
  "InappropriateGate     -- Gate type was inappropriate for the spectrum",
  "ObjectTableFull       -- Xamine's object table was already full",
  "DuplicateId           -- An object with this Id exists on this spectrum",
  "NoSuchObject          -- Search failed to find a matching object.",
  "CheckErrno            -- This exception should be an CErronException",
  "BadCoordinates        -- Bad Button box coordinates ",
  "BadType               -- Invalid button type ",
  "BadPrompter           -- Invalid prompter type ",
  "NoSuch Message        -- Invalid error code at exception instantiation"
};
const static unsigned int nMessages = sizeof(pMessages)/sizeof(char*);

const static char* Sensitivity[] = {
  "Always",
  "When selected pane has spectrum",
  "When selected pane has 1-d spectrum",
  "When selected pane has 2-d spectrum"
};
// Functions for class CXamineButtonException

//////////////////////////////////////////////////////////////////////////
//
// Function:
//    CXamineButtonException(Int_t nStatus,
// 		             const CXamineButton& rButton,
//		             const std::string&   rDoing);
//
// Operation Type:
//    Constructor
//
CXamineButtonException::CXamineButtonException(Int_t nStatus,
					       const CXamineButton& rButton,
					       const std::string&   rDoing) :
  CException(rDoing),
  m_nError(nStatus),
  m_fFormatted(kfFALSE)
{
  SetButton(rButton);
}
//////////////////////////////////////////////////////////////////////////
//
//  Function:
//     CXamineButtonException
//               (const CXamineButtonException& aCXamineButtonException )
//  Operation Type:
//      Copy constructor
//
CXamineButtonException::CXamineButtonException
             (const CXamineButtonException& aCXamineButtonException ) :
	       CException(aCXamineButtonException),
	       m_nError(aCXamineButtonException.m_nError),
	       m_fFormatted(aCXamineButtonException.m_fFormatted)
{
  std::stringstream& str = (std::stringstream&)aCXamineButtonException.m_ReasonText;
  SetButton(*(aCXamineButtonException.m_pButton));
  m_ReasonText << str.str();
}
//////////////////////////////////////////////////////////////////////////
//
// Function:
//    CXamineButtonException& operator=
//         (const CXamineButtonException& aCXamineButtonException)
// Operation Type:
//    Assignment operator
//
CXamineButtonException& 
CXamineButtonException::operator=(
		const CXamineButtonException& aCXamineButtonException)
{
  std::stringstream& rStr = (std::stringstream&)aCXamineButtonException.m_ReasonText;
  if(this != &aCXamineButtonException) {
    CException::operator=(aCXamineButtonException);
    m_nError            = aCXamineButtonException.m_nError;
    delete m_pButton;
    SetButton(*aCXamineButtonException.m_pButton);
    m_fFormatted        = aCXamineButtonException.m_fFormatted;
    m_ReasonText       << rStr.str();
  }

  return *this;
}
//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    const char* ReasonText (  )
//  Operation Type:
//     Selector.
//
const char* 
CXamineButtonException::ReasonText() const
{
// Returns textual reason the exception was thrown.
  assert(m_nError >= 0);

  if(!m_fFormatted) {
    UInt_t nReason = -m_nError;
    if(nReason >= nMessages) nReason = nMessages-1;
    m_ReasonText << " Exception thrown during Xamine button manipulations\n";
    m_ReasonText << "     " << pMessages[nReason] << endl;
    m_ReasonText << "   Button dump follows:   "  << endl;
    m_ReasonText << "   Button Type        :   " << (m_pButton->PushButton() ?
						 "PushButton" : 
						 "ToggleButton") << endl;
    m_ReasonText << "   Button Code        :   " << m_pButton->getReturnCode() 
	                                     << endl;
    m_ReasonText << "   Button Label       :   " 
		 << m_pButton->getLabel().c_str()
	                                     << endl;
    m_ReasonText << "   State              :   " << (m_pButton->getEnabled() ?
                                                         "Enabled " : 
						         "Disabled") << endl;
    m_ReasonText << "   Sensitive          :   " 
             << Sensitivity[m_pButton->getWhenSensitive()] << endl;

    m_fFormatted = kfTRUE;
  }
  static string result = m_ReasonText.str();
  return result.c_str();

}
//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    Int_t ReasonCode (  )
//  Operation Type:
//     Selector
//
Int_t 
CXamineButtonException::ReasonCode() const
{
// Returns an error code reason the exception was thrown
// Exceptions:  

  return m_nError;
}
//////////////////////////////////////////////////////////////////////////
//
// Function:
//    void SetButton(const CXamineButton& rButton)
// Operation Type:
//    Protected utility:
//
void
CXamineButtonException::SetButton(const CXamineButton& rButton)
{
  CXamineButton b = rButton;
  CXaminePushButton*    pPButton = b.PushButton();
  CXamineToggleButton*  pTButton = b.ToggleButton();

  assert( (pPButton != kpNULL) || (pTButton != kpNULL));

  if(pPButton) {
    m_pButton = new CXaminePushButton(*pPButton);
  }
  else {
    m_pButton = new CXamineToggleButton(*pTButton);
  }
}
