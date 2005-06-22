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
//  CROI.h
//  Implementation of the Class CROI
//  Created on:      20-Jun-2005 10:10:55 AM
//  Original author: Ron Fox
///////////////////////////////////////////////////////////

#if !defined(__CROI_H)
#define      __CROI_H

#ifndef __HISTOTYPES_H
#include <histotypes.h>
#endif

// Forward definitions:

class CAxis;

/**
 * This class encapsulates the various methods we have for determining insidedness
 * for regions of interest.  We must deal with regions of interest in contours,
 * gamma gates and 'true' gates.
 * @author Ron Fox
 * @version 1.0
 * @created 20-Jun-2005 10:10:55 AM
 */
class CROI
{
private:

  //! Defines transformation between X channel and parameter coordinates.
  
  CAxis& m_xAxisSpecification;
  /*!
   * Defines the transformation between y axis channel values and the y parameter
   * values.
   */
  CAxis& m_yAxisSpecification;
  
  
public:
  CROI(CAxis& xAxis, CAxis& yAxis);
  CROI(const CROI& rhs);	//!< Copy construction supported.
  virtual ~CROI();
  CROI& operator=(const CROI& rhs);
  int   operator==(const CROI& rhs) const;
  int   operator!=(const CROI& rhs) const;


  virtual bool operator()(int x, int y) =0;
  
protected:
  double xToParam(Int_t x);
  double yToParam(Int_t y);
  
  
};


#endif 
