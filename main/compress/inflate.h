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
# @file   inflate.h
# @brief  Define the class that implements inflate.
# @author <fox@nscl.msu.edu>
*/

#ifndef INFLATE_H
#define INFLATE_H
#include <TCLObjectProcessor.h>

class CTCLInterpreter;
class CTCLObject;

/**
 * @class CInflate
 *
 *    Provides a class that implements an inflate command. The
 *    inflate command decompresses data that was compressed by
 *    zlib.  The form of the command is:
 *
 * \verbatim
 *    set result [inflate compressed-data]
 *  \endverbatim
 *
 *  The following should be a time wasting null transform:
 *
 * \verbatim
 *   set result [inflate [deflate some-data]]
 * \endverbatim
 */

class CInflate : public CTCLObjectProcessor
{
public:
    CInflate(CTCLInterpreter& interp, const char* command = "inflate");
    virtual ~CInflate();
    
    int operator()(CTCLInterpreter& interp, std::vector<CTCLObject>& objv);
};

#endif