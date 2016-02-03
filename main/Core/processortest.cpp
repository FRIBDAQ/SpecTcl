#include <config.h>
#include <buffer.h>
#include <buftypes.h>


#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Asserter.h>
#include "Asserts.h"

#include <vector>

#include "CBufferProcessor.h"
#include "CBufferCallback.h"
#include "SRunContext.h"

#include <iostream>


#ifdef HAVE_STD_NAMESPACE
using namespace std;
#endif

class processorlist : public CppUnit::TestFixture
{
  CPPUNIT_TEST_SUITE(processorlist);
  CPPUNIT_TEST(accesstest);
  CPPUNIT_TEST(addcallback);
  CPPUNIT_TEST(extendtest);
  CPPUNIT_TEST(remove);
  CPPUNIT_TEST_SUITE_END();

private:
  CBufferProcessor* m_pProcessor;

public:
  void setUp()
  {
    m_pProcessor = new CBufferProcessor;
  }
  void tearDown()
  {
    delete m_pProcessor;
  }

protected:
  void accesstest();
  void addcallback();
  void extendtest();
  void remove();

};

CPPUNIT_TEST_SUITE_REGISTRATION(processorlist);


void
processorlist::accesstest()
{
  SRunContext* pContext = m_pProcessor->getContext();

  // Check s_fScalersThisRun:

  ASSERT(!(m_pProcessor->scalersSeen()));
  pContext->s_fScalersThisRun = true;
  ASSERT(m_pProcessor->scalersSeen());

  // Check s_nScalerCount:

  pContext->s_nScalerCount = 10;
  EQ(10U, m_pProcessor->scalerCount());

  // check s_Increments:

  for(int i =0; i < 10; i++) {
    pContext->s_Increments.push_back(i);
  }
  for(int i = 0; i < 10; i++) {
    EQ((unsigned long)i, m_pProcessor->lastIncrement(i));
  }
  // Check s_Totals..

  for(int i = 0; i < 10; i++) {
    pContext->s_Totals.push_back(i);
  }
  for(int i = 0; i < 10; i++) {
    EQ((float)i, m_pProcessor->Total(i));
  }

  // Check Interval start/stop time

  pContext->s_IntervalStartTime = 1234L;
  pContext->s_IntervalEndTime   = 5678L;
  EQ(1234UL, m_pProcessor->lastIntervalStart());
  EQ(5678UL, m_pProcessor->lastIntervalEnd());

  // Check run number:

  pContext->s_nRunNumber = 987;
  EQ(987, m_pProcessor->runNumber());

  // Check the run start/end time strings.

  pContext->s_RunStarted = "Mary had a little lamb";
  EQ(string("Mary had a little lamb"), m_pProcessor->runStartTime());
  pContext->s_RunEnded = "who's fleece was white as snow";
  EQ(string("who's fleece was white as snow"), m_pProcessor->runEndTime());


}

// The stuff below is needed to support the add callback test:

static unsigned int reported;

class ReportCallback : public CBufferCallback
{
public:
  virtual void operator()(unsigned int type, const void* pbuffer) {
    reported = type;
  }
};

void
processorlist::addcallback()
{
  reported = 0xffff;
  vector<ReportCallback*> callbacks;

  // We can't to 11 since we have not faked up a control buffer.

  for(int i =0; i < 10; i++) {
    ReportCallback* pcb = new ReportCallback;
    callbacks.push_back(pcb);
    m_pProcessor->addCallback(i, *pcb);
  }
  
  bheader header;
  header.ssignature = 0x0102;
  header.lsignature = 0x01020304;
  header.nevt       = 0;


  for(int i =0; i < 10; i++) {
    header.type = i;
    (*m_pProcessor)(&header);
    EQ((unsigned int)i, reported);
  }
  for(int i =0; i < 10; i++) {
    delete callbacks[i];
  }
}
void
processorlist::extendtest()
{
  reported = 0xffff;
  vector<ReportCallback*> callbacks;

  for(int i =100; i < 120; i++) {
    ReportCallback* pcb = new ReportCallback;
    callbacks.push_back(pcb);
    m_pProcessor->addCallback(i, *pcb);
  }

  bheader header;
  header.ssignature = 0x0102;
  header.lsignature = 0x01020304;
  header.nevt       = 0;

  for(int i =100; i < 120; i++) {
    header.type = i;
    (*m_pProcessor)(&header);
    EQ((unsigned int)i, reported);
  }
  for(int i =0; i < callbacks.size(); i++) {
    delete callbacks[i];
  }
  
}

// The stuff below supports the remove test:

static vector<int> order;
class Appender : public CBufferCallback
{
  int myid;
public:
  Appender(int i) : myid(i) {}
  virtual void operator()(unsigned int type, const void* pbuffer) {
    order.push_back(myid);
  }
};

void
processorlist::remove()
{
  Appender cb1(1);
  Appender cb2(2);
  Appender cb3(3);

  if(!order.empty()) {
    order.erase(order.begin(), order.end());
  }
  m_pProcessor->addCallback(5, cb1);
  m_pProcessor->addCallback(5, cb2);
  m_pProcessor->addCallback(5, cb3);

  m_pProcessor->removeCallback(5, cb2);	// cut out the middle one.


  bheader buffer;
  buffer.type = 5;
  buffer.ssignature = 0x0102;
  buffer.lsignature = 0x01020304;

  (*m_pProcessor)(&buffer);

  EQ((size_t)2, order.size());
  EQ(1, order[0]);
  EQ(3, order[1]);


}
