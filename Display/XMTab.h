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

#ifndef XMTAB_H
#define XMTAB_H
#include <stdio.h>

#include <X11/StringDefs.h>
#include <Xm/Xm.h>
#include <Xm/RowColumn.h>
#include <Xm/Form.h>
#include <Xm/Frame.h>
#include <Xm/Text.h>
#include <Xm/Label.h>
#include <Xm/PushB.h>
#include <Xm/ToggleB.h>
#include <Xm/DrawingA.h>
#include <Xm/XmP.h>

#ifndef XMWIDGET_H
#include "XMWidget.h"
#endif

#ifndef __TABP_H__
#include "TabP.h"
#endif

/*
** #Defines:
**
*/
#define XMFILE_DEFAULT_DIR     "./"
#define XMFILE_DEFAULT_DIRMASK "./*"

class XMTab : public XMManagedWidget
{
 protected:
  Widget *tab;

 public:
  XMTab(const char *n, XMWidget &parent, 
	ArgList args = NULL, Cardinal arg_count = 0);
  XMTab(const char *n, Widget parent, char *text,
	ArgList args = NULL, Cardinal arg_count = 0);
  XMTab(const char *n);
  XMTab(Widget w);

  Callback_data *AddCallback(void (*cb)(XMWidget *, XtPointer, XtPointer) 
			     = NULL, XtPointer cd = NULL);
};

#endif
