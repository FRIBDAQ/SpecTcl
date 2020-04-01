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
# @file   CSqliteException.h
# @brief  Encapsulate SQLITE errors in an exception
# @author <fox@nscl.msu.edu>
*/

#ifndef __CSQLITEEXCEPTION_H
#define __CSQLITEEXCEPTION_H
#include <exception>


/**
 *  @class CSqliteException
 *
 *  Simple wrapper exception class for sqlite3 errors
 */
class CSqliteException : public std::exception
{
private:
    int m_errorCode;
    static const char* m_errorMessages[];
    
    // Canonicals implemented:
public:
    CSqliteException(int error) noexcept;
    CSqliteException(const CSqliteException& rhs) noexcept;
    virtual ~CSqliteException() noexcept;
    
    // Methods required by the base class:
public:
    CSqliteException& operator=(const CSqliteException& rhs) noexcept;
    virtual const char* what()  const noexcept;
};

#endif