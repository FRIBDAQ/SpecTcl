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
# @file   deflate.h
# @brief  Define command class to deflate byte array.
# @author <fox@nscl.msu.edu>
*/

#ifndef __DEFLATE_H
#define __DEFLATE_H

#include <TCLObjectProcessor.h>

class CTCLInterpreter;
class CTCLObject;

/**
 * @class CDeflate
 *
 *    Provides the deflate class.  This is a simple hook to zlib that
 *    takes an object byte array and uses zlib's compress function
 *    to turn it into a compressed version of itself.
 *    The inflate command in this package can be used to reverse this
 *    process.
 *
 *  Usage:
 *
 *  \verbatim
 *
 *  set compressed-data [compress something-to-compress]
 *
 *  \endverbatim
 */
class CDeflate : public CTCLObjectProcessor
{
public:
    CDeflate(CTCLInterpreter& interp, const char* command="deflate");
    virtual ~CDeflate();
    
    int operator()(CTCLInterpreter& interp, std::vector<CTCLObject>& objv);
};


#endif