/*
** Facility:
**   Xamine - Motif support methods.
** Abstract:
**   XMShell.cc:
**     This file implements methods for the Shell family of widgets.  
**     Shell widgets are widgets which can interact directly with the
**     window manager.  They can be considered top level entities in widget
**     hierarchies.
** Author:
**    Ron Fox
**    NSCL
**    Michigan State University
**    East Lansing, MI 48824-1321
*/
static char *sccsinfo = "@(#)XMShell.cc	8.1 6/23/95 ";

/*
** Include files:
*/
#include "XMShell.h"



/*
** Functional Description:
**    Shell::Shell
**      This function instantiates a shell widget.
**      This is done as follows:
**       0) The null constructor for the widget superclass is invoked.
**       1) Call XtAppCreateShell to create the shell.
**       2) Set the callbacks for pop up and down to invoke PopupCb and
**          PopdnCb methods respectively.
** Formal Parameters:
**    String shellname:
**      Name of the shell to create.
**    WidgetClass shelltype:
**      Type of shell to create.
**    XMWidget *parent:
**      A widget identifying the shell's parent... used to derive the display.
**    Arglist args:
**      Creation time resource list.
**    Cardinal num_args:
**      Number of resource entries.
*/
Shell::Shell(String shellname, WidgetClass shelltype, XMWidget *parent,
	     ArgList args, Cardinal num_args) : 
   XMWidget(shellname),		// Invoke the null action parent constructor
   Popdown(this),
   Popup(this)
{

  /* First get the info needed to instantiate the shell:  */

  Display *disp = XtDisplay(parent->getid()); // Get the shell's display id.

#define NINHRESOURCES	6	// Number of resources set here.
  int      nresources = num_args + NINHRESOURCES;
  Arg      inhresources[NINHRESOURCES];

  Pixmap   icon;

  Widget pshell = parent->getid();		// Parent shell.

  while(!XtIsShell(pshell)) pshell = XtParent(pshell);

  XtVaGetValues(pshell, XmNiconPixmap, &icon,
		NULL);

  /* Set up the argument list as the merger of our resources and the caller's.
  */

  XtSetArg(inhresources[0], XmNminWidth,    100);
  XtSetArg(inhresources[1], XmNminHeight,   100); // Make sure there's height.
  XtSetArg(inhresources[2], XmNheight, 100);
  XtSetArg(inhresources[3], XmNwidth, 100);
  XtSetArg(inhresources[4], XmNtitle, shellname);
  XtSetArg(inhresources[5], XmNiconPixmap, icon);

  ArgList resources = XtMergeArgLists(inhresources, NINHRESOURCES, 
				     args, num_args);

  /* Now we can create the shell */

  id = XtAppCreateShell(shellname, shellname, shelltype,
			disp, resources, nresources);


  XtFree((char *)resources);

  /* Set up the callbacks for the virtual functions.  Specialized  behavior is
  ** Then produced by deriving a subclass with replacement functions for the
  ** callbacks.
  */

#ifndef __ALPHA
  Popdown.Register(this, XmNpopdownCallback, &Shell::PopdnCb, NULL);
  Popup.Register(this, XmNpopupCallback, &Shell::PopupCb, NULL);
#endif
}


/*
** Functional Description:
**   ~Shell:
**       This function is called when it's time to destroy a shell widget.
*/	     
Shell::~Shell()
{
  UnManage();
#ifndef __ALPHA
  Popup.UnRegister();
  Popdown.UnRegister();
#endif
}
/*
** Below are the callback method templates for the pop up and pop down
** functions. These are intended to be overridden by shells with pop up/down
** requirements, and don't do anything for now.
*/

void Shell::PopupCb(XMWidget *shell, XtPointer user_d, XtPointer call_d)
{
}
void Shell::PopdnCb(XMWidget *shell, XtPointer user_d, XtPointer call_d)
{
}


/*
** Functional Description:
**   TopLevelShell:
**      This method instantiates a top level shell.  It is used for 
**      additional shells that operate like the main window shell.
** Formal Parameters:
**   String shellname:
**     Name of the shell to create.... also used as class for shell.
**   XMWidget *parent:
**     Shell's parent, used to inherit some resources.
*/
TopLevelShell::TopLevelShell(String shellname, XMWidget *parent) :
               Shell(shellname, topLevelShellWidgetClass, 
		     parent, NULL, 0)
{
		       	// Super class takes care of everything.
}


