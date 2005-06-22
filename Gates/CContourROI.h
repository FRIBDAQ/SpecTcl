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
//  CContourROI.h
//  Implementation of the Class CContourROI
//  Created on:      20-Jun-2005 10:10:54 AM
//  Original author: Ron Fox
///////////////////////////////////////////////////////////

#if !defined(__CCONTOURROI_H)
#define __CCONTOURROI_H

#include "CROI.h"

// Forward definitions.


class CAxis;
class CGateContainer;

/**
 * Determines if points are inside a region of interest determined by a contour or
 * a gamma contour (which is a subclass of a contour at this point).
 * @author Ron Fox
 * @version 1.0
 * @created 20-Jun-2005 10:10:54 AM
 */
class CContourROI : public CROI
{
private:
  /**
   * A gate container that points to a contour or a gamma contour that
   * describes the region of interest.
   */
  CGateContainer* m_pContour;
  
public:
  CContourROI(CGateContainer* pGate, CAxis& xAxis, CAxis& yAxis);
  virtual ~CContourROI();
  CContourROI(const CContourROI& rhs);
  CContourROI& operator=(const CContourROI& rhs);
  int operator==(const CContourROI& rhs) const;
  int operator!=(const CContourROI& rhs) const;


  virtual bool operator()(int x, int y);
  
  
};


#endif 
