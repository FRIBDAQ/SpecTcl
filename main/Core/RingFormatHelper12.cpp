/*
    This software is Copyright by the Board of Trustees of Michigan
    
    State University (c) Copyright 2017.

    You may use this software under the terms of the GNU public license
    (GPL).  The terms of this license are described at:

     http://www.gnu.org/licenses/gpl.txt

     Authors:
             Ron Fox
             Giordano Cerriza
	     FRIB
	     Michigan State University
	     East Lansing, MI 48824-1321
*/

/** @file:  RingFormatHelper12.cpp
 *  @brief: Implement the ring buffer decoder helper for version 12.
 */
#include "RingFormatHelper12.h"
#include <RingItemFactoryBase.h>
#include <NSCLDAQFormatFactorySelector.h>



/**
 * constructor
 */
CRingFormatHelper12::CRingFormatHelper12() : CRingFormatHelper11()
{
    m_Factory = &FormatSelector::selectFactory(FormatSelector::v12);  // Replace with v12.
}

// destructor:

CRingFormatHelper12::~CRingFormatHelper12() {}

