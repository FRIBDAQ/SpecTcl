// Template for a test suite.

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Asserter.h>
#include <Asserts.h>

#define private public
#define protected public
#include "TreeBuilder.h"
#undef private
#undef protected



class paramtreetests : public CppUnit::TestFixture {
  CPPUNIT_TEST_SUITE(paramtreetests);
  CPPUNIT_TEST(construct);
  
  CPPUNIT_TEST(pathsep1);
  CPPUNIT_TEST(pathsep2);
  CPPUNIT_TEST(pathsep3);
  
  CPPUNIT_TEST(clearsubtree1);
  CPPUNIT_TEST(clearsubtree2);
  CPPUNIT_TEST(clearsubtree3);
  CPPUNIT_TEST(clearsubtree4);
  
  CPPUNIT_TEST(makepath1);
  CPPUNIT_TEST(makepath2);
  CPPUNIT_TEST(makepath3);
  
  CPPUNIT_TEST(addparam1);
  CPPUNIT_TEST(addparam2);
  CPPUNIT_TEST(addparam3);
  CPPUNIT_TEST(addparam4);
  CPPUNIT_TEST(addparam5);
  
  CPPUNIT_TEST(build1);
  CPPUNIT_TEST_SUITE_END();


private:

public:
  void setUp() {
  }
  void tearDown() {
  }
protected:
  void construct();
  
  void pathsep1();
  void pathsep2();
  void pathsep3();
  
  void clearsubtree1();
  void clearsubtree2();
  void clearsubtree3();
  void clearsubtree4();
  
  void makepath1();
  void makepath2();
  void makepath3();
  
  void addparam1();
  void addparam2();
  void addparam3();
  void addparam4();
  void addparam5();
  
  void build1();
};

CPPUNIT_TEST_SUITE_REGISTRATION(paramtreetests);

// Some stuff for testing:

static size_t terminals(0);
static size_t folders(0);

class TestTerminal : public TreeTerminal
{
public:
  TestTerminal(const char* name, unsigned id) :
    TreeTerminal(name, id) { terminals++; }
  ~TestTerminal() {terminals--;}
};

class TestFolder : public TreeFolder
{
public:
  TestFolder(const char* name) :
    TreeFolder(name) {folders++;}
  ~TestFolder() {folders--; }
};

// Looks like an empty, nameless folder on construction.

void paramtreetests::construct() {
  ParameterTree t;
  
  EQ(std::string(""), t.getName());
  ASSERT(t.isFolder());
  EQ(size_t(0), t.size());
}
// If only one path element, we just get it back.

void paramtreetests::pathsep1()
{
  std::vector<std::string> path  = ParameterTree::pathElements("test");
  EQ(size_t(1), path.size());
  EQ(std::string("test"), path[0]);
}

// Multi level path ending in a good tail:

void paramtreetests::pathsep2()
{
  std::vector<std::string> path = ParameterTree::pathElements("a.bb.ccc.dddd");
  
  EQ(size_t(4), path.size());
  EQ(std::string("a"), path[0]);
  EQ(std::string("bb"), path [1]);
  EQ(std::string("ccc"), path[2]);
  EQ(std::string("dddd"), path[3]);
}
// Path ending in a period has empty string for tail.

void paramtreetests::pathsep3()
{
  std::vector<std::string> path = ParameterTree::pathElements("aa.bbb.c.");
  EQ(size_t(4), path.size());
  EQ(std::string(""), path[3]);
}

// Clear subtree for empty subtree is well behaved.

void paramtreetests::clearsubtree1()
{
  TreeFolder top("top");
  CPPUNIT_ASSERT_NO_THROW(
    ParameterTree::clearSubTree(top)
  );
}
// Empty out flat directory subtree:

void paramtreetests::clearsubtree2()
{
  TreeFolder top("top");
  top.addItem(new TestTerminal("item1", 1));
  top.addItem(new TestTerminal("item2", 2));
  top.addItem(new TestTerminal("item3", 3));
  
  EQ(size_t(3), terminals);
  ParameterTree::clearSubTree(top);
  EQ(size_t(0), terminals);
  ASSERT(top.m_contents.empty());
}
// Empty out deep subtree:

void paramtreetests::clearsubtree3()
{
  TreeFolder top("top");
  TestFolder* pf1 = new TestFolder("f1");
  TestFolder* pf2 = new TestFolder("f2");
  TestFolder* pf3 = new TestFolder("f3");
  
  top.addItem(pf1);
  pf1->addItem(pf2);
  pf2->addItem(pf3);
  
  pf3->addItem(new TestTerminal("item1", 1));
  pf3->addItem(new TestTerminal("item2", 2));
  pf3->addItem(new TestTerminal("item3", 3));
  
  EQ(size_t(3), folders);
  EQ(size_t(3), terminals);
  
  ParameterTree::clearSubTree(top);
  
  EQ(size_t(0), folders);
  EQ(size_t(0), terminals);
  ASSERT(top.m_contents.empty());
}
// Multilevel mixed contents.

void paramtreetests::clearsubtree4()
{
  TreeFolder top("top");
  TestFolder* pf1 = new TestFolder("f1");
  TestFolder* pf2 = new TestFolder("f2");
  TestFolder* pf3 = new TestFolder("f3");
  
  top.addItem(pf1);
  pf1->addItem(pf2);
  pf1->addItem(pf3);
  
  top.addItem(new TestTerminal("item1", 1));
  pf1->addItem(new TestTerminal("item2", 2));
  pf2->addItem(new TestTerminal("item3", 3));
  pf3->addItem(new TestTerminal("item4", 4));
  
  EQ(size_t(3), folders);
  EQ(size_t(4), terminals);
  
  ParameterTree::clearSubTree(top);
  
  EQ(size_t(0), folders);
  EQ(size_t(0), terminals);
  
  ASSERT(top.m_contents.empty());
}
// Single path element does not yet exist:

void paramtreetests::makepath1()
{
  std::vector<std::string> path = {"directory"};
  ParameterTree            t;
  
  TreeFolder* f = t.makeFolderPath(path);
  
  EQ(std::string("directory"), f->getName());
  EQ(size_t(1), t.m_contents.size());
  EQ(f, reinterpret_cast<TreeFolder*>(t.m_contents.begin()->second));
  EQ(std::string("directory"), t.m_contents.begin()->first);
}

// Deeper path all need to be created.

void paramtreetests::makepath2()
{
  std::vector<std::string> path = ParameterTree::pathElements("top.mid.bottom");
  ParameterTree t;
  TreeFolder* f = t.makeFolderPath(path);
  
  // First look at the item returned, then trace the path.
  
  EQ(std::string("bottom"), f->getName());
  TreeFolder::Contents& c(t.m_contents);
  
  EQ(size_t(1), c.size());
  TreeItemBaseClass* i = c.begin()->second;
  EQ(std::string("top"), i->getName());
  ASSERT(i->isFolder());
  TreeFolder* folder = reinterpret_cast<TreeFolder*>(i);
  EQ(size_t(1), folder->m_contents.size());
  
  i = folder->m_contents.begin()->second;
  EQ(std::string("mid"), i->getName());
  ASSERT(i->isFolder());
  folder = reinterpret_cast<TreeFolder*>(i);
  EQ(size_t(1), folder->m_contents.size());
  
  i = folder->m_contents.begin()->second;
  EQ(std::string("bottom"), i->getName());
  ASSERT(i->isFolder());
  folder = reinterpret_cast<TreeFolder*>(i);
  EQ(f, folder);
}

void paramtreetests::makepath3()
{
  ParameterTree t;
  TreeFolder *top = new TreeFolder("top");
  t.addItem(top);                         // TreeFolder::addItem
  
  std::vector<std::string> path = ParameterTree::pathElements("top.bottom");
  TreeFolder* f = t.makeFolderPath(path);
  
  // Top level still only has one folder.
  
  TreeFolder::Contents& c(t.m_contents);
  EQ(size_t(1), c.size());
  
  TreeItemBaseClass* item = c.begin()->second;
  ASSERT(item->isFolder());             // "top" folder.
  TreeFolder* folder = reinterpret_cast<TreeFolder*>(item);
  EQ(top, folder);
  
  EQ(size_t(1), folder->m_contents.size());
  item = folder->m_contents.begin()->second;
  ASSERT(item->isFolder());
  folder = reinterpret_cast<TreeFolder*>(item);
  EQ(f, folder);
}
// Add parameter to empty top level.

void paramtreetests::addparam1()
{
  ParameterTree t;
  ParameterTree::ParameterDef param("item", 1234);
  t.addParameter(param);
  
  EQ(size_t(1), t.size());
  const TreeItemBaseClass* item = t.begin()->second;
  EQ(std::string("item"), item->getName());
  ASSERT(!item->isFolder());
  const TreeTerminal* p = reinterpret_cast<const TreeTerminal*>(item);
  EQ(unsigned(1234), p->id());
}

// Add two distinct parameter to top level.

void paramtreetests::addparam2()
{
  ParameterTree t;
  ParameterTree::ParameterDef p1("item1", 1);
  ParameterTree::ParameterDef p2("item2", 2);
  
  t.addParameter(p1);
  CPPUNIT_ASSERT_NO_THROW(
    t.addParameter(p2)
  );
  
  EQ(size_t(2), t.size());
  
  // Note that the parameter tree should iterate in alpha order.
  
  TreeFolder::Contents::const_iterator i = t.begin();
  const TreeItemBaseClass* item = i->second;
  
  EQ(std::string("item1"), item->getName());
  ASSERT(!item->isFolder());
  const TreeTerminal* param = reinterpret_cast<const TreeTerminal*>(item);
  EQ(unsigned(1), param->id());
  
  i++;
  item = i->second;
  EQ(std::string("item2"), item->getName());
  ASSERT(!item->isFolder());
  param = reinterpret_cast<const TreeTerminal*>(item);
  EQ(unsigned(2), param->id());
  
  i++;
  ASSERT(i == t.end());
  
}

// Adding a duplicat parameter is an exception.

void paramtreetests::addparam3()
{
  ParameterTree t;
  ParameterTree::ParameterDef p("item", 1);
  
  t.addParameter(p);
  CPPUNIT_ASSERT_THROW(
    t.addParameter(p),
    std::invalid_argument
  );
}
// Tests:
//  - Ability to make folders to the item.
//  - Ability to distinguish between two parameters with the same name in
//    differing folders.
//
void paramtreetests::addparam4()
{
  ParameterTree t;
  ParameterTree::ParameterDef p1("pads", 1);
  ParameterTree::ParameterDef p2("s800.pads", 2);
  
  t.addParameter(p1);
  CPPUNIT_ASSERT_NO_THROW(
    t.addParameter(p2)
  );
  
  // take advantage of the fact iterators are alphabetizers.
  
  TreeFolder::Contents::const_iterator i = t.begin();
  TreeItemBaseClass* item = i->second;
  EQ(std::string("pads"), item->getName());
  ASSERT(!item->isFolder());
  TreeTerminal* param = reinterpret_cast<TreeTerminal*>(item);
  EQ(unsigned(1), param->id());
  
  i++;               // Should 'point' to the folder "s800".
  item = i->second;
  EQ(std::string("s800"), item->getName());
  ASSERT(item->isFolder());
  TreeFolder* folder = reinterpret_cast<TreeFolder*>(item);
  
  // No more items in the top level
  
  i++;
  ASSERT( i == t.end());
  i = folder->begin();                // Iterate subfolder.
  item = i->second;
  EQ(std::string("pads"), item->getName());
  ASSERT(!item->isFolder());
  param = reinterpret_cast<TreeTerminal*>(item);
  EQ(unsigned(2), param->id());
  
}
// duplicate names on parallel branches:

void paramtreetests::addparam5()
{
  ParameterTree t;
  ParameterTree::ParameterDef p1("crdc1.pads", 1);
  ParameterTree::ParameterDef p2("crdc2.pads",2 );
  
  t.addParameter(p1);
  CPPUNIT_ASSERT_NO_THROW(
    t.addParameter(p2);
  );
  
  EQ(size_t(2), t.size());           // Top levels are dirs.
  TreeFolder::Contents::const_iterator i = t.begin();
  TreeItemBaseClass* pItem = i->second;
  ASSERT(pItem->isFolder());
  TreeFolder* folder = reinterpret_cast<TreeFolder*>(pItem);
  EQ(std::string("crdc1"), folder->getName());
  EQ(size_t(1), folder->size());
  EQ(std::string("pads"), folder->getContents().begin()->first);
  ASSERT(!folder->getContents().begin()->second->isFolder());
  
  i++;
  pItem = i->second;
  ASSERT(pItem->isFolder());
  folder = reinterpret_cast<TreeFolder*>(pItem);
  EQ(std::string("crdc2"), folder->getName());
  EQ(size_t(1), folder->size());
  EQ(std::string("pads"), folder->getContents().begin()->first);
  ASSERT(!folder->getContents().begin()->second->isFolder());
  
  i++;
  ASSERT(i == t.end());
  
}
// Just do the addparam5 test but with buildTree rather than individual adds.

void paramtreetests::build1()
{
  ParameterTree t;
  ParameterTree::ParameterDef p1("crdc1.pads", 1);
  ParameterTree::ParameterDef p2("crdc2.pads",2 );
  std::vector<ParameterTree::ParameterDef> params={p1, p2};
  t.buildTree(params);
  
  // Don't like this copy but otherwise assertion failures won't point back
  // to the right spot if factored out.
  
  EQ(size_t(2), t.size());           // Top levels are dirs.
  TreeFolder::Contents::const_iterator i = t.begin();
  TreeItemBaseClass* pItem = i->second;
  ASSERT(pItem->isFolder());
  TreeFolder* folder = reinterpret_cast<TreeFolder*>(pItem);
  EQ(std::string("crdc1"), folder->getName());
  EQ(size_t(1), folder->size());
  EQ(std::string("pads"), folder->getContents().begin()->first);
  ASSERT(!folder->getContents().begin()->second->isFolder());
  
  i++;
  pItem = i->second;
  ASSERT(pItem->isFolder());
  folder = reinterpret_cast<TreeFolder*>(pItem);
  EQ(std::string("crdc2"), folder->getName());
  EQ(size_t(1), folder->size());
  EQ(std::string("pads"), folder->getContents().begin()->first);
  ASSERT(!folder->getContents().begin()->second->isFolder());
  
  i++;
  ASSERT(i == t.end());  
  
}