#ifndef XMTab_h
#define XMTab_h
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
#include "TabP.h"

/*
** #Defines:
**
*/
#ifdef unix
#define XMFILE_DEFAULT_DIR     "./"
#define XMFILE_DEFAULT_DIRMASK "./*"
#endif

#ifdef VMS
#define XMFILE_DEFAULT_DIR     "SYS$DISK:[]"
#define XMFILE_DEFAULT_DIRMASK "SYS$DISK:[]*.*;0"
#endif

class XMTab : public XMManagedWidget
{
 protected:
  Widget *tab;

 public:
  XMTab(char *n, XMWidget &parent, 
	ArgList args = NULL, Cardinal arg_count = 0) :
    XMManagedWidget(n, xmTabWidgetClass, parent, args, arg_count) 
    { }
  XMTab(char *n, Widget parent, char *text,
	ArgList args = NULL, Cardinal arg_count = 0) :
    XMManagedWidget(n, xmLabelWidgetClass, parent, args, arg_count) 
    { }
  XMTab(char *n) : XMManagedWidget(n)
    { }
  XMTab(Widget w) : XMManagedWidget(w) 
    { }

  Callback_data *AddCallback
    (void (*cb)(XMWidget *, XtPointer, XtPointer) = NULL, XtPointer cd = NULL)
    { return XMWidget::AddCallback(XmNactivateCallback, cb, cd); }
};

#endif
