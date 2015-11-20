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


/*
** Facility:
**   Motif C++ support.  
** Abstract:
**   XMLabel.h   - This file contains a class definition for the XMLabel class.
**                 this class is derived from the XMManagedWidget class.
**                 It implements an object oriented encapsulation of the Motif
**                 Label widget.
** Author:
**   Ron Fox
**   NSCL
**   Michigan State University
**   East Lansing, MI 48824-1321
** Version:
**   @(#)XMLabel.h	8.1 6/23/95 
*/

#ifndef XMLABEL_H
#define XMLABEL_H

#include <Xm/Label.h>

#ifndef XMWIDGET_H
#include "XMWidget.h"
#endif

class XMLabel :public  XMManagedWidget
   {
   public:
     XMLabel(const char *n, XMWidget &parent, const char *text,
	     ArgList args = NULL, Cardinal arg_count = 0);
     XMLabel(const char *n, Widget parent, const char *text,
	     ArgList args = NULL, Cardinal arg_count = 0);
     XMLabel(const char *n);
     XMLabel(Widget w);
     void SetLabel(const char *text);
     void SetLabel(XmString lbl);
   };
#endif
