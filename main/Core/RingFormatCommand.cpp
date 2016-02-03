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

/**
 * @file RingFormatCommand.cpp
 * @brief Code that implements the ringformat command processor.
 */

#include <config.h>
#include "RingFormatCommand.h"
#include <TCLInterpreter.h>
#include <TCLCommandPackage.h>
#include <TCLResult.h>
#include <RingFormatHelper.h>
#include <RingFormatHelperFactory.h>
#include <CRingBufferDecoder.h>
#include <SpecTcl.h>
#include "TCLAnalyzer.h"
#include <BufferDecoder.h>

#include <stdio.h>

/**
 * constructor
 *    Create the command processor object.
 *    @param pInterp - Pointer to the Tcl interpreter on which the command
 *                     processor will be registered.
 *    @param rPackage- Package that contains the command processor.
 */
CRingFormatCommand::CRingFormatCommand(
    CTCLInterpreter* pInterp, CTCLCommandPackage& rPackage
) : CTCLPackagedCommand("ringformat", pInterp, rPackage)
{
    
}
/**
 * destructor
 *  - A no-op.
 */
CRingFormatCommand::~CRingFormatCommand()
{
    
}

/**
 * operator()
 *
 * Called to execute the 'ringformat' command:
 * - The current buffer decoder must be a CRingBufferDecoder.
 * - There must be exactly one command parameter; the version string.
 * - The version string must parse into a version.
 * - The resulting major.minor versions must be able to construct a CRingFormatHelper
 *   object via the CRingFormatHelperFactory.
 *
 * @param rInterp - Reference to the interpretr that is executing this command.
 * @param rResult - Reference to the result object.
 * @param argc    - Number of command words.
 * @param argv    - Array of pointers to the command words.
 *
 * @return int
 * @retval TCL_OK - Normal, correct completion.
 * @retval TCL_ERROR - And error occured that prevented normal
 *                   completion.
 *
 * @note - The successful result is the selected version.  The failure
 *         result is an error  message that describes the failure.
 */
int
CRingFormatCommand::operator()(
    CTCLInterpreter& rInterp, CTCLResult& rResult, int argc, char* argv[])
{
    
    try {
        // Get the decoder so we can set its helper when the time comes:
        
        CRingBufferDecoder* pDecoder = getDecoder();
        if (!pDecoder) {
            throw std::string(
                "You can only use this command when connected to ring formatted data sources"
            );
        }
        // Check the argument count and argument type:
        if (argc != 2) {
            throw Usage();
        }
        RingVersion version = parseVersion(argv[1]);
        unsigned maj = version.s_major;
        unsigned min = version.s_minor;
        
        // Now build the ring format helper from the factory if we can.
        
        CRingFormatHelperFactory* helperFact(pDecoder->getFormatFactory());
        CRingFormatHelper* pHelper = helperFact->create(maj, min);
        if (!pHelper) {
            std::string message = "Ring format: ";
            message += argv[1];
            message += "  cannot be decoded by this version of SpecTcl";
            throw message;
        }
        /*
           Nothing left but to set the helper... we're going to set the
           default and current helper both...The default so that we'll fall
           back to this type, the current in case there's a different one in
           effect.
           Note that since CRingBufferDecoder may eventually do a delete
           on one of the helpers, it is important to make two distinct
           helpers rather than pass the same pointer to both!!!!
        */
        pDecoder->setFormatHelper(pHelper);
        pDecoder->setDefaultFormatHelper(helperFact->create(maj, min));;
        
        // Falling through the catch block will return normal
        
        rResult = argv[1];
    }
    catch(std::string msg) {
        rResult = msg;
        return TCL_ERROR;
    }
    
    return TCL_OK;
}
/*-----------------------------------------------------------------------------
* Private methods
*/

/**
 * getDecoder
 *
 *  Returns a pointer to the current buffer decoder.
 *
 *  @return CRingBufferDecoder*
 *  @retval 0 - Implies the current decoder is not a CRingBufferDecoder.
 */
CRingBufferDecoder*
CRingFormatCommand::getDecoder()
{
    SpecTcl* pApi = SpecTcl::getInstance();
    CTclAnalyzer* pAnalyzer = pApi->GetAnalyzer();
    
    CBufferDecoder* pDecoder = pAnalyzer->getDecoder();
    if (!pDecoder) {
        return reinterpret_cast<CRingBufferDecoder*>(0);
    }
    
    return dynamic_cast<CRingBufferDecoder*>(pDecoder);  // 0 if cast fails.
    
}
/**
 * Usage
 *
 *   Returns a command usage string.
 *
 *   @return std::string
 */
std::string
CRingFormatCommand::Usage() const
{
    std::string msg = "Usage:\n";
    msg            += "  ringformat  major.minor\n";
    msg            += "Where:\n";
    msg            += "   major   - The ring buffer format major version\n";
    msg            += "   minor   - The ring buffer format minor version\n";
    
    
    return msg;
}
/**
 * parseVersion
 *   Decode a string as version.  The string is of the form major.minor
 *   - Locate the period.. if there is no . the minor is 0.
 *   - Decode major as an integer.
 *   - If there is a minor, decode that as an integer as well.
 *   - Any integer decode fails, throw an invalid format string
 *   - along with the usage text.
 *   
 *   @param pVersionString Pointer to the version string.
 *
 *   @return RingVersion - struct with the major/minor version included.
 *   @throw std::string error message as described above.
 */
CRingFormatCommand::RingVersion
CRingFormatCommand::parseVersion(const char* pVersionString) const throw(std::string)
{
    /*
      Simplest way to deal with this is just good ole sscanf..though that's a
      bit syntactically loose as 16, is perfectly good for version 16.0 in
      that usage.
    */
    
    RingVersion result = {0,0};
    int nconverted = sscanf(
        pVersionString, "%u.%u", &result.s_major, &result.s_minor
    );
    if (nconverted < 1) {
        std::string msg = "Invalid version string: ";
        msg += pVersionString;
        msg += "\n";
        msg += Usage();
        
        throw msg;
    }
    return result;
}