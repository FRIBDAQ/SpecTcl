
 
// Class: CXamineEventHandler                     //ANSI C++
//
// Set as a Tcl File handler on the Xamine gates file descriptor.
// This class manages Xamine file events by getting an event
// and calling either the OnGate or OnButton member functions.
//
// Author:
//     Ron Fox
//     NSCL
//     Michigan State University
//     East Lansing, MI 48824-1321
//     mailto: fox@nscl.msu.edu
// 
// (c) Copyright NSCL 1999, All rights reserved XamineEventHandler.h
//

#ifndef __XAMINEEVENTHANDLER_H  //Required for current class
#define __XAMINEEVENTHANDLER_H

#ifndef __TCLFILEHANDLER_H
#include <TCLFileHandler.h>
#endif                               

                               //Required for 1:1 association classes
#ifndef __HISTOGRAMMER_H
#include <Histogrammer.h>
#endif

#ifndef __XAMINEPLUS_H
#include <Xamineplus.h>
#endif

#ifndef _TK_H
#include <tk.h>
#define _TK_H
#endif


class CDisplayGate;
class CButtonEvent;

// XamineEventHandler class declaration:
                                                               
class CXamineEventHandler : public CTCLFileHandler      
{                       
			
  CHistogrammer* m_pHistogrammer; //1:1 association object data member      
			

public:

   // Constructors and other cannonical operations:

  CXamineEventHandler (CTCLInterpreter* pInterp,
		       CHistogrammer*   pHistogrammer) :
    CTCLFileHandler(pInterp, pHistogrammer->getDisplayer()->GetEventFd()),
    m_pHistogrammer(pHistogrammer)
  { 
    Set(TK_READABLE);		// Starts out enabled.
  } 
  virtual  ~ CXamineEventHandler ( )  // Destructor 
     { }  
  
   //Copy constructor 

  CXamineEventHandler (const CXamineEventHandler& aCXamineEventHandler ) :
    CTCLFileHandler(aCXamineEventHandler)
  { 
   
    setHistogrammer(aCXamineEventHandler.m_pHistogrammer);

  }                                     

   // Operator= Assignment Operator 

  CXamineEventHandler& operator= 
  (const CXamineEventHandler& aCXamineEventHandler) {
    if(this != &aCXamineEventHandler) {
      setHistogrammer(aCXamineEventHandler.m_pHistogrammer);
    }
    return *this;
  }
 
   //Operator== Equality Operator 

  int operator== (const CXamineEventHandler& aCXamineEventHandler) const {
    return ( CTCLFileHandler::operator==(aCXamineEventHandler)      &&
	     (m_pHistogrammer == aCXamineEventHandler.m_pHistogrammer));
  }
	
// Selectors:

public:
                       
  CHistogrammer* getHistogrammer() const
  { 
    return m_pHistogrammer;
  }
                       
// Mutators:

protected:
       
  void setHistogrammer (CHistogrammer* am_pHistogrammer)
  { 
    Clear();			// Disable callback on current fid.
    m_pHistogrammer = m_pHistogrammer;
    setFid(m_pHistogrammer->getDisplayer()->GetEventFd()); // Update file id.
    Set(TK_READABLE);		// Set callback on next fid.
  }

public:

 virtual   void operator() (int mask)    ;
protected:
 virtual   void OnGate (CDisplayGate& rXamineGate)    ;
 virtual   void OnButton (CButtonEvent& rButton)    ;
 virtual   UInt_t FindDisplayBinding (const std::string& rName);


};

#endif
