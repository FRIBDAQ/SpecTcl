//  CEventSink.h:
//
//    This file defines the CEventSink class.
//
// Author:
//    Ron Fox
//    NSCL
//    Michigan State University
//    East Lansing, MI 48824-1321
//    mailto:fox@nscl.msu.edu
//
//  Copyright 1999 NSCL, All Rights Reserved.
//
/////////////////////////////////////////////////////////////

#ifndef __EVENTSINK_H  //Required for current class
#define __EVENTSINK_H
                               
class CAnalyzer;		// Forward definition 
class CEventList;

class CEventSink      
{
  
public:
			//Default constructor

  CEventSink ()   
  { } 
  virtual  ~CEventSink ( ) { }       //Destructor	
			//Copy constructor

  CEventSink (const CEventSink& aCEventSink ) 
  { 
                
  }                                     
                        // Assignment operator

  CEventSink& operator= (const CEventSink& aCEventSink)
  {
    return *this; 
  }                                     

			//Operator== Equality Operator

  int operator== (const CEventSink& aCEventSink)
  { 
    return (1==1);
  }                             
  // public interface:
  //
public:
  virtual   void OnAttach (CAnalyzer& rAnalyzer)  
  { }
  virtual   void OnDetach (CAnalyzer& rAnalyzer)
  { }
  //
  //  This operator must be supplied by the sink.
  //
  virtual   void operator() (CEventList& rEvents) = 0;

};

#endif
