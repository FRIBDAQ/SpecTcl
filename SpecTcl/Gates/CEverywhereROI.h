/*
    This software is Copyright by the Board of Trustees of Michigan
    State University (c) Copyright 2005.

    You may use this software under the terms of the GNU public license
    (GPL).  The terms of this license are described at:

     http://www.gnu.org/licenses/gpl.txt

     Author:
             Ron Fox
	     NSCL
	     Michigan State University
	     East Lansing, MI 48824-1321
*/


///////////////////////////////////////////////////////////
//  CEverywhereROI.h
//  Implementation of the Class CEverywhereROI
//  Created on:      20-Jun-2005 10:10:54 AM
//  Original author: Ron Fox
///////////////////////////////////////////////////////////

#if !defined(__CEVERYWHEREROI_H)
#define __CEVERYWHEREROI_H

#include "CROI.h"

// Forward definitions:

class CAxis;

/**
 * Determines the insidedness of a region of interest that covers the entire
 * spectrum.
 * @author Ron Fox
 * @version 1.0
 * @created 20-Jun-2005 10:10:54 AM
 */
class CEverywhereROI : public CROI
{

public:
  CEverywhereROI(CAxis& xAxis, CAxis& yAxis);
  virtual ~CEverywhereROI();
  CEverywhereROI(const CEverywhereROI& rhs);
  CEverywhereROI& operator=(const CEverywhereROI& rhs);
  int operator==(const CEverywhereROI& rhs) const;
  int operator!=(const CEverywhereROI& rhs) const;

  virtual bool operator()(int x, int y);
  
};


#endif
