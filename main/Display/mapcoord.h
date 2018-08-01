/*
    This software is Copyright by the Board of Trustees of Michigan
    State University (c) Copyright 2014.

    You may use this software under the terms of the GNU public license
    (GPL).  The terms of this license are described at:

     http://www.gnu.org/licenses/gpl.txt

     Authors:
             Ron Fox
             Jeromy Tompkins 
	     NSCL
	     Michigan State University
	     East Lansing, MI 48824-1321
*/



/*
** Facility:
**   Xamine - NSCL Display program.
** Abstract:
**   mapcoord.h:
**     This include file contains the definitions needed for clients of
**     mapped coordinates. These functions convert a value in a mapped 
**     coordinate system into a channel value, and vice-versa.
** Author:
**   Ron Fox
**   NSCL
**   Michigan State University
**   East Lansing, MI 48824-1321
** History:
**   @(#)convert.h	8.1 6/23/95 
*/

#ifndef MAPCOORD_H
#define MAPCOORD_H


float Transform(float fSourceLow, float fSourceHigh,
		float fDestLow,   float fDestHigh, 
		float point);
int Xamine_XMappedToChan(int specno, float value);
int Xamine_YMappedToChan(int specno, float value);
float Xamine_XChanToMapped(int specno, float chan);
float Xamine_YChanToMapped(int specno, float chan);

#endif

