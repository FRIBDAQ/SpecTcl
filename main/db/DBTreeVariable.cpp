/*
    This software is Copyright by the Board of Trustees of Michigan
    State University (c) Copyright 2017.

    You may use this software under the terms of the GNU public license
    (GPL).  The terms of this license are described at:

     http://www.gnu.org/licenses/gpl.txt

     Authors:
             Ron Fox
             Giordano Cerriza
	     NSCL
	     Michigan State University
	     East Lansing, MI 48824-1321
*/

/** @file:  DBTreeVariable.cpp
 *  @brief: Implement the SpecTcl::DBTreeVariable class
 */

#include "DBTreeVariable.h"
#include "CSqlite.h"
#include "CSqliteStatement.h"


namespace SpecTcl {
//////////////////////////////////////////////////////////////////
// Constructors.


/**
 * constructor - private
 *    Used by static creationals to create an object given we
 *    already have it's info built:
 * @param conn - Sqlite connection object.
 * @param Info - Info block to use for this object.
 */
DBTreeVariable::DBTreeVariable(CSqlite& conn, const Info& info) :
    m_connection(conn), m_Info(info)
{}

///////////////////////////////////////////////////////////////
// Object methods.



////////////////////////////////////////////////////////////////
// Static methods.


}                         // namespace SpecTcl