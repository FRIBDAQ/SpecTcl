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

#include "XMWidget.h"
#include "XMCallback.h"
/*
** Shell provide basic Window manager interaction functionality
*/

class Shell : public XMWidget {
 public:
  Shell(String shellname,  WidgetClass shelltype,
		XMWidget *parent,
		ArgList args = NULL, Cardinal num_args = 0);
  ~Shell();

  void AllowResize(Boolean allow) 
    { SetAttribute(XmNallowShellResize, allow); }
  Boolean IsResizeAllowed() 
    { 
      Boolean yes;
      GetAttribute(XmNallowShellResize, &yes);
      return yes;
    }
  
  void SetGeometry(String newg)
    { SetAttribute(XmNgeometry, newg); }
  void GetGeometry(String geom) 
    { GetAttribute(XmNgeometry, geom); }

  void SetSaveUnder(Boolean newstate) 
    { SetAttribute(XmNsaveUnder, newstate); }
  Boolean GetSaveUnder() 
    { 
      Boolean state;
      GetAttribute(XmNsaveUnder, &state);
      return state;
    }

  void Realize() 
    { XtRealizeWidget(id); 
      Manage();
    }

  void Manage()   { XtPopup(id, XtGrabNone); }
  void UnManage() { XtPopdown(id); }
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
  TopLevelShell(String shellname, XMWidget *parent);
  ~TopLevelShell() {}

  void RealizeIconic(Boolean val)
    { SetAttribute(XmNiconic, val); }
  Boolean GetIconic()
    { 
      Boolean val;
      GetAttribute(XmNiconic, &val);
      return val;
    }

  void SetIconName(String name)
    { SetAttribute(XmNiconName, name); }
  void GetIconName(String name)
    { GetAttribute(XmNiconName, name); }


};
#endif
