// Template for a test suite.
#include <config.h>
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Asserter.h>
#include "Asserts.h"
#include "buffer.h"
#include "buftypes.h"

#ifdef HAVE_STD_NAMESPACE
using namespace std;
#endif
#include <histotypes.h>
#include <TCLInterpreter.h>
#include <TCLVariable.h>
#include "DocumentationCallback.h"
#include <VariableTraceCallback.h>

#include <string.h>
#include <string>

struct buffer {
  bheader header;
  char   body[8192];		// size not really important.
};

class callbacks : public CppUnit::TestFixture {
  CPPUNIT_TEST_SUITE(callbacks);
  CPPUNIT_TEST(getvariables);
  CPPUNIT_TEST(decode);
  CPPUNIT_TEST(Callbacks);
  CPPUNIT_TEST_SUITE_END();


private:
  CDocumentationCallback*   m_pCallback;
  buffer*                   m_pBuffer;

public:
  void setUp() {
    m_pCallback = new CDocumentationCallback;
    m_pBuffer   = new buffer;
    SetupBuffer();
  }
  void tearDown() {
    delete m_pBuffer;
    delete m_pCallback;
  }
protected:
  void getvariables();
  void decode();
  void Callbacks();
private:
  void SetupBuffer();
};

static  const char* s1 = "set atest testing\n";
static  const char* s2 = "set test(atest) moretests\n";


class tracer : public CVariableTraceCallback
{
public:
  static int times;
  string m_var;
  string m_element;
  int    m_flags;

  virtual  char* operator()(CTCLInterpreter* pInterp,
			 char* pVariable,
			 char* pElement,
			 int   flags) { 
    m_var = pVariable;
    if(pElement) {
      m_element = (pElement);
    } else {
      m_element ="";
    }
    m_flags = flags;
    times++;
    return NULL;
  }
};

int tracer::times;


void
callbacks::SetupBuffer()
{

  m_pBuffer->header.type = RUNVARBF;
  m_pBuffer->header.nevt = 2;		// 2 strings for now.
  m_pBuffer->header.ssignature = 0x0102;
  m_pBuffer->header.lsignature = 0x01020304;

  char* ptr = m_pBuffer->body;
  strcpy(ptr, s1);
  ptr += strlen(s1);
  *ptr++ = '\0';			// At least one null termination.
  if( (strlen(s1) + 1) % 2) {	// if odd size with one null...
    *ptr++ = '\0';		// padd with 2 nulls.
  }

  strcpy(ptr, s2);
  ptr += strlen(s2);
  *ptr++ = '\0';			// At least one null termination.
  if( (strlen(s2) + 1) % 2) {	// if odd size with one null...
    *ptr++ = '\0';		// padd with 2 nulls.
  }


}





CPPUNIT_TEST_SUITE_REGISTRATION(callbacks);

void 
callbacks::getvariables() {
  CTCLInterpreter* pI = m_pCallback->getInterpreter();
  CTCLVariable atest(pI, "atest", kfFALSE);
  CTCLVariable testing(pI, "testing", kfFALSE);

  atest.Set("avalue");
  testing.Set("atest", "bvalue");

  EQ(string("avalue"), m_pCallback->getValue("atest"));
  EQ(string("bvalue"), m_pCallback->getElementValue("testing", "atest"));


}

void
callbacks::decode()
{
  (*m_pCallback)(RUNVARBF, m_pBuffer);
  EQ(string("testing"), m_pCallback->getValue("atest"));
  EQ(string("moretests"), m_pCallback->getElementValue("test", "atest"));
}

void
callbacks::Callbacks()
{
  tracer::times = 0;
  
  tracer trace1;
  m_pCallback->addChangeHandler("atest", trace1);
  m_pCallback->addChangeHandler("test(atest)", trace1);
  m_pCallback->addChangeHandler("atest", trace1);
  
  m_pCallback->removeChangeHandler("atest", trace1);

  // At this time I should have a single trace on each of the two vars.

  (*m_pCallback)(RUNVARBF, m_pBuffer); // Should fire two traces...

  EQ(2, tracer::times);

}
