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
**   Motif support:
** Abstract:
**   XMShell.h:
**     This file defines classes which encapsulate the behaior of 
**     Motif shell widgets.  Motif shell widgets are responsible for holding
**     windows which have decorations.
** Author:
**   Ron Fox
**   NSCL
**   Michigan State University
**   East Lansing, MI 48824-1321
**   @(#)XMShell.h	8.1 6/23/95 6/17/99
*/
#ifndef XMSHELL_H
#define XMSHELL_H

#include <Xm/Xm.h>
#include <X11/Shell.h>

#ifndef XMWIDGET_H
#include "XMWidget.h"
#endif

#ifndef XMCALLBACK_H
#include "XMCallback.h"
#endif

/*
** Shell provide basic Window manager interaction functionality
*/

class Shell : public XMWidget {
 public:
  Shell(const char* shellname,  WidgetClass shelltype,
		XMWidget *parent,
		ArgList args = NULL, Cardinal num_args = 0);
  ~Shell();

  void AllowResize(Boolean allow);
  Boolean IsResizeAllowed(); 
  
  void SetGeometry(String newg);
  void GetGeometry(String geom);

  void SetSaveUnder(Boolean newstate);
  Boolean GetSaveUnder();

  void Realize();

  void Manage();
  void UnManage();
 protected:
  XMCallback<Shell> Popdown;
  XMCallback<Shell> Popup;
 private:
  virtual void PopupCb(XMWidget *shell, XtPointer user_d, XtPointer call_d);
  virtual void PopdnCb(XMWidget *shell, XtPointer user_d, XtPointer call_d);
};


/*
** TopLevelShell - This shell is derived from Shell and provides the functions
**                 required of a secondary application shell.
*/

class TopLevelShell : public Shell {
 public:
  TopLevelShell(const char* shellname, XMWidget *parent);
  ~TopLevelShell();

  void RealizeIconic(Boolean val);
  Boolean GetIconic();

  void SetIconName(String name);
  void GetIconName(String name);
};
#endif
