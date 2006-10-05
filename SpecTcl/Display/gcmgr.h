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

//
// Facility:
//    Xamine -- NSCL Display program.
// Abstract:
//    gcmgr.h:
//      This file contains declarations for the graphical context
//      cache manager used in Xamine.  It's function is to instantiate,
//      and cache graphical contexts commonly used in Xamine.
// Author:
//    Ron Fox
//    NSCL
//    Michigan State University
//    East Lansing, MI 48824-1321
//    @(#)gcmgr.h	1.1 10/3/95 
//
//

#ifndef _GCMGR_H
#define _GCMGR_H
#include "xaminegc.h"

XamineSpectrumGC *Xamine_GetSpectrumGC(XMWidget &wid);
XamineTextGC     *Xamine_GetTextGC(XMWidget &wid);
XamineGrobjGC    *Xamine_GetGrobjGC(XMWidget &wid);


#endif
