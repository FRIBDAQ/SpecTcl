/*
    This software is Copyright by the Board of Trustees of Michigan
    State University (c) Copyright 2008

    You may use this software under the terms of the GNU public license
    (GPL).  The terms of this license are described at:

     http://www.gnu.org/licenses/gpl.txt

     Author:
             Ron Fox
	     NSCL
	     Michigan State University
	     East Lansing, MI 48824-1321
*/
#ifndef RINGFORMATCOMMAND_H
#define RINGFORMATCOMMAND_H


/**
 * @file RingFormatCommand.h
 * @brief Define command processor that specifies a ring item format.
 */

#include "TCLPackagedCommand.h"
#include <string>
#include <utility>


class CTCLInterpreter;
class CTCLCommandPackage;
class CTCLResult;
class CRingBufferDecoder;


/**
 * @class CRingFormatCommand
 *
 * This class provides a command that allows the current format of a ring item
 * data source to be set.  It is an error to invoke this command if the
 * current buffer decoder is not a CRingBufferDecoder object.
 *
 * Format of the command is:
 * \verbatim
 *   ringformat version
 * \endverbatim
 *
 * Where version is the ring item version to select.  version is a string that
 * must be of the form major.minor or just major  If the minor part is omitted
 * it is assumed to be 0.  Thus the versioon strings 10.0 and 10 are identical
 * and specify a version of major=10 and minor of 0.
 *
 * We only can support versions that the CRingFormatHelperFactory can
 * instantiate.
 */
class CRingFormatCommand : public CTCLPackagedCommand
{
    // Local data types
private:
    typedef struct _RingVersion {
        unsigned s_major;
        unsigned s_minor;
    } RingVersion, *pRingVersion;
    
    // Allowed Canonical methods:
public:
    CRingFormatCommand(CTCLInterpreter* pInterp, CTCLCommandPackage& rPackage);
    virtual ~CRingFormatCommand();

    // Forbidden canonical methods:
    
private:
    CRingFormatCommand(const CRingFormatCommand&);
    CRingFormatCommand& operator=(const CRingFormatCommand&);
    int operator==(const CRingFormatCommand&) const;
    int operator!=(const CRingFormatCommand&) const;
    
    // Operations on the class.

public:
    virtual int operator()(
        CTCLInterpreter& rInterp, CTCLResult& rResult, int argc, char* argv[]
    );
    
    // Private utilities
private:  
    CRingBufferDecoder* getDecoder();
    std::string Usage() const;
    RingVersion parseVersion(const char* pVersionString) const;
};

#endif
