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
**   Xamine  - utility function.
** Abstract:
**   procede.h  - This file defines a set of functions which allow
**                wait prompted X/Motif interactions.
** Author:
**   Ron Fox
**   NSCL
**   Michigan State University
**   East Lansing, MI 48824-1321
** Version:
**   @(#)procede.h	8.1 6/23/95 
*/

#ifndef PROCEDE_H
#define PROCEDE_H

#include "XMWidget.h"
#include "XMDialogs.h"

int Procede(XMWidget *parent, char *text);

#endif
