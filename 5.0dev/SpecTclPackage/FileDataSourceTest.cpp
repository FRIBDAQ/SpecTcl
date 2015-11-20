/* Test data */

// Template for a test suite.

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Asserter.h>

#include <exception>
#include <TCLInterpreter.h>
#include <TCLObject.h>

#include "Asserts.h"

#define private public		// So we can peek into the data source innards.
#include "CFileDataSource.h"



#undef private

#include <stdint.h>
#include <sys/stat.h>
#include <tcl.h>
#include <string>




class FileDataSourceTest : public CppUnit::TestFixture {
  CPPUNIT_TEST_SUITE(FileDataSourceTest);
  CPPUNIT_TEST(constructTest);

  CPPUNIT_TEST(attachOk);
  CPPUNIT_TEST(attachFail);

  CPPUNIT_TEST(establishHandler);
  CPPUNIT_TEST(handleEvent);
  CPPUNIT_TEST(handleRightParameter);
  CPPUNIT_TEST(handleClientData);

  CPPUNIT_TEST(readok);
  CPPUNIT_TEST(readfail);
  CPPUNIT_TEST(readeofzero);

  CPPUNIT_TEST(closetest);
  CPPUNIT_TEST(closeAlreadyClosed);

  CPPUNIT_TEST(eof);
  CPPUNIT_TEST(reopen);
  CPPUNIT_TEST_SUITE_END();


private:
  std::string m_hier;		// A possible URL tail:
public:
  void setUp() {
    m_hier = "//";		// Always leading with //.
    m_hier += __FILE__;
  }
  void tearDown() {
  }
protected:
  void constructTest();

  void attachOk();
  void attachFail();

  void establishHandler();
  void handleEvent();
  void handleRightParameter();
  void handleClientData();

  void readok();
  void readfail();
  void readeofzero();

  void closetest();
  void closeAlreadyClosed();

  void eof();
  void reopen();
private:
  void readFile(CFileDataSource& file, size_t nBytes);
};

CPPUNIT_TEST_SUITE_REGISTRATION(FileDataSourceTest);

void
FileDataSourceTest::readFile(CFileDataSource& file, size_t nBytes)
{
  while(nBytes) {
    char buffer[1024];
    size_t readSize = sizeof(buffer);
    if (nBytes < readSize) readSize = nBytes;
    int bytesRead = file.read(buffer, readSize);
    nBytes -= bytesRead;
  }

}


// Construction should initialize the data appropriately.

void FileDataSourceTest::constructTest() {

  CFileDataSource file;

  EQ(Tcl_Channel(0), file.m_fd);
  EQ(false, file.m_eof);

}
//  Attachment:  The pSource parameter is the name of the file (stuff after file://).
//  we'll hand our source filename which should open ok and:
//  - Should not get an exception.
//  - fd should be > 0.
//  - eof should be  false
//
void
FileDataSourceTest::attachOk()
{
  CTCLInterpreter interp;
  CFileDataSource file;
  CPPUNIT_ASSERT_NO_THROW(file.onAttach(interp, m_hier.c_str()));
  ASSERT(file.m_fd != 0);
  EQ(false, file.m_eof);

  
}
// Attachment to /no/such/file/exists.evt
// Should throw a error_condition exception type.

void 
FileDataSourceTest::attachFail()
{
  CTCLInterpreter interp;
  CFileDataSource file;
  bool thrown = false;
  bool rightExcept = false;
  try {
    file.onAttach(interp, "/no/such/file/exists.evt");
  }
  catch (std::exception& err) {
    thrown = true;
    rightExcept = true;
  }
  catch (...) {
    thrown = true;
    rightExcept = false;
  }
  ASSERT(thrown);
  ASSERT(rightExcept);
  
}

/**
 * establishHandler  - when we do this, m_pHandler should be nonnull.
 */

static void dummyHandler(CDataSource* pSource, void* pData) {
}

void 
FileDataSourceTest::establishHandler()
{
  CTCLInterpreter interp;
  CFileDataSource file;



  file.onAttach(interp, m_hier.c_str());
  file.createEvent(interp, dummyHandler);


  EQ(&dummyHandler, file.m_pHandler);
}
/**
 *
 * Handle the file event:
 */
static  int handled(0);
static void counterHandler(CDataSource* pSource, void* pData) {
  handled++;
}

void
FileDataSourceTest::handleEvent()
{
  CTCLInterpreter interp;
  CFileDataSource file;
  
  file.onAttach(interp, m_hier.c_str());
  file.createEvent(interp, counterHandler);

  
  ASSERT(Tcl_DoOneEvent(TCL_DONT_WAIT) != 0);

  EQ(1, handled);



}
/**
 * handleRightParameter
 *  Ensure the handler gets a pointer to the data source as a parameter.
 */
static void* pParam(0);
static void paramHandler(CDataSource* pSource, void* pData) 
{
  pParam = reinterpret_cast<void*>(pSource);
}

void
FileDataSourceTest::handleRightParameter()
{
  CTCLInterpreter interp;
  CFileDataSource file;
  
  file.onAttach(interp, m_hier.c_str());
  file.createEvent(interp, paramHandler);

  
  ASSERT(Tcl_DoOneEvent(TCL_DONT_WAIT) != 0);

  EQ(reinterpret_cast<void*>(&file), pParam);
}

/**
 * handleClientData
 *
 *  Ensure the right client data is passed to the handler.
 */
static FileDataSourceTest* pClientData;
static void clientDataHandler(CDataSource* pSource, void* pData)
{
  pClientData = reinterpret_cast<FileDataSourceTest*>(pData);
}

void
FileDataSourceTest::handleClientData()
{
  CTCLInterpreter interp;
  CFileDataSource file;
  
  file.onAttach(interp, m_hier.c_str());
  file.createEvent(interp, clientDataHandler, this);

  
  ASSERT(Tcl_DoOneEvent(TCL_DONT_WAIT) != 0);

  EQ(this, pClientData);
}

/**
 * readok
 *    Read on a file data source opened on __FILE__ first 2 characters should be
 ** readable and be /*
 */
void
FileDataSourceTest::readok()
{
  CTCLInterpreter interp;
  CFileDataSource file;

  file.onAttach(interp, m_hier.c_str());

  char data[2];

  int nread = file.read(data, 2);
  EQ(2, nread);
  EQ('/', data[0]);
  EQ('*', data[1]);
}

/**
 * readfail
 *   Read on an unconnected file descriptor should throw an error:
 */
void
FileDataSourceTest::readfail()
{
  CTCLInterpreter interp;
  CFileDataSource file;

  bool thrown(false);
  bool rightException(false);
  
  try {
    char buffer[1];
    int result = file.read(buffer, 1);
  }
  catch(data_source_exception& e) {
    thrown = true;
    rightException = true;
  }
  catch(...) {
    thrown = true;
  }
  ASSERT(thrown);
  ASSERT(rightException);
 
}
/**
 * readeofzero
 *    Reading the end of file must give a zero result.
 */
void
FileDataSourceTest::readeofzero()
{
  CTCLInterpreter interp;
  CFileDataSource file;
  CTCLObject      fileName;
  fileName.Bind(interp);
  fileName = __FILE__;


  file.onAttach(interp, m_hier.c_str());
  uint64_t nBytes;
  Tcl_StatBuf* sbuf= Tcl_AllocStatBuf();;
  Tcl_FSStat(fileName.getObject(), sbuf);
  nBytes = sbuf->st_size;

  readFile(file, nBytes);


  // SB at end file now:

  char buf;
  EQ(static_cast<size_t>(0), file.read(&buf,1));

  Tcl_Free(reinterpret_cast<char*>(sbuf));

}


/**
 * closetest
 *
 *   Closing a device should set m_fd to zero and m_eof to false.
 *   should also clear the interpreter field.
 */
void
FileDataSourceTest::closetest()
{
  CTCLInterpreter interp;
  CFileDataSource file;
  file.onAttach(interp, m_hier.c_str());	//  Open..
  file.close();			// close right away:

  EQ(reinterpret_cast<Tcl_Channel>(0), file.m_fd);
  EQ(false, file.m_eof);
  EQ(reinterpret_cast<CTCLInterpreter*>(0), file.m_pInterp);
}

/**
 * closeAlreadyClosed
 *
 *  Close an already closed channel (not opened yet).
 *  should throw an exception.
 */
void
FileDataSourceTest::closeAlreadyClosed()
{
  CFileDataSource file;
  
  bool threw=false;
  bool rightException=false;

  try {
    file.close();
  }
  catch (data_source_exception& e) {
    threw = true;
    rightException = true;
  }
  catch(...) {
    threw = true;
  }

  ASSERT(threw);
  ASSERT(rightException);
}
/**
 * eof
 *
 * After reading the file we should be able to see the isEof true
 */
void
FileDataSourceTest::eof()
{
  CTCLInterpreter interp;
  CFileDataSource file;
  CTCLObject      fileName;
  fileName.Bind(interp);
  fileName = __FILE__;


  file.onAttach(interp, m_hier.c_str());
  uint64_t nBytes;
  Tcl_StatBuf* sbuf = Tcl_AllocStatBuf();
  Tcl_FSStat(fileName.getObject(), sbuf);
  nBytes = sbuf->st_size;

  readFile(file, nBytes);

  // trigger end file condition:

  char b;
  file.read(&b, sizeof(char));

  ASSERT(file.isEof());
  Tcl_Free(reinterpret_cast<char*>(sbuf));
}

/**
 * reopen
 *
 * after reading to eof and then reopening the eof condition should be clear:
 */
void
FileDataSourceTest::reopen()
{
  CTCLInterpreter interp;
  CFileDataSource file;
  CTCLObject      fileName;
  fileName.Bind(interp);
  fileName = __FILE__;


  file.onAttach(interp, m_hier.c_str());
  uint64_t nBytes;
  Tcl_StatBuf* sbuf = Tcl_AllocStatBuf();
  Tcl_FSStat(fileName.getObject(), sbuf);
  nBytes = sbuf->st_size;

  readFile(file, nBytes);

  // trigger end file condition:

  char b;
  file.read(&b, sizeof(char));

  ASSERT(file.isEof());
  Tcl_Free(reinterpret_cast<char*>(sbuf));

  file.close();
  file.onAttach(interp, m_hier.c_str());
  ASSERT(!file.isEof());
}
