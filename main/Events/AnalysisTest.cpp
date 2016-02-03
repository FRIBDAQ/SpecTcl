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
static const char* Copyright = "(C) Copyright Michigan State University 2006, All rights reserved";
//  Test program for analysis subsystem.
//  We'll tie together the following:
//    An analyzer with a simple buffer decoder and event unpacker, and
//    simple event sink.
//  We'll Tie a Run ctonrll with an event file (test source) and the 
//  analyzer and then start up analysis with with that. 
//  Note that the Run control will be customized as well.
// 
//

// Header files:
#include <config.h>
#include <histotypes.h>
#include "Event.h"
#include "EventList.h"
#include "EventSink.h"
#include "Analyzer.h"
#include "NSCLBufferDecoder.h"
#include "EventUnpacker.h"
#include "EventSink.h"
#include "GaussianDistribution.h"
#include "RunControl.h"
#include "TestFile.h"
#include "Exception.h"
#include <string>
#include <iostream>


#ifdef HAVE_STD_NAMESPACE
using namespace std;
#endif
////////////////////////////////////////////////////////////////////
//
//  Event sink class - just prints event information
//
class CTestSink : public CEventSink
{
public:
  virtual void operator()(CEventList& rEvents);
  // protected:
  void ShowEvent(ostream& rStream, CEvent& rEvent);
};

void
CTestSink::operator()(CEventList& rEvents)
{
  for(UInt_t i = 0; i < rEvents.size(); i++) {
    CEvent& rEvent(*(rEvents[i]));
    ShowEvent(cout, rEvent);
  }
};   

void
CTestSink::ShowEvent(ostream& rStream, CEvent& rEvent)
{
  cout << "--------------------Event-------------------------------------\n";
  cout << " Event has " << rEvent.size() << " words\n";
  for(int i = 0; i < rEvent.size(); i++) {
    if( (i % 8) == 0) cout << endl;
    cout << rEvent[i] << ' ';
    if(rEvent[i] < 0) break;
  }
  cout << "\n-------------------------------------------------------------\n";
}

/////////////////////////////////////////////////////////////////////////
//
//  Event Unpacker class..
//
class CTestUnpacker : public CEventUnpacker
{
public:
  UInt_t operator()(const Address_t pEvent,
		    CEvent&         rEvent,
		    CAnalyzer&      rAnalyzer,
		    CBufferDecoder& rDecoder);
};
UInt_t
CTestUnpacker::operator()(const Address_t pEvent,
			  CEvent&         rEvent,
			  CAnalyzer&      rAnalyzer,
			  CBufferDecoder& rDecoder)
{
  //
  //  The event consists of a count follwed by a number of parameters.
  //  fixed length fixed format events.
  //
  UShort_t* p    = (UShort_t*)pEvent;
  UShort_t  nWds = *p++;
  UInt_t    i = 0;
  nWds--;
  while(nWds) {
    rEvent[i] = ((Int_t)*p);
    p++;
    nWds--;
    i++;
  }
  p = (UShort_t*)pEvent;
  return (UInt_t)(*p)*sizeof(UShort_t);
}

///////////////////////////////////////////////////////////////////////////
//
//  Run control class... a Run is just a fixed number of blocks:
//

class CTestRun : public CRunControl
{
public:
  CTestRun(CAnalyzer& rAnalyzer,
	   CFile&     rFile) :
    CRunControl(rAnalyzer, rFile) {}

  virtual void Start();

};

void
CTestRun::Start()		// A run is 10 blocks.
{
  CRunControl::Start();
  for(int i = 0; i < 10; i++) {
    OnBuffer(8192);
  }
  CRunControl::Stop();
}

//
//   We're going to need a few gaussian distributions in order to run the
//   test.  These can be statically declared:
//
//

CGaussianDistribution d1(512.0, 128.0, 1024.0),
  d2(128.0, 64.0,  1024.0),
  d3(64.0,  32.0,  1024.0),
  d4(32.0,  16.0,  1024.0);
//
//  We'll also need an NSCLBufferDecoder, Event sink,  and an unpacker:
//

CNSCLBufferDecoder Decoder;
CTestUnpacker      Unpacker;
CTestSink          Displayer;



// Main program ties everything together and runs a run:
//

int main(int argc, char** pargv)
{

  // Build the analyzer:
  
  CAnalyzer      Analyzer;
  Analyzer.AttachUnpacker(Unpacker);
  Analyzer.AttachDecoder(Decoder);
  Analyzer.AttachSink(Displayer);

  // Build the test event source:

  CTestFile      DataSource;
  DataSource.AddDistribution(d1);
  DataSource.AddDistribution(d2);
  DataSource.AddDistribution(d3);
  DataSource.AddDistribution(d4);
  DataSource.Open(std::string("Test"), kacRead);


  // Build the Run control object and start a run:
  //
  CTestRun Run(Analyzer, DataSource);
  try {
    Run.Start();
  }
  catch (CException& e) {
    cerr << "Exception caught at main: " << endl;
    cerr << e.ReasonText() << endl;
  }
  return 0;
}
