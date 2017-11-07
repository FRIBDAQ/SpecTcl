// Template for a test suite.

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Asserter.h>
#include <Asserts.h>
#include "TreeBuilder.h"
#include <stdexcept>


class treefoldertests : public CppUnit::TestFixture {
  CPPUNIT_TEST_SUITE(treefoldertests);
  CPPUNIT_TEST(constructtest);
  CPPUNIT_TEST(isfoldertest);
  CPPUNIT_TEST(initiallyempty);
  
  CPPUNIT_TEST(add1);
  CPPUNIT_TEST(add2);
  CPPUNIT_TEST(adddup);
  
  CPPUNIT_TEST(contents);
  
  CPPUNIT_TEST(iter1);
  CPPUNIT_TEST(iter2);
  
  CPPUNIT_TEST(freestorage1);
  CPPUNIT_TEST(freestorage2);
  CPPUNIT_TEST_SUITE_END();



private:

public:
  void setUp() {
  }
  void tearDown() {
  }
protected:
  void constructtest();
  void isfoldertest();
  void initiallyempty();
  void add1();
  void add2();
  void adddup();
  
  void contents();
  
  void iter1();
  void iter2();
  
  void freestorage1();
  void freestorage2();
};

// Tree item we can insert/instantiate:

int freed(0);
class TreeTestItem : public TreeItemBaseClass
{
public:
  TreeTestItem(const char* name) : TreeItemBaseClass(name) {freed = 0;}
  ~TreeTestItem() {freed++;}
  bool isFolder() const { return false; }
};

CPPUNIT_TEST_SUITE_REGISTRATION(treefoldertests);

void treefoldertests::constructtest() {
  TreeFolder f("someName");
  EQ(std::string("someName"), f.getName());
}
void treefoldertests::isfoldertest()
{
  TreeFolder f("someName");
  ASSERT(f.isFolder());
}
void treefoldertests::initiallyempty()
{
  TreeFolder f("someName");
  EQ(size_t(0), f.getContents().size());
}
void treefoldertests::add1()
{
  TreeFolder f("somename");
  f.addItem(new TreeTestItem("atest"));
  
  EQ(size_t(1), f.size());
  TreeFolder::Contents c = f.getContents();
  EQ(size_t(1), c.count("atest"));
  
}

void treefoldertests::add2()
{
  TreeFolder f("somename");
  f.addItem(new TreeTestItem("btest"));
  f.addItem(new TreeTestItem("atest"));
  
  EQ(size_t(2), f.size());
  TreeFolder::Contents c = f.getContents();
  EQ(size_t(1), c.count("atest"));
  EQ(size_t(1), c.count("btest"));
}

void treefoldertests::adddup()
{
  TreeFolder f("somename");
  f.addItem(new TreeTestItem("atest"));
  CPPUNIT_ASSERT_THROW(
    f.addItem(new TreeTestItem("atest")),
    std::invalid_argument
  );
}

void treefoldertests::contents()
{
  TreeFolder f("somename");
  TreeItemBaseClass* pAtest = new TreeTestItem("atest");
  TreeItemBaseClass* pBtest = new TreeTestItem("btest");
  
  f.addItem(pAtest);
  f.addItem(pBtest);
  
  TreeFolder::Contents c = f.getContents();

  EQ(size_t(2), c.size());  
  EQ(pAtest, c["atest"]);
  EQ(pBtest, c["btest"]);

}
void
treefoldertests::iter1()
{
  TreeFolder f("somename");
  TreeItemBaseClass* pAtest = new TreeTestItem("atest");
  f.addItem(pAtest);
  
  const TreeFolder::Contents& c = f.getContents();
  TreeFolder::Contents::const_iterator i  = f.begin();
  
  ASSERT(c.begin() == i);
  EQ(std::string("atest"), i->first);
  EQ(pAtest, i->second);
}

void
treefoldertests::iter2()
{
  TreeFolder f("somename");
  TreeItemBaseClass* pAtest = new TreeTestItem("atest");
  TreeItemBaseClass* pBtest = new TreeTestItem("btest");
  
  f.addItem(pBtest);
  f.addItem(pAtest);
  
  TreeFolder::Contents::const_iterator p = f.begin();
  EQ(std::string("atest"), p->first);
  p++;
  EQ(std::string("btest"), p->first);
  p++;
  ASSERT(p == f.end());
}

void
treefoldertests::freestorage1()
{
  TreeItemBaseClass* pAtest = new TreeTestItem("atest");
  {
    TreeFolder f("somename");
    f.addItem(pAtest);
  }
  // pAtest should have been destroyed
 
  EQ(1, freed); 
}
void
treefoldertests::freestorage2()
{
  TreeItemBaseClass* pAtest = new TreeTestItem("atest");
  {
    TreeFolder f("somename");
    f.freeStorage(false);
    f.addItem(pAtest);
  }
  // pAtest not freed.
  
  EQ(0, freed);
  delete pAtest;
}