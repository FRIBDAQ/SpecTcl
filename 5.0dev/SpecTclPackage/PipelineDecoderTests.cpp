// Template for a test suite.

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Asserter.h>
#include "Asserts.h"

// The #def below supports whitebox testing.

#define private public
#include "CAnalysisPipeline.h"
#include "CSpecTclInterpreter.h"
#undef private

#include "CDataSource.h"
#include "CDecoder.h"
#include "CEventProcessor.h"
#include "CParameter.h"

#include <vector>

class PipelineDecoderTests : public CppUnit::TestFixture {

  // Initialization/construction

  CPPUNIT_TEST_SUITE(PipelineDecoderTests);
  CPPUNIT_TEST(singletonInit);
  CPPUNIT_TEST(singletonInstance);

  // Data source attachment.

  CPPUNIT_TEST(preAttachNoDataSource);
  CPPUNIT_TEST(attachSetsMember);
  CPPUNIT_TEST(attachDeletesOld);
  CPPUNIT_TEST(attachSetReadable);
  
  // Decoder attachment.

  CPPUNIT_TEST(preDecoderNoDecoder);
  CPPUNIT_TEST(decoderAttach);


  // Interactions between pipeline/data source and decoder.
  
  CPPUNIT_TEST(haveData);
  CPPUNIT_TEST(haveMultipleChunks);
  CPPUNIT_TEST(sourceEof);

  // Pipeline manipulation tests

  CPPUNIT_TEST(initiallyEmpty);
  CPPUNIT_TEST(add1);
  CPPUNIT_TEST(add2);
  CPPUNIT_TEST(addmid);
  CPPUNIT_TEST(addsearch);
  CPPUNIT_TEST(addsearchfail);
  CPPUNIT_TEST(begin);
  CPPUNIT_TEST(end);
  CPPUNIT_TEST(size);
  CPPUNIT_TEST(find);
  CPPUNIT_TEST(erase);

  // Dispatch of data down the pipeline.

  CPPUNIT_TEST(dispatch);
  CPPUNIT_TEST(dispatchelementfailed);

  CPPUNIT_TEST_SUITE_END();


private:

public:
  void setUp() {
    Tcl_Interp* pInterp = Tcl_CreateInterp();
    CSpecTclInterpreter::setInterp(pInterp);
  }
  void tearDown() {
    // Idempotency demands we kill-off/reinit m_pInstance:


    if (CAnalysisPipeline::m_pInstance) {
      CAnalysisPipeline::m_pInstance->clearPipeline();
    }

    delete CAnalysisPipeline::m_pInstance;
    CAnalysisPipeline::m_pInstance = 0;  

    Tcl_DeleteInterp(CSpecTclInterpreter::m_pInterp);
    delete CSpecTclInterpreter::m_pInstance;
    CSpecTclInterpreter::m_pInterp = 0;
    CSpecTclInterpreter::m_pInstance = 0;
  }
protected:
  void singletonInit();
  void singletonInstance();

  void preAttachNoDataSource();
  void attachSetsMember();
  void attachDeletesOld();
  void attachSetReadable();

  void preDecoderNoDecoder();
  void decoderAttach();


  void haveData();
  void haveMultipleChunks();
  void sourceEof();

  void initiallyEmpty();
  void add1();
  void add2();
  void addmid();
  void addsearch();
  void addsearchfail();
  void begin();
  void end();
  void size();
  void find();
  void erase();

  void dispatch();
  void dispatchelementfailed();
};

// Stub for CParameter::invalidateAll

void
CParameter::invalidateAll() {}


CPPUNIT_TEST_SUITE_REGISTRATION(PipelineDecoderTests);


/**
 * singletonInit
 *
 *   As everthing starts, before a getInstancde, m_pInstancce == 0.
 */

void PipelineDecoderTests::singletonInit() {
  EQ(reinterpret_cast<CAnalysisPipeline*>(0), CAnalysisPipeline::m_pInstance);
}
/**
 * singletonInstance
 *
 *   Better get the same instance each time:
 */
void PipelineDecoderTests::singletonInstance()
{
  EQ(CAnalysisPipeline::instance(), CAnalysisPipeline::instance());
  EQ(CAnalysisPipeline::m_pInstance, CAnalysisPipeline::instance());
}
/**
 * preAttachNoDataSource
 *
 *   Prior to attachment, there is no data source, so m_pDataSource == 0:
 */
void PipelineDecoderTests::preAttachNoDataSource()
{
  CAnalysisPipeline* pPipe = CAnalysisPipeline::instance();
  EQ(reinterpret_cast<CDataSource*>(0), pPipe->m_pDataSource);
}
/**
 * attachSetsMember
 *
 *   Attaching a data source should set the member m_pDataSource:
 */

class CNullDataSource : public CDataSource
{
public:
  virtual ~CNullDataSource() {}

  // Interface definition.

  virtual void    onAttach(CTCLInterpreter& interp, const char* pSource) {}
  virtual void    createEvent(CTCLInterpreter& interp, ChannelHandler* pHandler, void* pClientData) {}
  virtual size_t  read(void* pBuffer, size_t nBytes) { return 0;}
  virtual void    close() {}
  virtual bool    isEof() {return true;}
};

void
PipelineDecoderTests::attachSetsMember()
{
  CAnalysisPipeline* pPipe   = CAnalysisPipeline::instance();
  CDataSource*       pSource = new CNullDataSource;
  
  pPipe->setDataSource(pSource);
  EQ(pSource, pPipe->m_pDataSource);
 
}

/**
 * attachDeletesOld
 *
 *   Attaching a data source should close and delete the pre-existing one.
 */

static bool closed(false);
static bool deleted(false);
class CDeleteRecordingDataSource : public CDataSource
{
public:
  CDeleteRecordingDataSource() {closed = false; deleted=false;}
  virtual ~CDeleteRecordingDataSource() {deleted = true;}

  // Interface definition.

  virtual void    onAttach(CTCLInterpreter& interp, const char* pSource) {}
  virtual void    createEvent(CTCLInterpreter& interp, ChannelHandler* pHandler, void* pClientData) {}
  virtual size_t  read(void* pBuffer, size_t nBytes) { return 0;}
  virtual void    close() {closed=true;}
  virtual bool    isEof() {return true;}
};

void
PipelineDecoderTests::attachDeletesOld()
{
  CAnalysisPipeline* pPipe = CAnalysisPipeline::instance();
  CDataSource* pFirst       = new CDeleteRecordingDataSource();

  pPipe->setDataSource(pFirst);

  CDataSource* pSecond = new CNullDataSource;
  pPipe->setDataSource(pSecond);

  ASSERT(closed);
  ASSERT(deleted);
  delete pSecond;
}
/**
 * attachSetReadable
 *
 *   When attaching a data source, the data source's attachReadable must be
 *   invoked to hook callbacks to sorueReadable to the event loop.
 */
bool sourceReadableCalled(false);

class CRecordEventDataSource : public CDataSource
{
public:
  CRecordEventDataSource() {sourceReadableCalled = false; }
  

  // Interface definition.

  virtual void    onAttach(CTCLInterpreter& interp, const char* pSource) {}
  virtual void    createEvent(CTCLInterpreter& interp, ChannelHandler* pHandler, void* pClientData) {
    sourceReadableCalled = true;
  }
  virtual size_t  read(void* pBuffer, size_t nBytes) { return 0;}
  virtual void    close() {closed=true;}
  virtual bool    isEof() {return true;}
};

void
PipelineDecoderTests::attachSetReadable()
{
  CAnalysisPipeline* pPipe = CAnalysisPipeline::instance();
  CDataSource*       pSrc  = new CRecordEventDataSource;


  pPipe->setDataSource(pSrc);
  ASSERT(sourceReadableCalled);

  delete pSrc;
}

/**
 * preDecoderNoDecoder
 *   Before attaching a decoder, there should be no decoer.
 */
void
PipelineDecoderTests::preDecoderNoDecoder()
{
  CAnalysisPipeline* pPipe = CAnalysisPipeline::instance();

  EQ(reinterpret_cast<CDecoder*>(0), pPipe->m_pDecoder);
}
/**
 * decoderAttach
 *   After doing a call to setDecoder, there shouild be a non -null in m_pDecoder.
 */
class CNullDecoder : public CDecoder {
public:
  
  virtual void   onData(CDataSource* pSource) {}
  virtual size_t getItemSize()                 {return 0;}
  virtual void*  getItemPointer()            {return 0;}
  virtual bool   next()                      {return false;}
};
void
PipelineDecoderTests::decoderAttach()
{
  CAnalysisPipeline* pPipe = CAnalysisPipeline::instance();
  CDecoder* pDecoder = new CNullDecoder;
  pPipe->setDecoder(pDecoder);
  
  EQ(pDecoder, pPipe->m_pDecoder);
  
  delete pDecoder;
}

/**
 * haveData
 *
 *   This is a rather big test.  It verifies the top level parts
 *   of the interaction diagram that shows how data from the
 *   data sourcde are handled.
 *   Specifically:
 *     *  The pipeline's sourceReadable is called indicating that data is ready.
 *     *  The pipeline calls the decoder's onData method.
 *     *  The pipeline calls getItemPointer and getItem size to get information 
 *        about the item.
 *     *  The pipeline calls next and repeats the above until next returns false.
 */


class StatsDecoder : public CDecoder
{
public:
  int m_nEvents;
  int m_onData;
  int m_getItemSize;
  int m_getItemPointer;
  int m_next;

public:
  StatsDecoder(int nEvents = 1) :
    m_nEvents(nEvents),
    m_onData(0), m_getItemSize(0), m_getItemPointer(0), m_next(0) {}

  virtual void   onData(CDataSource* pSource) {m_onData++;}
  virtual size_t getItemSize()               {
    m_getItemSize++; 
    return sizeof(*this);
  }
  virtual void*  getItemPointer()            {
    m_getItemPointer++; 
    if (m_nEvents == 0) return 0; // EOF.
    m_nEvents--;
    return this;

  }
  virtual bool   next()                      {
    m_next++; 
    return m_nEvents != 0;
  }
};

void
PipelineDecoderTests::haveData()
{
  StatsDecoder*      pDecoder = new StatsDecoder;
  CDataSource*       pSource  = new CNullDataSource;
  CAnalysisPipeline* pPipe    = CAnalysisPipeline::instance();

  pPipe->setDataSource(pSource);
  pPipe->setDecoder(pDecoder);

  // Start the ball rolling:

  pPipe->sourceReadable(pSource, pPipe); // We know client data is actually the pipe.

  EQ(1, pDecoder->m_onData);
  EQ(1, pDecoder->m_getItemSize);
  EQ(1, pDecoder->m_getItemPointer);
  EQ(1, pDecoder->m_next);

  delete pDecoder;
  delete pSource;
}
/**
 * haveMultipleChunks
 *
 *   Make sure that the event processing pipeline processes all chunks of data
 *   before returning.
 */
void
PipelineDecoderTests::haveMultipleChunks()
{
  StatsDecoder*      pDecoder = new StatsDecoder(10);
  CDataSource*       pSource  = new CNullDataSource;
  CAnalysisPipeline* pPipe    = CAnalysisPipeline::instance();

  pPipe->setDataSource(pSource);
  pPipe->setDecoder(pDecoder);

  // Start the ball rolling:

  pPipe->sourceReadable(pSource, pPipe); // We know client data is actually the pipe.


  EQ(1, pDecoder->m_onData);
  EQ(10, pDecoder->m_getItemSize);
  EQ(10, pDecoder->m_getItemPointer);
  EQ(10, pDecoder->m_next);

  delete pDecoder;
  delete pSource;
  
}
/** 
 * sourceEof
 *
 * Check that the data source gets closed on EOF.
 * We'll use the deleterecording data source but only 
 * care that it gets closed.
 */
void
PipelineDecoderTests::sourceEof()
{
  StatsDecoder*      pDecoder = new StatsDecoder(10);
  CDataSource*       pSource  = new CDeleteRecordingDataSource;
  CAnalysisPipeline* pPipe    = CAnalysisPipeline::instance();

  pPipe->setDataSource(pSource);
  pPipe->setDecoder(pDecoder);

  // Start the ball rolling:

  pPipe->sourceReadable(pSource, pPipe); // This will process 10 items.
  pPipe->sourceReadable(pSource, pPipe); // Should flag EOF.

  ASSERT(closed);

}
/**
 * initiallyEmpty
 *
 *  The event processing pipeline should be initially empty.
 *  this means thtat m_pipeline.empty() is true and
 *  size() is 0.
 */
void
PipelineDecoderTests::initiallyEmpty()
{
  CAnalysisPipeline* pPipe = CAnalysisPipeline::instance();

  ASSERT(pPipe->m_pipeline.empty());
  EQ(static_cast<size_t>(0), pPipe->size());
}
/**
 * add1
 *   Add a single element to the event processing pipeline.
 *   *   Size -> 1
 *   *   Not empty
 *   *   Peeking at the front should give the element we inserted.
 */
class NullEventProcessor : public CEventProcessor
{
public:
  NullEventProcessor(const char* pName = 0) :
    CEventProcessor(pName) {}
  bool onEvent(void* pEvent, size_t nBytes, CAnalysisPipeline* pipeline) {
    return true;
  }
};
void
PipelineDecoderTests::add1()
{
  NullEventProcessor* pProcessor = new NullEventProcessor("null");
  CAnalysisPipeline*  pPipe      = CAnalysisPipeline::instance();

  pPipe->addElement(pProcessor);

  ASSERT(!pPipe->m_pipeline.empty());
  EQ(static_cast<size_t>(1), pPipe->size());

  EQ(reinterpret_cast<CEventProcessor*>(pProcessor), pPipe->m_pipeline.front());
}
/**
 * add2
 *
 *   Adds two elements and ensures the second one got put on the back of the
 *   pipeline.
 */
void
PipelineDecoderTests::add2()
{
  NullEventProcessor* p1    = new NullEventProcessor("null1");
  NullEventProcessor* p2    = new NullEventProcessor("null2");
  CAnalysisPipeline*  pPipe = CAnalysisPipeline::instance();

  pPipe->addElement(p1);
  pPipe->addElement(p2);

  EQ(reinterpret_cast<CEventProcessor*>(p1), pPipe->m_pipeline.front());
  EQ(reinterpret_cast<CEventProcessor*>(p2), pPipe->m_pipeline.back());
  

}
/**
 * addmid
 *
 *  The iterator add should allow us to put an item in the middle
 *  of the pipeline.
 */
void
PipelineDecoderTests::addmid()
{
  NullEventProcessor* p1    = new NullEventProcessor("null1");
  NullEventProcessor* p2    = new NullEventProcessor("null2");
  NullEventProcessor* pMid  = new NullEventProcessor("mid");

  CAnalysisPipeline*  pPipe = CAnalysisPipeline::instance();

  pPipe->addElement(p1);
  pPipe->addElement(p2);

  CAnalysisPipeline::PipelineIterator p = pPipe->m_pipeline.begin();
  p++;
  pPipe->addElement(pMid, p);

  EQ(static_cast<size_t>(3), pPipe->m_pipeline.size());

  // Check element order

  p = pPipe->m_pipeline.begin();
  EQ(std::string("null1"), (*p)->getName());
  p++;
  EQ(std::string("mid"), (*p)->getName());
  p++;
  EQ(std::string("null2"), (*p)->getName());
  
}
/**
 * addsearch
 *   Adds an element prior to the named element.
 *
 *  This version tests the successful branch.
 */
void
PipelineDecoderTests::addsearch()
{
  NullEventProcessor* p1    = new NullEventProcessor("null1");
  NullEventProcessor* p2    = new NullEventProcessor("null2");
  NullEventProcessor* pMid  = new NullEventProcessor("mid");

  CAnalysisPipeline*  pPipe = CAnalysisPipeline::instance();

  pPipe->addElement(p1);
  pPipe->addElement(p2);

  pPipe->addElement(pMid, "null2");

  EQ(static_cast<size_t>(3), pPipe->m_pipeline.size());

  // Check element order

  CAnalysisPipeline::PipelineIterator p = pPipe->m_pipeline.begin();
  EQ(std::string("null1"), (*p)->getName());
  p++;
  EQ(std::string("mid"), (*p)->getName());
  p++;
  EQ(std::string("null2"), (*p)->getName());
}
/**
 * addsearchfail
 *
 *  A search add when the target pipeline element does not
 *  exist should throw an exception and not change the pipeline.
 */
void
PipelineDecoderTests::addsearchfail()
{
  NullEventProcessor* p1    = new NullEventProcessor("null1");
  NullEventProcessor* p2    = new NullEventProcessor("null2");
  NullEventProcessor* pMid  = new NullEventProcessor("mid");

  CAnalysisPipeline*  pPipe = CAnalysisPipeline::instance();

  pPipe->addElement(p1);
  pPipe->addElement(p2);


  bool thrown(false);
  bool rightThrow(false);
  try {
    pPipe->addElement(pMid, "does not exist");
  }
  catch (analysis_pipeline_exception& e) {
    thrown = true;
    rightThrow = true;
  }
  catch (...) {
    thrown = true;
  }

  ASSERT(thrown);
  ASSERT(rightThrow);

  EQ(static_cast<size_t>(2), pPipe->m_pipeline.size());

  // Check element order

  CAnalysisPipeline::PipelineIterator p = pPipe->m_pipeline.begin();
  EQ(std::string("null1"), (*p)->getName());
  p++;
  EQ(std::string("null2"), (*p)->getName());
}
/**
 * begin
 *   Ensure the begin method 'points' to the first pipeline element.
 *   and matches the m_pipeline.begin() result.
 */
void
PipelineDecoderTests::begin()
{
  NullEventProcessor* p1    = new NullEventProcessor("null1");
  NullEventProcessor* p2    = new NullEventProcessor("null2");

  CAnalysisPipeline*  pPipe = CAnalysisPipeline::instance();

  pPipe->addElement(p1);
  pPipe->addElement(p2);

  CAnalysisPipeline::PipelineIterator p = pPipe->begin();

  EQ(reinterpret_cast<CEventProcessor*>(p1), *p);

}
/**
 * end
 *  ensure the end() method returns an iterator just past the end.
 *  of the pipeline.
 */
void
PipelineDecoderTests::end()
{
  NullEventProcessor* p1    = new NullEventProcessor("null1");
  NullEventProcessor* p2    = new NullEventProcessor("null2");

  CAnalysisPipeline*  pPipe = CAnalysisPipeline::instance();

  pPipe->addElement(p1);
  pPipe->addElement(p2);

  CAnalysisPipeline::PipelineIterator p = pPipe->end();
  p--;

  EQ(reinterpret_cast<CEventProcessor*>(p2), *p);
}
/**
 * size
 *   ensure the size member returns the number of pipeline elements.
 */
void
PipelineDecoderTests::size()
{
  NullEventProcessor* p1    = new NullEventProcessor("null1");
  NullEventProcessor* p2    = new NullEventProcessor("null2");

  CAnalysisPipeline*  pPipe = CAnalysisPipeline::instance();

  EQ(static_cast<size_t>(0), pPipe->size());

  pPipe->addElement(p1);
  EQ(static_cast<size_t>(1), pPipe->size());
  pPipe->addElement(p2);
  EQ(static_cast<size_t>(2), pPipe->size());

  
}
/**
 * find
 *   See that find can locate an element and tell that an element does not exist.
 *   NOTE EQ can't be used for the end comparison because that requires objects
 *   that work with std::ostream::operator<<
 */
void
PipelineDecoderTests::find()
{
  NullEventProcessor* p1    = new NullEventProcessor("null1");
  NullEventProcessor* p2    = new NullEventProcessor("null2");
  NullEventProcessor* pMid  = new NullEventProcessor("mid");

  CAnalysisPipeline*  pPipe = CAnalysisPipeline::instance();

  pPipe->addElement(p1);
  pPipe->addElement(pMid);
  pPipe->addElement(p2);

  CAnalysisPipeline::PipelineIterator p;

  p = pPipe->find("null1");
  EQ(reinterpret_cast<CEventProcessor*>(p1), *p);
  p = pPipe->find("null2");
  EQ(reinterpret_cast<CEventProcessor*>(p2), *p);

  p = pPipe->find("does not exist");
  ASSERT(p == pPipe->end());
}
/**
 * erase
 *   removing an item should:
 *   *  Decrease the size.
 *   *  Make the item un-findable.
 */
void
PipelineDecoderTests::erase()
{
  NullEventProcessor* p1    = new NullEventProcessor("null1");
  NullEventProcessor* p2    = new NullEventProcessor("null2");
  NullEventProcessor* pMid  = new NullEventProcessor("mid");

  CAnalysisPipeline*  pPipe = CAnalysisPipeline::instance();

  pPipe->addElement(p1);
  pPipe->addElement(pMid);
  pPipe->addElement(p2);

  // locate mid and erase it:

  CAnalysisPipeline::PipelineIterator p = pPipe->find("mid");
  ASSERT(p != pPipe->end());
  pPipe->erase(p);

  EQ(static_cast<size_t>(2), pPipe->size());
  p = pPipe->find("mid");
  ASSERT(p == pPipe->end());

  delete pMid;			// since teardown won't see it anymore.
}
/**
 * dispatch
 *
 *   This test ensures that an event chunk will
 *   * Be dispatched to all pipeline elements.
 *   * The order of dispatch matches the insertion order.
 */
class RecordingEventProcessor : public CEventProcessor
{
public:
  static std::vector<std::string> m_NameOrder;

  RecordingEventProcessor(const char* pName) : CEventProcessor(pName) 
  {
    m_NameOrder.clear();	// They get created in clumps so this is ok.
  }
  virtual bool onEvent(void* pEvent, size_t n, CAnalysisPipeline* pipe) {
    m_NameOrder.push_back(getName());
    return true;
  }
};
std::vector<std::string> RecordingEventProcessor::m_NameOrder;

void
PipelineDecoderTests::dispatch()
{
  CAnalysisPipeline* pPipe = CAnalysisPipeline::instance();
  RecordingEventProcessor* p1 = new RecordingEventProcessor("first");
  RecordingEventProcessor* p2 = new RecordingEventProcessor("second");
  RecordingEventProcessor* p3 = new RecordingEventProcessor("last");

  pPipe->addElement(p1);
  pPipe->addElement(p2);
  pPipe->addElement(p3);

  // set up the decoder and the data source:

  StatsDecoder decoder;
  pPipe->setDecoder(&decoder);
  CNullDataSource source;
  pPipe->setDataSource(&source);

  // Process the one event we have:

  pPipe->sourceReadable(&source, pPipe);

  // Check the m_NameOrder values

  EQ(static_cast<size_t>(3), RecordingEventProcessor::m_NameOrder.size());
  EQ(std::string("first"), RecordingEventProcessor::m_NameOrder[0]);
  EQ(std::string("second"), RecordingEventProcessor::m_NameOrder[1]);
  EQ(std::string("last"), RecordingEventProcessor::m_NameOrder[2]);
  
  

}
/**
 * dispatchelementfailed
 *
 *  Pipeline dispatch where one element fails by returning false.... to abort pipeline processing.
 */
class FailingElement : public CEventProcessor
{
public:
  FailingElement(const char* name) : CEventProcessor(name) {}
  bool onEvent(void* pData, size_t nBytes, CAnalysisPipeline* pPipe) {
    return false;
  }
};

void
PipelineDecoderTests::dispatchelementfailed()
{
  CAnalysisPipeline* pPipe = CAnalysisPipeline::instance();
  RecordingEventProcessor* p1 = new RecordingEventProcessor("first");
  RecordingEventProcessor* p2 = new RecordingEventProcessor("second");
  FailingElement*          pF = new FailingElement("fails");
  RecordingEventProcessor* p3 = new RecordingEventProcessor("last");

  pPipe->addElement(p1);
  pPipe->addElement(p2);
  pPipe->addElement(pF);
  pPipe->addElement(p3);
  // set up the decoder and the data source:

  StatsDecoder decoder;
  pPipe->setDecoder(&decoder);
  CNullDataSource source;
  pPipe->setDataSource(&source);

  // Process the one event we have:

  pPipe->sourceReadable(&source, pPipe);

  // We should only see two element, 'first' and 'second'.

  EQ(static_cast<size_t>(2),  RecordingEventProcessor::m_NameOrder.size());
  EQ(std::string("first"), RecordingEventProcessor::m_NameOrder[0]);
  EQ(std::string("second"), RecordingEventProcessor::m_NameOrder[1]);

  
}
