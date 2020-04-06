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
# @file   CSqliteWhere.cpp
# @brief  Implementation of the Sqlite filter classes.
# @author <fox@nscl.msu.edu>
*/
#include "CSqliteWhere.h"
#include <stdexcept>
#include <sstream>

/*-------------------------------------------------------------------------
 * Useful canned queries:
 */

CRawFilter DAQ::acceptAll("1=1");

/*-----------------------------------------------------------------------------
 *  Implementation of CRawFilter.
 */

/**
 * Constructor
 *    @param filterString -- the filter string fragment.
 */
CRawFilter::CRawFilter(std::string filterString) :
    m_filterString(filterString)
{}

/**
 * toString
 *   @return std::string - the filter string that was passed in.
 */
std::string
CRawFilter::toString()
{
    return m_filterString;
}
/*----------------------------------------------------------------------------
 *  Implementation of CBinaryRelationFilter
 *  Binary relations only differ in the representation of the right hand
 *  side of the comparison.  We require a pure virtual method rhs that's
 *  supposed provide the appropriate representation of the rhs and can
 *  then implement toString in terms of that.
 */

/**
 * constructor
 *    @param field - name of field on the left side of the operator.
 *    @param op    - Relational operator.
 */
CBinaryRelationFilter::CBinaryRelationFilter(
    std::string field, CBinaryRelationFilter::binaryOp op
) :
    m_field(field),
    m_op(op)
{}

/**
 *   toString
 *      Create the WHERE fragment string
 *
 *   @return std::string - the fragment of the WHERE clause we're responsible
 *                         for.
 */
std::string
CBinaryRelationFilter::toString()
{
    std::string result(m_field);
    result += " ";
    result += opToString();
    result += " ";
    result += rhs();
    
    return result;
}
/**
 * opToString
 *    Turn the operator enum into its string representation.
 *
 *    @return std::string
 */
std::string
CBinaryRelationFilter::opToString()
{
    std::string result;
    
    switch(m_op) {
    case equal:
        result = "=";
        break;
    case notEqual:
        result = "<>";
        break;
    case gt:
        result = ">";
        break;
    case lt:
        result = "<";
        break;
    case ge:
        result = ">=";
        break;
    case le:
        result = "<=";
        break;
    default:
        throw std::range_error("Invalid relational operator provided to BinaryRelationFilter");
    }
    
    return result;
}
/*----------------------------------------------------------------------------
 *  Implement CRelationToNumberFilter
 *     This is a binary relation where the rhs of the relation is a number
 *     and therefore not quoted.
 */
/**
 * constructor
 *   @param field - name of the lhs field.
 *   @param op    - designator for the relational operator.
 *   @param value - rhs value.
 */
CRelationToNumberFilter::CRelationToNumberFilter(
    std::string field, CBinaryRelationFilter::binaryOp op, double value
) :
    CBinaryRelationFilter(field, op),
    m_value(value)
{}

/**
 * rhs
 *   If a value in SQL is numeric it should not be quoted:
 *
 *  @return std::string
 */
std::string
CRelationToNumberFilter::rhs()
{
    std::ostringstream out;
    out << m_value;
    return out.str();
}
/*----------------------------------------------------------------------------
 * Implement CRelationToStringFilter - the rhs must be quoted.
 */

/**
 * constructor
 *   @param op    - designator for the relational operator.
 *   @param value - rhs value.
 */
CRelationToStringFilter::CRelationToStringFilter(
    std::string field, CBinaryRelationFilter::binaryOp op, std::string value
) :
    CBinaryRelationFilter(field, op),
    m_value(value)
{
}
/**
 * rhs
 *   @return std::string - the quoted value of 'value'  note that we don't
 *     support embedded 's at this time.
 */
std::string
CRelationToStringFilter::rhs()
{
    std::string result("'");
    result += m_value;
    result += std::string("'");
    
    return result;
}
/*----------------------------------------------------------------------------
 *  Implement CRelationBetweenFieldsFilter - this just returns the field
 *  without quotations allowing filters that enforce a relationship between
 *  a pair of field.
 */

/**
 * constructor
 *   @param field  - Field on lhs of relationshp.
 *   @param op     - Relational operator.
 *   @param f2     - Field on rhs of relationship
 */
CRelationBetweenFieldsFilter::CRelationBetweenFieldsFilter(
    std::string field, CBinaryRelationFilter::binaryOp op, std::string f2
) :
  CBinaryRelationFilter(field, op),
  m_field2(f2)
  {}
  
/**
 * rhs
 *   @return std::string - the name of the second field.
 */
std::string
CRelationBetweenFieldsFilter::rhs()
{
    return m_field2;
}

/*-----------------------------------------------------------------------------
 *  CCompoundFilter implementation.
 *     Produces (clause1) join-string (clause2) [.... (clausen)]
 *     Note the parenthesization of the clauses allowing compound filters to
 *     properly nest.
 */

/**
 * constructor (overloaded)
 *    Produces a compound filter with no clauses.  At least two clauses must
 *    later be added by addClause prior to invoking toString
 *
 *  @param joinString - the string that will be used to join the clauses together.
 */
CCompoundFilter::CCompoundFilter(std::string joinString) :
    m_joinString(joinString)
{}
/**
 * constructor (overloaded)
 *    Produces a compound filter with a set of clauses preloaded.  Note that
 *    addClause can still be invoked priot to a call to toString to extend the
 *    set of clauses in the filter.
 *
 * @param joinString - String used to join the clauses.
 * @param subFilters - Set of initial clauses.
 */
CCompoundFilter::CCompoundFilter(
    std::string joinString, std::vector<CQueryFilter*>& subFilters
) :
   m_joinString(joinString),
   m_subClauses(subFilters)
{}

/**
 * addClause
 *    Add an additional condition clause to the filter.
 *
 * @param clause - new subclause
 */
void
CCompoundFilter::addClause(CQueryFilter& clause)
{
    m_subClauses.push_back(&clause);
}

/**
 * begin
 *    Returns an iterator to the start of the subclauses collection.
 *
 * @return std::vector<CQueryFilter>::iterator
 */
std::vector<CQueryFilter*>::iterator
CCompoundFilter::begin()
{
    return m_subClauses.begin();
}
/**
 * end
 *   Returns an iterator off the back end of the subclauses collection
 *
 * @return std::vector<CQueryFilter>::iterator
 */
std::vector<CQueryFilter*>::iterator
CCompoundFilter::end()
{
    return m_subClauses.end();
}
/**
 * size
 *
 * @return size_t  - number of elements in the array.
 */
size_t
CCompoundFilter::size()
{
    return m_subClauses.size();
}
/**
 * isLast
 *    determine if the iterator is 'pointing' to the last element.
 *
 * @param p     - Iterator to check.
 * @return bool - true if next increment will result in end.
 *
 */
bool
CCompoundFilter::isLast(std::vector<CQueryFilter*>::iterator& p)
{
    return std::next(p) == end();
}

/**
 * toString
 *   @return std::string - string representation of the compound clause.
 */
std::string
CCompoundFilter::toString()
{
    std::string result;
    
    // It's an error to not have at least two clauses:
    
    if (size() < 2) {
        throw std::range_error("Compound filters must have at least two elements");
    }
    
    auto p = begin();
    while (p != end()) {
        // Parenthesized clause:
        
        result += " (";
        result += (*p)->toString();
        result += ") ";
        
        // Put in the join string after all but the last item.
        
        if (!isLast(p)) {
            result += m_joinString;
        }
        
        p++;
    }
    return result;
}
/*---------------------------------------------------------------------------
 *  Implementation of CInFilter - This generates the SQL
 *  IN (item1, item2, item3).  Where it not for the () enclosing each element
 *  we could use the compund filter :-(.
 *
 *  As it is there's a bit of a cheat.  We're creating an inner class
 *  CItem that can encapsulate either a string or a number.  It has a toString
 *  method just like any other CCompoundFilter and is used to generate
 *  the elements of the 'in' list.
 */

/*--------------------------------------------------
 *  CInFilter::CItem inner class:
 */

/**
 * constructor (number)
 *   @param value - the numeric value to encapsulate:
 */
CInFilter::CItem::CItem(double value)
{
    std::ostringstream s;
    s << value;
    m_value = s.str();
}
/**
 * constructor (string)
 *
 * @param value - the string value to encapsulate.
 */
CInFilter::CItem::CItem(std::string value)
{
    m_value = "'";
    m_value += value;
    m_value += "'";
}
/**
 * constructor(int)
 *  @param  value - integer value.
 */
CInFilter::CItem::CItem(int value)
{
  std::ostringstream s;
  s << value;
  m_value = s.str();
}
/**
 * toString
 *   @return std::string - stringified item value:
 */
std::string
CInFilter::CItem::toString()
{
    return m_value;
}

/*-------------------------------
 *  Outer class (CInFilter)
 */

/**
 *  constructor - no items yet.  Must be added via addItem.
 *    @param field - the field where applying the IN clause to.
 */
CInFilter::CInFilter(std::string field) : m_field(field)
{}
/**
 * constructor with a vector of doubles:
 *   @param field - the field where applying the IN clause to.
 *   @param values - the input vector.
 */
CInFilter::CInFilter(std::string field, const std::vector<double>& values) :
    m_field(field)
{
    for(int i =0; i < values.size(); i++) {
      addItem(values[i]);
    }
}
/**
 * constructor with vector of strings.
 *   @param field - the field where applying the IN clause to.
 *   @param values - the input vector.
 */
CInFilter::CInFilter(std::string field, const std::vector<std::string>& values) :
    m_field(field)
{
    for (int i = 0; i < values.size(); i++) {
      addItem(values[i]);
    }
}
/**
 * constructor with vector of ints.
 *   @param field - field to do the in on.
 *   @param values - vector of values.
 */
CInFilter::CInFilter(std::string field, const std::vector<int>& values) :
  m_field(field)
{
    for (int i =0; i < values.size(); i++) {
      addItem(values[i]);
      
    }
}
/**
 * destructor - kill off the CItem's in m_items.
 */
CInFilter::~CInFilter()
{
    for (int i = 0; i < m_items.size(); i++ )
    {
        delete m_items[i];
    }
}

/**
 * addItem (double)
 *    Add a new item to the IN set:
 *
 *  @param value - new item to add
 */
void
CInFilter::addItem(double value)
{
    CItem* p = new CItem(value);
    m_items.push_back(p);
}
/**
 * addItem (string)
 *    Add a new item to the IN set:
 *
 * @param value - new item value to add.
 */
void
CInFilter::addItem(std::string value)
{
    CItem* p = new CItem(value);
    m_items.push_back(p);
}
/**
 * addItem(int)
 *   @param value -new item value to add.
 */
void
CInFilter::addItem(int value)
{
  CItem* p = new CItem(value);
  m_items.push_back(p);
}

/**
 * toString
 *   Convert the vector to an IN string:
 */
std::string
CInFilter::toString()
{
    // Must have at least one elemen:
    
    if (m_items.size() < 1) {
        throw std::range_error("IN filter requires at least one element.");
    }
    std::string result = m_field;
    result +=  " IN (";
    for (auto p = m_items.begin(); p != m_items.end(); p++) {
        result += (*p)->toString();
        if (std::next(p) != m_items.end()) {
            result += ", ";
        }
    }
    result += ")";
    return result;
}