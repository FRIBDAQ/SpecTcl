// Template for a test suite.

#include <config.h>
#include <histotypes.h>
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Asserter.h>
#include "Histogrammer.h"              // Since this has parameter dicts.
#include "SpecTcl.h"
#include "Globals.h"
#include "TreeParameter.h"
#include "Asserts.h"
#include "Event.h"
#include "fragment.h"
#include <stdlib.h>
#include <stdint.h>
#include <vector>

// Open box testing:

#define private public
#include "CEventBuilderEventProcessor.h"
#undef private

class EvbTest : public CppUnit::TestFixture {
  CPPUNIT_TEST_SUITE(EvbTest);
  CPPUNIT_TEST(testInit);
  CPPUNIT_TEST(addProcessor1);
  CPPUNIT_TEST(addProcessor2);
  
  CPPUNIT_TEST(dispatch0);
  CPPUNIT_TEST(dispatch1);
  CPPUNIT_TEST_SUITE_END();


private:
    CHistogrammer*               m_pHistogrammer;
    CEventBuilderEventProcessor* m_processor;    
public:
  void setUp() {
    m_pHistogrammer = new CHistogrammer;
    gpEventSink     = m_pHistogrammer;
    m_processor    = new CEventBuilderEventProcessor(100.0, "evb");
  }
  void tearDown() {
    delete m_pHistogrammer;
    gpEventSink = nullptr;
    delete m_processor;
    
  }
protected:
  void testInit();
  void addProcessor1();
  void addProcessor2();
  
  void dispatch0();
  void dispatch1();
};

//  Dummy event processors we'll use.

class noop : public CEventProcessor       // Do nothing event processor.
{
public:
    Bool_t operator()(
        const Address_t p, CEvent& rEvent, CAnalyzer& rAnalyzer,
        CBufferDecoder& rDecoder
    ) { return kfTRUE; }
};

class countevents : public CEventProcessor    // Count  operator() calls.
{
public:
    unsigned m_calls;
    countevents() : m_calls(0) {}
    Bool_t operator()(
        const Address_t p, CEvent& rEvent, CAnalyzer& rAnalyzer,
        CBufferDecoder& rDecoder
    ) {
        m_calls++;
        return kfTRUE;
      }
};

// Event body generators.

/*
 * makeEmptyFragments
 * 
 *  Make events with fragments whose bodies are empty.
 *
 * @param info - Vector of timestamps and ids.  This is a vector of pairs
 *               whose first elements are the source ids and whose second are
 *               the timestamps.  One fragment is made for each element.
 *               The fragment body sizes will be zero.
 *          
 * @return void* - Pointer to the event.  This will be dynamically allocated
 *                the caller must use free(3) to release storage.
*/
static void*
makeEmptyFragments(const std::vector<std::pair<uint32_t, uint64_t> >& info)
{
    // Figure out how big the event will be... it'll lead with a size
    // and just be a list of FragmentHeader structs.
    
    size_t nBytes = sizeof(uint32_t) + info.size() * sizeof(EVB::FragmentHeader);
    void* pResult = malloc(nBytes);
    ASSERT(pResult);                // just fail if we can't make an event.
    
    // Plug in the size:
    
    uint32_t* pSize = reinterpret_cast<uint32_t*>(pResult);
    *pSize++ = nBytes;
    
    // Now the fragments:
    
    EVB::pFragmentHeader pFrag = reinterpret_cast<EVB::pFragmentHeader>(pSize);
    for (int i = 0; i < info.size(); i++) {
        pFrag->s_timestamp = info[i].second;
        pFrag->s_sourceId  = info[i].first;
        pFrag->s_size      = 0;             // No payload.
        pFrag->s_barrier   = 0;             // Don't actualy care about this.
        
        pFrag++;                            // Points at the next body-less frag.
    }
    
    return pResult;
}

/////
/**
 * getParameterValue
 *    Given an event and a parameter name, return its value.
 *    This will make an assertion failure with a message indicating the
 *    parameter that could not be gotten if the parameter is not valid.
 * @param event - References a CEvent to get the value from.
 * @param name  - Name of parameter to return the value.
 * @return double - the parameter value.
 * @throw  whatever CPPUNIT_ASSERT_MESSAGE throws.
 */
static double
getParameterValue(CEvent& event, const char* name)
{
    SpecTcl*    pApi   = SpecTcl::getInstance();
    CParameter* pParam = pApi->FindParameter(name);
    std::string message("Checking for existence of: ");
    message += name;
    CPPUNIT_ASSERT_MESSAGE(message, pParam);
    
    unsigned id = pParam->getNumber();
    
    message = "Checking for validity of ";
    message += name;
    CPPUNIT_ASSERT_MESSAGE(message, event[id].isValid());
    
    return event[id];
}
CPPUNIT_TEST_SUITE_REGISTRATION(EvbTest);

/** When the tree parameters have been registered:
 *  - the clock MHz should be right, as is the basename.  m_nEvnts is 0
 *  - handler map and time difference matrix is empty.
 *  - The tree parameters should be non null, have the right names
 *    and result in parameters.
 */

void EvbTest::testInit() {
    CEventBuilderEventProcessor& p(*m_processor);  // I'm typing lazy.
    
    EQ(100.0, p.m_ClockMHz);
    EQ(std::string("evb"), p.m_baseName);
    EQ(unsigned(0), p.m_nEvents);
    
    ASSERT(p.m_sourceHandlers.empty());
    ASSERT(p.m_timeDifferenceParams.empty());
    
    // Tree parameters are not null:
    
    ASSERT(p.m_sourceCount);
    ASSERT(p.m_unrecognizedSourceCount);
    ASSERT(p.m_eventNumber);
    ASSERT(p.m_seconds);
    
    // After binding the tree parameters I should be able to locate the
    // correct parameters by  name.  We'll assume if we can do that the
    // tree parameters are all set.
    
    CTreeParameter::BindParameters();
    
    SpecTcl* pApi = SpecTcl::getInstance();
    CParameter* pParam;
    pParam = pApi->FindParameter("evb.sources");
    ASSERT(pParam);
    
    pParam = pApi->FindParameter("evb.unrecognized_source");
    ASSERT(pParam);
    
    pParam = pApi->FindParameter("evb.event_no");
    ASSERT(pParam);
    
    pParam = pApi->FindParameter("evb.run_time");
    ASSERT(pParam);
}


/*
 *  If I add a single event processor,
 *  - I can find it in the source handlers map
 *  - A parameter for it's presence has been created  evb.sid_present
 *  - A placeholder has been created in the time difference matrix.
 */
void EvbTest::addProcessor1()
{
   noop evp1;
   m_processor->addEventProcessor(123, evp1);
   CTreeParameter::BindParameters();
   
   // It's in the source handlers map and correct:
   
   ASSERT(!m_processor->m_sourceHandlers.empty());
   auto p = m_processor->m_sourceHandlers.find(123);
   ASSERT(p != m_processor->m_sourceHandlers.end());
   
   CEventBuilderEventProcessor::SourceData& d(p->second);
   EQ(unsigned(123), d.s_sourceId);
   EQ(reinterpret_cast<CEventProcessor*>(&evp1), d.s_processor);
   ASSERT(d.s_SourcePresent);
   
   // There's an empty placeholder in the time difference matrix.
   
   ASSERT(!m_processor->m_timeDifferenceParams.empty());
   auto m = m_processor->m_timeDifferenceParams.find(123);
   ASSERT(m != m_processor->m_timeDifferenceParams.end());
   ASSERT(m->second.empty());
   
   // Check that the parameter got made:
   
   SpecTcl* pApi = SpecTcl::getInstance();
   ASSERT(pApi->FindParameter("evb.123_present"));
}

/*
 *  Adding two event processors.  This should do also add a
 *  time difference parameter for the difference in time between
 *  the two source ids.
 */

void EvbTest::addProcessor2()
{
    noop evp1;
    noop evp2;
    
    m_processor->addEventProcessor(123, evp1);
    m_processor->addEventProcessor(321, evp2);
    
    CTreeParameter::BindParameters();
    
    // We'll make sure the two processors are properly indexed.
    
    auto p = m_processor->m_sourceHandlers.find(123);
    EQ(reinterpret_cast<CEventProcessor*>(&evp1), p->second.s_processor);
    p = m_processor->m_sourceHandlers.find(321);
    EQ(reinterpret_cast<CEventProcessor*>(&evp2), p->second.s_processor);
    
    // Make sure there's now a time difference entry (hung off 123's map).
    
    auto m = m_processor->m_timeDifferenceParams.find(123);
    ASSERT(!m->second.empty());                // Not empty.
    auto pm = m->second.find(321);
    ASSERT(pm != m->second.end());
    
    // Check that all evp parameters have been created with the right names.
    
    SpecTcl* pApi = SpecTcl::getInstance();
    ASSERT(pApi->FindParameter("evb.123_present"));
    ASSERT(pApi->FindParameter("evb.321_present"));
    
    // Time difference param:
    
    ASSERT(pApi->FindParameter("evb.tdiffs.123-321"));
}
/**
 *   If i dispatch events with no event processors registered, the unrecognized
 *   source count should be the number of fragment in the event.  Event number
 *   should increment too.  Second should be calculated properly.
 */
void EvbTest::dispatch0()
{
    CTreeParameter::BindParameters();
    CEvent event;
    CTreeParameter::setEvent(event);     // Set the current event
    
    // Make an event:
    
    std::vector<std::pair<uint32_t, uint64_t> > descriptor =
    {
        {0, 100000000},             // Sourceid/timestamp pairs. 1 second into run.
        {2, 100000003},
        {3,  99999997}
    };
    Address_t pEvent = makeEmptyFragments(descriptor);
    CAnalyzer* pAnalyzer(0);              // are not used
    CBufferDecoder* pDecoder(0);          // by us so fake.
    try {
        // The dereferencing below is a bit confusing.  We don't segfault
        // cause those parameters are passed by reference.
        
        Bool_t result = (*m_processor)(pEvent, event, *pAnalyzer, *pDecoder);
        ASSERT(result);            // Should return true.
        EQ((double)(3.0), getParameterValue(event, "evb.sources")); 
        EQ((double)(3.0), getParameterValue(event, "evb.unrecognized_source"));
        EQ((double)(1.0), getParameterValue(event, "evb.event_no"));
        EQ((double)(1.0), getParameterValue(event, "evb.run_time"));
        
    }
    catch (...) {           // Assert failures throw -- this releases storage.
        free(pEvent);
        throw;
    }
    free(pEvent);
    
}
/**
 *  I dispatch an event with two fragments.  One has a handler, one does not.
 *  Check that
 *  - The handler got called.
 *  - The variables are all set properly.
 */
void EvbTest::dispatch1()
{
    countevents handler;
    m_processor->addEventProcessor(1, handler);
    CEvent      event;
    
    CTreeParameter::BindParameters();
    CTreeParameter::setEvent(event);
    
    // Make the event.  Use a try/catch block to ensure it's freed in the
    // presence of assertion failures:
    
    
    std::vector<std::pair<uint32_t, uint64_t>> description = {
        {0,  200000000},            // Not us 2 seconds into the run.
        {1,  200000000}             // Us.
    };
    Address_t pEvent = makeEmptyFragments(description);
    
    // this try block uses that we know assertion failures are exceptions
    // giving us a chance to release storage associated with the event:
    
    try {
        CAnalyzer*      pAnalyzer(0);
        CBufferDecoder* pDecoder(0);
        (*m_processor)(pEvent, event ,*pAnalyzer, *pDecoder);
        
        // Let's look at the variables:
        
        EQ((double)(2.0), getParameterValue(event, "evb.sources")); 
        EQ((double)(1.0), getParameterValue(event, "evb.unrecognized_source"));
        EQ((double)(1.0), getParameterValue(event, "evb.event_no"));
        EQ((double)(2.0), getParameterValue(event, "evb.run_time"));
        
        // We should have present for the source we have a handler for.
        
        EQ((double)(1.0), getParameterValue(event, "evb.1_present"));
        
        // Our processor should have also been called.
        
        EQ(unsigned(1), handler.m_calls);
    }
    catch (...) {
        free(pEvent);
        throw;
    }
    free(pEvent);
}