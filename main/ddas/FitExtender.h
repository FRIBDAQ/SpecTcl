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

/** @file:  FitExtender.h
 *  @brief: Public exports of data structures from FitExtender.
 */
#ifndef FITEXTENDER_H
#define FITEXTENDER_H
#include <lmfit.h>
#include <string.h>

// Here are the hit extensions, Constructors fil  in the hit extension sizes.

typedef struct _nullExtension {
    uint32_t s_size;
    _nullExtension() : s_size(sizeof(uint32_t)) {}
} nullExtension, *pNullExtension;

typedef struct _FitInfo {
    uint32_t  s_size;
    DDAS::HitExtension s_extension;
    _FitInfo();
} FitInfo, *pFitInfo;

#endif