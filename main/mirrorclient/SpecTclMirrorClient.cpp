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

/** @file:  SpecTclMirrorClient.cpp
 *  @brief: Implements the APi described in SpecTclMirrorClient.h
 */

#include "SpecTclMirrorClient.h"

static unsigned lastError = MIRROR_SUCCESS;

static const char* ppMessages[] = {
    "Successful completion"
};
static const unsigned nMsgs = sizeof(ppMessages)/sizeof(const char*);
// External entries:

extern "C" {
void*
getSpecTclMemory(const char* host, const char* rest, const char* mirror, const char*user)
{
    return nullptr;
}

}


extern "C" {
int
errorCode()
{
    return lastError;
}
}

extern "C" {
const char*
errorString(unsigned code)
{
    if (code < nMsgs) {
        return ppMessages[lastError];
    } else {
        return "Invalid error code";
    }
}
}


