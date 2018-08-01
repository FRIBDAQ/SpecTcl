#ifndef ASSERTS_H
#define ASSERTS_H

// Abbreviations for assertions in cppunit.

#define EQMSG(msg, a, b)   CPPUNIT_ASSERT_EQUAL_MESSAGE(msg,a,b)
#define EQ(a,b)            CPPUNIT_ASSERT_EQUAL(a,b)
#define ASSERT(expr)       CPPUNIT_ASSERT(expr)
#define FAIL(msg)          CPPUNIT_FAIL(msg)



#endif
