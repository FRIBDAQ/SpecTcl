/*
    This software is Copyright by the Board of Trustees of Michigan
    State University (c) Copyright 2005.

    You may use this software under the terms of the GNU public license
    (GPL).  The terms of this license are described at:

     http://www.gnu.org/licenses/gpl.txt

     Author:
             Ron Fox
	     NSCL
	     Michigan State University
	     East Lansing, MI 48824-1321
*/
// This implementation of TreeParameter is based on the ideas and original code of::
//    Daniel Bazin
//    National Superconducting Cyclotron Lab
//    Michigan State University
//    East Lansing, MI 48824-1321
//


// Template for a test suite.
#include <config.h>
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Asserter.h>
#include "Asserts.h"

#include "CTreeParameter.h"
#include "CTreeException.h"
#include <Event.h>
#include <Parameter.h>

#include "TreeTestSupport.h"

#ifdef HAVE_STD_NAMESPACE
using namespace std;
#endif



class TreeParamTests : public CppUnit::TestFixture {
  CPPUNIT_TEST_SUITE(TreeParamTests);
  CPPUNIT_TEST(InitialState);
  CPPUNIT_TEST(Constructors);	// Various types of construction.
  CPPUNIT_TEST(PostInitialize);  // Ensure post construction initialize registers.
  CPPUNIT_TEST(SetEvent);        // Ensure we can set an event..
  CPPUNIT_TEST(Binding);	// Bind parameters to events etc.
  CPPUNIT_TEST(SimpleAssigns);  // Simple assignments.
  CPPUNIT_TEST(OpAssigns);      // E.g. += .
  CPPUNIT_TEST(OpAuto);        // ++, -- pre and post.
  CPPUNIT_TEST(ParamId);	// Can I fetch the parameter id ?
  CPPUNIT_TEST(SetGetValue);    // setValue() getValue()
  CPPUNIT_TEST(SetGetBins);     // setBins(), getBins()
  CPPUNIT_TEST(StartStopControl);  // set/getStart(), set/getStop().
  CPPUNIT_TEST(IncControl);	// Inc modifies end keeping start and nchans constant.
  CPPUNIT_TEST(UnitControl);	// Should be able to modify the units:
  CPPUNIT_TEST(Validity);	// Test the whole isvalid thing.
  CPPUNIT_TEST(ChangeManagement); // Test management of m_f
  CPPUNIT_TEST(Throws);		// Check the throwif utilities.
  CPPUNIT_TEST(DoubleInit1); // Check double initialization throws.
  CPPUNIT_TEST(DoubleInit2); // Check double init throws.
  CPPUNIT_TEST(DoubleInit3);
  CPPUNIT_TEST(DoubleInit4);
  CPPUNIT_TEST(DoubleInit5);
  CPPUNIT_TEST(DoubleInit6);
  CPPUNIT_TEST(DoubleInit7);
  CPPUNIT_TEST(DoubleInit8);
  CPPUNIT_TEST(DoubleInit9);
  CPPUNIT_TEST(DoubleInit10);
  CPPUNIT_TEST(DoubleInit11);
  CPPUNIT_TEST(DoubleInit12);
  CPPUNIT_TEST(DoubleInit13);
  CPPUNIT_TEST_SUITE_END();

private:

public:
  void setUp() {
  }
  void tearDown() {
    TreeTestSupport::ClearMap();
  }
protected:
  void InitialState();
  void Constructors();
  void PostInitialize();
  void SetEvent();
  void Binding();
  void SimpleAssigns();
  void OpAssigns();
  void OpAuto();
  void ParamId();
  void SetGetValue();
  void SetGetBins();
  void StartStopControl();
  void IncControl();
  void UnitControl();
  void Validity();
  void ChangeManagement();
  void Throws();
  
  void DoubleInit1();
  void DoubleInit2();
  void DoubleInit3();
  void DoubleInit4();
  void DoubleInit5();
  void DoubleInit6();
  void DoubleInit7();
  void DoubleInit8();
  void DoubleInit9();
  void DoubleInit10();
  void DoubleInit11();
  void DoubleInit12();
  void DoubleInit13();
private:
  static void CheckProperlyBound(CTreeParameter& pParam);
  void CheckDoubleInit(CTreeParameter& param);
};



// Test functions:



//  Must be a member to get acces to m_pEvent (friendship)

void
TreeParamTests::CheckProperlyBound(CTreeParameter& param)
{
  string name = param.getName();
  string comment;

  comment = name + " should show isBound() true";
  EQMSG(comment, true, param.isBound());

  comment = name + "Should have non null parameter field";
  if(!TreeTestSupport::getParameter(param)) FAIL(comment);

  comment = name + " should be bound to a like-named parameter";
  EQMSG(comment, name, TreeTestSupport::getParameter(param)->getName());

}

CPPUNIT_TEST_SUITE_REGISTRATION(TreeParamTests);

// In the initial state, begin() == end()
// size.

void 
TreeParamTests::InitialState()
{
  ASSERT(CTreeParameter::begin() == CTreeParameter::end());
}

// Exercise all the constructors.
//  Note that due to the way that the CTreeParameter::testClearMap works
//  we must instantiate with new as the tree params in the map will be deleted.
//  This is not an issue in 'normal' use as the lifetime of a tree parameter is the
//  program lifetime.
void
TreeParamTests::Constructors()
{
  CTreeParameter* pDefault     = new CTreeParameter;
  CTreeParameter* pNameOnly    = new CTreeParameter("nameonly");
  CTreeParameter* pNameAndUnits= new CTreeParameter("name&units", "cm");
  CTreeParameter* pLowHighUnits= new CTreeParameter("lowhighunits",
						    1.0, 2.0, "mm");
  CTreeParameter* pChanAndLimit= new CTreeParameter("chanandlimits",
						    100, 0.0, 2.0, "Channels");
  CTreeParameter* pClassic     = new CTreeParameter("classic", 12);
  CTreeParameter* pMappedWid   = new CTreeParameter("mappedwid", 12, 0.0,  1.0,
						    "arbitrary", true);
  CTreeParameter* pMappedHigh  = new CTreeParameter("mappedhigh", 12, 0.0, 4096.0,
						    "channels", false);
  CTreeParameter* pFromTemplate= new CTreeParameter("fromtemplate", *pChanAndLimit);
  CTreeParameter* pCopied      = new CTreeParameter(*pChanAndLimit);


  // Now check properties of all of these.  We can check:
  // Name     - Should match what we think it is.
  // Bins     - Should be able to compute this.
  // Start    - Low limit
  // Stop     - High limit.
  // Inc      - Calcluated width.
  // Units    - Units string is also local.
  // hasChanged - should be false.

  CheckConstructed(*pDefault, "Default constructed",
		   "", 100, 1.0, 100.0, (99.0)/100, "", false);
  CheckConstructed(*pNameOnly, "Constructed with name only",
		   "nameonly", 100, 1.0, 100.0, 99.0/100, "unknown", false);
  CheckConstructed(*pNameAndUnits, "Constructed with name and units",
		   "name&units", 100, 1.0, 100.0, 99.0/100, "cm", false);
  CheckConstructed(*pLowHighUnits, "Constructed with low, high and units",
		   "lowhighunits", 100, 1.0, 2.0, (2.0-1.0)/100.0, "mm", false);
  CheckConstructed(*pChanAndLimit, "Channels and limits",
		   "chanandlimits", 100, 0.0, 2.0, 2.0/100, "Channels", false);
  CheckConstructed(*pClassic, "Classic",
		   "classic", 4096, 0.0, 4096.0, 1.0, "unknown", false);
  CheckConstructed(*pMappedWid, "Mapped with width specification",
		   "mappedwid", 4096, 0.0, 4096.0, 1.0, "arbitrary", false);
  CheckConstructed(*pMappedHigh, "mapped with upper limit",
		   "mappedhigh", 4096, 0.0, 4096.0, 1.0, "channels", false);
  CheckConstructed(*pFromTemplate, "Copied from a template",
		   "fromtemplate", 100, 0.0, 2.0, 2.0/100, "Channels", false);
	     
  CheckConstructed(*pCopied, "Copy constructed", 
		   "chanandlimits", 100, 0.0, 2.0, 2.0/100, "Channels", false);
}
//
void
TreeParamTests::PostInitialize()
{
  CTreeParameter* pDefault = new CTreeParameter;   // should not be registered:
  EQMSG("Should be unregistered", false, TreeTestSupport::isRegistered(*pDefault));

  pDefault->Initialize("testing", "mm");
  EQMSG("Should be registered", true, TreeTestSupport::isRegistered(*pDefault));

  ASSERT(CTreeParameter::find("testing") != CTreeParameter::end());
}
//
void
TreeParamTests::SetEvent()
{
  CEvent event;
  EQMSG("Should not have an event", (CEvent*)NULL, TreeTestSupport::getEvent());

  CTreeParameter::setEvent(event);
  EQMSG("Should have our event", &event, TreeTestSupport::getEvent());
}
//
void
TreeParamTests::Binding()
{
  CEvent event;
  CTreeParameter* pDefault     = new CTreeParameter;
  CTreeParameter* pNameOnly    = new CTreeParameter("nameonly");
  CTreeParameter* pNameAndUnits= new CTreeParameter("name&units", "cm");
  CTreeParameter* pLowHighUnits= new CTreeParameter("lowhighunits",
						    1.0, 2.0, "mm");

  CTreeParameter::BindParameters();
  CTreeParameter::setEvent(event);

  // Only the default constructed guy should be unbound and
  // all the others should also have a nonzero m_pParameter attribute as well.

  // Check the unbound one:

  EQMSG("Should not be bound", false, pDefault->isBound());

  CheckProperlyBound(*pNameOnly);
  CheckProperlyBound(*pNameAndUnits);
  CheckProperlyBound(*pLowHighUnits);

  
}
//
void
TreeParamTests::SimpleAssigns()
{
  CEvent event;
  CTreeParameter* pNameOnly    = new CTreeParameter("nameonly");
  CTreeParameter* pNameAndUnits= new CTreeParameter("name&units", "cm");

  CTreeParameter::BindParameters();
  CTreeParameter::setEvent(event);


  // Assign a float to the Nameonly guy:  CTreeParameter can
  // use doubles all it wants but at present, CEvent holds floats, not doubles!!xv
  float pi = 3.1416;

  *pNameOnly = 3.1416;
  EQMSG("getValue pi", pi, (float)pNameOnly->getValue());

  float ppi = *pNameOnly;
  EQMSG("double cast", pi, ppi);

  *pNameAndUnits = *pNameOnly;	// Assignment from treeparam:
  EQMSG("assignfrom tree: ", pi, (float)pNameAndUnits->getValue());

  // We want to be sure that only the value of pNameOnly transferred:

  CheckConstructed(*pNameAndUnits, "Constructed with name and units",
		   "name&units", 100, 1.0, 100.0, 99.0/100, "cm", false);
}
//
void
TreeParamTests::OpAssigns()
{
  CEvent event;
  CTreeParameter* pNameOnly    = new CTreeParameter("nameonly");
  CTreeParameter* pNameAndUnits= new CTreeParameter("name&units", "cm");

  CTreeParameter::BindParameters();
  CTreeParameter::setEvent(event);

  // +/- =.

  *pNameOnly = 0.0;		// Make it valid.
  *pNameOnly += 2.0;		// Add to it:

  EQMSG("+=", (double)2.0, pNameOnly->getValue());

  *pNameOnly -= 0.5;
  EQMSG("-=", (double)(2.0 - 0.5), pNameOnly->getValue());

  // *= /=:

  *pNameAndUnits = 5.0;
  *pNameAndUnits *= 15.0;
  EQMSG("*=", (double)(5.0*15), pNameAndUnits->getValue());

  *pNameAndUnits /= 5.0;
  EQMSG("/=", (double)15.0, pNameAndUnits->getValue());
}
//
void
TreeParamTests::OpAuto()
{
  CEvent event;
  CTreeParameter& NameOnly(*(new CTreeParameter("nameonly")));
  
  CTreeParameter::BindParameters();
  CTreeParameter::setEvent(event);


  // Autoincrement.

  double starting = 1234;
  NameOnly = starting;
  double value = NameOnly++; 
  EQMSG("a++ value: ", starting, value);
  EQMSG("a++ param: ", (starting+1), (double)NameOnly);

  value = ++NameOnly;
  EQMSG("++a value: ", starting+2, value);
  EQMSG("++a param: ", starting+2, (double)NameOnly);
  

  // Autodecremenet:

  NameOnly = starting;
  value = NameOnly--;
  EQMSG("a-- value: ", starting, value);
  EQMSG("a-- param: ", starting-1, (double)NameOnly);

  value = --NameOnly;
  EQMSG("--a value: ", starting-2, value);
  EQMSG("--a param: ", starting-2, (double)NameOnly);
  
  
}
//
void
TreeParamTests::ParamId()
{
  CEvent event;
  CTreeParameter& NameOnly(*(new CTreeParameter("nameonly")));
  
  CTreeParameter::BindParameters();
  CTreeParameter::setEvent(event);

  int nId            = NameOnly.getId();
  CParameter* pParam = TreeTestSupport::getParameter(NameOnly);
  int npId           = pParam->getNumber();

  ASSERT(nId = npId);
}
//
void
TreeParamTests::SetGetValue()
{
  CEvent event;
  CTreeParameter& NameOnly(*(new CTreeParameter("nameonly")));
  
  CTreeParameter::BindParameters();
  CTreeParameter::setEvent(event);

  NameOnly = 1234;

  NameOnly.setValue(0.0);
  ASSERT(0.0 == NameOnly.getValue());
  
}
//
void
TreeParamTests::SetGetBins()
{
  CEvent event;
  CTreeParameter& NameOnly(*(new CTreeParameter("nameonly")));
  
  CTreeParameter::BindParameters();
  CTreeParameter::setEvent(event);


  UInt_t bins = NameOnly.getBins();
  bins += 100;
  NameOnly.setBins(bins);

  ASSERT(bins == NameOnly.getBins());
}
//
void
TreeParamTests::StartStopControl()
{
  CEvent event;
  CTreeParameter& NameOnly(*(new CTreeParameter("nameonly")));
  
  CTreeParameter::BindParameters();
  CTreeParameter::setEvent(event);

  double start = NameOnly.getStart();
  double stop  = NameOnly.getStop();
  UInt_t bins  = NameOnly.getBins();

  // slide by the range:

  start = stop;
  stop += bins;
  NameOnly.setStart(start);
  NameOnly.setStop(stop);

  EQMSG("Start: ", start, NameOnly.getStart());
  EQMSG("Stop:  ", stop,  NameOnly.getStop());
}
//
//  Inc really is not an independent parameter.. modifying it modifies end,
//  holding numchans and start constant.

void
TreeParamTests::IncControl()
{
  CEvent event;
  CTreeParameter& NameOnly(*(new CTreeParameter("nameonly")));
  
  CTreeParameter::BindParameters();
  CTreeParameter::setEvent(event);


  UInt_t oldChans = NameOnly.getBins();
  double oldStart = NameOnly.getStart();

  double Inc   = NameOnly.getInc();
  Inc  *= 2.0;
  NameOnly.setInc(Inc);

  EQMSG("Inc ",  Inc, NameOnly.getInc());
  EQMSG("Chans", oldChans, NameOnly.getBins());
  EQMSG("Start", oldStart, NameOnly.getStart());
  EQMSG("Stop",  oldStart + (Inc*oldChans), NameOnly.getStop());


}
//  Units:
void 
TreeParamTests::UnitControl()
{
  CEvent event;
  CTreeParameter& NameOnly(*(new CTreeParameter("nameonly")));
  
  CTreeParameter::BindParameters();
  CTreeParameter::setEvent(event);


  NameOnly.setUnit("arbitrary");

  ASSERT("arbitrary" == NameOnly.getUnit());
}

void
TreeParamTests::Validity()
{
  CEvent event;
  CTreeParameter& NameOnly(*(new CTreeParameter("nameonly")));
  
  CTreeParameter::BindParameters();
  CTreeParameter::setEvent(event);


  // At this time we should have an 'invalid' parameter:

  ASSERT(!NameOnly.isValid());

  // Once we assign to it it becomes valid:

  NameOnly = 3.14159;
  ASSERT(NameOnly.isValid());

  // setInvalid should invalidate it:

  NameOnly.setInvalid();
  ASSERT(!NameOnly.isValid());

  // As should Reset:

  NameOnly = 1.2345;
  ASSERT(NameOnly.isValid());
  NameOnly.Reset();
  ASSERT(!NameOnly.isValid());

  // As should clear:

  NameOnly = 2.345;
  ASSERT(NameOnly.isValid());
  NameOnly.clear();
  ASSERT(!NameOnly.isValid());

  // And finally, Reset All should  as well:

  NameOnly = 5.555;
  CTreeParameter::ResetAll();
  ASSERT(!NameOnly.isValid());


}
//
void 
TreeParamTests::ChangeManagement()
{
  CEvent event;
  CTreeParameter& NameOnly(*(new CTreeParameter("nameonly")));
  
  CTreeParameter::BindParameters();
  CTreeParameter::setEvent(event);

  // Should be in unchanged state:

  ASSERT(!NameOnly.hasChanged());

  // Now change it via the normal ways:

  NameOnly.setStart(0.0);
  ASSERT(NameOnly.hasChanged());
  NameOnly.resetChanged();
  ASSERT(!NameOnly.hasChanged());

  NameOnly.setStop(200.0);
  ASSERT(NameOnly.hasChanged());
  NameOnly.resetChanged();

  NameOnly.setBins(500);
  ASSERT(NameOnly.hasChanged());
  NameOnly.resetChanged();

  NameOnly.setInc(0.5);
  ASSERT(NameOnly.hasChanged());
  NameOnly.resetChanged();

  NameOnly.setUnit("arbitrary");
  ASSERT(NameOnly.hasChanged());
  NameOnly.resetChanged();

  // Should be able to force change:

  NameOnly.setChanged();
  ASSERT(NameOnly.hasChanged());


}

//
void
TreeParamTests::Throws()
{
  CEvent event;
  CTreeParameter& NameOnly(*(new CTreeParameter("nameonly")));

  // Should have no parameters.

  bool thrown = false;
  try {
    TreeTestSupport::ThrowIfNoParameter(NameOnly, "testing");
  }
  catch(...) {
    thrown = true;
  }
  EQMSG("Just constructed", true, thrown);

  // Should have a parameter but no event.

  thrown = false;

  CTreeParameter::BindParameters();

  try {
    TreeTestSupport::ThrowIfNoParameter(NameOnly, "testing");
  }
  catch (...) {
    thrown = true;
  }
  EQMSG("Bound", false, thrown);

  try {
    TreeTestSupport::ThrowIfNoEvent(NameOnly, "Testing");
  }
  catch (...) {
    thrown = true;
  }
  EQMSG("No parameter", true, thrown);
  thrown = false;

  CTreeParameter::setEvent(event);

  // This is already known to work.


}



/**
 * DoubleInit1
 *    If construction sets a name, any post initialization that changes
 *    the name will throw a CTreeException.
 *      This is done with first named constructor.
 */

void TreeParamTests::CheckDoubleInit(CTreeParameter& p1)  // assume named param1
{
  CPPUNIT_ASSERT_THROW(
    p1.Initialize("param2", 12), CTreeException      // Throw on name change.
  );
  CPPUNIT_ASSERT_NO_THROW(
    p1.Initialize("param1", 12)                       // No name change so ok.
  );
  CPPUNIT_ASSERT_THROW(
    p1.Initialize("param2", 1222, 0, 100, "junk", true),  CTreeException
  );
  CPPUNIT_ASSERT_THROW(
    p1.Initialize("param2"), CTreeException
  );
  CPPUNIT_ASSERT_THROW(
    p1.Initialize("param2", "fulongs"), CTreeException
  );
  CPPUNIT_ASSERT_THROW(
    p1.Initialize("param2", 128, 0, 127, "furlongs"), CTreeException
  );
  
}

void
TreeParamTests::DoubleInit1()
{
  CTreeParameter p1("param1");
  CheckDoubleInit(p1);  
  
  
}

void
TreeParamTests::DoubleInit2()
{
  CTreeParameter p1("param1", "furlongs");   // Second named constructor
  CheckDoubleInit(p1);  

}
void TreeParamTests::DoubleInit3()
{
  CTreeParameter p1("param1", 0.0, 128.0, "furlongs");
  CheckDoubleInit(p1);
}
void TreeParamTests::DoubleInit4()
{
  CTreeParameter p1("param1", 256, 0, 255, "furlong");
  CheckDoubleInit(p1);
}
void TreeParamTests::DoubleInit5()
{
  CTreeParameter p1("param1", 12);
  CheckDoubleInit(p1);
}
void TreeParamTests::DoubleInit6()
{
  CTreeParameter p1("param1", 12, 0, 1024, "furlongs", true);
  CheckDoubleInit(p1);
}
void TreeParamTests::DoubleInit7()
{
  CTreeParameter p2("param2");
  CTreeParameter p1("param1", p2);
  CheckDoubleInit(p1);
}
void TreeParamTests::DoubleInit8()
{
  CTreeParameter p1;           // Double init allowed.
  CPPUNIT_ASSERT_NO_THROW(
    p1.Initialize("param2", 12)
  );

  CTreeParameter p2;
  CPPUNIT_ASSERT_NO_THROW(
    p2.Initialize("param2", 1222, 0, 100, "junk", true)
  );
  
  CTreeParameter p3;
  CPPUNIT_ASSERT_NO_THROW(
    p3.Initialize("param2")
  );
  
  CTreeParameter p4;
  CPPUNIT_ASSERT_NO_THROW(
    p4.Initialize("param2", "fulongs")
  );
  
  CTreeParameter p5;
  CPPUNIT_ASSERT_NO_THROW(
    p5.Initialize("param2", 128, 0, 127, "furlongs")
  );
    
}

// These are default constructions followed by inits.

void TreeParamTests::DoubleInit9()
{
  CTreeParameter p1;
  p1.Initialize("param1", 12);
  CheckDoubleInit(p1);
}
void TreeParamTests::DoubleInit10()
{
  CTreeParameter p1;
  p1.Initialize("param1", 12, 0, 1024, "furlong", true);
  CheckDoubleInit(p1);
}
void TreeParamTests::DoubleInit11()
{
  CTreeParameter p1;
  p1.Initialize("param1");
  CheckDoubleInit(p1);
}
void TreeParamTests::DoubleInit12()
{
  CTreeParameter p1;
  p1.Initialize("param1", "furlongs");
  CheckDoubleInit(p1);
}
void TreeParamTests::DoubleInit13()
{
  CTreeParameter p1;
  p1.Initialize("param1", 1024, 0, 1023, "furlongs");
  CheckDoubleInit(p1);
}