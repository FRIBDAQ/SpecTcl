// Template for a test suite.

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Asserter.h>
#include "Asserts.h"
#include "CSqliteWhere.h"
#include <stdexcept>

class FilterTests : public CppUnit::TestFixture {
  CPPUNIT_TEST_SUITE(FilterTests);
  CPPUNIT_TEST(rawTest);
  
  // RelationToNumberFilte ris also used to est CBinaryRelationFilter:
  
  CPPUNIT_TEST(feqNum);
  CPPUNIT_TEST(fneNum);
  CPPUNIT_TEST(fgtNum);
  CPPUNIT_TEST(fltNum);
  CPPUNIT_TEST(fgeNum);
  CPPUNIT_TEST(fleNum);
  
  // Subsequent binary relationships only need to look at the rhs method.
  
  CPPUNIT_TEST(stringRhs);
  CPPUNIT_TEST(fieldRhs);
  
  CPPUNIT_TEST(comp1);
  CPPUNIT_TEST(comp2);
  CPPUNIT_TEST(comp3);
  
  CPPUNIT_TEST(in0);
  CPPUNIT_TEST(in1num);
  CPPUNIT_TEST(in2num);
  CPPUNIT_TEST(in3num);
  CPPUNIT_TEST(in3str);
  CPPUNIT_TEST(invecnum);
  CPPUNIT_TEST(invecstr);
  CPPUNIT_TEST_SUITE_END();


private:

public:
  void setUp() {
  }
  void tearDown() {
  }
protected:
  void rawTest();
  
  void feqNum();
  void fneNum();
  void fgtNum();
  void fltNum();
  void fgeNum();
  void fleNum();
  
  void stringRhs();
  void fieldRhs();
  
  void comp1();
  void comp2();
  void comp3();
  void compvec();
  
  void in0();
  void in1num();
  void in2num();
  void in3num();
  void in3str();
  void invecnum();
  void invecstr();
};

CPPUNIT_TEST_SUITE_REGISTRATION(FilterTests);

// Instantiating a raw filter gives us the input string back

void FilterTests::rawTest() {
  std::string input="this IN (1,2,3) AND id IN (SELECT fk_id FROM other_table WHERE cost > 12.34";
  CRawFilter f(input);
  EQ(input, f.toString());
}

// Field = number:

void FilterTests::feqNum()
{
  CRelationToNumberFilter f("afield", CBinaryRelationFilter::equal, 1234);
  
  EQ(std::string("afield = 1234"), f.toString());
}

// field <> number.

void FilterTests::fneNum()
{
  CRelationToNumberFilter f("afield", CBinaryRelationFilter::notEqual, 1234);
  
  EQ(std::string("afield <> 1234"), f.toString());
}
// field < number

void FilterTests::fltNum()
{
  CRelationToNumberFilter f("afield", CBinaryRelationFilter::lt, 1234);
  
  EQ(std::string("afield < 1234"), f.toString());
}
// field > number

void FilterTests::fgtNum()
{
  CRelationToNumberFilter f("afield", CBinaryRelationFilter::gt, 1234);
  
  EQ(std::string("afield > 1234"), f.toString());
}

// field <= number:

void FilterTests::fleNum()
{
  CRelationToNumberFilter f("afield", CBinaryRelationFilter::le, 1234);
  
  EQ(std::string("afield <= 1234"), f.toString());
}
// field >= number
void FilterTests::fgeNum()
{
  CRelationToNumberFilter f("afield", CBinaryRelationFilter::ge, 1234);
  
  EQ(std::string("afield >= 1234"), f.toString());
}
// field = 'string'.

void FilterTests::stringRhs()
{
  CRelationToStringFilter f("afield", CBinaryRelationFilter::equal, "Some string");
  EQ(std::string("'Some string'"), f.rhs());
}
// field1 = field2 - field2 is not quoted.  Note that field2 could also be a
//                   a valid function.

void FilterTests::fieldRhs()
{
  CRelationBetweenFieldsFilter f("afield", CBinaryRelationFilter::equal, "anotherField");
  EQ(std::string("anotherField"), f.rhs());
}

// Compount 1 element - fails with std::range_error

void FilterTests::comp1()
{
  CRelationToNumberFilter f1("field1", CBinaryRelationFilter::equal, 100);
  CCompoundFilter          c("AND");
  c.addClause(f1);
  CPPUNIT_ASSERT_THROW(
    c.toString(),
    std::range_error
  );
}
// Compound 2 elements  " (el1) AND (el2) "

void FilterTests::comp2()
{
  CRelationToNumberFilter f1("field1", CBinaryRelationFilter::equal, 100);
  CRelationToNumberFilter f2("field2", CBinaryRelationFilter::notEqual, 200);
  CCompoundFilter          c("AND");
  c.addClause(f1);
  c.addClause(f2);
  
  std::string where;
  CPPUNIT_ASSERT_NO_THROW(
    where = c.toString()
  );
  
  EQ(std::string(" (field1 = 100) AND (field2 <> 200) "), where);
}
// Compound 3 elements " (el1) AND (el2) AND (el3) "

void FilterTests::comp3()
{
  CRelationToNumberFilter f1("field1", CBinaryRelationFilter::equal, 100);
  CRelationToNumberFilter f2("field2", CBinaryRelationFilter::notEqual, 200);
  CRelationToNumberFilter f3("field3", CBinaryRelationFilter::gt, 150);
  
  CCompoundFilter          c("AND");
  c.addClause(f1);
  c.addClause(f2);
  c.addClause(f3);
  
  EQ(std::string(" (field1 = 100) AND (field2 <> 200) AND (field3 > 150) "), c.toString());
}

// Construct with a pre-build vector of stuff.

void FilterTests::compvec()
{
  CRelationToNumberFilter f1("field1", CBinaryRelationFilter::equal, 100);
  CRelationToNumberFilter f2("field2", CBinaryRelationFilter::notEqual, 200);
  CRelationToNumberFilter f3("field3", CBinaryRelationFilter::gt, 150);
  std::vector<CQueryFilter*> filters;
  filters.push_back(&f1); filters.push_back(&f2); filters.push_back(&f3);
  
  CCompoundFilter c("AND", filters);
  
  EQ(std::string(" (field1 = 100) AND (field2 <> 200) AND (field3 > 150) "), c.toString());
}

// IN filter fails if there' no items:

void FilterTests::in0()
{
  CInFilter f("afield");
  CPPUNIT_ASSERT_THROW(
    f.toString(),
    std::range_error
  );
}

// 1 item produces: IN (item)

void FilterTests::in1num()
{
  CInFilter f("afield");
  f.addItem(0);
  std::string result;
  CPPUNIT_ASSERT_NO_THROW(
    result = f.toString()
  );
  
  EQ(std::string("afield IN (0)"), result);
}

// two items: IN (i1, i2).

void FilterTests::in2num()
{
  CInFilter f("afield");
  f.addItem(0);
  f.addItem(1);
  EQ(std::string("afield IN (0, 1)"), f.toString());
}
void FilterTests::in3num()
{
  CInFilter f("afield");
  f.addItem(0);
  f.addItem(1);
  f.addItem(2);
  
  EQ(std::string("afield IN (0, 1, 2)"), f.toString());
}

// See if strings get properly quoted:

void FilterTests::in3str()
{
  CInFilter f("afield");
  f.addItem("one");
  f.addItem("two");
  f.addItem("three");
  EQ(std::string("afield IN ('one', 'two', 'three')"), f.toString());
}
// instantiate with a vector of numbers:

void FilterTests::invecnum()
{
  std::vector<double> values = {1,2,3};
  CInFilter f("afield", values);
  
  EQ(std::string("afield IN (1, 2, 3)"), f.toString());
}
// Vector of strings:

void FilterTests::invecstr()
{
  std::vector<std::string> values = {"one", "two", "three"};
  CInFilter f("afield", values);
  
  EQ(std::string("afield IN ('one', 'two', 'three')"), f.toString());  
}
