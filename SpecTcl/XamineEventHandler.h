
 
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
                                                               
class CXamineEventHandler
{                       
  CTCLInterpreter* m_pInterp;
  CHistogrammer* m_pHistogrammer; //1:1 association object data member      
  int            m_nFd;
  Tcl_Channel    m_SocketChannel; // Channel open on the gates socket.

public:

   // Constructors and other cannonical operations:

  CXamineEventHandler (CTCLInterpreter* pInterp,
		       CHistogrammer*   pHistogrammer) :
    m_pInterp(pInterp),
    m_pHistogrammer(pHistogrammer),
    m_nFd(pHistogrammer->getDisplayer()->GetEventFd()),
    m_SocketChannel(Tcl_MakeTcpClientChannel((ClientData)m_nFd))
  { 
    Set(TK_READABLE);		// Starts out enabled.
  } 
  virtual  ~ CXamineEventHandler ( )  // Destructor 
     { }  
  
   //Copy constructor 

  CXamineEventHandler (const CXamineEventHandler& rhs ) :
    m_pInterp(rhs.m_pInterp),
    m_pHistogrammer(rhs.m_pHistogrammer),
    m_nFd(rhs.m_nFd),
    m_SocketChannel(rhs.m_SocketChannel)
  { 
   


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

  int operator== (const CXamineEventHandler& rhs) const {
    return ( (m_pInterp == rhs.m_pInterp)      &&
	     (m_pHistogrammer == rhs.m_pHistogrammer) &&
	     (m_nFd == rhs.m_nFd)                     &&
	     (m_SocketChannel == rhs.m_SocketChannel));
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
    m_nFd = (m_pHistogrammer->getDisplayer()->GetEventFd()); // Update file id.
    Set(TK_READABLE);		// Set callback on next fid.
  }

public:

 virtual   void operator() (int mask)    ;
 virtual   void Set(int mask);
 virtual   void Clear();

protected:
 virtual   void OnGate (CDisplayGate& rXamineGate)    ;
 virtual   void OnButton (CButtonEvent& rButton)    ;
 virtual   UInt_t FindDisplayBinding (const std::string& rName);

  static void CallbackRelay(ClientData pObject, int mask);

};

#endif
