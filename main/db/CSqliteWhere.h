/**

#    This software is Copyright by the Board of Trustees of Michigan
#    State University (c) Copyright 2013.
#
#    You may use this software under the terms of the GNU public license
#    (GPL).  The terms of this license are described at:
#
#     http://www.gnu.org/licenses/gpl.txt
#
#    Author:
#            Ron Fox
#            NSCL
#            Michigan State University
#            East Lansing, MI 48824-1321

##
# @file   CSqliteWhere.h
# @brief  Define a set of classes for building Sqlite where clauses.
# @author <fox@nscl.msu.edu>
*/

#ifndef CSQLITEWHERE_H
#define CSQLITEWHERE_H
#include <string>
#include <vector>

/**
 *  CSqliteWhere defines a class hierarchy that allows application writers to
 *  generate SQLITE where clauses without knowing anything about Sql.
 *  Its intended use is for client classes to a high level API that may want
 *  to hide the fact that SQL is invovled at all.  The idea is that
 *  one can build up a set of filter conditions by composing objects
 *  that represent conditions.  Each condition has a 'tostring' virtual method
 *  that knows how to generate its part of a where clause.
 *
 *  @note the hierarchy can and is intended to be extended or even wrapped to
 *        provide higher level filters (e.g. a CNewerThan might know about the
 *        underlying schema in some way to generate a clause that selects records
 *        newer than some criterion).
 *
 *  This is a bit like the inverse of the interpreter patter (See GOF book).
 */

/**
 * @class CQueryFilter
 *   Provides the base class of the filter hierarchy.  This defines the
 *   interface exported by the CQueryFilter hierarchy.  This is an abstract
 *   base class.
 */
class CQueryFilter {
public:
    virtual ~CQueryFilter() {}                      // Ensure destructor chaining.
    virtual std::string toString() = 0;             // Generate where clause.    
};


/**
 * @class CRawFilter
 *    This filter can be used to supply arbitrary query fragments.
 *    One use case is a low level driver that wants to provide a relatively
 *    complex bit of a where clause whithout bothering to build it up from
 *    the bits and pieces of this hierarchy.
 */
class CRawFilter : public CQueryFilter
{
    std::string m_filterString;
public:
    CRawFilter(std::string filterString);
    virtual ~CRawFilter() {}
    virtual std::string toString();
};

/*--------------------------------------------------------------------------
 *  Define simple queries.
 */

/**
 * @class CBinaryRelationFilter
 *   Base class for producing a binary relationship.  This is complicated by
 *   the fact that relationships between fields and number, fields and strings and
 *   fields and other fields must be specified differently.
 */
class CBinaryRelationFilter : public CQueryFilter
{
public:
    typedef enum _binaryOp {
        equal, notEqual, gt, lt, ge, le    
    } binaryOp;                       // Allowed relational ops.
protected:
    std::string m_field;
    binaryOp    m_op;
public:
    CBinaryRelationFilter(std::string field, binaryOp op);
    virtual ~CBinaryRelationFilter() {}
    virtual std::string rhs() = 0;
    virtual std::string toString();
private:
    std::string opToString();
};

/**
 * @class CRelationToNumberFilter
 *    Provides a relationship between a field in the database and
 *    a numeric value.
 */
class CRelationToNumberFilter : public CBinaryRelationFilter
{
private:
    double m_value;
public:
    CRelationToNumberFilter(
        std::string field, CBinaryRelationFilter::binaryOp op, double value
    );
    virtual ~CRelationToNumberFilter() {}
    virtual std::string rhs();
};

/**
 * @class CRelationToStringFilter
 *    Provides a relationship between a field to a string value.
 */
class CRelationToStringFilter : public CBinaryRelationFilter
{
private:
    std::string m_value;
public:
    CRelationToStringFilter(
        std::string field, CBinaryRelationFilter::binaryOp op, std::string value
    );
    virtual ~CRelationToStringFilter() {}
    virtual std::string rhs();
};
/**
 * @class CRelationBetweenFieldsFilter
 *  Provides a relationship between a pair of fields.
 */
class CRelationBetweenFieldsFilter : public CBinaryRelationFilter
{
private:
    std::string m_field2;
public:
    CRelationBetweenFieldsFilter(
        std::string field, CBinaryRelationFilter::binaryOp op, std::string f2
    );
    virtual ~CRelationBetweenFieldsFilter() {}
    std::string rhs();
};

/**
 *  @class CCompoundFilter
 *     This type of filter provides a base class for e.g. AND and OR which
 *     are filter clauses that operate on a set of sub-clauses.
 */
class CCompoundFilter : public CQueryFilter
{
private:
    std::vector<CQueryFilter*> m_subClauses;
    std::string               m_joinString;
public:
    CCompoundFilter(std::string joinString);
    CCompoundFilter(std::string joinString, std::vector<CQueryFilter*>& subFilters);
    virtual ~CCompoundFilter() {}
    
    // Allow manipulation/introspection of the clause vector:
    
    void addClause(CQueryFilter& clause);
    std::vector<CQueryFilter*>::iterator begin();
    std::vector<CQueryFilter*>::iterator end();
    size_t                              size();
    bool isLast(std::vector<CQueryFilter*>::iterator& p);
    
    //
    
    virtual std::string toString();
};

/**
 * @class CAndFilter
 *    Does and of several clauses:
 */
class CAndFilter : public CCompoundFilter
{
public:
    CAndFilter() : CCompoundFilter("AND") {}
    CAndFilter(std::vector<CQueryFilter*>& clauses) :
        CCompoundFilter("AND", clauses) {}
};

/**
 * @class COrFilter
 *    Does or of several clauses.
 */
class COrFilter : public CCompoundFilter
{
public:
    COrFilter() : CCompoundFilter("OR") {}
    COrFilter(std::vector<CQueryFilter*>& clauses) :
        CCompoundFilter("OR", clauses) {}
    
};
/**
 * @class CInFilter
 *     Does an IN (el1, el2, el3).
 */
class CInFilter : public CQueryFilter
{
private:
    class CItem : public CQueryFilter {
    private:
        std::string m_value;
    public:
        CItem(double value);
        CItem(std::string value);
        CItem(int value);
        virtual std::string toString();
    };
    std::vector<CItem*> m_items;
    std::string         m_field;
public:
    CInFilter(std::string field);
    CInFilter(std::string field, const std::vector<double>& values);
    CInFilter(std::string field, const std::vector<std::string>& values);
    CInFilter(std::string field, const std::vector<int>& values);
    virtual ~CInFilter();
    
    void addItem(double value);
    void addItem(std::string value);
    void addItem(int value);
    
    virtual std::string toString();
    
};
namespace DAQ {
    extern CRawFilter acceptAll;
}
#endif
