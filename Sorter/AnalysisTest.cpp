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
#include "Histogrammer.h"
#include "Spectrum1DL.h"
#include "Spectrum2DW.h"
#include "Spectrum2DB.h"
#include "Spectrum1DW.h"
#include "Gamma1DW.h"
#include "Gamma1DL.h"
#include "Gamma2DW.h"
#include "Gamma2DB.h"
#include <string>
#include <iostream.h>



/////////////////////////////////////////////////////////////////////////
//
//  TestUnpacker class..
//
class CTestUnpacker
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
  UInt_t    pIdx = nWds;
  UInt_t    i = 0;
  nWds--;
  while(nWds) {
    rEvent[i] = ((Int_t)*p);
    p++;
    nWds--;
    i++;
  }
  rEvent[pIdx] = rEvent[0] + rEvent[1];

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

const static UInt_t knBlocks = 100; // Blocks in a run.
void
CTestRun::Start()		
{
  CRunControl::Start();
  for(int i = 0; i < knBlocks; i++) {
    OnBuffer(8192);
  }
  CRunControl::Stop();
}

class CTestAnalyzer : public CAnalyzer
{
  CTestUnpacker mTestUnpacker;
  
public:

  virtual UInt_t OnEvent (Address_t pRawData, CEvent& anEvent)
    {
      CBufferDecoder* Deco = (CBufferDecoder*)getDecoder();
      return mTestUnpacker(&(*pRawData), anEvent, *this, *Deco);
    }
};

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
CHistogrammer      Displayer(kn1M);
CEvent             Event;


// Main program ties everything together and runs a run:
//

int main(int argc, char** pargv)
{

  // Build the analyzer:
  CTestAnalyzer TestAnalyzer;
  TestAnalyzer.AttachDecoder(Decoder);
  TestAnalyzer.AttachSink(Displayer);

  // Build the test event source:
  CTestFile      DataSource;
  DataSource.AddDistribution(d1);
  DataSource.AddDistribution(d2);
  DataSource.AddDistribution(d3);
  DataSource.AddDistribution(d4);
  DataSource.Open(std::string("Test"), kacRead);
  UInt_t nEventSize = TestAnalyzer.OnEvent(&DataSource, Event);

  //
  // Create 4 parameters  which correspond to the ones 
  // which will be put in the event by the test data source.
  // 
  CParameter* p1 =
    Displayer.AddParameter(std::string("First Distribution"), 0, 10);
  CParameter* p2 =
    Displayer.AddParameter(std::string("Distribution 2"), 1, 10);
  CParameter* p3 =
    Displayer.AddParameter(std::string("3'd distribution"), 2, 10);
  CParameter* p4 =
    Displayer.AddParameter(std::string("Last Distribution"), 3, 10);
  CParameter* p5 =
    Displayer.AddParameter(std::string("Psuedo parameter"), 5, 11);

  // Create histograms and add them to the histogrammer.
  //
  CSpectrum1DL h1(std::string("Distribution1"), 1, *p1, 10);
  CSpectrum1DL h2(std::string("Distribution2"), 2, *p2, 10);
  CSpectrum1DL h3(std::string("3-d Distribution"), 3, *p3, 10);
  CSpectrum1DL h4(std::string("Last one"), 4, *p4, 10);
  CSpectrum1DL h5(std::string("Compressed distribution 1"), 5, *p1, 8);
  CSpectrum2DB h6(std::string("Two-d gauss 1&2"), 6, *p1,*p2,  7,7);
  CSpectrum2DB h10(std::string("Two-d gauss 2&3"), 10, *p2, *p3, 7,7);
  CSpectrum2DB h11(std::string("Two-d gauss 3&4"), 11, *p3, *p4, 7,7);
  CSpectrum2DB h12(std::string("Two-d gauss 1&4"), 12, *p1, *p4, 7,7);
  CSpectrum2DB h13(std::string("Two-d gauss 2&4"), 13, *p2, *p4, 7,7);
  CSpectrum2DB h15(std::string("Two-d gauss 1&3"), 15, *p1, *p3, 7,7);
  CSpectrum1DL h7(std::string("Psuedo"), 7, *p5, 11);

  vector<CParameter> ParamVec1D;
  ParamVec1D.push_back(*p1);
  ParamVec1D.push_back(*p2);
  //ParamVec1D.push_back(*p3);
  //ParamVec1D.push_back(*p4);
  CGamma1DW h8(std::string("Gamma1D"), 8, ParamVec1D, 10);
  
  vector<CParameter> ParamVec2D;
  ParamVec2D.push_back(*p1);
  ParamVec2D.push_back(*p2);
  CGamma2DB h16(std::string("Gamma2D 1,2"), 16, ParamVec2D, 7, 7);
  ParamVec2D.push_back(*p3);
  CGamma2DB h14(std::string("Gamma2D 1,2,3"), 14, ParamVec2D, 7, 7);
  ParamVec2D.push_back(*p4);
  CGamma2DB h9(std::string("Gamma2D 1,2,3,4"), 9, ParamVec2D, 7, 7);

  Displayer.AddSpectrum(h1);  
  Displayer.AddSpectrum(h2);  
  Displayer.AddSpectrum(h3);  
  Displayer.AddSpectrum(h4);
  Displayer.AddSpectrum(h5);
  Displayer.AddSpectrum(h6);
  Displayer.AddSpectrum(h7);
  Displayer.AddSpectrum(h8);
  Displayer.AddSpectrum(h9);
  Displayer.AddSpectrum(h10);
  Displayer.AddSpectrum(h11);
  Displayer.AddSpectrum(h12);
  Displayer.AddSpectrum(h13);
  Displayer.AddSpectrum(h14);
  Displayer.AddSpectrum(h15);
  Displayer.AddSpectrum(h16);

  // Bind the spectra to Xamine:

  try {
    Displayer.BindToDisplay(std::string("Distribution1"));
    Displayer.BindToDisplay(std::string("Distribution2"));
    Displayer.BindToDisplay(std::string("3-d Distribution"));
    Displayer.BindToDisplay(std::string("Last one"));
    Displayer.BindToDisplay(std::string("Compressed distribution 1"));
    Displayer.BindToDisplay(std::string("Two-d gauss 1&2"));
    Displayer.BindToDisplay(std::string("Psuedo"));
    Displayer.BindToDisplay(std::string("Gamma1D"));
    Displayer.BindToDisplay(std::string("Gamma2D 1,2,3"));
    Displayer.BindToDisplay(std::string("Two-d gauss 2&3"));
    Displayer.BindToDisplay(std::string("Two-d gauss 3&4"));
    Displayer.BindToDisplay(std::string("Two-d gauss 1&4"));
    Displayer.BindToDisplay(std::string("Two-d gauss 2&4"));
    Displayer.BindToDisplay(std::string("Gamma2D 1,2,3,4"));
    Displayer.BindToDisplay(std::string("Two-d gauss 1&3"));
    Displayer.BindToDisplay(std::string("Gamma2D 1,2"));
  }
  catch(CException& e) {
    cerr << "Failed to bind spectrum to Xamine" << endl;
    cerr << e.ReasonText() << endl;
  }

  // Build the Run control object and start a run:
  //
  CTestRun Run(TestAnalyzer, DataSource);
  try {
    while(Displayer.getDisplayer()->isAlive()) {
      Run.Start();
      Run.Stop();
    }
  }
  catch (CException& e) {
    cerr << "Exception caught at main: " << endl;
    cerr << e.ReasonText() << endl;
  }
  return 0;
}
