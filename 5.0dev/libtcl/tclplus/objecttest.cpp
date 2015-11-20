// Template for a test suite.

#include <config.h>
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Asserter.h>
#include "Asserts.h"
#include "TCLObject.h"
#include "TCLInterpreter.h"
#include "TCLList.h"

#include <tcl.h>


using namespace std;


class objects : public CppUnit::TestFixture {
  CPPUNIT_TEST_SUITE(objects);
  CPPUNIT_TEST(construction);
  CPPUNIT_TEST(assignments);
  CPPUNIT_TEST(listcast);
  CPPUNIT_TEST(lappend);
  CPPUNIT_TEST(range);
  CPPUNIT_TEST(call);
  CPPUNIT_TEST(concat);
  CPPUNIT_TEST(getElements);
  CPPUNIT_TEST(setList);
  CPPUNIT_TEST(llength);
  CPPUNIT_TEST(lindex);
  CPPUNIT_TEST(lreplace);
  CPPUNIT_TEST_SUITE_END();


private:
  Tcl_Interp*      m_pRawInterp;
  CTCLInterpreter* m_pInterp;
public:
  void setUp() {
    m_pRawInterp = Tcl_CreateInterp();
    m_pInterp    = new CTCLInterpreter(m_pRawInterp);
  }
  void tearDown() {
    delete m_pInterp;

  }
protected:
  void construction();
  void assignments();
  void listcast();
  void lappend();
  void range();
  void call();
  void concat();
  void getElements();
  void setList();
  void llength();
  void lindex();
  void lreplace();
};

CPPUNIT_TEST_SUITE_REGISTRATION(objects);

// Test construction...

void objects::construction() {
  Tcl_Obj* obj = Tcl_NewObj();
  CTCLObject objObject(obj);	// 

  EQMSG("Tcl_Obj*", obj, objObject.getObject());

  // Copy construction should result in the same object pointer:

  CTCLObject copy(objObject);
  EQMSG("copyConstruct", objObject.getObject(), copy.getObject());

}
// Test assignments... in particular, assignment should
// break the object into a second one... as well as making the
// new object contain the new data.
//
/// Note that this also tests conversions to string, int, double.
void objects::assignments()
{
  CTCLObject object;		//  New object... reference count is 1.
  CTCLObject second(object);	//  These share an object pointer now.

  object = "This is a test";	// This should split the objects.
  EQMSG("assign const char*", string("This is a test"), (string)object);
  ASSERT(object.getObject() != second.getObject()); // Check they were split.

  object = string("A string test");
  EQMSG("assign string", string("A string test"), (string)object);

  // To support integerization we need an interpreter:

  object.Bind(m_pInterp);
  object = 5;
  EQMSG("assign int", 5, (int)object);
  
  object = 3.14159;
  EQMSG("assign double", (double)3.14159, (double)object);

  Tcl_Obj* testing = Tcl_NewStringObj("Test object", -1);
  object = testing;
  EQMSG("assign object", string("Test object"), (string)object);

  CTCLList list(m_pInterp, "This list has several parts");
  object = list;
  EQMSG("Assign CTCLList", string("This list has several parts"), (string)object);
}
//  The test above got all casts but to CTCLList... this picks up that void:

void objects::listcast()
{
  CTCLObject object;
  object.Bind(m_pInterp);

  CTCLList list(m_pInterp, "This is a list");
  object = list;
  CTCLList copy = (CTCLList)object;	// Tests implicit conversion.
  StringArray elements;
  copy.Split(elements);

  EQMSG("count", (size_t)4, elements.size());
  EQMSG("first", string("This"), elements[0]);
  EQMSG("last",  string("list"), elements[3]);

}
// operator+= is like lappend.

void objects::lappend()
{
  CTCLObject list;
  list.Bind(m_pInterp);

  CTCLObject source;
  source = "this is";
  list  += source;
  EQMSG("+= object", string("{this is}"), string(list));

  list += 123;
  EQMSG("+= int", string("{this is} 123"), string(list));

  list += string("more data");
  EQMSG("+= string", string("{this is} 123 {more data}"), string(list));

  list += "word";
  EQMSG("+= char*", string("{this is} 123 {more data} word"), string(list));

  list += 1.234;
  EQMSG("+= double",  string("{this is} 123 {more data} word 1.234"), string(list));
}
// Test the getRange function
void objects::range()
{
  CTCLObject input;
  input = string("This is a test");
  input.Bind(m_pInterp);

  CTCLObject substring = input.getRange(0, 3);
  EQMSG("First word", string("This"), string(substring));

  substring=  input.getRange(5,6);
  EQMSG("second word", string("is"), string(substring));

  substring = input.getRange(5,8);
  EQMSG("2 words", string("is a"), string(substring));
}
// Test treating objects as scripts.
void objects::call()
{
  CTCLObject script;
  script.Bind(m_pInterp);	// scripts need an interpreter.
  script = "set a 4";
  CTCLObject result = script();
  result.Bind(m_pInterp);
  EQ(4, (int)result);
}
// Test list concatenation.

void objects::concat()
{
  CTCLObject lhs;
  CTCLObject rhs;

  lhs.Bind(m_pInterp);
  rhs.Bind(m_pInterp);

  lhs = "First elements";
  rhs = "second elements";
  lhs.concat(rhs);

  EQ(string("First elements second elements"), string(lhs));
}

// Get list elements into an object vector.

void objects::getElements()
{
  CTCLObject list;
  list.Bind(m_pInterp);
  list = "This {is a} test list";

  vector<CTCLObject> elements = list.getListElements();
  EQMSG("count", (size_t)4, elements.size());
  EQMSG("0", string("This"), (string)(elements[0]));
  EQMSG("1", string("is a"), (string)(elements[1]));
  EQMSG("2", string("test"), (string)(elements[2]));
  EQMSG("3", string("list"), (string)(elements[3]));
}
// Set list elements from an object vector.

void objects::setList()
{
  CTCLObject source;
  source.Bind(m_pInterp);
  source = "This {is a} test list";
  vector<CTCLObject> elements = source.getListElements(); // Cheap way to make input vector.
  
  CTCLObject dest;
  dest.Bind(m_pInterp);
  dest.setList(elements);

  EQ((string)source, (string)dest);
}
//  Get list length.

void objects::llength()
{
  CTCLObject list;
  list = "This {is a test} list";
  list.Bind(m_pInterp);

  EQ(3, list.llength());
}
// index into list.

void objects::lindex()
{
  CTCLObject source;
  source.Bind(m_pInterp);
  source = "This {is a} test list";
  vector<CTCLObject> elements = source.getListElements();

  for (int i =0; i < elements.size(); i++) {
    CTCLObject index;
    index = i;
    index.Bind(m_pInterp);
    EQMSG((string)(index), (string)elements[i], (string)source.lindex(i));
  }
  
}
// Replace sublist.

void objects::lreplace()
{
  CTCLObject source;
  source.Bind(m_pInterp);
  source = "This {is a} test list";

  CTCLObject replacement;
  replacement = "Replace {{is a} test} with this";
  replacement.Bind(m_pInterp);
  vector<CTCLObject> elements = replacement.getListElements();

  source.lreplace(1,2, elements);

  EQMSG("non-null replace", string("This Replace {{is a} test} with this list"), string(source));

  vector<CTCLObject> empty;
  source.lreplace(0,2,empty);
  EQMSG("lreplace as delete", string("{{is a} test} with this list"), string(source));
}
