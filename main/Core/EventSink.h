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

#ifndef EVENTSINK_H  //Required for current class
#define EVENTSINK_H




// Forward definition.
class CAnalyzer;
class CEventList;

class CEventSink {
 public:
  // Constructors.
  CEventSink() {}
  virtual ~CEventSink() {}
  CEventSink(const CEventSink& aCEventSink) {}

  // Operators.
  CEventSink& operator=(const CEventSink& aCEventSink) {
    return *this;
  }

  int operator==(const CEventSink& aCEventSink) { 
    return (1==1);
  }

  // public interface:
 public:
  virtual void OnAttach(CAnalyzer& rAnalyzer) {}
  virtual void OnDetach(CAnalyzer& rAnalyzer) {}

  // This operator must be supplied by the sink.
  virtual void operator()(CEventList& rEvents) = 0;
};

#endif
